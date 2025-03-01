#include "blipkit_interpreter.hpp"
#include "blipkit_assembler.hpp"
#include "blipkit_bytecode.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_track.hpp"
#include "blipkit_waveform.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include <BlipKit.h>

using namespace BlipKit;
using namespace godot;

typedef BlipKitAssembler::Opcode Opcode;

BlipKitInterpreter::BlipKitInterpreter() {
	stack.reserve(STACK_SIZE_MAX);
	instruments.resize(SLOT_COUNT);
	waveforms.resize(SLOT_COUNT);
}

int BlipKitInterpreter::fail_with_error(Status p_status, const String &p_error_message) {
	status = p_status;
	error_message = p_error_message;

	// Seek to end.
	const int size = byte_code.size();
	byte_code.seek(size);

	ERR_FAIL_V_MSG(-1, error_message);
}

void BlipKitInterpreter::set_instrument(int p_slot, const Ref<BlipKitInstrument> &p_instrument) {
	ERR_FAIL_INDEX(p_slot, instruments.size());
	instruments[p_slot] = p_instrument;
}

Ref<BlipKitInstrument> BlipKitInterpreter::get_instrument(int p_slot) const {
	ERR_FAIL_INDEX_V(p_slot, instruments.size(), nullptr);
	return instruments[p_slot];
}

void BlipKitInterpreter::set_waveform(int p_slot, const Ref<BlipKitWaveform> &p_waveform) {
	ERR_FAIL_INDEX(p_slot, waveforms.size());
	waveforms[p_slot] = p_waveform;
}

Ref<BlipKitWaveform> BlipKitInterpreter::get_waveform(int p_slot) const {
	ERR_FAIL_INDEX_V(p_slot, waveforms.size(), nullptr);
	return waveforms[p_slot];
}

void BlipKitInterpreter::set_register(int p_register, int p_value) {
	ERR_FAIL_INDEX(p_register, REGISTER_COUNT);
	registers.aux[p_register] = p_value;
}

int BlipKitInterpreter::get_register(int p_register) const {
	ERR_FAIL_INDEX_V(p_register, REGISTER_COUNT, 0);
	return registers.aux[p_register];
}

bool BlipKitInterpreter::load_byte_code(const Ref<BlipKitBytecode> &p_byte_code, const String &p_start_label) {
	ERR_FAIL_COND_V(p_byte_code.is_null(), false);

	if (!p_byte_code->is_valid()) {
		fail_with_error(ERR_INVALID_BINARY, p_byte_code->get_error_message());
		return false;
	}

	if (!p_start_label.is_empty()) {
		if (!p_byte_code->has_label(p_start_label)) {
			fail_with_error(ERR_INVALID_BINARY, vformat("Label '%s' does not exist.", p_start_label));
			return false;
		}
	}

	byte_code_res = p_byte_code;
	byte_code.set_bytes(byte_code_res->get_bytes());

	reset(p_start_label);

	return true;
}

int BlipKitInterpreter::advance(const Ref<BlipKitTrack> &p_track) {
	ERR_FAIL_COND_V(p_track.is_null(), 0);

	while (byte_code.get_available_bytes() > 0) {
		const uint32_t code_offset = byte_code.get_position();
		const Opcode opcode = static_cast<Opcode>(byte_code.get_u8());

		switch (opcode) {
			case Opcode::OP_NOOP: {
				// Do nothing.
			} break;
			case Opcode::OP_ATTACK: {
				p_track->set_note(byte_code.get_f16());
			} break;
			case Opcode::OP_RELEASE: {
				p_track->release();
			} break;
			case Opcode::OP_MUTE: {
				p_track->mute();
			} break;
			case Opcode::OP_VOLUME: {
				p_track->set_volume(byte_code.get_f16());
			} break;
			case Opcode::OP_MASTER_VOLUME: {
				p_track->set_master_volume(byte_code.get_f16());
			} break;
			case Opcode::OP_PANNING: {
				p_track->set_panning(byte_code.get_f16());
			} break;
			case Opcode::OP_PITCH: {
				p_track->set_pitch(byte_code.get_f16());
			} break;
			case Opcode::OP_WAVEFORM: {
				p_track->set_waveform(static_cast<BlipKitTrack::Waveform>(byte_code.get_u8()));
			} break;
			case Opcode::OP_CUSTOM_WAVEFORM: {
				const int index = byte_code.get_u8();
				const Ref<BlipKitWaveform> &waveform = waveforms[index];
				p_track->set_custom_waveform(waveform);
			} break;
			case Opcode::OP_DUTY_CYCLE: {
				p_track->set_duty_cycle(byte_code.get_u8());
			} break;
			case Opcode::OP_PHASE_WRAP: {
				p_track->set_phase_wrap(byte_code.get_u8());
			} break;
			case Opcode::OP_INSTRUMENT: {
				const int index = byte_code.get_u8();
				const Ref<BlipKitInstrument> &instrument = instruments[index];
				p_track->set_instrument(instrument);
			} break;
			case Opcode::OP_EFFECT_DIV: {
				p_track->set_effect_divider(byte_code.get_u16());
			} break;
			case Opcode::OP_VOLUME_SLIDE: {
				p_track->set_volume_slide(byte_code.get_u16());
			} break;
			case Opcode::OP_PANNING_SLIDE: {
				p_track->set_panning_slide(byte_code.get_u16());
			} break;
			case Opcode::OP_PORTAMENTO: {
				p_track->set_portamento(byte_code.get_u16());
			} break;
			case Opcode::OP_ARPEGGIO_DIV: {
				p_track->set_arpeggio_divider(byte_code.get_u16());
			} break;
			case Opcode::OP_INSTRUMENT_DIV: {
				p_track->set_instrument_divider(byte_code.get_u16());
			} break;
			case Opcode::OP_TICK: {
				const int ticks = byte_code.get_u16();
				return ticks;
			} break;
			case Opcode::OP_TREMOLO: {
				const int ticks = byte_code.get_u16();
				const float delta = byte_code.get_f16();
				const int slide_ticks = byte_code.get_u16();
				p_track->set_tremolo(ticks, delta, slide_ticks);
			} break;
			case Opcode::OP_VIBRATO: {
				const int ticks = byte_code.get_u16();
				const float delta = byte_code.get_f16();
				const int slide_ticks = byte_code.get_u16();
				p_track->set_vibrato(ticks, delta, slide_ticks);
			} break;
			case Opcode::OP_ARPEGGIO: {
				const int count = byte_code.get_u8();
				arpeggio.resize(count);

				for (uint32_t i = 0; i < count; i++) {
					arpeggio[i] = byte_code.get_f16();
				}
				p_track->set_arpeggio(arpeggio);
			} break;
			case Opcode::OP_CALL: {
				const int offset = byte_code.get_s32();
				const int position = byte_code.get_position();

				if (stack.size() >= STACK_SIZE_MAX) {
					return fail_with_error(ERR_STACK_OVERFLOW, "Stack overflow.");
				}

				stack.push_back(position);
				const int jump_offset = position + offset - 4; // Subtract size of address.
				byte_code.seek(jump_offset);
			} break;
			case Opcode::OP_JUMP: {
				const int offset = byte_code.get_s32();
				const int position = byte_code.get_position();
				const int jump_offset = position + offset - 4; // Subtract size of address.
				byte_code.seek(jump_offset);
			} break;
			case Opcode::OP_RETURN: {
				if (stack.is_empty()) {
					return fail_with_error(ERR_STACK_OVERFLOW, "Stack underflow.");
				}

				// Pop last value.
				const int index = stack.size() - 1;
				const int offset = stack[index];
				stack.remove_at(index);
				byte_code.seek(offset);
			} break;
			case Opcode::OP_RESET: {
				p_track->reset();
			} break;
			case Opcode::OP_STORE: {
				const int number = CLAMP(byte_code.get_u8(), 0, REGISTER_COUNT);
				const int value = byte_code.get_s32();
				registers.aux[number] = value;
			} break;
			case Opcode::OP_HALT: {
				// Seek to end.
				byte_code.seek(byte_code.size());
			} break;
			default: {
				return fail_with_error(ERR_INVALID_OPCODE, vformat("Invalid opcode %d at offset %d.", opcode, code_offset));
			} break;
		}
	}

	// At this point there are no more instrutions.
	if (status == OK_RUNNING) {
		status = OK_FINISHED;
	}

	if (!error_message.is_empty()) {
		return -1;
	}

	return 0;
}

