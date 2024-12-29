#pragma once

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
		INSTR_NOTE,
		INSTR_WAVEFORM,
		INSTR_CUSTOM_WAVEFORM,
		INSTR_DUTY_CYCLE,
		INSTR_EFFECT_DIVIDER,
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
		INSTR_ARPEGGIO_DIVIDER,
		INSTR_INSTRUMENT,
		INSTR_INSTRUMENT_DIVIDER,
		INSTR_WAIT,
		INSTR_CALL,
		INSTR_RETURN,
		INSTR_JUMP,
		INSTR_RESET,
		INSTR_MAX,
	};

	enum Error {
		OK,
		ERR_INVALID_INSTRUCTION,
		ERR_INVALID_ARGUMENT,
		ERR_DUPLICATE_LABEL,
		ERR_UNDEFINED_LABEL,
	};

private:
	struct Label {
		String name;
		int byte_offset = -1;
	};

	struct Address {
		int label_index = 0;
		int byte_offset = 0;
	};

	struct Command {
		Variant args[3];
	};

	Ref<StreamPeerBuffer> bytes;
	HashMap<String, int> label_indices;
	LocalVector<Label> labels;
	LocalVector<Address> addresses;
	String error_message;
	bool compiled = false;

protected:
	static void _bind_methods();
	String _to_string() const;

	Error put_cmd(Instruction p_instr, const Command &p_cmd);
	int add_label(const String p_label);
	Error check_args(const Command &p_cmd, Variant::Type p_type_1, Variant::Type p_type_2, Variant::Type p_type_3);

public:
	BlipKitAssembler();
	~BlipKitAssembler() = default;

	Error put(Instruction p_instr, const Variant &p_arg_1 = nullptr, const Variant &p_arg_2 = nullptr, const Variant &p_arg_3 = nullptr);
	Error put_label(const String p_label);
	Error compile();
	PackedByteArray get_byte_code() const;
	String get_error_message() const;
	void clear();
};

} // namespace detomon::BlipKit

VARIANT_ENUM_CAST(detomon::BlipKit::BlipKitAssembler::Instruction);
VARIANT_ENUM_CAST(detomon::BlipKit::BlipKitAssembler::Error);
