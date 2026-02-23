#include "blipkit_assembler.hpp"
#include "blipkit_bytecode.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_interpreter.hpp"
#include "blipkit_sample.hpp"
#include "blipkit_waveform.hpp"
#include "godot_cpp/core/math.hpp"
#include <BlipKit.h>
#include <godot_cpp/variant/char_string.hpp>

using namespace BlipKit;
using namespace godot;

static constexpr int INITIAL_SPACE = 4096;
static constexpr int LABEL_MAX_SIZE = 255;

BlipKitAssembler::BlipKitAssembler() {
	clear();
}

void BlipKitAssembler::write_header() {
	const uint8_t *header_ptr = reinterpret_cast<const uint8_t *>(&BlipKitBytecode::binary_header);

	byte_code.put_bytes(header_ptr, sizeof(BlipKitBytecode::binary_header));
	code_section_offset = byte_code.get_position();
}

void BlipKitAssembler::write_sections() {
	write_labels();
}

void BlipKitAssembler::write_labels() {
	uint32_t label_count = 0;

	for (const Label &label : labels) {
		if (label.is_public) {
			label_count++;
		}
	}

	if (not label_count) {
		return;
	}

	const uint8_t magic[4] = { 'l', 'a', 'b', 'l' };
	byte_code.put_bytes(magic, 4);

	// Prepare section size.
	const uint32_t section_size_position = byte_code.get_position();
	byte_code.put_u32(0);

	// Set label count.
	const uint32_t label_count_position = byte_code.get_position();
	byte_code.put_u32(label_count);

	for (const KeyValue<String, uint32_t> &label_index : label_indices) {
		const Label &label = labels[label_index.value];

		if (not label.is_public) {
			continue;
		}

		const CharString &chars = label.name.utf8();
		const uint32_t chars_size = chars.size() - 1; // Remove terminating NUL.

		byte_code.put_u32(label.byte_offset);
		byte_code.put_u8(chars_size);
		byte_code.put_bytes(reinterpret_cast<const uint8_t *>(chars.ptr()), chars_size);
	}

	const uint32_t end_position = byte_code.get_position();

	// Set section size.
	byte_code.seek(section_size_position);
	byte_code.put_u32(end_position - label_count_position);

	byte_code.seek(end_position);
}

bool BlipKitAssembler::check_arg_type(const Variant &p_var, Variant::Type p_type, uint32_t p_index) {
	if (p_var.get_type() != p_type) {
		fail_argument_type(p_type, p_index);
		return false;
	}

	return true;
}

bool BlipKitAssembler::check_arg_types(const Args &p_args, const Types &p_types) {
	for (uint32_t i = 0; i < Args::COUNT_MAX; i++) {
		if (p_args.args[i].get_type() != p_types.types[i]) {
			fail_argument_type(p_types.types[i], i + 1);
			return false;
		}
	}

	return true;
}

bool BlipKitAssembler::check_args_number_nil_nil(const Args &p_args) {
	const Variant::Type arg1_type = p_args.args[0].get_type();

	if (arg1_type != Variant::INT && arg1_type != Variant::FLOAT) [[unlikely]] {
		fail_with_error(vformat("Expected instruction argument %d to be of type int or float.", 1));
		return false;
	}

	if (not check_arg_type(p_args.args[1], Variant::NIL, 2)) [[unlikely]] {
		return false;
	}

	if (not check_arg_type(p_args.args[2], Variant::NIL, 3)) [[unlikely]] {
		return false;
	}

	return true;
}

void BlipKitAssembler::fail_with_error(const String &p_error_message) {
	state = STATE_FAILED;
	error_message = p_error_message;

	ERR_FAIL_MSG(error_message);
}

void BlipKitAssembler::fail_argument_type(Variant::Type p_type, uint32_t p_index) {
	const String &type_name = Variant::get_type_name(p_type);
	fail_with_error(vformat("Expected instruction argument %d to be of type %s.", p_index, type_name));
}

