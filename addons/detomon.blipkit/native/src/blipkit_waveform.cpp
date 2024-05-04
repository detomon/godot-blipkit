#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>

#include "audio_stream_blipkit.hpp"
#include "blipkit_waveform.hpp"

using namespace detomon::BlipKit;
using namespace godot;

void BlipKitWaveform::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_frames"), &BlipKitWaveform::get_frames);
	ClassDB::bind_method(D_METHOD("set_frames"), &BlipKitWaveform::set_frames);

	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "frames"), "set_frames", "get_frames");
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

void BlipKitWaveform::set_frames(PackedFloat32Array p_frames) {
	ERR_FAIL_COND(p_frames.size() < 2);
	ERR_FAIL_COND(p_frames.size() > BK_WAVE_MAX_LENGTH);

	frames = p_frames.duplicate();

	// Normalize frames.
	{
		float max_value = 0.0;

		for (int i = 0; i < frames.size(); i++) {
			max_value = MAX(max_value, ABS(frames[i]));
		}

		float *ptrw = frames.ptrw();
		for (int i = 0; i < frames.size(); i++) {
			frames[i] /= max_value;
		}
	}

	Vector<BKFrame> wave_frames;
	wave_frames.resize(p_frames.size());
	BKFrame *ptrw = wave_frames.ptrw();

	for (int i = 0; i < p_frames.size(); i++) {
		ptrw[i] = (BKFrame)(CLAMP(frames[i], -1.0, +1.0) * (real_t)BK_FRAME_MAX);
	}

	AudioStreamBlipKit::lock();
	BKDataSetFrames(&waveform, ptrw, wave_frames.size(), 1, true);
	AudioStreamBlipKit::unlock();
}
