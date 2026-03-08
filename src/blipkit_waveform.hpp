#pragma once

#include "fixed_vector.hpp"
#include "server/blipkit_server.hpp"
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
	RID rid;

public:
	BlipKitWaveform();
	~BlipKitWaveform();

	static Ref<BlipKitWaveform> create_with_frames(const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);

	void set_frames(const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);
	PackedFloat32Array get_frames() const;

	_ALWAYS_INLINE_ BKData *get_data() { return BlipKitServer::get_singleton()->waveform_get_data(rid); };
	_ALWAYS_INLINE_ int size() const { return BlipKitServer::get_singleton()->waveform_get_size(rid); };
	_ALWAYS_INLINE_ bool is_valid() const { return BlipKitServer::get_singleton()->waveform_get_is_valid(rid); };

	_FORCE_INLINE_ RID get_rid() const { return rid; }

protected:
	static void _bind_methods();
	String _to_string() const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
};

} // namespace BlipKit
