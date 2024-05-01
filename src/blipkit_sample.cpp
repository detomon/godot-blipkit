#include "blipkit_sample.hpp"
#include "audio_stream_blipkit.hpp"
#include "strings.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

using namespace BlipKit;
using namespace godot;

BlipKitSample::BlipKitSample() {
	const BKInt result = BKDataInit(&data);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKData: %s.", BKStatusGetName(result)));
}

BlipKitSample::~BlipKitSample() {
	BK_THREAD_SAFE_METHOD

	BKDispose(&data);
}

Ref<BlipKitSample> BlipKitSample::create_with_wav(const Ref<AudioStreamWAV> &p_wav, bool p_normalize, float p_amplitude) {
	ERR_FAIL_COND_V(not p_wav.is_valid(), nullptr);

	const PackedByteArray &data = p_wav->get_data();
	const AudioStreamWAV::Format format = p_wav->get_format();
	const bool stereo = p_wav->is_stereo();
	const uint32_t data_size = data.size();
	const uint32_t size = data_size / (stereo ? 2 : 1);

	ERR_FAIL_COND_V(size < 2, nullptr);

	PackedFloat32Array frames;
	frames.resize(size);
	float *ptrw = frames.ptrw();

	switch (format) {
		case AudioStreamWAV::FORMAT_8_BITS: {
			const int8_t *ptr = reinterpret_cast<const int8_t *>(data.ptr());

			if (stereo) {
				constexpr float scale = 0.5 / float(INT8_MAX);

				// Merge to mono.
				for (uint32_t i = 0; i < size; i++) {
					const float left = float(ptr[i * 2 + 0]);
					const float right = float(ptr[i * 2 + 1]);
					ptrw[i] = (left + right) * scale;
				}
			} else {
				constexpr float scale = 1.0 / float(INT8_MAX);

				for (uint32_t i = 0; i < size; i++) {
					const float left = float(ptr[i]);
					ptrw[i] = left * scale;
				}
			}
		} break;
		case AudioStreamWAV::FORMAT_16_BITS: {
			const int16_t *ptr = reinterpret_cast<const int16_t *>(data.ptr());

			if (stereo) {
				constexpr float scale = 0.5 / float(INT16_MAX);

				// Merge to mono.
				for (uint32_t i = 0; i < size; i++) {
					const float left = float(ptr[i * 2 + 0]);
					const float right = float(ptr[i * 2 + 1]);
					ptrw[i] = (left + right) * scale;
				}
			} else {
				constexpr float scale = 1.0 / float(INT16_MAX);

				for (uint32_t i = 0; i < size; i++) {
					const float left = float(ptr[i]);
					ptrw[i] = left * scale;
				}
			}
		} break;
		default: {
			ERR_FAIL_V_MSG(nullptr, vformat("Unsupported AudioStreamWAV format: %d", format));
		} break;
	}

	Ref<BlipKitSample> instance;
	instance.instantiate();
	instance->set_frames(frames, p_normalize, p_amplitude);

	const AudioStreamWAV::LoopMode loop_mode = p_wav->get_loop_mode();
	int sustain_offset = p_wav->get_loop_begin();
	int sustain_end = p_wav->get_loop_end();
	BlipKitSample::RepeatMode repeat_mode = BlipKitSample::REPEAT_NONE;

	switch (loop_mode) {
		case AudioStreamWAV::LOOP_DISABLED: {
			repeat_mode = BlipKitSample::REPEAT_NONE;
		} break;
		case AudioStreamWAV::LOOP_FORWARD: {
			repeat_mode = BlipKitSample::REPEAT_FORWARD;
		} break;
		case AudioStreamWAV::LOOP_PINGPONG: {
			repeat_mode = BlipKitSample::REPEAT_PING_PONG;
		} break;
		case AudioStreamWAV::LOOP_BACKWARD: {
			repeat_mode = BlipKitSample::REPEAT_BACKWARD;
		} break;
	}

	instance->set_repeat_mode(repeat_mode);
	instance->set_sustain_offset(sustain_offset);
	instance->set_sustain_end(sustain_end);

	return instance;
}

void BlipKitSample::set_frames(const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
	const uint32_t frames_size = p_frames.size();
	ERR_FAIL_COND(frames_size < 2);

	p_amplitude = CLAMP(p_amplitude, 0.0, 1.0);

	const float *ptr = p_frames.ptr();
	float scale = 1.0;

	if (p_normalize) {
		float max_value = 0.0;
		for (uint32_t i = 0; i < frames_size; i++) {
			max_value = MAX(max_value, ABS(ptr[i]));
		}

		scale = 0.0;
		if (not Math::is_zero_approx(max_value)) {
			scale = p_amplitude / max_value;
		}
	}

	BK_THREAD_SAFE_METHOD

	frames.resize(frames_size);

	for (uint32_t i = 0; i < frames_size; i++) {
		const float value = CLAMP(ptr[i] * scale, -1.0, +1.0);
		frames[i] = BKFrame(value * float(BK_FRAME_MAX));
	}

	BKInt result = BKDataSetFrames(&data, frames.ptr(), frames.size(), 1, false);

	if (result != BK_SUCCESS) [[unlikely]] {
		ERR_FAIL_MSG(vformat("Failed to update BKData: %s.", BKStatusGetName(result)));
	}

	emit_changed();
}

PackedFloat32Array BlipKitSample::get_frames() const {
	const uint32_t frames_size = frames.size();
	constexpr float scale = 1.0 / float(BK_FRAME_MAX);

	PackedFloat32Array ret;
	ret.resize(frames_size);
	float *ptrw = ret.ptrw();

	for (uint32_t i = 0; i < frames_size; i++) {
		ptrw[i] = float(frames[i]) * scale;
	}

	return ret;
}