BlipKitAssembler::Error BlipKitAssembler::put(Opcode p_opcode, const Variant &p_arg1, const Variant &p_arg2, const Variant &p_arg3) {
	ERR_FAIL_INDEX_V(p_opcode, OP_MAX, ERR_INVALID_OPCODE);
	ERR_FAIL_COND_V(state != STATE_ASSEMBLE, ERR_INVALID_STATE);

	const Args args = { p_arg1, p_arg2, p_arg3 };

	switch (p_opcode) {
		case OP_ATTACK:
		case OP_PITCH:
		case OP_SAMPLE_PITCH:
		case OP_VOLUME_SLIDE:
		case OP_PANNING_SLIDE:
		case OP_PORTAMENTO: {
			if (not check_args_number_nil_nil(args)) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			byte_code.put_u8(p_opcode);
			byte_code.put_f16(p_arg1);
		} break;
		case OP_VOLUME:
		case OP_MASTER_VOLUME:
		case OP_PANNING: {
			if (not check_arg_types(args, { Variant::FLOAT, Variant::NIL, Variant::NIL })) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			byte_code.put_u8(p_opcode);
			byte_code.put_f16(p_arg1);
		} break;
		case OP_WAVEFORM:
		case OP_CUSTOM_WAVEFORM:
		case OP_SAMPLE:
		case OP_DUTY_CYCLE:
		case OP_PHASE_WRAP:
		case OP_INSTRUMENT: {
			if (not check_arg_types(args, { Variant::INT, Variant::NIL, Variant::NIL })) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			byte_code.put_u8(p_opcode);
			byte_code.put_u8(p_arg1);
		} break;
		case OP_EFFECT_DIV:
		case OP_ARPEGGIO_DIV:
		case OP_INSTRUMENT_DIV: {
			if (not check_arg_types(args, { Variant::INT, Variant::NIL, Variant::NIL })) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			const int32_t value = CLAMP(int32_t(p_arg1), 0, UINT16_MAX);

			byte_code.put_u8(p_opcode);
			byte_code.put_u16(value);
		} break;
		case OP_TICK:
		case OP_STEP: {
			if (not check_arg_types(args, { Variant::INT, Variant::NIL, Variant::NIL })) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			const int32_t value = CLAMP(int32_t(p_arg1), 0, UINT16_MAX);

			if (value) [[likely]] {
				byte_code.put_u8(p_opcode);
				byte_code.put_u16(value);
			}
		} break;
		case OP_STEP_TICKS: {
			if (not check_arg_types(args, { Variant::INT, Variant::NIL, Variant::NIL })) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			const int32_t ticks = CLAMP(int32_t(p_arg1), 1, UINT16_MAX);

			byte_code.put_u8(p_opcode);
			byte_code.put_u16(ticks);
		} break;
		case OP_TREMOLO:
		case OP_VIBRATO: {
			if (not check_arg_types(args, { Variant::FLOAT, Variant::FLOAT, Variant::FLOAT })) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			const float ticks = CLAMP(float(p_arg1), 0, UINT16_MAX);
			const float delta = CLAMP(float(p_arg2), -float(BK_MAX_NOTE), +float(BK_MAX_NOTE));
			const float slide_ticks = CLAMP(float(p_arg3), 0, UINT16_MAX);

			byte_code.put_u8(p_opcode);
			byte_code.put_f16(ticks);
			byte_code.put_f16(delta);
			byte_code.put_f16(slide_ticks);
		} break;
		case OP_ARPEGGIO: {
			if (not check_arg_types(args, { Variant::PACKED_FLOAT32_ARRAY, Variant::NIL, Variant::NIL })) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			const PackedFloat32Array &values = p_arg1;
			const float *values_ptr = values.ptr();
			const uint32_t count = MIN(values.size(), BK_MAX_ARPEGGIO);

			byte_code.put_u8(p_opcode);
			byte_code.put_u8(count);
			for (uint32_t i = 0; i < count; i++) {
				const float delta = CLAMP(values_ptr[i], -float(BK_MAX_NOTE), +float(BK_MAX_NOTE));
				byte_code.put_f16(delta);
			}
		} break;
		case OP_CALL:
		case OP_JUMP: {
			if (not check_arg_types(args, { Variant::STRING, Variant::NIL, Variant::NIL })) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			const String &label = p_arg1;
			uint32_t label_index = 0;
			const Error error = get_or_add_label(label, label_index);

			if (error != OK) {
				return error;
			}

			byte_code.put_u8(p_opcode);

			const int32_t byte_offset = byte_code.get_position() - code_section_offset;
			addresses.push_back({ .label_index = uint32_t(label_index), .byte_offset = byte_offset });

			// Placeholder address.
			byte_code.put_s32(0);
		} break;
		case OP_RELEASE:
		case OP_MUTE:
		case OP_RETURN:
		case OP_RESET: {
			if (not check_arg_types(args, { Variant::NIL, Variant::NIL, Variant::NIL })) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			byte_code.put_u8(p_opcode);
		} break;
		case Opcode::OP_DELAY_TICK: {
			if (not check_arg_types(args, { Variant::INT, Variant::NIL, Variant::NIL })) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			const uint32_t ticks = CLAMP(int32_t(p_arg1), 0, UINT16_MAX);

			if (ticks) [[likely]] {
				byte_code.put_u8(p_opcode);
				byte_code.put_u16(ticks);
			}
		} break;

		case Opcode::OP_DELAY_STEP: {
			if (not check_args_number_nil_nil(args)) [[unlikely]] {
				return ERR_INVALID_ARGUMENT;
			}

			const float steps = CLAMP(float(p_arg1), 0, UINT16_MAX);

			if (not Math::is_zero_approx(steps)) [[likely]] {
				byte_code.put_u8(p_opcode);
				byte_code.put_f16(steps);
			}
		} break;
		default: {
			fail_with_error(vformat("Invalid opcode %d.", p_opcode));
			return ERR_INVALID_OPCODE;
		} break;
	}

	return OK;
}

