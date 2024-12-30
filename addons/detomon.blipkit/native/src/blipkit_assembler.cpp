#include "blipkit_assembler.hpp"
#include <BlipKit.h>
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace detomon::BlipKit;
using namespace godot;

BlipKitAssembler::BlipKitAssembler() {
	byte_code.instantiate();
}

void BlipKitAssembler::_bind_methods() {
	ClassDB::bind_method(D_METHOD("put", "instruction", "arg_1", "arg_2", "arg_3"), &BlipKitAssembler::put, DEFVAL(nullptr), DEFVAL(nullptr), DEFVAL(nullptr));
	ClassDB::bind_method(D_METHOD("put_code", "code"), &BlipKitAssembler::put_code);
	ClassDB::bind_method(D_METHOD("put_label", "label"), &BlipKitAssembler::put_label);
	ClassDB::bind_method(D_METHOD("compile"), &BlipKitAssembler::compile);
	ClassDB::bind_method(D_METHOD("get_byte_code"), &BlipKitAssembler::get_byte_code);
	ClassDB::bind_method(D_METHOD("get_error_message"), &BlipKitAssembler::get_error_message);
	ClassDB::bind_method(D_METHOD("clear"), &BlipKitAssembler::clear);

	BIND_ENUM_CONSTANT(INSTR_ATTACK);
	BIND_ENUM_CONSTANT(INSTR_RELEASE);
	BIND_ENUM_CONSTANT(INSTR_MUTE);
	BIND_ENUM_CONSTANT(INSTR_WAVEFORM);
	BIND_ENUM_CONSTANT(INSTR_CUSTOM_WAVEFORM);
	BIND_ENUM_CONSTANT(INSTR_DUTY_CYCLE);
	BIND_ENUM_CONSTANT(INSTR_EFFECT_DIVIDER);
	BIND_ENUM_CONSTANT(INSTR_VOLUME);
	BIND_ENUM_CONSTANT(INSTR_VOLUME_SLIDE);
	BIND_ENUM_CONSTANT(INSTR_MASTER_VOLUME);
	BIND_ENUM_CONSTANT(INSTR_PANNING);
	BIND_ENUM_CONSTANT(INSTR_PANNING_SLIDE);
	BIND_ENUM_CONSTANT(INSTR_PITCH);
	BIND_ENUM_CONSTANT(INSTR_PHASE_WRAP);
	BIND_ENUM_CONSTANT(INSTR_PORTAMENTO);
	BIND_ENUM_CONSTANT(INSTR_VIBRATO);
	BIND_ENUM_CONSTANT(INSTR_TREMOLO);
	BIND_ENUM_CONSTANT(INSTR_ARPEGGIO);
	BIND_ENUM_CONSTANT(INSTR_ARPEGGIO_DIVIDER);
	BIND_ENUM_CONSTANT(INSTR_INSTRUMENT);
	BIND_ENUM_CONSTANT(INSTR_INSTRUMENT_DIVIDER);
	BIND_ENUM_CONSTANT(INSTR_WAIT);
	BIND_ENUM_CONSTANT(INSTR_CALL);
	BIND_ENUM_CONSTANT(INSTR_RETURN);
	BIND_ENUM_CONSTANT(INSTR_JUMP);
	BIND_ENUM_CONSTANT(INSTR_RESET);
	BIND_ENUM_CONSTANT(INSTR_STORE);

	BIND_ENUM_CONSTANT(OK);
	BIND_ENUM_CONSTANT(ERR_INVALID_INSTRUCTION);
	BIND_ENUM_CONSTANT(ERR_INVALID_ARGUMENT);
	BIND_ENUM_CONSTANT(ERR_DUPLICATE_LABEL);
	BIND_ENUM_CONSTANT(ERR_UNDEFINED_LABEL);
	BIND_ENUM_CONSTANT(ERR_PARSER_ERROR);
}

String BlipKitAssembler::_to_string() const {
	return vformat("BlipKitAssembler");
}

