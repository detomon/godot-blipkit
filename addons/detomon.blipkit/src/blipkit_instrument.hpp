#pragma once

#include <BlipKit.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>

using namespace godot;

namespace detomon::BlipKit {

class BlipKitInstrument : public RefCounted {
	GDCLASS(BlipKitInstrument, RefCounted)

private:
	BKInstrument instrument;

protected:
	enum Sequence {
		SEQUENCE_PITCH      = BK_SEQUENCE_PITCH,
		SEQUENCE_VOLUME     = BK_SEQUENCE_VOLUME,
		SEQUENCE_PANNING    = BK_SEQUENCE_PANNING,
		SEQUENCE_DUTY_CYCLE = BK_SEQUENCE_DUTY_CYCLE,
	};

	static void _bind_methods();

	void set_sequence_float(Sequence p_sequence, const PackedFloat32Array &p_values, int p_sustain_offset, int p_sustain_length, real_t p_multiplier);
	void set_sequence_int(Sequence p_sequence, const PackedInt32Array &p_values, int p_sustain_offset, int p_sustain_length);
	void set_envelope_float(Sequence p_sequence, const PackedInt32Array &p_steps, const PackedFloat32Array &p_values, int p_sustain_offset, int p_sustain_length, real_t p_multiplier);
	void set_envelope_int(Sequence p_sequence, const PackedInt32Array &p_steps, const PackedInt32Array &p_values, int p_sustain_offset, int p_sustain_length);

public:
	BlipKitInstrument();
	~BlipKitInstrument();

	_FORCE_INLINE_ BKInstrument *get_instrument() { return &instrument; };

	void set_sequence_pitch(PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length);
	void set_sequence_volume(PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length);
	void set_sequence_panning(PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length);
	void set_sequence_duty_cycle(PackedInt32Array p_values, int p_sustain_offset, int p_sustain_length);

	void set_envelope_pitch(PackedInt32Array p_steps, PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length);
	void set_envelope_volume(PackedInt32Array p_steps, PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length);
	void set_envelope_panning(PackedInt32Array p_steps, PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length);
	void set_envelope_duty_cycle(PackedInt32Array p_steps, PackedInt32Array p_values, int p_sustain_offset, int p_sustain_length);
	void set_envelope_adsr(int p_attack, int p_decay, real_t p_sustain, int p_release);

};

}