BlipKitAssembler::Error BlipKitAssembler::put_byte_code(const Ref<BlipKitBytecode> &p_byte_code, bool p_public) {
	ERR_FAIL_COND_V(state != STATE_ASSEMBLE, ERR_INVALID_STATE);
	ERR_FAIL_COND_V(p_byte_code.is_null(), ERR_INVALID_ARGUMENT);
	ERR_FAIL_COND_V_MSG(not p_byte_code->is_valid(), ERR_INVALID_ARGUMENT, p_byte_code->get_error_message());

	const uint32_t label_count = p_byte_code->get_label_count();
	const int32_t code_offset = byte_code.get_position();

	for (uint32_t i = 0; i < label_count; i++) {
		const String &label_name = p_byte_code->get_label_name(i);
		const int32_t label_offset = code_offset + p_byte_code->get_label_position(i);
		const Error error = put_label(label_name, label_offset, p_public);

		if (error != OK) {
			return error;
		}
	}

	// Append code section.
	const uint32_t code_section_offset = p_byte_code->get_code_section_offset();
	const uint32_t code_section_size = p_byte_code->get_code_section_size() - sizeof(uint8_t); // Without OP_HALT.
	const Vector<uint8_t> &bytes = p_byte_code->get_bytes();
	byte_code.put_bytes(bytes, code_section_offset, code_section_size);

	return OK;
}

