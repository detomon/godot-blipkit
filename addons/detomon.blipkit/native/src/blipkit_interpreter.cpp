#include "blipkit_interpreter.hpp"
#include "blipkit_assembler.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_track.hpp"
#include "blipkit_waveform.hpp"
#include <BlipKit.h>

using namespace detomon::BlipKit;
using namespace godot;
typedef BlipKitAssembler::Instruction Instruction;

BlipKitInterpreter::BlipKitInterpreter() {
	byte_code.instantiate();
	stack.reserve(STACK_SIZE_MAX);
	instruments.resize(SLOT_COUNT);
	waveforms.resize(SLOT_COUNT);
}

void BlipKitInterpreter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_instrument_at", "slot", "instruments"), &BlipKitInterpreter::set_instrument_at);
	ClassDB::bind_method(D_METHOD("get_instrument_at", "slot"), &BlipKitInterpreter::get_instrument_at);
	ClassDB::bind_method(D_METHOD("set_waveform_at", "slot", "waveforms"), &BlipKitInterpreter::set_waveform_at);
	ClassDB::bind_method(D_METHOD("get_waveform_at", "slot"), &BlipKitInterpreter::get_waveform_at);
	ClassDB::bind_method(D_METHOD("set_byte_code", "byte_code"), &BlipKitInterpreter::set_byte_code);
	ClassDB::bind_method(D_METHOD("advance", "track"), &BlipKitInterpreter::advance);
	ClassDB::bind_method(D_METHOD("get_status"), &BlipKitInterpreter::get_status);
	ClassDB::bind_method(D_METHOD("get_error_message"), &BlipKitInterpreter::get_error_message);
	ClassDB::bind_method(D_METHOD("reset"), &BlipKitInterpreter::reset);

	BIND_ENUM_CONSTANT(OK_RUNNING);
	BIND_ENUM_CONSTANT(OK_FINISHED);
	BIND_ENUM_CONSTANT(ERR_INVALID_ARGUMENT);
	BIND_ENUM_CONSTANT(ERR_INVALID_INSTRUCTION);
	BIND_ENUM_CONSTANT(ERR_STACK_OVERFLOW);
	BIND_ENUM_CONSTANT(ERR_STACK_UNDERFLOW);
	BIND_ENUM_CONSTANT(ERR_RECURSION);
}

String BlipKitInterpreter::_to_string() const {
	return vformat("BlipKitInterpreter");
}

void BlipKitInterpreter::fail_with_error(Status p_status, const String &p_error_message) {
	status = p_status;
	error_message = p_error_message;

	// Seek to end.
	int size = byte_code->get_size();
	byte_code->seek(size);
}

void BlipKitInterpreter::set_instrument_at(int p_slot, const Ref<BlipKitInstrument> &p_instrument) {
	ERR_FAIL_INDEX(p_slot, SLOT_COUNT);
	instruments[p_slot] = p_instrument;
}

Ref<BlipKitInstrument> BlipKitInterpreter::get_instrument_at(int p_slot) const {
	ERR_FAIL_INDEX_V(p_slot, SLOT_COUNT, nullptr);
	return instruments[p_slot];
}

void BlipKitInterpreter::set_waveform_at(int p_slot, const Ref<BlipKitWaveform> &p_waveform) {
	ERR_FAIL_INDEX(p_slot, SLOT_COUNT);
	waveforms[p_slot] = p_waveform;
}

Ref<BlipKitWaveform> BlipKitInterpreter::get_waveform_at(int p_slot) const {
	ERR_FAIL_INDEX_V(p_slot, SLOT_COUNT, nullptr);
	return waveforms[p_slot];
}

void BlipKitInterpreter::set_byte_code(const PackedByteArray &p_byte) {
	byte_code->set_data_array(p_byte);
	reset();
}

