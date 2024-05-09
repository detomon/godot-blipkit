#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>

#include "audio_stream_blipkit.hpp"
#include "blipkit_waveform.hpp"

using namespace detomon::BlipKit;
using namespace godot;

void BlipKitWaveform::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_length"), &BlipKitWaveform::get_length);
	ClassDB::bind_method(D_METHOD("is_valid"), &BlipKitWaveform::is_valid);
	ClassDB::bind_method(D_METHOD("get_frames"), &BlipKitWaveform::get_frames);
	ClassDB::bind_method(D_METHOD("set_frames", "frames"), &BlipKitWaveform::set_frames);
	ClassDB::bind_method(D_METHOD("normalize", "amplitude"), &BlipKitWaveform::normalize, DEFVAL(1.0));

	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "frames", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE), "set_frames", "get_frames");
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

void BlipKitWaveform::_update_waveform() {
	BKFrame wave_frames[BK_WAVE_MAX_LENGTH];
	const float *ptr = frames.ptr();

	for (int i = 0; i < frames.size(); i++) {
		wave_frames[i] = (BKFrame)(CLAMP(ptr[i], -1.0, +1.0) * (real_t)BK_FRAME_MAX);
	}

	AudioStreamBlipKit::lock();
	BKDataSetFrames(&waveform, wave_frames, frames.size(), 1, true);
	AudioStreamBlipKit::unlock();
}

PackedFloat32Array BlipKitWaveform::get_frames() {
	return frames;
}

void BlipKitWaveform::set_frames(PackedFloat32Array p_frames) {
	ERR_FAIL_COND(p_frames.size() < 2);
	ERR_FAIL_COND(p_frames.size() > BK_WAVE_MAX_LENGTH);

	// TODO: Needed?
	AudioStreamBlipKit::lock();
	frames = p_frames.duplicate();
	AudioStreamBlipKit::unlock();

	_update_waveform();
}

void BlipKitWaveform::normalize(float p_amplitude) {
	float *ptrw = frames.ptrw();
	float max_value = 0.0;

	for (int i = 0; i < frames.size(); i++) {
		max_value = MAX(max_value, ABS(ptrw[i]));
	}

	AudioStreamBlipKit::lock();

	if (!Math::is_zero_approx(max_value)) {
		for (int i = 0; i < frames.size(); i++) {
			ptrw[i] /= max_value;
		}
	}

	AudioStreamBlipKit::unlock();

	_update_waveform();
}
