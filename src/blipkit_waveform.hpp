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
	static constexpr int WAVE_SIZE_MAX = BK_WAVE_MAX_LENGTH;

private:
	BKData data;
	LocalVector<BKFrame> frames;

public:
	BlipKitWaveform();
	~BlipKitWaveform();

	static Ref<BlipKitWaveform> create_with_frames(const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);

	_ALWAYS_INLINE_ BKData *get_data() { return &data; };
	_ALWAYS_INLINE_ int size() const { return frames.size(); };
	_ALWAYS_INLINE_ bool is_valid() const { return !frames.is_empty(); };

	void set_frames(const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);
	PackedFloat32Array get_frames() const;

protected:
	static void _bind_methods();
	String _to_string() const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
};

} // namespace BlipKit