int BlipKitAssembler::add_label(const String p_label) {
	if (label_indices.has(p_label)) {
		return label_indices[p_label];
	}

	const int index = label_indices.size();
	label_indices[p_label] = index;
	labels.push_back({ .name = p_label });

	return index;
}

BlipKitAssembler::Error BlipKitAssembler::put_instruction(Instruction p_instr, const Args &p_args) {
	ERR_FAIL_INDEX_V(p_instr, INSTR_MAX, ERR_INVALID_INSTRUCTION);

	switch (p_instr) {
		case INSTR_NOP: {
			ERR_FAIL_COND_V(check_args(p_args, Variant::NIL, Variant::NIL, Variant::NIL) != OK, ERR_INVALID_ARGUMENT);

			// Do nothing.
		} break;
		case INSTR_ATTACK:
		case INSTR_VOLUME:
		case INSTR_MASTER_VOLUME:
		case INSTR_PANNING:
		case INSTR_PITCH: {
			ERR_FAIL_COND_V(check_args(p_args, Variant::FLOAT, Variant::NIL, Variant::NIL) != OK, ERR_INVALID_ARGUMENT);

			byte_code->put_u8(p_instr);
			put_half(p_args.args[0]);
		} break;
		case INSTR_WAVEFORM:
		case INSTR_CUSTOM_WAVEFORM:
		case INSTR_DUTY_CYCLE:
		case INSTR_PHASE_WRAP:
		case INSTR_INSTRUMENT: {
			ERR_FAIL_COND_V(check_args(p_args, Variant::INT, Variant::NIL, Variant::NIL) != OK, ERR_INVALID_ARGUMENT);

			byte_code->put_u8(p_instr);
			byte_code->put_u8(p_args.args[0]);
		} break;
		case INSTR_EFFECT_DIVIDER:
		case INSTR_VOLUME_SLIDE:
		case INSTR_PANNING_SLIDE:
		case INSTR_PORTAMENTO:
		case INSTR_ARPEGGIO_DIVIDER:
		case INSTR_INSTRUMENT_DIVIDER:
		case INSTR_WAIT: {
			ERR_FAIL_COND_V(check_args(p_args, Variant::INT, Variant::NIL, Variant::NIL) != OK, ERR_INVALID_ARGUMENT);

			uint32_t value = CLAMP(int(p_args.args[0]), 0, UINT16_MAX);
			byte_code->put_u8(p_instr);
			byte_code->put_u16(value);
		} break;
		case INSTR_TREMOLO:
		case INSTR_VIBRATO: {
			ERR_FAIL_COND_V(check_args(p_args, Variant::INT, Variant::FLOAT, Variant::INT) != OK, ERR_INVALID_ARGUMENT);

			uint32_t ticks = CLAMP(int(p_args.args[0]), 0, UINT16_MAX);
			uint32_t slide_ticks = CLAMP(int(p_args.args[2]), 0, UINT16_MAX);

			byte_code->put_u8(p_instr);
			byte_code->put_u16(ticks);
			put_half(p_args.args[1]);
			byte_code->put_u16(slide_ticks);
		} break;
		case INSTR_ARPEGGIO: {
			ERR_FAIL_COND_V(check_args(p_args, Variant::PACKED_FLOAT32_ARRAY, Variant::NIL, Variant::NIL) != OK, ERR_INVALID_ARGUMENT);

			const PackedFloat32Array &deltas = p_args.args[0];
			const real_t *deltas_ptr = deltas.ptr();
			int count = MIN(deltas.size(), 8);

			byte_code->put_u8(p_instr);
			byte_code->put_u8(count);
			for (int i = 0; i < count; i++) {
				int delta = deltas_ptr[i];
				put_half(delta);
			}
		} break;
		case INSTR_CALL:
		case INSTR_JUMP: {
			ERR_FAIL_COND_V(check_args(p_args, Variant::STRING, Variant::NIL, Variant::NIL) != OK, ERR_INVALID_ARGUMENT);

			const String &label = p_args.args[0];
			int label_index = add_label(label);

			byte_code->put_u8(p_instr);

			int byte_offset = byte_code->get_position();
			addresses.push_back({ .byte_offset = byte_offset, .label_index = label_index });

			byte_code->put_u32(0);
		} break;
		case INSTR_RELEASE:
		case INSTR_MUTE:
		case INSTR_RETURN:
		case INSTR_RESET: {
			ERR_FAIL_COND_V(check_args(p_args, Variant::NIL, Variant::NIL, Variant::NIL) != OK, ERR_INVALID_ARGUMENT);

			byte_code->put_u8(p_instr);
		} break;
		case INSTR_STORE: {
			ERR_FAIL_COND_V(check_args(p_args, Variant::INT, Variant::INT, Variant::NIL) != OK, ERR_INVALID_ARGUMENT);

			int number = CLAMP(uint32_t(p_args.args[0]), 0, REGISTER_COUNT);

			byte_code->put_u8(p_instr);
			byte_code->put_u8(number);
			byte_code->put_32(p_args.args[1]);
		} break;
		default: {
			int byte_offset = byte_code->get_position();
			error_message = vformat("Invalid instruction %d at byte offset %d.", p_instr, byte_offset);
			ERR_FAIL_V_MSG(ERR_INVALID_INSTRUCTION, error_message);
		} break;
	}

	return OK;
}