int BlipKitInterpreter::advance(const Ref<BlipKitTrack> &p_track) {
	if (p_track.is_null()) {
		fail_with_error(ERR_INVALID_ARGUMENT, "Track is null.");
		return -1;
	}

	while (byte_code->get_available_bytes() > 0) {
		int instr_offset = byte_code->get_position();
		Instruction instr = static_cast<Instruction>(byte_code->get_u8());

		switch (instr) {
			case Instruction::INSTR_NOP: {
				// Do nothing.
			} break;
			case Instruction::INSTR_NOTE: {
				p_track->set_note(byte_code->get_float());
			} break;
			case Instruction::INSTR_VOLUME: {
				p_track->set_volume(byte_code->get_float());
			} break;
			case Instruction::INSTR_MASTER_VOLUME: {
				p_track->set_master_volume(byte_code->get_float());
			} break;
			case Instruction::INSTR_PANNING: {
				p_track->set_panning(byte_code->get_float());
			} break;
			case Instruction::INSTR_PITCH: {
				p_track->set_pitch(byte_code->get_float());
			} break;
			case Instruction::INSTR_WAVEFORM: {
				p_track->set_waveform(static_cast<BlipKitTrack::Waveform>(byte_code->get_u8()));
			} break;
			case Instruction::INSTR_CUSTOM_WAVEFORM: {
				int index = byte_code->get_u8();
				const Ref<BlipKitWaveform> &waveform = waveforms[index];
				p_track->set_custom_waveform(waveform);
			} break;
			case Instruction::INSTR_DUTY_CYCLE: {
				p_track->set_duty_cycle(byte_code->get_u8());
			} break;
			case Instruction::INSTR_PHASE_WRAP: {
				p_track->set_duty_cycle(byte_code->get_u8());
			} break;
			case Instruction::INSTR_INSTRUMENT: {
				int index = byte_code->get_u8();
				const Ref<BlipKitInstrument> &instrument = instruments[index];
				p_track->set_instrument(instrument);
			} break;
			case Instruction::INSTR_EFFECT_DIVIDER: {
				p_track->set_effect_divider(byte_code->get_u32());
			} break;
			case Instruction::INSTR_VOLUME_SLIDE: {
				p_track->set_volume_slide(byte_code->get_u32());
			} break;
			case Instruction::INSTR_PANNING_SLIDE: {
				p_track->set_panning_slide(byte_code->get_u32());
			} break;
			case Instruction::INSTR_PORTAMENTO: {
				p_track->set_portamento(byte_code->get_u32());
			} break;
			case Instruction::INSTR_ARPEGGIO_DIVIDER: {
				p_track->set_arpeggio_divider(byte_code->get_u32());
			} break;
			case Instruction::INSTR_INSTRUMENT_DIVIDER: {
				p_track->set_instrument_divider(byte_code->get_u32());
			} break;
			case Instruction::INSTR_WAIT: {
				uint32_t wait = byte_code->get_u32();
				return wait;
			} break;
			case Instruction::INSTR_TREMOLO: {
				int ticks = byte_code->get_u32();
				real_t delta = byte_code->get_float();
				int slide_ticks = byte_code->get_u32();
				p_track->set_tremolo(ticks, delta, slide_ticks);
			} break;
			case Instruction::INSTR_VIBRATO: {
				int ticks = byte_code->get_u32();
				real_t delta = byte_code->get_float();
				int slide_ticks = byte_code->get_u32();
				p_track->set_vibrato(ticks, delta, slide_ticks);
			} break;
			case Instruction::INSTR_ARPEGGIO: {
				int count = byte_code->get_u8();
				arpeggio.resize(count);

				for (int i = 0; i < count; i++) {
					arpeggio[i] = byte_code->get_float();
				}
				p_track->set_arpeggio(arpeggio);
			} break;
			case Instruction::INSTR_CALL: {
				int offset = byte_code->get_u32();
				int position = byte_code->get_position();

				if (stack.size() >= STACK_SIZE_MAX) {
					fail_with_error(ERR_STACK_OVERFLOW, "Stack overflow.");
					return -1;
				}

				stack.push_back(position);
				byte_code->seek(offset);
			} break;
			case Instruction::INSTR_JUMP: {
				int offset = byte_code->get_u32();
				byte_code->seek(offset);
			} break;
			case Instruction::INSTR_RETURN: {
				if (stack.is_empty()) {
					fail_with_error(ERR_STACK_OVERFLOW, "Stack underflow.");
					return -1;
				}

				int index = stack.size() - 1;
				int offset = stack[index];
				stack.remove_at(index);
				byte_code->seek(offset);
			} break;
			case Instruction::INSTR_RESET: {
				p_track->reset();
			} break;
			default: {
				fail_with_error(ERR_INVALID_INSTRUCTION, vformat("Invalid instruction %d at offset %d.", instr, instr_offset));
				return -1;
			} break;
		}
	}

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

void BlipKitInterpreter::reset() {
	byte_code->seek(0);
	status = OK_RUNNING;
	error_message.resize(0);
}
