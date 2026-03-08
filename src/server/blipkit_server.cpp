#include "blipkit_server.hpp"
#include "audio_stream_blipkit.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_sample.hpp"
#include "blipkit_track.hpp"
#include "blipkit_waveform.hpp"
#include <godot_cpp/core/error_macros.hpp>

using namespace BlipKit;
using namespace godot;

void BlipKitServer::create() {
	if (not singleton) {
		singleton = memnew(BlipKitServer);
	}
}

void BlipKitServer::free() {
	if (singleton) {
		memdelete(singleton);
		singleton = nullptr;
	}
}

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

bool BlipKitServer::waveform_set_frames(const RID &p_rid, const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
	Waveform *waveform = waveform_owner.get_or_null(p_rid);
	ERR_FAIL_NULL_V(waveform, false);

	return waveform->set_frames(p_frames, p_normalize, p_amplitude);
}

PackedFloat32Array BlipKitServer::waveform_get_frames(const RID &p_rid) const {
	Waveform *waveform = waveform_owner.get_or_null(p_rid);
	ERR_FAIL_NULL_V(waveform, {});

	return waveform->get_frames();
}

int BlipKitServer::waveform_get_size(const RID &p_rid) const {
	Waveform *waveform = waveform_owner.get_or_null(p_rid);
	ERR_FAIL_NULL_V(waveform, 0);

	return waveform->size();
}

bool BlipKitServer::waveform_get_is_valid(const RID &p_rid) const {
	Waveform *waveform = waveform_owner.get_or_null(p_rid);
	ERR_FAIL_NULL_V(waveform, false);

	return waveform->is_valid();
}

BKData *BlipKitServer::waveform_get_data(const RID &p_rid) const {
	Waveform *waveform = waveform_owner.get_or_null(p_rid);
	ERR_FAIL_NULL_V(waveform, nullptr);

	return waveform->get_data();
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

	ClassDB::bind_method(D_METHOD("waveform_set_frames", "rid", "frames", "normalize", "amplitude"), &BlipKitServer::waveform_set_frames, DEFVAL(false), DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("waveform_get_frames", "rid"), &BlipKitServer::waveform_get_frames);

	ClassDB::bind_method(D_METHOD("waveform_get_size", "rid"), &BlipKitServer::waveform_get_size);
	ClassDB::bind_method(D_METHOD("waveform_get_is_valid", "rid"), &BlipKitServer::waveform_get_is_valid);

	ClassDB::bind_method(D_METHOD("free_rid", "rid"), &BlipKitServer::free_rid);
}

String BlipKitServer::_to_string() const {
	return vformat("<BlipKitServer#%d>", get_instance_id());
}