BlipKitAssembler::Error BlipKitAssembler::check_args(const Args &p_args, Variant::Type p_type_1, Variant::Type p_type_2, Variant::Type p_type_3) {
	const Variant::Type types[3] = { p_type_1, p_type_2, p_type_3 };

	for (int i = 0; i < 3; i++) {
		if (p_args.args[i].get_type() != types[i]) {
			int byte_offset = byte_code->get_position();
			const String &type_name = Variant::get_type_name(types[i]);
			error_message = vformat("Expected argument %d to be type %s at byte offset %d.", i + 1, type_name, byte_offset);
			ERR_FAIL_V_MSG(ERR_INVALID_ARGUMENT, error_message);
		}
	}

	return OK;
}

BlipKitAssembler::Error BlipKitAssembler::put(Instruction p_instr, const Variant &p_arg_1, const Variant &p_arg_2, const Variant &p_arg_3) {
	return put_instruction(p_instr, { .args = { p_arg_1, p_arg_2, p_arg_3 } });
}

BlipKitAssembler::Error BlipKitAssembler::put_code(const String &p_code) {
	ERR_FAIL_V_MSG(ERR_PARSER_ERROR, "Not implemented.");
}

BlipKitAssembler::Error BlipKitAssembler::put_label(String p_label) {
	int label_index = add_label(p_label);
	Label &label = labels[label_index];

	if (label.byte_offset >= 0) {
		int byte_offset = byte_code->get_position();
		error_message = vformat("Label '%s' is already defined at byte offset %d.", p_label, byte_offset);
		ERR_FAIL_V_MSG(ERR_DUPLICATE_LABEL, error_message);
	}

	label.byte_offset = byte_code->get_position();

	return OK;
}

BlipKitAssembler::Error BlipKitAssembler::compile() {
	// Save byte position.
	int byte_position = byte_code->get_position();

	// Resolve label addresses.
	for (Address &address : addresses) {
		int label_index = address.label_index;
		const Label &label = labels[label_index];

		if (label.byte_offset < 0) {
			int address_offset = address.byte_offset;
			error_message = vformat("Label '%s' not defined at byte offset %d.", label.name, address_offset);
			ERR_FAIL_V_MSG(ERR_UNDEFINED_LABEL, error_message);
		}

		byte_code->seek(address.byte_offset);
		byte_code->put_32(label.byte_offset);
	}

	addresses.clear();
	compiled = true;

	// Restore byte position.
	byte_code->seek(byte_position);

	return OK;
}

PackedByteArray BlipKitAssembler::get_byte_code() const {
	if (!compiled) {
		return PackedByteArray();
	}

	return byte_code->get_data_array();
}

String BlipKitAssembler::get_error_message() const {
	return error_message;
}

void BlipKitAssembler::clear() {
	byte_code->clear();
	label_indices.clear();
	labels.clear();
	addresses.clear();
	error_message.resize(0);
	compiled = false;
}
