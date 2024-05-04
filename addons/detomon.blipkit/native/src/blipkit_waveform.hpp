#pragma once

#include <BlipKit.h>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace godot;

namespace detomon::BlipKit {

class BlipKitWaveform : public Resource {
	GDCLASS(BlipKitWaveform, Resource)

private:
	BKData waveform;
	PackedFloat32Array frames;

	void _set_frames(const PackedFloat32Array &p_frames, bool p_normalize);

protected:
	static void _bind_methods();
	String _to_string() const;

public:
	BlipKitWaveform();
	~BlipKitWaveform();

	_FORCE_INLINE_ BKData *get_waveform() { return &waveform; };
	_FORCE_INLINE_ bool is_valid() { return !frames.is_empty(); };

	PackedFloat32Array get_frames();
	void set_frames(PackedFloat32Array p_frames) { _set_frames(p_frames, false); }
	void set_frames_normalized(PackedFloat32Array p_frames) { _set_frames(p_frames, true); }
};

} // namespace detomon::BlipKit
