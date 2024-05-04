#pragma once

#include <BlipKit.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace godot;

namespace detomon::BlipKit {

class BlipKitWaveform : public RefCounted {
	GDCLASS(BlipKitWaveform, RefCounted)

private:
	BKData waveform;
	PackedFloat32Array frames;

protected:
	static void _bind_methods();
	String _to_string() const;

public:
	BlipKitWaveform();
	~BlipKitWaveform();

	_FORCE_INLINE_ BKData *get_waveform() { return &waveform; };
	_FORCE_INLINE_ bool is_valid() { return frames.size() > 0; };

	PackedFloat32Array get_frames();
	void set_frames(PackedFloat32Array p_frames, bool p_normalize = true);
};

} // namespace detomon::BlipKit
