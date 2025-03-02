#include "blipkit_assembler.hpp"
#include "blipkit_bytecode.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_interpreter.hpp"
#include "blipkit_track.hpp"
#include "blipkit_waveform.hpp"
#include <BlipKit.h>
#include <godot_cpp/variant/char_string.hpp>

using namespace BlipKit;
using namespace godot;

static constexpr int INITIAL_SPACE = 256;
static constexpr int ARGS_COUNT_MAX = 3;

struct Args {
	const Variant args[ARGS_COUNT_MAX];
};

struct Types {
	Variant::Type types[ARGS_COUNT_MAX];
};

void BlipKitAssembler::init_byte_code() {
	if (likely(byte_code.size() > 0)) {
		return;
	}

	// Reserve some space.
	byte_code.reserve(INITIAL_SPACE);

	// Add header.
	const void *header_ptr = &BlipKitBytecode::binary_header;
	byte_code.put_bytes(static_cast<const uint8_t *>(header_ptr), sizeof(BlipKitBytecode::binary_header));
}

static bool check_arg_types(const Args &p_args, const Types &p_types, int &failed_arg_index) {
	for (uint32_t i = 0; i < ARGS_COUNT_MAX; i++) {
		if (p_args.args[i].get_type() != p_types.types[i]) {
			failed_arg_index = i;
			return false;
		}
	}

	return true;
}

void BlipKitAssembler::write_meta() {
	// const BlipKitBytecode::Magic meta = { 'L', 'B', 'L', 'S' };
	// byte_code.put_bytes(meta, sizeof(meta));

	// Save labels in footer.
	byte_code.put_u32(label_indices.size());
	for (const KeyValue<String, uint32_t> &label_index : label_indices) {
		const Label &label = labels[label_index.value];
		const CharString &chars = label.name.utf8();
		const uint32_t chars_size = chars.size() - 1; // Remove terminating NUL.

		byte_code.put_u32(label.byte_offset);
		byte_code.put_u8(chars_size);
		byte_code.put_bytes(reinterpret_cast<const uint8_t *>(chars.ptr()), chars_size);
	}
}