BlipKitAssembler::Error BlipKitAssembler::get_or_add_label(const String p_label, uint32_t &r_label_index) {
	const HashMap<String, uint32_t>::Iterator label_itor = label_indices.find(p_label);

	// Label already exists.
	if (label_itor != label_indices.end()) {
		r_label_index = label_itor->value;
		return OK;
	}

	if (p_label.is_empty()) {
		fail_with_error("Label cannot be empty.");
		return ERR_INVALID_LABEL;
	}

	if (p_label.utf8().size() > LABEL_MAX_SIZE + 1) { // Includes terminating NUL.
		fail_with_error(vformat("Label '%s' is longer than %d bytes.", p_label, LABEL_MAX_SIZE));
		return ERR_INVALID_LABEL;
	}

	const uint32_t label_index = label_indices.size();

	if (label_index >= UINT16_MAX) {
		fail_with_error("Too many labels.");
		return ERR_INVALID_LABEL;
	}

	label_indices[p_label] = label_index;
	labels.push_back({ .name = p_label });
	r_label_index = label_index;

	return OK;
}

BlipKitAssembler::Error BlipKitAssembler::put_label(String p_label, int32_t p_label_position, bool p_public) {
	ERR_FAIL_COND_V(state != STATE_ASSEMBLE, ERR_INVALID_STATE);

	uint32_t label_index = 0;
	const Error error = get_or_add_label(p_label, label_index);

	if (error != OK) {
		return error;
	}

	Label &label = labels[label_index];

	if (label.byte_offset >= 0) {
		fail_with_error(vformat("Label '%s' is already defined.", p_label));
		return ERR_DUPLICATE_LABEL;
	}

	label.is_public = p_public;
	label.byte_offset = p_label_position - code_section_offset;

	return OK;
}

BlipKitAssembler::Error BlipKitAssembler::put_label_bind(const String p_label, bool p_public) {
	const uint32_t label_position = byte_code.get_position();

	return put_label(p_label, label_position, p_public);
}

BlipKitAssembler::Error BlipKitAssembler::compile() {
	ERR_FAIL_COND_V(state != STATE_ASSEMBLE, ERR_INVALID_STATE);

	// Terminate code.
	byte_code.put_u8(OP_HALT);

	// Save byte position.
	const uint32_t byte_position = byte_code.get_position();

	// Write size of code segment.
	byte_code.seek(offsetof(BlipKitBytecode::Header, bytecode_size));
	byte_code.put_u32(byte_position - sizeof(BlipKitBytecode::Header));

	// Restore byte position.
	byte_code.seek(byte_position);

	// Resolve label addresses.
	for (Address &address : addresses) {
		const uint32_t label_index = address.label_index;
		const int32_t address_offset = code_section_offset + address.byte_offset;
		const Label &label = labels[label_index];

		if (label.byte_offset < 0) {
			fail_with_error(vformat("Label '%s' not defined at address offset %d.", label.name, address_offset));
			return ERR_UNDEFINED_LABEL;
		}

		const int32_t jump_offset = label.byte_offset - address.byte_offset; // Jump relative to byte code position.
		byte_code.seek(address_offset);
		byte_code.put_s32(jump_offset);
	}

	addresses.clear();

	// Restore byte position.
	byte_code.seek(byte_position);

	write_sections();

	state = STATE_COMPILED;

	return OK;
}

Vector<uint8_t> BlipKitAssembler::get_bytes() const {
	return byte_code.get_bytes();
}

Ref<BlipKitBytecode> BlipKitAssembler::get_byte_code() {
	ERR_FAIL_COND_V_MSG(state != STATE_COMPILED, nullptr, "Byte code is not compiled.");

	if (compiled_byte_code.is_valid()) {
		return compiled_byte_code;
	}

	compiled_byte_code.instantiate();
	compiled_byte_code->set_bytes(get_bytes());

	if (not compiled_byte_code->is_valid()) {
		compiled_byte_code.unref();
		return nullptr;
	}

	return compiled_byte_code;
}

String BlipKitAssembler::get_error_message() const {
	return error_message;
}

void BlipKitAssembler::clear() {
	byte_code.clear();
	byte_code.reserve(INITIAL_SPACE);

	label_indices.clear();
	labels.clear();
	addresses.clear();
	compiled_byte_code.unref();
	error_message.resize(0);
	state = STATE_ASSEMBLE;
	code_section_offset = 0;

	write_header();
}

