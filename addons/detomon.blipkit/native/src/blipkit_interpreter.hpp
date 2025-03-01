#pragma once

#include "blipkit_bytecode.hpp"
#include "byte_stream.hpp"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitInstrument;
class BlipKitTrack;
class BlipKitWaveform;

class BlipKitInterpreter : public RefCounted {
	GDCLASS(BlipKitInterpreter, RefCounted)

public:
	static constexpr int SLOT_COUNT = 256;
	static constexpr int STACK_SIZE_MAX = 64;
	static constexpr int REGISTER_COUNT = 16;

	enum Status {
		OK_RUNNING,
		OK_FINISHED,
		ERR_INVALID_BINARY,
		ERR_INVALID_OPCODE,
		ERR_STACK_OVERFLOW,
		ERR_STACK_UNDERFLOW,
	};

private:
	struct Registers {
		int32_t aux[REGISTER_COUNT] = { 0 };
	};

	ByteStream byte_code;
	String start_label;
	LocalVector<uint32_t> stack;
	Registers registers;
	LocalVector<Ref<BlipKitInstrument>> instruments;
	LocalVector<Ref<BlipKitWaveform>> waveforms;
	PackedFloat32Array arpeggio;
	Ref<BlipKitBytecode> byte_code_res;
	Status status = OK_RUNNING;
	String error_message;

	int fail_with_error(Status p_status, const String &error_message);

public:
	BlipKitInterpreter();
	~BlipKitInterpreter() = default;

	void set_instrument(int p_slot, const Ref<BlipKitInstrument> &p_instrument);
	Ref<BlipKitInstrument> get_instrument(int p_slot) const;
	void set_waveform(int p_slot, const Ref<BlipKitWaveform> &p_waveform);
	Ref<BlipKitWaveform> get_waveform(int p_slot) const;
	void set_register(int p_number, int p_value);
	int get_register(int p_number) const;

	bool load_byte_code(const Ref<BlipKitBytecode> &p_byte_code, const String &p_start_label = "");

	int advance(const Ref<BlipKitTrack> &p_track);
	Status get_status() const;
	String get_error_message() const;

	void reset(const String &p_start_label = "");

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit

VARIANT_ENUM_CAST(BlipKit::BlipKitInterpreter::Status);
