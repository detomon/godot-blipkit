#include "blipkit_server.hpp"
#include "audio_stream_blipkit.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_sample.hpp"
#include "blipkit_track.hpp"
#include "blipkit_waveform.hpp"
#include <godot_cpp/core/error_macros.hpp>

using namespace BlipKit;
using namespace godot;

RID BlipKitServer::create_instrument() {
	const RID rid = instrument_owner.make_rid();
	Instrument *instance = instrument_owner.get_or_null(rid);
	ERR_FAIL_NULL_V(instance, RID());

	return rid;
}

RID BlipKitServer::create_sample() {
	const RID rid = sample_owner.make_rid();
	Sample *instance = sample_owner.get_or_null(rid);
	ERR_FAIL_NULL_V(instance, RID());

	return rid;
}

RID BlipKitServer::create_track() {
	const RID rid = track_owner.make_rid();
	Track *instance = track_owner.get_or_null(rid);
	ERR_FAIL_NULL_V(instance, RID());

	return rid;
}

RID BlipKitServer::create_waveform() {
	const RID rid = waveform_owner.make_rid();
	Waveform *instance = waveform_owner.get_or_null(rid);
	ERR_FAIL_NULL_V(instance, RID());

	if (not instance->initialize()) {
		waveform_owner.free(rid);
		return RID();
	}

	return rid;
}

RID BlipKitServer::create_divider(const RID &p_track, int p_tick_interval, const Callable &p_callable) {
	Track *track = track_owner.get_or_null(p_track);
	ERR_FAIL_NULL_V(track, RID());

	const RID rid = divider_owner.make_rid();
	Divider *instance = divider_owner.get_or_null(rid);
	ERR_FAIL_NULL_V(instance, RID());

	instance->initialize(p_callable, p_tick_interval);

	// TODO: Add divider to track.

	return rid;
}

bool BlipKitServer::Waveform::initialize() {
	BKInt result = BKDataInit(&data);
	ERR_FAIL_COND_V_MSG(false, result != BK_SUCCESS, vformat("Failed to initialize BKData: %s.", BKStatusGetName(result)));

	return true;
}

BlipKitServer::Waveform::~Waveform() {
	BKDispose(&data);
}

bool BlipKitServer::Waveform::set_frames(const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
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

bool BlipKitServer::waveform_set_frames(const RID &p_rid, const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
	Waveform *waveform = waveform_owner.get_or_null(p_rid);
	ERR_FAIL_NULL_V(waveform, false);

	return waveform->set_frames(p_frames, p_normalize, p_amplitude);
}

void BlipKitServer::free_rid(const RID &p_rid) {
	BK_THREAD_SAFE_METHOD

	if (track_owner.owns(p_rid)) {
		track_owner.free(p_rid);
	} else if (instrument_owner.owns(p_rid)) {
		instrument_owner.free(p_rid);
	} else if (sample_owner.owns(p_rid)) {
		sample_owner.free(p_rid);
	} else if (waveform_owner.owns(p_rid)) {
		waveform_owner.free(p_rid);
	} else if (divider_owner.owns(p_rid)) {
		divider_owner.free(p_rid);
	} else {
		ERR_FAIL_MSG("Invalid ID.");
	}
}

void BlipKitServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_instrument"), &BlipKitServer::create_instrument);
	ClassDB::bind_method(D_METHOD("create_sample"), &BlipKitServer::create_sample);
	ClassDB::bind_method(D_METHOD("create_track"), &BlipKitServer::create_track);
	ClassDB::bind_method(D_METHOD("create_waveform"), &BlipKitServer::create_waveform);
	ClassDB::bind_method(D_METHOD("create_divider", "track_rid", "tick_interval", "callback"), &BlipKitServer::create_divider);
	ClassDB::bind_method(D_METHOD("free_rid", "rid"), &BlipKitServer::free_rid);
}

String BlipKitServer::_to_string() const {
	return vformat("<BlipKitServer#%d>", get_instance_id());
}