void BlipKitAssembler::_bind_methods() {
	ClassDB::bind_method(D_METHOD("put", "opcode", "arg1", "arg2", "arg3"), &BlipKitAssembler::put, DEFVAL(nullptr), DEFVAL(nullptr), DEFVAL(nullptr));
	ClassDB::bind_method(D_METHOD("put_byte_code", "byte_code", "public"), &BlipKitAssembler::put_byte_code, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("put_label", "label", "public"), &BlipKitAssembler::put_label_bind, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("compile"), &BlipKitAssembler::compile);
	ClassDB::bind_method(D_METHOD("get_byte_code"), &BlipKitAssembler::get_byte_code);
	ClassDB::bind_method(D_METHOD("get_error_message"), &BlipKitAssembler::get_error_message);
	ClassDB::bind_method(D_METHOD("clear"), &BlipKitAssembler::clear);

	BIND_ENUM_CONSTANT(OP_ATTACK);
	BIND_ENUM_CONSTANT(OP_RELEASE);
	BIND_ENUM_CONSTANT(OP_MUTE);
	BIND_ENUM_CONSTANT(OP_VOLUME);
	BIND_ENUM_CONSTANT(OP_MASTER_VOLUME);
	BIND_ENUM_CONSTANT(OP_PANNING);
	BIND_ENUM_CONSTANT(OP_WAVEFORM);
	BIND_ENUM_CONSTANT(OP_DUTY_CYCLE);
	BIND_ENUM_CONSTANT(OP_PITCH);
	BIND_ENUM_CONSTANT(OP_PHASE_WRAP);
	BIND_ENUM_CONSTANT(OP_PORTAMENTO);
	BIND_ENUM_CONSTANT(OP_VIBRATO);
	BIND_ENUM_CONSTANT(OP_TREMOLO);
	BIND_ENUM_CONSTANT(OP_VOLUME_SLIDE);
	BIND_ENUM_CONSTANT(OP_PANNING_SLIDE);
	BIND_ENUM_CONSTANT(OP_EFFECT_DIV);
	BIND_ENUM_CONSTANT(OP_ARPEGGIO);
	BIND_ENUM_CONSTANT(OP_ARPEGGIO_DIV);
	BIND_ENUM_CONSTANT(OP_TICK);
	BIND_ENUM_CONSTANT(OP_STEP);
	BIND_ENUM_CONSTANT(OP_STEP_TICKS);
	BIND_ENUM_CONSTANT(OP_DELAY_TICK);
	BIND_ENUM_CONSTANT(OP_DELAY_STEP);
	BIND_ENUM_CONSTANT(OP_JUMP);
	BIND_ENUM_CONSTANT(OP_CALL);
	BIND_ENUM_CONSTANT(OP_RETURN);
	BIND_ENUM_CONSTANT(OP_RESET);
	BIND_ENUM_CONSTANT(OP_INSTRUMENT);
	BIND_ENUM_CONSTANT(OP_INSTRUMENT_DIV);
	BIND_ENUM_CONSTANT(OP_CUSTOM_WAVEFORM);
	BIND_ENUM_CONSTANT(OP_SAMPLE);
	BIND_ENUM_CONSTANT(OP_SAMPLE_PITCH);

	BIND_ENUM_CONSTANT(OK);
	BIND_ENUM_CONSTANT(ERR_INVALID_STATE);
	BIND_ENUM_CONSTANT(ERR_INVALID_OPCODE);
	BIND_ENUM_CONSTANT(ERR_INVALID_ARGUMENT);
	BIND_ENUM_CONSTANT(ERR_DUPLICATE_LABEL);
	BIND_ENUM_CONSTANT(ERR_UNDEFINED_LABEL);
	BIND_ENUM_CONSTANT(ERR_INVALID_LABEL);
}

String BlipKitAssembler::_to_string() const {
	return vformat("<BlipKitAssembler#%d>", get_instance_id());
}
