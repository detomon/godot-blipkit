#pragma once

#include "fixed_vector.hpp"
#include <BlipKit.h>
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace godot;

namespace BlipKit {

class Waveform {
public:
	static constexpr int WAVE_SIZE_MAX = BK_WAVE_MAX_LENGTH;

private:
	BKData data = { { 0 } };
	FixedVector<BKFrame, WAVE_SIZE_MAX> frames;

public:
	~Waveform();

	bool initialize();
	bool set_frames(const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);
	PackedFloat32Array get_frames() const;

	_ALWAYS_INLINE_ int size() const { return frames.size(); };
	_ALWAYS_INLINE_ bool is_valid() const { return not frames.is_empty(); };
	_ALWAYS_INLINE_ BKData *get_data() { return &data; };
};

} //namespace BlipKit
