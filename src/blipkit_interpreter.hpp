#pragma once

#include "blipkit_bytecode.hpp"
#include "byte_stream.hpp"
#include <BlipKit.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitInstrument;
class BlipKitSample;
class BlipKitTrack;
class BlipKitWaveform;

class BlipKitInterpreter : public RefCounted {
	GDCLASS(BlipKitInterpreter, RefCounted)

public:
	static constexpr int STACK_SIZE_MAX = 16;
	static constexpr int SLOT_COUNT = 256;
	static constexpr int STEP_TICKS_DEFAULT = 24;

	enum State {
		OK_RUNNING,
		OK_FINISHED,
		ERR_INVALID_BINARY, // NOTE: Keep as first error.
		ERR_INVALID_OPCODE,
		ERR_INVALID_LABEL,
		ERR_STACK_OVERFLOW,
		ERR_STACK_UNDERFLOW,
	};

private:
	enum DelayState : uint8_t {
		DELAY_STATE_NONE,
		DELAY_STATE_DELAY,
		DELAY_STATE_EXEC,
	};

	struct DelayRegister {
		static constexpr int MAX_DELAYS = 8;

		uint32_t code_offset = 0;
		uint32_t ticks = 0;
		DelayState state = DELAY_STATE_NONE;
		uint8_t delay_index = 0;
		uint8_t delay_size = 0;
		uint32_t delays[MAX_DELAYS] = { 0 };
	};

	ByteStreamReader byte_code;
	Ref<BlipKitBytecode> byte_code_res;

	LocalVector<uint32_t> stack;
	DelayRegister delay_register;
	PackedFloat32Array arpeggio;
	uint32_t step_ticks = STEP_TICKS_DEFAULT;

	LocalVector<Ref<BlipKitInstrument>> instruments;
	LocalVector<Ref<BlipKitWaveform>> waveforms;
	LocalVector<Ref<BlipKitSample>> samples;

	State state = OK_RUNNING;
	String error_message;

	uint32_t exec_delay_begin(uint32_t p_ticks, uint32_t p_code_offset);
	uint32_t exec_delay_step(uint32_t p_ticks);
	uint32_t exec_delay_shift();

	int fail_with_error(State p_status, const String &p_error_message);

public:
	BlipKitInterpreter();

	void set_instrument(int p_slot, const Ref<BlipKitInstrument> &p_instrument);
	Ref<BlipKitInstrument> get_instrument(int p_slot) const;
	void set_waveform(int p_slot, const Ref<BlipKitWaveform> &p_waveform);
	Ref<BlipKitWaveform> get_waveform(int p_slot) const;
	void set_sample(int p_slot, const Ref<BlipKitSample> &p_sample);
	Ref<BlipKitSample> get_sample(int p_slot) const;

	void set_step_ticks(int p_step_ticks);
	int get_step_ticks() const;

	bool load_byte_code(const Ref<BlipKitBytecode> &p_byte_code, const String &p_start_label = "");

	int advance(const Ref<BlipKitTrack> &p_track);
	State get_state() const;
	String get_error_message() const;

	void reset(const String &p_start_label = "");

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit

VARIANT_ENUM_CAST(BlipKit::BlipKitInterpreter::State);
