#include "blipkit_assembler.hpp"
#include <BlipKit.h>
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace detomon::BlipKit;
using namespace godot;

BlipKitAssembler::BlipKitAssembler() {
	bytes.instantiate();
}

void BlipKitAssembler::_bind_methods() {
	ClassDB::bind_method(D_METHOD("put", "instruction", "arg_1", "arg_2", "arg_3"), &BlipKitAssembler::put, DEFVAL(nullptr), DEFVAL(nullptr), DEFVAL(nullptr));
	ClassDB::bind_method(D_METHOD("put_label", "label"), &BlipKitAssembler::put_label);
	ClassDB::bind_method(D_METHOD("compile"), &BlipKitAssembler::compile);
	ClassDB::bind_method(D_METHOD("get_byte_code"), &BlipKitAssembler::get_byte_code);
	ClassDB::bind_method(D_METHOD("get_error_string"), &BlipKitAssembler::get_error_string);
	ClassDB::bind_method(D_METHOD("clear"), &BlipKitAssembler::clear);

	BIND_ENUM_CONSTANT(INSTR_NOTE);
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

	BIND_ENUM_CONSTANT(OK);
	BIND_ENUM_CONSTANT(ERR_INVALID_INSTRUCTION);
	BIND_ENUM_CONSTANT(ERR_INVALID_ARGUMENT);
	BIND_ENUM_CONSTANT(ERR_DUPLICATE_LABEL);
	BIND_ENUM_CONSTANT(ERR_UNDEFINED_LABEL);
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

BlipKitAssembler::Error BlipKitAssembler::put_cmd(Instruction p_instr, const Command &p_cmd) {
	ERR_FAIL_INDEX_V(p_instr, INSTR_MAX, ERR_INVALID_INSTRUCTION);

	switch (p_instr) {
		case INSTR_NOP: {
			// Do nothing.
		} break;
		case INSTR_NOTE:
		case INSTR_VOLUME:
		case INSTR_MASTER_VOLUME:
		case INSTR_PANNING:
		case INSTR_PITCH: {
			ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::FLOAT, Variant::NIL, Variant::NIL), ERR_INVALID_ARGUMENT);

			bytes->put_8(p_instr);
			bytes->put_float(p_cmd.args[0]);
		} break;
		case INSTR_WAVEFORM:
		case INSTR_CUSTOM_WAVEFORM:
		case INSTR_DUTY_CYCLE:
		case INSTR_PHASE_WRAP:
		case INSTR_INSTRUMENT: {
			ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::INT, Variant::NIL, Variant::NIL), ERR_INVALID_ARGUMENT);

			bytes->put_8(p_instr);
			bytes->put_u8(p_cmd.args[0]);
		} break;
		case INSTR_EFFECT_DIVIDER:
		case INSTR_VOLUME_SLIDE:
		case INSTR_PANNING_SLIDE:
		case INSTR_PORTAMENTO:
		case INSTR_ARPEGGIO_DIVIDER:
		case INSTR_INSTRUMENT_DIVIDER:
		case INSTR_WAIT: {
			ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::INT, Variant::NIL, Variant::NIL), ERR_INVALID_ARGUMENT);

			bytes->put_8(p_instr);
			bytes->put_u32(p_cmd.args[0]);
		} break;
		case INSTR_TREMOLO:
		case INSTR_VIBRATO: {
			ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::INT, Variant::FLOAT, Variant::INT), ERR_INVALID_ARGUMENT);

			bytes->put_8(p_instr);
			bytes->put_u32(p_cmd.args[0]);
			bytes->put_float(p_cmd.args[1]);
			bytes->put_u32(p_cmd.args[2]);
		} break;
		case INSTR_ARPEGGIO: {
			ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::PACKED_FLOAT32_ARRAY, Variant::NIL, Variant::NIL), ERR_INVALID_ARGUMENT);

			const PackedFloat32Array &deltas = p_cmd.args[0];
			int count = MIN(deltas.size(), 8);

			bytes->put_8(p_instr);
			bytes->put_u8(count);
			for (int i = 0; i < count; i++) {
				int delta = deltas[i];
				bytes->put_float(delta);
			}
		} break;
		case INSTR_CALL:
		case INSTR_JUMP: {
			ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::STRING, Variant::NIL, Variant::NIL), ERR_INVALID_ARGUMENT);

			const String &label = p_cmd.args[0];
			int label_index = add_label(label);
			int byte_offset = bytes->get_position();

			addresses.push_back({ .byte_offset = byte_offset, .label_index = label_index });

			bytes->put_8(p_instr);
			bytes->put_u32(0);
		} break;
		case INSTR_RETURN:
		case INSTR_RESET: {
			// No arguments.
			bytes->put_8(p_instr);
		} break;
		default: {
			int byte_offset = bytes->get_position();
			error_string = vformat("Invalid instruction %s at byte offset %d.", p_instr, byte_offset);
			ERR_FAIL_V_MSG(ERR_INVALID_INSTRUCTION, error_string);
		} break;
	}

	return OK;
}

BlipKitAssembler::Error BlipKitAssembler::expect_args(const Command &p_cmd, Variant::Type p_type_1, Variant::Type p_type_2, Variant::Type p_type_3) {
	const Variant::Type types[3] = { p_type_1, p_type_2, p_type_3 };

	for (int i = 0; i < 3; i++) {
		if (p_cmd.args[i].get_type() != types[i]) {
			const String &type_name = Variant::get_type_name(types[i]);
			int byte_offset = bytes->get_position();
			error_string = vformat("Expected argument %d to be type %s at byte offset %d.", i + 1, type_name, byte_offset);
			ERR_FAIL_V_MSG(ERR_INVALID_ARGUMENT, error_string);
		}
	}

	return OK;
}

BlipKitAssembler::Error BlipKitAssembler::put(Instruction p_instr, const Variant &p_arg_1, const Variant &p_arg_2, const Variant &p_arg_3) {
	return put_cmd(p_instr, { .args = { p_arg_1, p_arg_2, p_arg_3 } });
}

BlipKitAssembler::Error BlipKitAssembler::put_label(String p_label) {
	int label_index = add_label(p_label);
	Label &label = labels[label_index];

	if (label.byte_offset >= 0) {
		int byte_offset = bytes->get_position();
		error_string = vformat("Label '%s' is already defined at byte offset %d.", p_label, byte_offset);
		ERR_FAIL_V_MSG(ERR_DUPLICATE_LABEL, error_string);
	}

	label.byte_offset = bytes->get_position();

	return OK;
}

BlipKitAssembler::Error BlipKitAssembler::compile() {
	// Resolve label addresses.
	for (Address &address : addresses) {
		int label_index = address.label_index;
		const Label &label = labels[label_index];

		if (label.byte_offset < 0) {
			int address_offset = address.byte_offset;
			error_string = vformat("Label '%s' not defined at byte offset %d.", label.name, address_offset);
			ERR_FAIL_V_MSG(ERR_UNDEFINED_LABEL, error_string);
		}

		bytes->seek(address.byte_offset);
		bytes->put_32(label.byte_offset);
	}

	addresses.clear();

	return OK;
}

PackedByteArray BlipKitAssembler::get_byte_code() const {
	return bytes->get_data_array();
}

String BlipKitAssembler::get_error_string() const {
	return error_string;
}

void BlipKitAssembler::clear() {
	bytes->clear();
	label_indices.clear();
	labels.clear();
	addresses.clear();
	error_string.resize(0);
}