BlipKitAssembler::Error BlipKitAssembler::put(Opcode p_opcode, const Variant &p_arg1, const Variant &p_arg2, const Variant &p_arg3) {
	ERR_FAIL_INDEX_V(p_opcode, OP_MAX, ERR_INVALID_OPCODE);
	ERR_FAIL_COND_V(state != STATE_ASSEMBLE, ERR_INVALID_STATE);

	init_byte_code();

	Types types;
	const Args args = { p_arg1, p_arg2, p_arg3 };
	int failed_arg_index = 0;

	switch (p_opcode) {
		case OP_ATTACK:
		case OP_VOLUME:
		case OP_MASTER_VOLUME:
		case OP_PANNING:
		case OP_PITCH: {
			types = { Variant::FLOAT, Variant::NIL, Variant::NIL };
			if (unlikely(!check_arg_types(args, types, failed_arg_index))) {
				goto invalid_argument;
			}

			byte_code.put_u8(p_opcode);
			byte_code.put_f16(p_arg1);
		} break;
		case OP_WAVEFORM:
		case OP_CUSTOM_WAVEFORM:
		case OP_DUTY_CYCLE:
		case OP_PHASE_WRAP:
		case OP_INSTRUMENT: {
			types = { Variant::INT, Variant::NIL, Variant::NIL };
			if (unlikely(!check_arg_types(args, types, failed_arg_index))) {
				goto invalid_argument;
			}

			byte_code.put_u8(p_opcode);
			byte_code.put_u8(p_arg1);
		} break;
		case OP_EFFECT_DIV:
		case OP_VOLUME_SLIDE:
		case OP_PANNING_SLIDE:
		case OP_PORTAMENTO:
		case OP_ARPEGGIO_DIV:
		case OP_INSTRUMENT_DIV:
		case OP_TICK: {
			types = { Variant::INT, Variant::NIL, Variant::NIL };
			if (unlikely(!check_arg_types(args, types, failed_arg_index))) {
				state = STATE_FAILED;
				goto invalid_argument;
			}

			const int value = CLAMP(int(p_arg1), 0, UINT16_MAX);
			byte_code.put_u8(p_opcode);
			byte_code.put_u16(value);
		} break;
		case OP_TREMOLO:
		case OP_VIBRATO: {
			types = { Variant::INT, Variant::FLOAT, Variant::INT };
			if (unlikely(!check_arg_types(args, types, failed_arg_index))) {
				goto invalid_argument;
			}

			const int ticks = CLAMP(int(p_arg1), 0, UINT16_MAX);
			const float delta = CLAMP(float(p_arg2), -float(BK_MAX_NOTE), +float(BK_MAX_NOTE));
			const int slide_ticks = CLAMP(int(p_arg3), 0, UINT16_MAX);

			byte_code.put_u8(p_opcode);
			byte_code.put_u16(ticks);
			byte_code.put_f16(delta);
			byte_code.put_u16(slide_ticks);
		} break;
		case OP_ARPEGGIO: {
			types = { Variant::PACKED_FLOAT32_ARRAY, Variant::NIL, Variant::NIL };
			if (unlikely(!check_arg_types(args, types, failed_arg_index))) {
				goto invalid_argument;
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
			types = { Variant::STRING, Variant::NIL, Variant::NIL };
			if (unlikely(!check_arg_types(args, types, failed_arg_index))) {
				goto invalid_argument;
			}

			const String &label = p_arg1;
			const uint32_t label_index = get_or_add_label(label);

			byte_code.put_u8(p_opcode);

			const int byte_offset = byte_code.get_position();
			addresses.push_back({ .label_index = label_index, .byte_offset = byte_offset });

			byte_code.put_u32(0);
		} break;
		case OP_RELEASE:
		case OP_MUTE:
		case OP_RETURN:
		case OP_RESET: {
			types = { Variant::NIL, Variant::NIL, Variant::NIL };
			if (unlikely(!check_arg_types(args, types, failed_arg_index))) {
				goto invalid_argument;
			}

			byte_code.put_u8(p_opcode);
		} break;
		case OP_STORE: {
			types = { Variant::INT, Variant::INT, Variant::NIL };
			if (unlikely(!check_arg_types(args, types, failed_arg_index))) {
				goto invalid_argument;
			}

			const int number = CLAMP(int(p_arg1), 0, BlipKitInterpreter::REGISTER_COUNT);
			const int value = p_arg2;

			byte_code.put_u8(p_opcode);
			byte_code.put_u8(number);
			byte_code.put_s32(value);
		} break;
		default: {
			state = STATE_FAILED;

			error_message = vformat("Invalid opcode %d.", p_opcode);
			ERR_FAIL_V_MSG(ERR_INVALID_OPCODE, error_message);
		} break;
	}

	return OK;

invalid_argument:

	const String &type_name = Variant::get_type_name(types.types[failed_arg_index]);

	state = STATE_FAILED; // Fail.
	error_message = vformat("Expected argument %d to be type %s.", failed_arg_index + 1, type_name);

	ERR_FAIL_V_MSG(ERR_INVALID_ARGUMENT, error_message);
}

uint32_t BlipKitAssembler::get_or_add_label(const String p_label) {
	const HashMap<String, uint32_t>::Iterator label_itor = label_indices.find(p_label);

	if (label_itor != label_indices.end()) {
		return label_itor->value;
	}

	const uint32_t label_index = label_indices.size();
	labels.push_back({ .name = p_label });
	label_indices[p_label] = label_index;

	return label_index;
}

BlipKitAssembler::Error BlipKitAssembler::put_code(const String &p_code) {
	ERR_FAIL_COND_V(state != STATE_ASSEMBLE, ERR_INVALID_STATE);

	init_byte_code();

	// a:c#4; t:16; r; e:vb:16:0.5; t:24; r;

	ERR_FAIL_V_MSG(ERR_PARSER_ERROR, "Not implemented.");
}

BlipKitAssembler::Error BlipKitAssembler::put_label(String p_label) {
	ERR_FAIL_COND_V(state != STATE_ASSEMBLE, ERR_INVALID_STATE);
	ERR_FAIL_COND_V_MSG(p_label.is_empty(), ERR_INVALID_LABEL, "Label cannot be empty.");
	ERR_FAIL_COND_V_MSG(p_label.utf8().size() > 255, ERR_INVALID_LABEL, vformat("Label '%s' is longer than 255 bytes.", p_label));

	const uint32_t label_index = get_or_add_label(p_label);
	Label &label = labels[label_index];

	if (label.byte_offset >= 0) {
		error_message = vformat("Label '%s' is already defined.", p_label);
		ERR_FAIL_V_MSG(ERR_DUPLICATE_LABEL, error_message);
	}

	init_byte_code();

	label.byte_offset = byte_code.get_position();

	return OK;
}

BlipKitAssembler::Error BlipKitAssembler::compile() {
	ERR_FAIL_COND_V(state != STATE_ASSEMBLE, ERR_INVALID_STATE);

	init_byte_code();

	// Terminate.
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
		const int32_t address_offset = address.byte_offset;
		const Label &label = labels[label_index];

		if (label.byte_offset < 0) {
			error_message = vformat("Label '%s' not defined at address offset %d.", label.name, address_offset);
			ERR_FAIL_V_MSG(ERR_UNDEFINED_LABEL, error_message);
		}

		const int32_t jump_offset = label.byte_offset - address_offset; // Jump relative to byte code position.
		byte_code.seek(address_offset);
		byte_code.put_s32(jump_offset);
	}

	// Restore byte position.
	byte_code.seek(byte_position);

	write_meta();

	addresses.clear();
	state = STATE_COMPILED;

	return OK;
}

