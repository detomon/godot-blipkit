#pragma once

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
	enum Status {
		OK_RUNNING,
		OK_FINISHED,
		ERR_INVALID_ARGUMENT,
		ERR_INVALID_INSTRUCTION,
		ERR_STACK_OVERFLOW,
		ERR_STACK_UNDERFLOW,
		ERR_RECURSION,
	};

private:
	const int STACK_SIZE_MAX = 64;
	const int SLOT_COUNT = 256;

	Ref<StreamPeerBuffer> byte_code;
	LocalVector<uint32_t> stack;
	LocalVector<Ref<BlipKitInstrument>> instruments;
	LocalVector<Ref<BlipKitWaveform>> waveforms;
	PackedFloat32Array arpeggio;
	Status status = OK_RUNNING;
	String error_message;

protected:
	static void _bind_methods();
	String _to_string() const;

	void fail_with_error(Status p_status, const String &error_message);

public:
	BlipKitInterpreter();
	~BlipKitInterpreter() = default;

	void set_instrument_at(int p_slot, const Ref<BlipKitInstrument> &p_instrument);
	Ref<BlipKitInstrument> get_instrument_at(int p_slot) const;
	void set_waveform_at(int p_slot, const Ref<BlipKitWaveform> &p_waveform);
	Ref<BlipKitWaveform> get_waveform_at(int p_slot) const;
	void set_byte_code(const PackedByteArray &p_byte);

	int advance(const Ref<BlipKitTrack> &p_track);
	Status get_status() const;
	String get_error_message() const;

	void reset();
};

} // namespace detomon::BlipKit

VARIANT_ENUM_CAST(detomon::BlipKit::BlipKitInterpreter::Status);
