#pragma once

#include "math.hpp"
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/stream_peer_buffer.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

using namespace godot;

namespace detomon::BlipKit {

class BlipKitInstrument;
class BlipKitTrack;
class BlipKitWaveform;

class BlipKitInterpreter : public RefCounted {
	GDCLASS(BlipKitInterpreter, RefCounted)

public:
	static constexpr int REGISTER_COUNT = 16;
	static constexpr int VERSION = 1;

	enum Status {
		OK_RUNNING,
		OK_FINISHED,
		ERR_INVALID_ARGUMENT,
		ERR_INVALID_INSTRUCTION,
		ERR_STACK_OVERFLOW,
		ERR_STACK_UNDERFLOW,
		ERR_RECURSION,
		ERR_UNSUPPORTED_VERSION,
	};

private:
	static constexpr int STACK_SIZE_MAX = 64;
	static constexpr int SLOT_COUNT = 256;

	Ref<StreamPeerBuffer> byte_code;
	LocalVector<uint32_t> stack;
	int32_t registers[REGISTER_COUNT] = { 0 };
	LocalVector<Ref<BlipKitInstrument>> instruments;
	LocalVector<Ref<BlipKitWaveform>> waveforms;
	PackedFloat32Array arpeggio;
	Status status = OK_RUNNING;
	String error_message;

protected:
	static void _bind_methods();
	String _to_string() const;

	int fail_with_error(Status p_status, const String &error_message);

	_ALWAYS_INLINE_ float get_half() { return half_to_float(byte_code->get_u16()); }

public:
	BlipKitInterpreter();
	~BlipKitInterpreter() = default;

	void set_instrument(int p_slot, const Ref<BlipKitInstrument> &p_instrument);
	Ref<BlipKitInstrument> get_instrument(int p_slot) const;
	void set_waveform(int p_slot, const Ref<BlipKitWaveform> &p_waveform);
	Ref<BlipKitWaveform> get_waveform(int p_slot) const;
	void set_byte_code(const PackedByteArray &p_byte);

	void set_register(int p_number, int p_value);
	int get_register(int p_number) const;

	int advance(const Ref<BlipKitTrack> &p_track);
	Status get_status() const;
	String get_error_message() const;

	void reset();
};

} // namespace detomon::BlipKit

VARIANT_ENUM_CAST(detomon::BlipKit::BlipKitInterpreter::Status);
