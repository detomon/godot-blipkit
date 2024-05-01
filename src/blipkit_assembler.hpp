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
	enum Opcode : uint8_t {
		OP_NOOP,
		OP_HALT,
		OP_ATTACK,
		OP_RELEASE,
		OP_MUTE,
		OP_VOLUME,
		OP_MASTER_VOLUME,
		OP_PANNING,
		OP_WAVEFORM,
		OP_DUTY_CYCLE,
		OP_PITCH,
		OP_PHASE_WRAP,
		OP_PORTAMENTO,
		OP_VIBRATO,
		OP_TREMOLO,
		OP_VOLUME_SLIDE,
		OP_PANNING_SLIDE,
		OP_EFFECT_DIV,
		OP_ARPEGGIO,
		OP_ARPEGGIO_DIV,
		OP_TICK,
		OP_STEP,
		OP_STEP_TICKS,
		OP_DELAY_TICK,
		OP_DELAY_STEP,
		OP_JUMP,
		OP_CALL,
		OP_RETURN,
		OP_RESET,
		OP_INSTRUMENT,
		OP_INSTRUMENT_DIV,
		OP_CUSTOM_WAVEFORM,
		OP_SAMPLE,
		OP_SAMPLE_PITCH,
		OP_MAX,
		// NOTE: Update 'blipc_file.md' when changing list.
	};

	enum Error {
		OK,
		ERR_INVALID_STATE,
		ERR_INVALID_OPCODE,
		ERR_INVALID_ARGUMENT,
		ERR_DUPLICATE_LABEL,
		ERR_UNDEFINED_LABEL,
		ERR_INVALID_LABEL,
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

	struct Args {
		static constexpr int COUNT_MAX = 3;

		const Variant args[COUNT_MAX];
	};

	struct Types {
		Variant::Type types[Args::COUNT_MAX];
	};

	ByteStreamWriter byte_code;
	HashMap<String, uint32_t> label_indices;
	LocalVector<Label> labels;
	LocalVector<Address> addresses;
	Ref<BlipKitBytecode> compiled_byte_code;
	String error_message;
	State state = STATE_ASSEMBLE;
	uint32_t code_section_offset = 0;

	void write_header();
	void write_sections();
	void write_labels();
	Error get_or_add_label(const String p_label, uint32_t &r_label_index);

	bool check_arg_type(const Variant &p_var, Variant::Type p_type, uint32_t p_index);
	bool check_arg_types(const Args &p_args, const Types &p_types);
	bool check_args_number_nil_nil(const Args &p_args);

	void fail_with_error(const String &p_error_message);
	void fail_argument_type(Variant::Type p_type, uint32_t p_index);

public:
	BlipKitAssembler();

	Error put(Opcode p_opcode, const Variant &p_arg1 = nullptr, const Variant &p_arg2 = nullptr, const Variant &p_arg3 = nullptr);
	Error put_byte_code(const Ref<BlipKitBytecode> &p_byte_code, bool p_public = false);
	Error put_label(const String p_label, int32_t p_label_position, bool p_public);
	Error put_label_bind(const String p_label, bool p_public = false);
	Error compile();

	Vector<uint8_t> get_bytes() const;
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
