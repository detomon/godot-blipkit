#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>

#include "audio_stream_blipkit.hpp"
#include "blipkit_waveform.hpp"

using namespace detomon::BlipKit;
using namespace godot;

void BlipKitWaveform::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_frames"), &BlipKitWaveform::get_frames);
	ClassDB::bind_method(D_METHOD("set_frames", "frames", "normalize"), &BlipKitWaveform::set_frames, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("is_valid"), &BlipKitWaveform::is_valid);
}

String BlipKitWaveform::_to_string() const {
	return vformat("BlipKitWaveform: frames=%d", frames.size());
}

BlipKitWaveform::BlipKitWaveform() {
	BKInt result = BKDataInit(&waveform);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKData: %s.", BKStatusGetName(result)));
}

BlipKitWaveform::~BlipKitWaveform() {
	AudioStreamBlipKit::lock();
	BKDispose(&waveform);
	AudioStreamBlipKit::unlock();
}

PackedFloat32Array BlipKitWaveform::get_frames() {
	return frames;
}

void BlipKitWaveform::set_frames(PackedFloat32Array p_frames, bool p_normalize) {
	ERR_FAIL_COND(p_frames.size() < 2);
	ERR_FAIL_COND(p_frames.size() > BK_WAVE_MAX_LENGTH);

	PackedFloat32Array new_frames = p_frames;
	float *ptrw = new_frames.ptrw();

	if (p_normalize) {
		float max_value = 0.0;
		for (int i = 0; i < new_frames.size(); i++) {
			max_value = MAX(max_value, ABS(ptrw[i]));
		}

		if (!Math::is_zero_approx(max_value)) {
			for (int i = 0; i < new_frames.size(); i++) {
				ptrw[i] /= max_value;
			}
		}
	}

	BKFrame wave_frames[BK_WAVE_MAX_LENGTH];

	for (int i = 0; i < new_frames.size(); i++) {
		wave_frames[i] = (BKFrame)(CLAMP(ptrw[i], -1.0, +1.0) * (real_t)BK_FRAME_MAX);
	}

	AudioStreamBlipKit::lock();
	frames = new_frames;
	BKDataSetFrames(&waveform, wave_frames, frames.size(), 1, true);
	AudioStreamBlipKit::unlock();
}
