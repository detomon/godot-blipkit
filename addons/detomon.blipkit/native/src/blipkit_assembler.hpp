#pragma once

#include <BlipKit.h>
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

	StreamPeerBuffer bytes;
	HashMap<String, int> label_indices;
	LocalVector<Label> labels;
	LocalVector<Address> addresses;
	String compile_error;

protected:
	static void _bind_methods();
	String _to_string() const;

	bool put(Instruction p_instr, const Command &p_cmd);
	int add_label(const String p_label);
	bool expect_args(const Command &p_cmd, Variant::Type p_type_1, Variant::Type p_type_2, Variant::Type p_type_3);

public:
	BlipKitAssembler();
	~BlipKitAssembler();

	void put_0(Instruction p_instr);
	void put_1(Instruction p_instr, const Variant p_arg_1);
	void put_2(Instruction p_instr, const Variant p_arg_1, const Variant p_arg_2);
	void put_3(Instruction p_instr, const Variant p_arg_1, const Variant p_arg_2, const Variant p_arg_3);

	bool put_label(const String p_label);
	int get_offset() const;

	PackedByteArray compile();
	String get_compile_error() const;
};

} // namespace detomon::BlipKit

VARIANT_ENUM_CAST(detomon::BlipKit::BlipKitAssembler::Instruction);
