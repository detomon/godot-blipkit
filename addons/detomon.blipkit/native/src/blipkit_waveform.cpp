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
	ClassDB::bind_method(D_METHOD("set_frames", "frames", "normalize", "amplitude"), &BlipKitWaveform::set_frames, DEFVAL(false), DEFVAL(1.0));
}

String BlipKitWaveform::_to_string() const {
	return vformat("BlipKitWaveform: frames=%s", get_frames());
}

void BlipKitWaveform::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "frames", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
}

bool BlipKitWaveform::_set(const StringName &p_name, const Variant &p_value) {
	const String name = p_name;

	if (name == "frames") {
		set_frames(p_value);
		return true;
	}

	return false;
}

bool BlipKitWaveform::_get(const StringName &p_name, Variant &r_ret) const {
	const String name = p_name;

	if (name == "frames") {
		r_ret = get_frames();
		return true;
	}

	return false;
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

	BKInt result = BKDataSetFrames(&waveform, wave_frames, p_frames.size(), 1, true);
	if (result == BK_SUCCESS) {
		frames = p_frames;
	}

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
	instance->set_frames(p_frames, p_normalize, p_amplitude);

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

void BlipKitWaveform::set_frames(const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
	ERR_FAIL_COND(p_frames.size() < 2);
	ERR_FAIL_COND(p_frames.size() > BK_WAVE_MAX_LENGTH);

	p_amplitude = CLAMP(p_amplitude, 0.0, 1.0);

	const real_t *ptr = p_frames.ptr();
	LocalVector<real_t> frames_copy;
	frames_copy.resize(p_frames.size());

	if (p_normalize) {
		real_t max_value = 0.0;
		for (int i = 0; i < frames_copy.size(); i++) {
			frames_copy[i] = ptr[i];
			max_value = MAX(max_value, ABS(frames_copy[i]));
		}

		real_t factor = 0.0;
		if (!Math::is_zero_approx(max_value)) {
			factor = p_amplitude / max_value;
		}

		for (int i = 0; i < frames_copy.size(); i++) {
			frames_copy[i] = CLAMP(frames_copy[i] * factor, -1.0, +1.0);
		}
	} else {
		for (int i = 0; i < frames_copy.size(); i++) {
			frames_copy[i] = CLAMP(ptr[i], -1.0, +1.0);
		}
	}

	_update_waveform(frames_copy);
}
