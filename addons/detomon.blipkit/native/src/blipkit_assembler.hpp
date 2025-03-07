#pragma once

#include "blipkit_bytecode.hpp"
#include "byte_stream.hpp"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitAssembler : public RefCounted {
	GDCLASS(BlipKitAssembler, RefCounted)

public:
	enum Opcode {
		OP_NOOP,
		OP_ATTACK,
		OP_RELEASE,
		OP_MUTE,
		OP_WAVEFORM,
		OP_CUSTOM_WAVEFORM,
		OP_DUTY_CYCLE,
		OP_EFFECT_DIV,
		OP_VOLUME,
		OP_VOLUME_SLIDE,
		OP_MASTER_VOLUME,
		OP_PANNING,
		OP_PANNING_SLIDE,
		OP_PITCH,
		OP_PHASE_WRAP,
		OP_PORTAMENTO,
		OP_VIBRATO,
		OP_TREMOLO,
		OP_ARPEGGIO,
		OP_ARPEGGIO_DIV,
		OP_INSTRUMENT,
		OP_INSTRUMENT_DIV,
		OP_TICK,
		OP_CALL,
		OP_RETURN,
		OP_JUMP,
		OP_RESET,
		OP_STORE,
		OP_HALT,
		OP_MAX,
	};

	enum Error {
		OK,
		ERR_INVALID_STATE,
		ERR_INVALID_OPCODE,
		ERR_INVALID_ARGUMENT,
		ERR_DUPLICATE_LABEL,
		ERR_UNDEFINED_LABEL,
		ERR_INVALID_LABEL,
		ERR_PARSER_ERROR,
	};

private:
	enum State {
		STATE_ASSEMBLE,
		STATE_COMPILED,
		STATE_FAILED,
	};

	struct Label {
		String name;
		int32_t byte_offset = -1;
		bool is_public = false;
	};

	struct Address {
		uint32_t label_index = 0;
		int32_t byte_offset = 0;
	};

	ByteStream byte_code;
	Ref<BlipKitBytecode> compiled_byte_code;
	HashMap<String, uint32_t> label_indices;
	LocalVector<Label> labels;
	LocalVector<Address> addresses;
	String error_message;
	State state = STATE_ASSEMBLE;

protected:
	void write_sections();
	void write_labels();
	uint32_t get_or_add_label(const String p_label);

	void fail_with_error(const String &p_error_message);

public:
	BlipKitAssembler();

	Error put(Opcode p_opcode, const Variant &p_arg1 = nullptr, const Variant &p_arg2 = nullptr, const Variant &p_arg3 = nullptr);
	Error put_byte_code(const Ref<BlipKitBytecode> &p_byte_code);
	Error put_code(const String &p_code);
	Error put_label(const String p_label, int p_label_position, bool p_public);
	Error put_label_bind(const String p_label, bool p_public = false);
	Error compile();

	Ref<BlipKitBytecode> get_byte_code();
	String get_error_message() const;

	void clear();

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit

VARIANT_ENUM_CAST(BlipKit::BlipKitAssembler::Opcode);
VARIANT_ENUM_CAST(BlipKit::BlipKitAssembler::Error);
