#include "blipkit_waveform.hpp"
#include "audio_stream_blipkit.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>

using namespace detomon::BlipKit;
using namespace godot;

void BlipKitWaveform::_bind_methods() {
	ClassDB::bind_static_method("BlipKitWaveform", D_METHOD("create_with_frames", "frames", "normalize", "amplitude"), &BlipKitWaveform::create_with_frames, DEFVAL(false), DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("size"), &BlipKitWaveform::size);
	ClassDB::bind_method(D_METHOD("is_valid"), &BlipKitWaveform::is_valid);
	ClassDB::bind_method(D_METHOD("get_frames"), &BlipKitWaveform::get_frames);
	ClassDB::bind_method(D_METHOD("set_frames", "frames"), &BlipKitWaveform::set_frames);
	ClassDB::bind_method(D_METHOD("set_frames_normalized", "frames", "amplitude"), &BlipKitWaveform::set_frames_normalized, DEFVAL(1.0));

	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "frames"), "set_frames", "get_frames");
}

String BlipKitWaveform::_to_string() const {
	return vformat("BlipKitWaveform: frames=%s", get_frames());
}

BlipKitWaveform::BlipKitWaveform() {
	BKInt result = BKDataInit(&waveform);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKData: %s.", BKStatusGetName(result)));

	frames.resize(2);
	frames[0] = 1.0;
	frames[1] = 0.0;
}

BlipKitWaveform::~BlipKitWaveform() {
	AudioStreamBlipKit::lock();
	BKDispose(&waveform);
	AudioStreamBlipKit::unlock();
}

void BlipKitWaveform::_update_waveform(const LocalVector<real_t> &p_frames) {
	BKFrame wave_frames[BK_WAVE_MAX_LENGTH];
	const real_t *ptr = p_frames.ptr();

	for (int i = 0; i < p_frames.size(); i++) {
		wave_frames[i] = BKFrame(ptr[i] * real_t(BK_FRAME_MAX));
	}

	AudioStreamBlipKit::lock();

	frames = p_frames;
	BKInt result = BKDataSetFrames(&waveform, wave_frames, p_frames.size(), 1, true);

	AudioStreamBlipKit::unlock();

	if (result != BK_SUCCESS) {
		ERR_FAIL_MSG(vformat("Failed to update waveform: %s.", BKStatusGetName(result)));
	}

	emit_changed();
}

Ref<BlipKitWaveform> BlipKitWaveform::create_with_frames(const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
	ERR_FAIL_COND_V(p_frames.size() < 2, nullptr);
	ERR_FAIL_COND_V(p_frames.size() > BK_WAVE_MAX_LENGTH, nullptr);

	Ref<BlipKitWaveform> instance;
	instance.instantiate();

	if (p_normalize) {
		instance->set_frames_normalized(p_frames, p_amplitude);
	} else {
		instance->set_frames(p_frames);
	}

	return instance;
}

PackedFloat32Array BlipKitWaveform::get_frames() const {
	PackedFloat32Array ret;

	AudioStreamBlipKit::lock();

	ret.resize(frames.size());
	for (int i = 0; i < frames.size(); i++) {
		ret[i] = frames[i];
	}

	AudioStreamBlipKit::unlock();

	return ret;
}

void BlipKitWaveform::set_frames(const PackedFloat32Array &p_frames) {
	ERR_FAIL_COND(p_frames.size() < 2);
	ERR_FAIL_COND(p_frames.size() > BK_WAVE_MAX_LENGTH);

	LocalVector<real_t> frames_copy;
	const real_t *ptr = p_frames.ptr();

	frames_copy.resize(p_frames.size());
	for (int i = 0; i < frames_copy.size(); i++) {
		frames_copy[i] = CLAMP(ptr[i], -1.0, +1.0);
	}

	_update_waveform(frames_copy);
}

void BlipKitWaveform::set_frames_normalized(const PackedFloat32Array &p_frames, float p_amplitude) {
	ERR_FAIL_COND(p_frames.size() < 2);
	ERR_FAIL_COND(p_frames.size() > BK_WAVE_MAX_LENGTH);

	p_amplitude = CLAMP(p_amplitude, 0.0, 1.0);

	LocalVector<real_t> frames_copy;
	const real_t *ptr = p_frames.ptr();
	real_t max_value = 0.0;

	frames_copy.resize(p_frames.size());
	for (int i = 0; i < frames_copy.size(); i++) {
		frames_copy[i] = CLAMP(ptr[i], -1.0, +1.0);
		max_value = MAX(max_value, ABS(frames_copy[i]));
	}

	if (!Math::is_zero_approx(max_value)) {
		real_t factor = p_amplitude / max_value;
		for (int i = 0; i < frames.size(); i++) {
			frames_copy[i] *= factor;
		}
	}

	_update_waveform(frames_copy);
}
