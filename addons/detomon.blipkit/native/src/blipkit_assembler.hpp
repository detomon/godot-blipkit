#pragma once

#include "math.hpp"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace detomon::BlipKit {

class BlipKitAssembler : public RefCounted {
	GDCLASS(BlipKitAssembler, RefCounted)

public:
	enum Instruction {
		INSTR_NOP,
		INSTR_INIT,
		INSTR_ATTACK,
		INSTR_RELEASE,
		INSTR_MUTE,
		INSTR_WAVEFORM,
		INSTR_CUSTOM_WAVEFORM,
		INSTR_DUTY_CYCLE,
		INSTR_EFFECT_DIV,
		INSTR_VOLUME,
		INSTR_VOLUME_SLIDE,
		INSTR_MASTER_VOLUME,
		INSTR_PANNING,
		INSTR_PANNING_SLIDE,
		INSTR_PITCH,
		INSTR_PHASE_WRAP,
		INSTR_PORTAMENTO,
		INSTR_VIBRATO,
		INSTR_TREMOLO,
		INSTR_ARPEGGIO,
		INSTR_ARPEGGIO_DIV,
		INSTR_INSTRUMENT,
		INSTR_INSTRUMENT_DIV,
		INSTR_TICK,
		INSTR_CALL,
		INSTR_RETURN,
		INSTR_JUMP,
		INSTR_RESET,
		INSTR_STORE,
		INSTR_MAX,
	};

	enum Error {
		OK,
		ERR_INVALID_STATE,
		ERR_INVALID_INSTR,
		ERR_INVALID_ARGUMENT,
		ERR_DUPLICATE_LABEL,
		ERR_UNDEFINED_LABEL,
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
		int byte_offset = -1;
	};

	struct Address {
		int label_index = 0;
		int byte_offset = 0;
	};

	struct Args {
		Variant args[3];
	};

	Ref<StreamPeerBuffer> byte_code;
	HashMap<String, int> label_indices;
	LocalVector<Label> labels;
	LocalVector<Address> addresses;
	String error_message;
	State state = STATE_ASSEMBLE;

protected:
	static void _bind_methods();
	String _to_string() const;

	void initialize();
	Error put_instruction(Instruction p_instr, const Args &p_args);
	int add_label(const String p_label);
	bool check_args(const Args &p_args, Variant::Type p_type1, Variant::Type p_type2, Variant::Type p_type3);

	_ALWAYS_INLINE_ void put_half(float p_value) { byte_code->put_u16(float_to_half(p_value)); }

public:
	BlipKitAssembler();
	~BlipKitAssembler() = default;

	Error put(Instruction p_instr, const Variant &p_arg1 = nullptr, const Variant &p_arg2 = nullptr, const Variant &p_arg3 = nullptr);
	Error put_code(const String &p_code);
	Error put_label(const String p_label);
	Error compile();

	PackedByteArray get_byte_code() const;
	String get_error_message() const;

	void clear();
};

} // namespace detomon::BlipKit

VARIANT_ENUM_CAST(detomon::BlipKit::BlipKitAssembler::Instruction);
VARIANT_ENUM_CAST(detomon::BlipKit::BlipKitAssembler::Error);
