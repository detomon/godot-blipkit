#include "blipkit_waveform.hpp"
#include "audio_stream_blipkit.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>

using namespace detomon::BlipKit;
using namespace godot;

void BlipKitWaveform::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_length"), &BlipKitWaveform::get_length);
	ClassDB::bind_method(D_METHOD("is_valid"), &BlipKitWaveform::is_valid);
	ClassDB::bind_method(D_METHOD("get_frames"), &BlipKitWaveform::get_frames);
	ClassDB::bind_method(D_METHOD("set_frames", "frames"), &BlipKitWaveform::set_frames);
	ClassDB::bind_method(D_METHOD("set_frames_normalized", "frames", "amplitude"), &BlipKitWaveform::set_frames_normalized);

	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "frames"), "set_frames", "get_frames");
}

String BlipKitWaveform::_to_string() const {
	return vformat("BlipKitWaveform: frames=%d", frames.size());
}

BlipKitWaveform::BlipKitWaveform() {
	BKInt result = BKDataInit(&waveform);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKData: %s.", BKStatusGetName(result)));

	frames.resize(2);
	frames[0] = 1.0;
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
		wave_frames[i] = BKFrame(ptr[i] * real_t(BK_FRAME_MAX));
	}

	AudioStreamBlipKit::lock();
	BKDataSetFrames(&waveform, wave_frames, frames.size(), 1, true);
	AudioStreamBlipKit::unlock();

	emit_changed();
}

PackedFloat32Array BlipKitWaveform::get_frames() {
	return frames;
}

void BlipKitWaveform::set_frames(PackedFloat32Array p_frames) {
	ERR_FAIL_COND(p_frames.size() < 2);
	ERR_FAIL_COND(p_frames.size() > BK_WAVE_MAX_LENGTH);

	LocalVector<real_t> frames_copy;
	real_t *ptrw = p_frames.ptrw();

	frames_copy.resize(p_frames.size());
	for (int i = 0; i < frames_copy.size(); i++) {
		frames_copy[i] = CLAMP(ptrw[i], -1.0, +1.0);
	}

	AudioStreamBlipKit::lock();

	frames.resize(frames_copy.size());
	for (int i = 0; i < frames_copy.size(); i++) {
		frames[i] = frames_copy[i];
	}

	AudioStreamBlipKit::unlock();

	_update_waveform();
}

void BlipKitWaveform::set_frames_normalized(PackedFloat32Array p_frames, float p_amplitude) {
	ERR_FAIL_COND(p_frames.size() < 2);
	ERR_FAIL_COND(p_frames.size() > BK_WAVE_MAX_LENGTH);

	p_amplitude = CLAMP(p_amplitude, 0.0, 1.0);

	real_t max_value = 0.0;
	LocalVector<real_t> frames_copy;
	real_t *ptrw = p_frames.ptrw();

	frames_copy.resize(p_frames.size());
	for (int i = 0; i < frames_copy.size(); i++) {
		frames_copy[i] = CLAMP(ptrw[i], -1.0, +1.0);
		max_value = MAX(max_value, ABS(frames_copy[i]));
	}

	if (!Math::is_zero_approx(max_value)) {
		real_t factor = p_amplitude / max_value;
		for (int i = 0; i < frames.size(); i++) {
			frames_copy[i] *= factor;
		}
	}

	AudioStreamBlipKit::lock();

	frames.resize(frames_copy.size());
	for (int i = 0; i < frames_copy.size(); i++) {
		frames[i] = frames_copy[i];
	}

	AudioStreamBlipKit::unlock();

	_update_waveform();
}