Ref<BlipKitBytecode> BlipKitAssembler::get_byte_code() {
	ERR_FAIL_COND_V_MSG(state != STATE_COMPILED, nullptr, "Byte code is not compiled.");

	if (bytecode.is_null()) {
		bytecode = BlipKitBytecode::create_with_byte_stream(byte_code);
	}

	return bytecode;
}

String BlipKitAssembler::get_error_message() const {
	return error_message;
}

void BlipKitAssembler::clear() {
	byte_code.clear();
	label_indices.clear();
	labels.clear();
	addresses.clear();
	bytecode.unref();
	error_message.resize(0);
	state = STATE_ASSEMBLE;
}

void BlipKitAssembler::_bind_methods() {
	ClassDB::bind_method(D_METHOD("put", "opcode", "arg1", "arg2", "arg3"), &BlipKitAssembler::put, DEFVAL(nullptr), DEFVAL(nullptr), DEFVAL(nullptr));
	ClassDB::bind_method(D_METHOD("put_code", "code"), &BlipKitAssembler::put_code);
	ClassDB::bind_method(D_METHOD("put_label", "label"), &BlipKitAssembler::put_label);
	ClassDB::bind_method(D_METHOD("compile"), &BlipKitAssembler::compile);
	ClassDB::bind_method(D_METHOD("get_byte_code"), &BlipKitAssembler::get_byte_code);
	ClassDB::bind_method(D_METHOD("get_error_message"), &BlipKitAssembler::get_error_message);
	ClassDB::bind_method(D_METHOD("clear"), &BlipKitAssembler::clear);

	BIND_ENUM_CONSTANT(OP_ATTACK);
	BIND_ENUM_CONSTANT(OP_RELEASE);
	BIND_ENUM_CONSTANT(OP_MUTE);
	BIND_ENUM_CONSTANT(OP_WAVEFORM);
	BIND_ENUM_CONSTANT(OP_CUSTOM_WAVEFORM);
	BIND_ENUM_CONSTANT(OP_DUTY_CYCLE);
	BIND_ENUM_CONSTANT(OP_EFFECT_DIV);
	BIND_ENUM_CONSTANT(OP_VOLUME);
	BIND_ENUM_CONSTANT(OP_VOLUME_SLIDE);
	BIND_ENUM_CONSTANT(OP_MASTER_VOLUME);
	BIND_ENUM_CONSTANT(OP_PANNING);
	BIND_ENUM_CONSTANT(OP_PANNING_SLIDE);
	BIND_ENUM_CONSTANT(OP_PITCH);
	BIND_ENUM_CONSTANT(OP_PHASE_WRAP);
	BIND_ENUM_CONSTANT(OP_PORTAMENTO);
	BIND_ENUM_CONSTANT(OP_VIBRATO);
	BIND_ENUM_CONSTANT(OP_TREMOLO);
	BIND_ENUM_CONSTANT(OP_ARPEGGIO);
	BIND_ENUM_CONSTANT(OP_ARPEGGIO_DIV);
	BIND_ENUM_CONSTANT(OP_INSTRUMENT);
	BIND_ENUM_CONSTANT(OP_INSTRUMENT_DIV);
	BIND_ENUM_CONSTANT(OP_TICK);
	BIND_ENUM_CONSTANT(OP_CALL);
	BIND_ENUM_CONSTANT(OP_RETURN);
	BIND_ENUM_CONSTANT(OP_JUMP);
	BIND_ENUM_CONSTANT(OP_RESET);
	BIND_ENUM_CONSTANT(OP_STORE);

	BIND_ENUM_CONSTANT(OK);
	BIND_ENUM_CONSTANT(ERR_INVALID_STATE);
	BIND_ENUM_CONSTANT(ERR_INVALID_OPCODE);
	BIND_ENUM_CONSTANT(ERR_INVALID_ARGUMENT);
	BIND_ENUM_CONSTANT(ERR_DUPLICATE_LABEL);
	BIND_ENUM_CONSTANT(ERR_UNDEFINED_LABEL);
	BIND_ENUM_CONSTANT(ERR_INVALID_LABEL);
	BIND_ENUM_CONSTANT(ERR_PARSER_ERROR);
}

String BlipKitAssembler::_to_string() const {
	return vformat("<BlipKitAssembler:#%d>", int64_t(this));
}