BlipKitInterpreter::Status BlipKitInterpreter::get_status() const {
	return status;
}

String BlipKitInterpreter::get_error_message() const {
	return error_message;
}

void BlipKitInterpreter::reset(const String &p_start_label) {
	ERR_FAIL_COND(status >= ERR_INVALID_BINARY);

	if (!p_start_label.is_empty()) {
		if (!byte_code_res->has_label(p_start_label)) {
			fail_with_error(ERR_INVALID_BINARY, vformat("Label '%s' does not exist.", p_start_label));
			return;
		}
	}

	start_label = p_start_label;

	int start_position = start_label.is_empty()
			? byte_code_res->get_start_position()
			: byte_code_res->get_label_position(start_label);
	byte_code.seek(start_position);

	stack.clear();
	registers = Registers();
	arpeggio.clear();
	status = OK_RUNNING;
	error_message.resize(0);
}

void BlipKitInterpreter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_instrument", "slot", "instruments"), &BlipKitInterpreter::set_instrument);
	ClassDB::bind_method(D_METHOD("get_instrument", "slot"), &BlipKitInterpreter::get_instrument);
	ClassDB::bind_method(D_METHOD("set_waveform", "slot", "waveforms"), &BlipKitInterpreter::set_waveform);
	ClassDB::bind_method(D_METHOD("get_waveform", "slot"), &BlipKitInterpreter::get_waveform);
	ClassDB::bind_method(D_METHOD("set_register", "register", "value"), &BlipKitInterpreter::set_register);
	ClassDB::bind_method(D_METHOD("get_register", "register"), &BlipKitInterpreter::get_register);
	ClassDB::bind_method(D_METHOD("load_byte_code", "byte_code", "start_label"), &BlipKitInterpreter::load_byte_code, DEFVAL(""));
	ClassDB::bind_method(D_METHOD("advance", "track"), &BlipKitInterpreter::advance);
	ClassDB::bind_method(D_METHOD("get_status"), &BlipKitInterpreter::get_status);
	ClassDB::bind_method(D_METHOD("get_error_message"), &BlipKitInterpreter::get_error_message);
	ClassDB::bind_method(D_METHOD("reset", "start_label"), &BlipKitInterpreter::reset, DEFVAL(""));

	BIND_ENUM_CONSTANT(OK_RUNNING);
	BIND_ENUM_CONSTANT(OK_FINISHED);
	BIND_ENUM_CONSTANT(ERR_INVALID_BINARY);
	BIND_ENUM_CONSTANT(ERR_INVALID_OPCODE);
	BIND_ENUM_CONSTANT(ERR_STACK_OVERFLOW);
	BIND_ENUM_CONSTANT(ERR_STACK_UNDERFLOW);

	BIND_CONSTANT(REGISTER_COUNT);
	BIND_CONSTANT(SLOT_COUNT);
}

String BlipKitInterpreter::_to_string() const {
	return vformat("<BlipKitInterpreter:#%d>", int64_t(this));
}