void BlipKitSample::set_sustain_offset(int p_sustain_offset) {
	if (p_sustain_offset < 0 && frames.size() > 0) {
		p_sustain_offset += frames.size() + 1;
	}
	p_sustain_offset = MAX(0, p_sustain_offset);
	if (frames.size() > 0) {
		p_sustain_offset = MIN(p_sustain_offset, frames.size());
	}

	sustain_offset = p_sustain_offset;
}

int BlipKitSample::get_sustain_offset() const {
	return sustain_offset;
}

void BlipKitSample::set_sustain_end(int p_sustain_end) {
	if (p_sustain_end < 0 && frames.size()) {
		p_sustain_end += frames.size() + 1;
	}
	p_sustain_end = MAX(0, p_sustain_end);
	if (frames.size() > 0) {
		p_sustain_end = MIN(p_sustain_end, frames.size());
	}

	sustain_end = p_sustain_end;
}

int BlipKitSample::get_sustain_end() const {
	return sustain_end;
}

void BlipKitSample::set_repeat_mode(RepeatMode p_repeat_mode) {
	ERR_FAIL_INDEX(p_repeat_mode, REPEAT_MAX);
	repeat_mode = p_repeat_mode;
}

BlipKitSample::RepeatMode BlipKitSample::get_repeat_mode() const {
	return repeat_mode;
}

void BlipKitSample::set_frame_bytes(const PackedByteArray &p_frames) {
	const uint32_t frame_count = p_frames.size() / sizeof(BKFrame);
	// TODO: Check for endianess.
	const BKFrame *ptr = reinterpret_cast<const BKFrame *>(p_frames.ptr());

	BK_THREAD_SAFE_METHOD

	frames.resize(frame_count);
	BKFrame *ptrw = frames.ptr();

	for (uint32_t i = 0; i < frame_count; i++) {
		ptrw[i] = ptr[i];
	}

	BKInt result = BKDataSetFrames(&data, frames.ptr(), frames.size(), 1, false);

	if (result != BK_SUCCESS) [[unlikely]] {
		ERR_FAIL_MSG(vformat("Failed to update BKData: %s.", BKStatusGetName(result)));
	}
}

PackedByteArray BlipKitSample::get_frame_bytes() const {
	const uint32_t byte_size = frames.size() * sizeof(frames[0]);
	PackedByteArray ret;

	ret.resize(byte_size);
	// TODO: Check for endianess.
	memcpy(ret.ptrw(), frames.ptr(), byte_size);

	return ret;
}

void BlipKitSample::_bind_methods() {
	ClassDB::bind_static_method("BlipKitSample", D_METHOD("create_with_wav", "wav", "normalize", "amplitude"), &BlipKitSample::create_with_wav, DEFVAL(false), DEFVAL(1.0));

	ClassDB::bind_method(D_METHOD("size"), &BlipKitSample::size);
	ClassDB::bind_method(D_METHOD("is_valid"), &BlipKitSample::is_valid);
	ClassDB::bind_method(D_METHOD("set_frames", "frames", "normalize", "amplitude"), &BlipKitSample::set_frames, DEFVAL(false), DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("get_frames"), &BlipKitSample::get_frames);
	ClassDB::bind_method(D_METHOD("set_sustain_offset", "sustain_offset"), &BlipKitSample::set_sustain_offset);
	ClassDB::bind_method(D_METHOD("get_sustain_offset"), &BlipKitSample::get_sustain_offset);
	ClassDB::bind_method(D_METHOD("get_sustain_end"), &BlipKitSample::get_sustain_end);
	ClassDB::bind_method(D_METHOD("set_sustain_end", "sustain_end"), &BlipKitSample::set_sustain_end);
	ClassDB::bind_method(D_METHOD("get_repeat_mode"), &BlipKitSample::get_repeat_mode);
	ClassDB::bind_method(D_METHOD("set_repeat_mode", "repeat_mode"), &BlipKitSample::set_repeat_mode);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "sustain_offset"), "set_sustain_offset", "get_sustain_offset");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "sustain_end"), "set_sustain_end", "get_sustain_end");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "repeat_mode", PROPERTY_HINT_ENUM, "None,Forward,Ping-Pong,Backward"), "set_repeat_mode", "get_repeat_mode");

	BIND_ENUM_CONSTANT(REPEAT_NONE);
	BIND_ENUM_CONSTANT(REPEAT_FORWARD);
	BIND_ENUM_CONSTANT(REPEAT_PING_PONG);
	BIND_ENUM_CONSTANT(REPEAT_BACKWARD);
}

String BlipKitSample::_to_string() const {
	return vformat("<BlipKitSample#%d>", get_instance_id());
}

void BlipKitSample::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "_frames", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
}

bool BlipKitSample::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == Strings::get()._frames) {
		set_frame_bytes(p_value);
	} else if (p_name == Strings::get().sustain_offset) {
		set_sustain_offset(p_value);
	} else if (p_name == Strings::get().sustain_end) {
		set_sustain_end(p_value);
	} else if (p_name == Strings::get().repeat_mode) {
		const int mode = p_value;
		set_repeat_mode(static_cast<RepeatMode>(mode));
	} else {
		return false;
	}

	return true;
}

bool BlipKitSample::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == Strings::get()._frames) {
		r_ret = get_frame_bytes();
	} else if (p_name == Strings::get().sustain_offset) {
		r_ret = get_sustain_offset();
	} else if (p_name == Strings::get().sustain_end) {
		r_ret = get_sustain_end();
	} else if (p_name == Strings::get().repeat_mode) {
		r_ret = get_repeat_mode();
	} else {
		return false;
	}

	return true;
}
