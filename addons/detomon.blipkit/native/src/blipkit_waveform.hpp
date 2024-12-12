#pragma once

#include <BlipKit.h>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace godot;

namespace detomon::BlipKit {

class BlipKitWaveform : public Resource {
	GDCLASS(BlipKitWaveform, Resource)

private:
	BKData waveform;
	LocalVector<real_t> frames;

protected:
	static void _bind_methods();
	String _to_string() const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;

	void _update_waveform(const LocalVector<real_t> &p_frames);

public:
	BlipKitWaveform();
	~BlipKitWaveform();

	static Ref<BlipKitWaveform> create_with_frames(const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);

	_FORCE_INLINE_ BKData *get_waveform() { return &waveform; };
	_FORCE_INLINE_ int size() const { return frames.size(); };
	_FORCE_INLINE_ bool is_valid() const { return !frames.is_empty(); };

	PackedFloat32Array get_frames() const;
	void set_frames(const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);
};

} // namespace detomon::BlipKit
