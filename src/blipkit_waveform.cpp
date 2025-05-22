#include "blipkit_waveform.hpp"
#include "audio_stream_blipkit.hpp"
#include "string_names.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>

using namespace BlipKit;
using namespace godot;

BlipKitWaveform::BlipKitWaveform() {
	BKInt result = BKDataInit(&data);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKData: %s.", BKStatusGetName(result)));

	frames.reserve(WAVE_SIZE_MAX);
	frames.resize(8);
	frames[0] = BK_FRAME_MAX;
	frames[1] = BK_FRAME_MAX;
	frames[2] = BK_FRAME_MAX;
	frames[3] = BK_FRAME_MAX;
	frames[4] = 0;
	frames[5] = 0;
	frames[6] = 0;
	frames[7] = 0;
}

BlipKitWaveform::~BlipKitWaveform() {
	BK_THREAD_SAFE_METHOD

	BKDispose(&data);
}

Ref<BlipKitWaveform> BlipKitWaveform::create_with_frames(const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
	const uint32_t frames_size = p_frames.size();
	ERR_FAIL_COND_V(frames_size < 2, nullptr);
	ERR_FAIL_COND_V(frames_size > WAVE_SIZE_MAX, nullptr);

	Ref<BlipKitWaveform> instance;
	instance.instantiate();
	instance->set_frames(p_frames, p_normalize, p_amplitude);

	return instance;
}

void BlipKitWaveform::set_frames(const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
	const uint32_t frames_size = p_frames.size();
	ERR_FAIL_COND(frames_size < 2);
	ERR_FAIL_COND(frames_size > WAVE_SIZE_MAX);

	p_amplitude = CLAMP(p_amplitude, 0.0, 1.0);

	const float *ptr = p_frames.ptr();
	const uint32_t size = MIN(frames_size, WAVE_SIZE_MAX);
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

	BKInt result = BKDataSetFrames(&data, frames.ptr(), frames.size(), 1, false);

	if (result != BK_SUCCESS) [[unlikely]] {
		ERR_FAIL_MSG(vformat("Failed to update BKData: %s.", BKStatusGetName(result)));
	}

	emit_changed();
}

PackedFloat32Array BlipKitWaveform::get_frames() const {
	PackedFloat32Array ret;

	ret.resize(frames.size());
	float *ptrw = ret.ptrw();
	const float scale = 1.0 / float(BK_FRAME_MAX);

	for (uint32_t i = 0; i < frames.size(); i++) {
		ptrw[i] = float(frames[i]) * scale;
	}

	return ret;
}

void BlipKitWaveform::_bind_methods() {
	ClassDB::bind_static_method("BlipKitWaveform", D_METHOD("create_with_frames", "frames", "normalize", "amplitude"), &BlipKitWaveform::create_with_frames, DEFVAL(false), DEFVAL(1.0));

	ClassDB::bind_method(D_METHOD("size"), &BlipKitWaveform::size);
	ClassDB::bind_method(D_METHOD("is_valid"), &BlipKitWaveform::is_valid);
	ClassDB::bind_method(D_METHOD("set_frames", "frames", "normalize", "amplitude"), &BlipKitWaveform::set_frames, DEFVAL(false), DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("get_frames"), &BlipKitWaveform::get_frames);

	BIND_CONSTANT(WAVE_SIZE_MAX);
}

String BlipKitWaveform::_to_string() const {
	return vformat("<BlipKitWaveform#%d>", get_instance_id());
}

void BlipKitWaveform::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "_frames", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
}

bool BlipKitWaveform::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == BKStringName(_frames)) {
		set_frames(p_value);
	} else {
		return false;
	}

	return true;
}

bool BlipKitWaveform::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == BKStringName(_frames)) {
		r_ret = get_frames();
	} else {
		return false;
	}

	return true;
}
