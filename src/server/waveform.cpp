#include "waveform.hpp"
#include "audio_stream_blipkit.hpp"
#include <godot_cpp/variant/variant.hpp>

using namespace BlipKit;
using namespace godot;

bool Waveform::initialize() {
	BKInt result = BKDataInit(&data);
	ERR_FAIL_COND_V_MSG(false, result != BK_SUCCESS, vformat("Failed to initialize BKData: %s.", BKStatusGetName(result)));

	return true;
}

Waveform::~Waveform() {
	BKDispose(&data);
}

bool Waveform::set_frames(const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
	const uint32_t frames_size = p_frames.size();
	ERR_FAIL_COND_V(frames_size < 2, false);
	ERR_FAIL_COND_V(frames_size > Waveform::WAVE_SIZE_MAX, false);

	p_amplitude = CLAMP(p_amplitude, 0.0, 1.0);

	const float *ptr = p_frames.ptr();
	const uint32_t size = MIN(frames_size, Waveform::WAVE_SIZE_MAX);
	float scale = 1.0;

	if (p_normalize) {
		float max_value = 0.0;
		for (uint32_t i = 0; i < size; i++) {
			max_value = MAX(max_value, ABS(ptr[i]));
		}

		scale = 0.0;
		if (not Math::is_zero_approx(max_value)) {
			scale = p_amplitude / max_value;
		}
	}

	BK_THREAD_SAFE_METHOD

	frames.resize(size);

	for (uint32_t i = 0; i < size; i++) {
		const float value = CLAMP(ptr[i] * scale, -1.0, +1.0);
		frames[i] = BKFrame(value * float(BK_FRAME_MAX));
	}

	const BKInt result = BKDataSetFrames(&data, frames.ptr(), frames.size(), 1, false);

	if (result != BK_SUCCESS) {
		ERR_FAIL_V_MSG(false, vformat("Failed to update BKData: %s.", BKStatusGetName(result)));
	}

	return true;
}

PackedFloat32Array Waveform::get_frames() const {
	PackedFloat32Array ret;

	ret.resize(frames.size());
	float *ptrw = ret.ptrw();
	const float scale = 1.0 / float(BK_FRAME_MAX);

	for (uint32_t i = 0; i < frames.size(); i++) {
		ptrw[i] = float(frames[i]) * scale;
	}

	return ret;
}
