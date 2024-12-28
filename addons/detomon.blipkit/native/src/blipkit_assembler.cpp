#include "blipkit_assembler.hpp"
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace detomon::BlipKit;
using namespace godot;

void BlipKitAssembler::_bind_methods() {
	ClassDB::bind_method(D_METHOD("put_0", "instruction"), &BlipKitAssembler::put_0);
	ClassDB::bind_method(D_METHOD("put_1", "instruction", "arg_1"), &BlipKitAssembler::put_1);
	ClassDB::bind_method(D_METHOD("put_2", "instruction", "arg_1", "arg_2"), &BlipKitAssembler::put_2);
	ClassDB::bind_method(D_METHOD("put_3", "instruction", "arg_1", "arg_2", "arg_3"), &BlipKitAssembler::put_3);
	ClassDB::bind_method(D_METHOD("put_label", "label"), &BlipKitAssembler::put_label);
	ClassDB::bind_method(D_METHOD("get_offset"), &BlipKitAssembler::get_offset);
	ClassDB::bind_method(D_METHOD("compile"), &BlipKitAssembler::compile);
	ClassDB::bind_method(D_METHOD("get_compile_error"), &BlipKitAssembler::get_compile_error);

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
	BIND_ENUM_CONSTANT(INSTR_MAX);
}

String BlipKitAssembler::_to_string() const {
	return "BlipKitAssembler";
}

BlipKitAssembler::BlipKitAssembler() {
}

BlipKitAssembler::~BlipKitAssembler() {
}

int BlipKitAssembler::add_label(const String p_label) {
	if (label_indices.has(p_label)) {
		return label_indices[p_label];
	}

	const int index = label_indices.size();
	label_indices[p_label] = index;
	labels.push_back({
			.name = p_label,
	});

	return index;
}

bool BlipKitAssembler::put(Instruction p_instr, const Command &p_cmd) {
	ERR_FAIL_INDEX_V(p_instr, INSTR_MAX, false);

	bytes.put_8(p_instr);

	switch (p_instr) {
	INSTR_NOTE:
	INSTR_VOLUME:
	INSTR_MASTER_VOLUME:
	INSTR_PANNING:
	INSTR_PITCH: {
		ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::FLOAT, Variant::NIL, Variant::NIL), false);
		bytes.put_float(p_cmd.args[0]);
	} break;
	INSTR_WAVEFORM:
	INSTR_CUSTOM_WAVEFORM:
	INSTR_DUTY_CYCLE:
	INSTR_PHASE_WRAP:
	INSTR_INSTRUMENT: {
		ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::INT, Variant::NIL, Variant::NIL), false);
		bytes.put_u8(p_cmd.args[0]);
	} break;
	INSTR_EFFECT_DIVIDER:
	INSTR_VOLUME_SLIDE:
	INSTR_PANNING_SLIDE:
	INSTR_PORTAMENTO:
	INSTR_ARPEGGIO_DIVIDER:
	INSTR_INSTRUMENT_DIVIDER:
	INSTR_WAIT: {
		ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::INT, Variant::NIL, Variant::NIL), false);
		bytes.put_u32(p_cmd.args[0]);
	} break;
	INSTR_TREMOLO:
	INSTR_VIBRATO: {
		ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::INT, Variant::FLOAT, Variant::INT), false);

		bytes.put_u32(p_cmd.args[0]);
		bytes.put_float(p_cmd.args[1]);
		bytes.put_u32(p_cmd.args[2]);
	} break;
	INSTR_ARPEGGIO: {
		ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::PACKED_FLOAT32_ARRAY, Variant::NIL, Variant::NIL), false);

		const PackedFloat32Array &deltas = p_cmd.args[0];
		int count = MIN(deltas.size(), 8);

		bytes.put_u8(count);
		for (int i = 0; i < count; i++) {
			int delta = deltas[i];
			bytes.put_float(delta);
		}
	} break;
	INSTR_CALL:
	INSTR_JUMP: {
		ERR_FAIL_COND_V(!expect_args(p_cmd, Variant::STRING, Variant::NIL, Variant::NIL), false);

		const String &label = p_cmd.args[0];
		int label_index = add_label(label);

		addresses.push_back({
				.byte_offset = bytes.get_position(),
				.label_index = label_index,
		});
		bytes.put_u32(0);
	} break;
	INSTR_RETURN:
	INSTR_RESET: {
		// No arguments.
	} break;
	default: {
		ERR_FAIL_V_MSG(false, vformat("Invalid instruction %s.", p_instr));
	} break;
	}

	return true;
}

bool BlipKitAssembler::expect_args(const Command &p_cmd, Variant::Type p_type_1, Variant::Type p_type_2, Variant::Type p_type_3) {
	const Variant::Type types[3] = { p_type_1, p_type_2, p_type_3 };

	for (int i = 0; i < 3; i++) {
		if (p_cmd.args[i].get_type() != types[i]) {
			ERR_FAIL_V_MSG(false, vformat("Expected argument %d to be type %s.", i, Variant::get_type_name(types[i])));
		}
	}

	return true;
}

void BlipKitAssembler::put_0(Instruction p_instr) {
	put(p_instr, { .args = { nullptr, nullptr, nullptr } });
}

void BlipKitAssembler::put_1(Instruction p_instr, const Variant p_arg_1) {
	put(p_instr, { .args = { p_arg_1, nullptr, nullptr } });
}

void BlipKitAssembler::put_2(Instruction p_instr, const Variant p_arg_1, const Variant p_arg_2) {
	put(p_instr, { .args = { p_arg_1, p_arg_2, nullptr } });
}

void BlipKitAssembler::put_3(Instruction p_instr, const Variant p_arg_1, const Variant p_arg_2, const Variant p_arg_3) {
	put(p_instr, { .args = { p_arg_1, p_arg_2, p_arg_3 } });
}

bool BlipKitAssembler::put_label(String p_label) {
	add_label(p_label);

	int label_index = label_indices[p_label];
	Label &label = labels[label_index];

	if (label.byte_offset >= 0) {
		ERR_FAIL_V_MSG(false, vformat("Label '%s' is already defined.", p_label));
	}

	label.byte_offset = bytes.get_position();

	return true;
}

int BlipKitAssembler::get_offset() const {
	return bytes.get_position();
}

PackedByteArray BlipKitAssembler::compile() {
	for (Address &address : addresses) {
		int label_index = address.label_index;
		const Label &label = labels[label_index];

		if (label.byte_offset < 0) {
			ERR_FAIL_V_MSG(PackedByteArray(), vformat("Label '%s' not defined.", label.name));
		}

		bytes.seek(address.byte_offset);
		bytes.put_32(label.byte_offset);
	}

	PackedByteArray data = bytes.get_data_array();

	bytes.clear();
	label_indices.clear();
	labels.clear();
	addresses.clear();
	compile_error.resize(0);

	return data;
}

String BlipKitAssembler::get_compile_error() const {
	return compile_error;
}
