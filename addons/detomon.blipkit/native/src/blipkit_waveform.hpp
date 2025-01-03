#pragma once

#include <BlipKit.h>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitWaveform : public Resource {
	GDCLASS(BlipKitWaveform, Resource)

public:
	static constexpr int WAVE_MAX_LENGTH = BK_WAVE_MAX_LENGTH;

private:
	LocalVector<BKFrame> frames;
	BKData waveform;

protected:
	static void _bind_methods();
	String _to_string() const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;

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

} // namespace BlipKit
