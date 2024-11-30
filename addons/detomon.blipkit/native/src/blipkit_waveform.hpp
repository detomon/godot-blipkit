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
	PackedFloat32Array frames;

protected:
	static void _bind_methods();
	String _to_string() const;

	void _update_waveform();

public:
	BlipKitWaveform();
	~BlipKitWaveform();

	static Ref<BlipKitWaveform> create_with_frames(PackedFloat32Array p_frames, bool p_normalized = false, float p_amplitude = 1.0);

	_FORCE_INLINE_ BKData *get_waveform() { return &waveform; };
	_FORCE_INLINE_ int size() const { return frames.size(); };
	_FORCE_INLINE_ bool is_valid() const { return !frames.is_empty(); };

	PackedFloat32Array get_frames();
	void set_frames(PackedFloat32Array p_frames);
	void set_frames_normalized(PackedFloat32Array p_frames, float p_amplitude = 1.0);
};

} // namespace detomon::BlipKit
