#pragma once

#include <BlipKit.h>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace detomon::BlipKit {

class BlipKitInstrument : public Resource {
	GDCLASS(BlipKitInstrument, Resource)

private:
	enum SequenceType {
		SEQUENCE_PITCH = BK_SEQUENCE_PITCH,
		SEQUENCE_VOLUME = BK_SEQUENCE_VOLUME,
		SEQUENCE_PANNING = BK_SEQUENCE_PANNING,
		SEQUENCE_DUTY_CYCLE = BK_SEQUENCE_DUTY_CYCLE,
	};

	struct Sequence {
		PackedInt32Array steps;
		Variant values;
		int sustain_offset = 0;
		int sustain_length = 0;
	};

	BKInstrument instrument;
	Sequence sequences[BK_MAX_SEQUENCES];

	void set_sequence_float(SequenceType p_sequence, PackedFloat32Array &p_values, int p_sustain_offset, int p_sustain_length, real_t p_multiplier);
	void set_sequence_int(SequenceType p_sequence, PackedInt32Array &p_values, int p_sustain_offset, int p_sustain_length);
	void set_envelope_float(SequenceType p_sequence, PackedInt32Array &p_steps, PackedFloat32Array &p_values, int p_sustain_offset, int p_sustain_length, real_t p_multiplier);
	void set_envelope_int(SequenceType p_sequence, PackedInt32Array &p_steps, PackedInt32Array &p_values, int p_sustain_offset, int p_sustain_length);

protected:
	static void _bind_methods();
	String _to_string() const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;

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

} // namespace detomon::BlipKit
