#pragma once

#include <BlipKit.h>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitInstrument : public Resource {
	GDCLASS(BlipKitInstrument, Resource)

public:
	enum EnvelopeType {
		ENVELOPE_VOLUME,
		ENVELOPE_PANNING,
		ENVELOPE_PITCH,
		ENVELOPE_DUTY_CYCLE,
		ENVELOPE_MAX,
	};

private:
	struct Sequence {
		PackedInt32Array steps;
		PackedFloat32Array values;
		int sustain_offset = 0;
		int sustain_length = 0;
	};

	BKInstrument instrument;
	Sequence sequences[ENVELOPE_MAX];

public:
	BlipKitInstrument();
	~BlipKitInstrument();

	static Ref<BlipKitInstrument> create_with_adsr(int p_attack, int p_decay, float p_sustain, int p_release);

	_ALWAYS_INLINE_ BKInstrument *get_instrument() { return &instrument; };

	void set_envelope(EnvelopeType p_type, const PackedInt32Array &p_steps, const PackedFloat32Array &p_values, int p_sustain_offset, int p_sustain_length);
	void set_adsr(int p_attack, int p_decay, float p_sustain, int p_release);
	bool has_envelope(EnvelopeType p_type) const;
	PackedInt32Array get_envelope_steps(EnvelopeType p_type) const;
	PackedFloat32Array get_envelope_values(EnvelopeType p_type) const;
	int get_envelope_sustain_offset(EnvelopeType p_type) const;
	int get_envelope_sustain_length(EnvelopeType p_type) const;
	void clear_envelope(EnvelopeType p_type);

protected:
	static void _bind_methods();
	String _to_string() const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
};

} // namespace BlipKit

VARIANT_ENUM_CAST(BlipKit::BlipKitInstrument::EnvelopeType);
