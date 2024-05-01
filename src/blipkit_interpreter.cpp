#include "blipkit_interpreter.hpp"
#include "blipkit_assembler.hpp"
#include "blipkit_bytecode.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_sample.hpp"
#include "blipkit_track.hpp"
#include "blipkit_waveform.hpp"
#include <BlipKit.h>

using namespace BlipKit;
using namespace godot;

typedef BlipKitAssembler::Opcode Opcode;

BlipKitInterpreter::BlipKitInterpreter() {
	stack.reserve(STACK_SIZE_MAX);
	arpeggio.resize(BlipKitTrack::ARPEGGIO_MAX);
	instruments.resize(SLOT_COUNT);
	waveforms.resize(SLOT_COUNT);
	samples.resize(SLOT_COUNT);
}

uint32_t BlipKitInterpreter::exec_delay_begin(uint32_t p_ticks, uint32_t p_code_offset) {
	if (delay_register.state == DELAY_STATE_EXEC) {
		return exec_delay_shift();
	} else {
		// Keep one entry for OP_TICK/OP_STEP instruction.
		if (not p_ticks || delay_register.delay_size >= DelayRegister::MAX_DELAYS - 1) {
			return 0;
		}

		// Save code offset of delay sequence.
		if (not delay_register.delay_size) {
			delay_register.state = DELAY_STATE_DELAY;
			delay_register.code_offset = p_code_offset;
		}
		delay_register.ticks += p_ticks;
		delay_register.delays[delay_register.delay_size++] = p_ticks;

		return 0;
	}
}

uint32_t BlipKitInterpreter::exec_delay_step(uint32_t p_ticks) {
	if (delay_register.state == DELAY_STATE_EXEC) {
		const uint32_t ticks = exec_delay_shift();

		delay_register.state = DELAY_STATE_NONE;
		delay_register.delay_index = 0;
		delay_register.delay_size = 0;

		return ticks;
	} else {
		// Ignore and reset when no more entries are available.
		if (not p_ticks || delay_register.delay_size >= DelayRegister::MAX_DELAYS) {
			delay_register.delay_index = 0;
			delay_register.delay_size = 0;
			return 0;
		}

		// Limit total delay to step ticks.
		delay_register.ticks = MIN(delay_register.ticks, p_ticks);

		const uint32_t ticks_rest = p_ticks - delay_register.ticks;
		delay_register.ticks += ticks_rest;
		delay_register.state = DELAY_STATE_EXEC;

		// Push delay entry.
		delay_register.delays[delay_register.delay_size++] = p_ticks;

		// Jump to beginning of delay sequences.
		const uint32_t code_offset = delay_register.code_offset;
		byte_code.seek(code_offset);

		return 0;
	}
}

uint32_t BlipKitInterpreter::exec_delay_shift() {
	if (delay_register.delay_index >= delay_register.delay_size) {
		return 0;
	}

	// Shift delay entry.
	uint32_t ticks = delay_register.delays[delay_register.delay_index++];

	ticks = MIN(ticks, delay_register.ticks);
	delay_register.ticks -= ticks;

	return ticks;
}

int BlipKitInterpreter::fail_with_error(State p_state, const String &p_error_message) {
	state = p_state;
	error_message = p_error_message;

	// Seek to end.
	const uint32_t size = byte_code.size();
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

void BlipKitInterpreter::set_sample(int p_slot, const Ref<BlipKitSample> &p_sample) {
	ERR_FAIL_INDEX(p_slot, samples.size());
	samples[p_slot] = p_sample;
}

Ref<BlipKitSample> BlipKitInterpreter::get_sample(int p_slot) const {
	ERR_FAIL_INDEX_V(p_slot, samples.size(), nullptr);
	return samples[p_slot];
}

void BlipKitInterpreter::set_step_ticks(int p_step_ticks) {
	step_ticks = CLAMP(p_step_ticks, 1, UINT16_MAX);
}

int BlipKitInterpreter::get_step_ticks() const {
	return step_ticks;
}

bool BlipKitInterpreter::load_byte_code(const Ref<BlipKitBytecode> &p_byte_code, const String &p_start_label) {
	ERR_FAIL_COND_V(p_byte_code.is_null(), false);

	if (not p_byte_code->is_valid()) {
		fail_with_error(ERR_INVALID_BINARY, p_byte_code->get_error_message());
		return false;
	}

	if (not p_start_label.is_empty()) {
		if (not p_byte_code->has_label(p_start_label)) {
			fail_with_error(ERR_INVALID_LABEL, vformat("Label '%s' does not exist.", p_start_label));
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
		const bool execute = delay_register.state != DELAY_STATE_DELAY;

		switch (opcode) {
			case Opcode::OP_NOOP: {
				// Do nothing.
			} break;
			case Opcode::OP_HALT: {
				state = OK_FINISHED;
				return 0;
			} break;
			case Opcode::OP_ATTACK: {
				const float value = byte_code.get_f16();

				if (execute) [[likely]] {
					p_track->set_note(value);
				}
			} break;
			case Opcode::OP_RELEASE: {
				if (execute) [[likely]] {
					p_track->release();
				}
			} break;
			case Opcode::OP_MUTE: {
				if (execute) [[likely]] {
					p_track->mute();
				}
			} break;
			case Opcode::OP_VOLUME: {
				const float value = byte_code.get_f16();

				if (execute) [[likely]] {
					p_track->set_volume(value);
				}
			} break;
			case Opcode::OP_MASTER_VOLUME: {
				const float value = byte_code.get_f16();

				if (execute) [[likely]] {
					p_track->set_master_volume(value);
				}
			} break;
			case Opcode::OP_PANNING: {
				const float value = byte_code.get_f16();

				if (execute) [[likely]] {
					p_track->set_panning(value);
				}
			} break;
			case Opcode::OP_WAVEFORM: {
				const int32_t value = byte_code.get_u8();

				if (execute) [[likely]] {
					p_track->set_waveform(static_cast<BlipKitTrack::Waveform>(value));
				}
			} break;
			case Opcode::OP_DUTY_CYCLE: {
				const int32_t value = byte_code.get_u8();

				if (execute) [[likely]] {
					p_track->set_duty_cycle(value);
				}
			} break;
			case Opcode::OP_PITCH: {
				const float value = byte_code.get_f16();

				if (execute) [[likely]] {
					p_track->set_pitch(value);
				}
			} break;
			case Opcode::OP_PHASE_WRAP: {
				const int32_t value = byte_code.get_u8();

				if (execute) [[likely]] {
					p_track->set_phase_wrap(value);
				}
			} break;
			case Opcode::OP_PORTAMENTO: {
				const float steps = byte_code.get_f16();

				if (execute) [[likely]] {
					const uint32_t ticks = steps * step_ticks;
					p_track->set_portamento(ticks);
				}
			} break;
			case Opcode::OP_VIBRATO: {
				const float steps = byte_code.get_f16();
				const float delta = byte_code.get_f16();
				const float slide_steps = byte_code.get_f16();

				if (execute) [[likely]] {
					const uint32_t ticks = steps * step_ticks;
					const uint32_t slide_ticks = slide_steps * step_ticks;
					p_track->set_vibrato(ticks, delta, slide_ticks);
				}
			} break;
			case Opcode::OP_TREMOLO: {
				const float steps = byte_code.get_f16();
				const float delta = byte_code.get_f16();
				const float slide_steps = byte_code.get_f16();

				if (execute) [[likely]] {
					const uint32_t ticks = steps * step_ticks;
					const uint32_t slide_ticks = slide_steps * step_ticks;
					p_track->set_tremolo(ticks, delta, slide_ticks);
				}
			} break;
			case Opcode::OP_VOLUME_SLIDE: {
				const float steps = byte_code.get_f16();

				if (execute) [[likely]] {
					const uint32_t ticks = steps * step_ticks;
					p_track->set_volume_slide(ticks);
				}
			} break;
			case Opcode::OP_PANNING_SLIDE: {
				const float steps = byte_code.get_f16();

				if (execute) [[likely]] {
					const uint32_t ticks = steps * step_ticks;
					p_track->set_panning_slide(ticks);
				}
			} break;
			case Opcode::OP_EFFECT_DIV: {
				const uint32_t value = byte_code.get_u16();

				if (execute) [[likely]] {
					p_track->set_effect_divider(value);
				}
			} break;
			case Opcode::OP_ARPEGGIO: {
				const int32_t count = byte_code.get_u8();

				if (execute) [[likely]] {
					arpeggio.resize(count);
					for (uint32_t i = 0; i < count; i++) {
						arpeggio[i] = byte_code.get_f16();
					}
					p_track->set_arpeggio(arpeggio);
				} else {
					const uint32_t position = byte_code.get_position();
					byte_code.seek(position + count * 2);
				}
			} break;
			case Opcode::OP_ARPEGGIO_DIV: {
				const int32_t value = byte_code.get_u16();

				if (execute) [[likely]] {
					p_track->set_arpeggio_divider(value);
				}
			} break;
			case Opcode::OP_TICK: {
				int32_t ticks = byte_code.get_u16();

				if (delay_register.delay_size) {
					ticks = exec_delay_step(ticks);
				}

				if (ticks) [[likely]] {
					return ticks;
				}
			} break;
			case Opcode::OP_STEP: {
				const int32_t steps = byte_code.get_u16();
				int32_t ticks = steps * step_ticks;

				if (delay_register.delay_size) {
					ticks = exec_delay_step(ticks);
				}

				if (ticks) [[likely]] {
					return ticks;
				}
			} break;
			case Opcode::OP_STEP_TICKS: {
				const uint32_t step_ticks = byte_code.get_u16();

				if (execute) [[likely]] {
					set_step_ticks(step_ticks);
				}
			} break;
			case Opcode::OP_DELAY_TICK: {
				uint32_t ticks = byte_code.get_u16();

				ticks = exec_delay_begin(ticks, code_offset);

				if (ticks) [[likely]] {
					return ticks;
				}
			} break;
			case Opcode::OP_DELAY_STEP: {
				const float delay = byte_code.get_f16();
				uint32_t ticks = delay * step_ticks;

				ticks = exec_delay_begin(ticks, code_offset);

				if (ticks) [[likely]] {
					return ticks;
				}
			} break;
			case Opcode::OP_JUMP: {
				const int32_t offset = byte_code.get_s32();

				if (execute) [[likely]] {
					const int32_t position = byte_code.get_position();
					const int32_t jump_offset = position + offset - sizeof(int32_t); // Subtract size of address.
					byte_code.seek(jump_offset);
				}
			} break;
			case Opcode::OP_CALL: {
				const int32_t offset = byte_code.get_s32();

				if (execute) [[likely]] {
					if (stack.size() >= STACK_SIZE_MAX) [[unlikely]] {
						return fail_with_error(ERR_STACK_OVERFLOW, "Stack overflow.");
					}

					const int32_t position = byte_code.get_position();
					const int32_t jump_offset = position + offset - sizeof(int32_t); // Subtract size of address.
					stack.push_back(position);
					byte_code.seek(jump_offset);
				}
			} break;
			case Opcode::OP_RETURN: {
				if (execute) [[likely]] {
					if (stack.is_empty()) {
						return fail_with_error(ERR_STACK_OVERFLOW, "Stack underflow.");
					}

					// Pop last value.
					const uint32_t index = stack.size() - 1;
					const uint32_t offset = stack[index];

					stack.resize(index);
					byte_code.seek(offset);
				}
			} break;
			case Opcode::OP_RESET: {
				if (execute) [[likely]] {
					p_track->reset();
				}
			} break;
			case Opcode::OP_INSTRUMENT: {
				const int32_t index = byte_code.get_u8();

				if (execute) [[likely]] {
					const Ref<BlipKitInstrument> &instrument = instruments[index];
					p_track->set_instrument(instrument);
				}
			} break;
			case Opcode::OP_INSTRUMENT_DIV: {
				const int32_t value = byte_code.get_u16();

				if (execute) [[likely]] {
					p_track->set_instrument_divider(value);
				}
			} break;
			case Opcode::OP_CUSTOM_WAVEFORM: {
				const int32_t index = byte_code.get_u8();

				if (execute) [[likely]] {
					const Ref<BlipKitWaveform> &waveform = waveforms[index];
					p_track->set_custom_waveform(waveform);
				}
			} break;
			case Opcode::OP_SAMPLE: {
				const int32_t index = byte_code.get_u8();

				if (execute) [[likely]] {
					const Ref<BlipKitSample> &sample = samples[index];
					p_track->set_sample(sample);
				}
			} break;
			case Opcode::OP_SAMPLE_PITCH: {
				const float pitch = byte_code.get_f16();

				if (execute) [[likely]] {
					p_track->set_sample_pitch(pitch);
				}
			} break;
			default: {
				return fail_with_error(ERR_INVALID_OPCODE, vformat("Invalid opcode %d at offset %d.", opcode, code_offset));
			} break;
		}
	}

	// At this point there are no more instrutions.
	if (state == OK_RUNNING) {
		state = OK_FINISHED;
	}

	// An error occurred.
	if (state >= ERR_INVALID_BINARY) {
		return -1;
	}

	return 0;
}

BlipKitInterpreter::State BlipKitInterpreter::get_state() const {
	return state;
}

String BlipKitInterpreter::get_error_message() const {
	return error_message;
}

void BlipKitInterpreter::reset(const String &p_start_label) {
	ERR_FAIL_COND(byte_code_res.is_null());

	if (not p_start_label.is_empty()) {
		if (not byte_code_res->has_label(p_start_label)) {
			fail_with_error(ERR_INVALID_LABEL, vformat("Label '%s' does not exist.", p_start_label));
			return;
		}
	}

	uint32_t start_position = byte_code_res->get_code_section_offset();

	if (not p_start_label.is_empty()) {
		const int32_t label_index = byte_code_res->find_label(p_start_label);
		start_position += byte_code_res->get_label_position(label_index);
	}

	byte_code.seek(start_position);

	stack.clear();
	delay_register = DelayRegister();
	arpeggio.clear();
	state = OK_RUNNING;
	error_message.resize(0);
}

void BlipKitInterpreter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_instrument", "slot", "instrument"), &BlipKitInterpreter::set_instrument);
	ClassDB::bind_method(D_METHOD("get_instrument", "slot"), &BlipKitInterpreter::get_instrument);
	ClassDB::bind_method(D_METHOD("set_waveform", "slot", "waveform"), &BlipKitInterpreter::set_waveform);
	ClassDB::bind_method(D_METHOD("get_waveform", "slot"), &BlipKitInterpreter::get_waveform);
	ClassDB::bind_method(D_METHOD("set_sample", "slot", "sample"), &BlipKitInterpreter::set_sample);
	ClassDB::bind_method(D_METHOD("get_sample", "slot"), &BlipKitInterpreter::get_sample);
	ClassDB::bind_method(D_METHOD("set_step_ticks", "step_ticks"), &BlipKitInterpreter::set_step_ticks);
	ClassDB::bind_method(D_METHOD("get_step_ticks"), &BlipKitInterpreter::get_step_ticks);
	ClassDB::bind_method(D_METHOD("load_byte_code", "byte_code", "start_label"), &BlipKitInterpreter::load_byte_code, DEFVAL(""));
	ClassDB::bind_method(D_METHOD("advance", "track"), &BlipKitInterpreter::advance);
	ClassDB::bind_method(D_METHOD("get_state"), &BlipKitInterpreter::get_state);
	ClassDB::bind_method(D_METHOD("get_error_message"), &BlipKitInterpreter::get_error_message);
	ClassDB::bind_method(D_METHOD("reset", "start_label"), &BlipKitInterpreter::reset, DEFVAL(""));

	ADD_PROPERTY(PropertyInfo(Variant::INT, "step_ticks"), "set_step_ticks", "get_step_ticks");

	BIND_ENUM_CONSTANT(OK_RUNNING);
	BIND_ENUM_CONSTANT(OK_FINISHED);
	BIND_ENUM_CONSTANT(ERR_INVALID_BINARY);
	BIND_ENUM_CONSTANT(ERR_INVALID_OPCODE);
	BIND_ENUM_CONSTANT(ERR_INVALID_LABEL);
	BIND_ENUM_CONSTANT(ERR_STACK_OVERFLOW);
	BIND_ENUM_CONSTANT(ERR_STACK_UNDERFLOW);

	BIND_CONSTANT(STACK_SIZE_MAX);
	BIND_CONSTANT(SLOT_COUNT);
	BIND_CONSTANT(STEP_TICKS_DEFAULT);
}

String BlipKitInterpreter::_to_string() const {
	return vformat("<BlipKitInterpreter#%d>", get_instance_id());
}
