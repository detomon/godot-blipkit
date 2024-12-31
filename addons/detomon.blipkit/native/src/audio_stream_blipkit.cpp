#include "audio_stream_blipkit.hpp"
#include "blipkit_track.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/memory.hpp>

using namespace detomon::BlipKit;
using namespace godot;

RecursiveSpinLock AudioStreamBlipKit::spin_lock;

void AudioStreamBlipKit::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_clock_rate"), &AudioStreamBlipKit::set_clock_rate);
	ClassDB::bind_method(D_METHOD("get_clock_rate"), &AudioStreamBlipKit::get_clock_rate);
	ClassDB::bind_method(D_METHOD("call_synced", "callback"), &AudioStreamBlipKit::call_synced);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "clock_rate", PROPERTY_HINT_RANGE, vformat("%d,%d,1", MIN_CLOCK_RATE, MAX_CLOCK_RATE)), "set_clock_rate", "get_clock_rate");
}

String AudioStreamBlipKit::_to_string() const {
	return vformat("AudioStreamBlipKit: clock_rate=%d", clock_rate);
}

Ref<AudioStreamBlipKitPlayback> AudioStreamBlipKit::get_playback() {
	if (playback.is_valid()) {
		return playback;
	}

	playback.instantiate();

	if (!playback->initialize(clock_rate)) {
		ERR_FAIL_V_MSG(playback, "Could not initialize AudioStreamBlipKitPlayback.");
	}

	return playback;
}

Ref<AudioStreamPlayback> AudioStreamBlipKit::_instantiate_playback() const {
	return const_cast<AudioStreamBlipKit *>(this)->get_playback();
}

String AudioStreamBlipKit::_get_stream_name() const {
	return "BlipKit";
}

double AudioStreamBlipKit::_get_length() const {
	return 0.0;
}

bool AudioStreamBlipKit::_is_monophonic() const {
	return true;
}

int AudioStreamBlipKit::get_clock_rate() const {
	return clock_rate;
}

void AudioStreamBlipKit::set_clock_rate(int p_clock_rate) {
	clock_rate = CLAMP(p_clock_rate, MIN_CLOCK_RATE, MAX_CLOCK_RATE);

	if (playback.is_valid()) {
		playback->set_clock_rate(clock_rate);
	}
}

void AudioStreamBlipKit::attach(BlipKitTrack *p_track) {
	get_playback()->attach(p_track);
}

void AudioStreamBlipKit::detach(BlipKitTrack *p_track) {
	get_playback()->detach(p_track);
}

void AudioStreamBlipKit::call_synced(Callable p_callable) {
	ERR_FAIL_COND(!p_callable.is_valid());

	get_playback()->call_synced(p_callable);
}

int AudioStreamBlipKitPlayback::divider_id = 0;

AudioStreamBlipKitPlayback::AudioStreamBlipKitPlayback() {
	const int sample_rate = ProjectSettings::get_singleton()->get_setting_with_override("audio/driver/mix_rate");
	const BKInt result = BKContextInit(&context, NUM_CHANNELS, sample_rate);

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKContext: %s.", BKStatusGetName(result)));

	const int buffer_size = CHANNEL_SIZE * NUM_CHANNELS;
	buffer.resize(buffer_size);
}

AudioStreamBlipKitPlayback::~AudioStreamBlipKitPlayback() {
	active = false;

	AudioStreamBlipKit::lock();

	clear_dividers(); // Explicitly clear functions to free memory.
	BKDispose(&context);

	for (BlipKitTrack *track : tracks) {
		track->detach();
	}

	AudioStreamBlipKit::unlock();
}

void AudioStreamBlipKitPlayback::_bind_methods() {
}

String AudioStreamBlipKitPlayback::_to_string() const {
	return "AudioStreamBlipKitPlayback";
}

bool AudioStreamBlipKitPlayback::initialize(int p_clock_rate) {
	set_clock_rate(p_clock_rate);

	return true;
}

void AudioStreamBlipKitPlayback::set_clock_rate(int p_clock_rate) {
	clock_rate = CLAMP(p_clock_rate, AudioStreamBlipKit::MIN_CLOCK_RATE, AudioStreamBlipKit::MAX_CLOCK_RATE);

	BKTime tick_rate = BKTimeFromSeconds(&context, 1.0 / double(p_clock_rate));

	AudioStreamBlipKit::lock();
	BKInt result = BKSetPtr(&context, BK_CLOCK_PERIOD, &tick_rate, sizeof(tick_rate));
	AudioStreamBlipKit::unlock();

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to set clock period: %s.", BKStatusGetName(result)));
}

int AudioStreamBlipKitPlayback::get_clock_rate() const {
	return clock_rate;
}

void AudioStreamBlipKitPlayback::call_synced(Callable p_callable) {
	ERR_FAIL_COND(!p_callable.is_valid());

	AudioStreamBlipKit::lock();

	if (active) {
		sync_callables.push_back(p_callable);
	} else {
		p_callable.call();
	}

	AudioStreamBlipKit::unlock();
}

void AudioStreamBlipKitPlayback::call_synced_callables() {
	for (Callable &callable : sync_callables) {
		callable.call();
	}

	sync_callables.clear();
}

void AudioStreamBlipKitPlayback::attach(BlipKitTrack *p_track) {
	if (!tracks.has(p_track)) {
		tracks.push_back(p_track);
	}
}

void AudioStreamBlipKitPlayback::detach(BlipKitTrack *p_track) {
	tracks.erase(p_track);
}

void AudioStreamBlipKitPlayback::_start(double p_from_pos) {
	if (active) {
		return;
	}

	active = true;
}

void AudioStreamBlipKitPlayback::_stop() {
	if (!active) {
		return;
	}

	active = false;
}

bool AudioStreamBlipKitPlayback::_is_playing() const {
	return active;
}

int32_t AudioStreamBlipKitPlayback::_mix(AudioFrame *p_buffer, double p_rate_scale, int32_t p_frames) {
	if (!active) {
		return 0;
	}

	AudioStreamBlipKit::lock();
	call_synced_callables();
	AudioStreamBlipKit::unlock();

	int out_count = 0;

	AudioFrame *out_buffer = p_buffer;
	BKFrame *chunk_buffer = buffer.ptr();

	while (out_count < p_frames) {
		BKInt chunk_size = MIN(p_frames - out_count, CHANNEL_SIZE);

		AudioStreamBlipKit::lock();
		// Generate frames; produces no errors.
		chunk_size = BKContextGenerate(&context, chunk_buffer, chunk_size);
		AudioStreamBlipKit::unlock();

		// Nothing was generated.
		if (chunk_size <= 0) {
			break;
		}

		out_count += chunk_size;

		// Fill output buffer.
		for (int i = 0; i < chunk_size; i++) {
			float left = float(buffer[i * NUM_CHANNELS + 0]) / float(BK_FRAME_MAX);
			float right = float(buffer[i * NUM_CHANNELS + 1]) / float(BK_FRAME_MAX);
			*out_buffer++ = { left, right };
		}
	}

	// Fill rest of output buffer if too few frames are generated.
	for (; out_count < p_frames; out_count++) {
		*out_buffer++ = { 0, 0 };
	}

	return out_count;
}

int AudioStreamBlipKitPlayback::add_divider(Callable p_callable, int p_tick_interval) {
	ERR_FAIL_COND_V(p_tick_interval <= 0, -1);

	AudioStreamBlipKit::lock();

	Divider *divider = memnew(Divider);
	divider->initialize(p_tick_interval, p_callable);
	divider->attach(this);

	divider_id++;
	dividers[divider_id] = divider;

	AudioStreamBlipKit::unlock();

	return divider_id;
}

void AudioStreamBlipKitPlayback::remove_divider(int p_id) {
	ERR_FAIL_COND(!dividers.has(p_id));

	AudioStreamBlipKit::lock();

	memdelete(dividers[p_id]);
	dividers.erase(p_id);

	AudioStreamBlipKit::unlock();
}

void AudioStreamBlipKitPlayback::clear_dividers() {
	AudioStreamBlipKit::lock();

	for (KeyValue<int, Divider *> &E : dividers) {
		memdelete(E.value);
	}
	dividers.clear();

	AudioStreamBlipKit::unlock();
}

void AudioStreamBlipKitPlayback::reset_divider(int p_id, int p_tick_interval) {
	ERR_FAIL_COND(!dividers.has(p_id));

	AudioStreamBlipKit::lock();
	dividers[p_id]->reset(p_tick_interval);
	AudioStreamBlipKit::unlock();
}

void AudioStreamBlipKitPlayback::enable_divider(int p_id, bool p_enable) {
	AudioStreamBlipKit::lock();

	if (dividers.has(p_id)) {
		Divider *divider = dividers[p_id];

		if (p_enable) {
			divider->detach();
		} else {
			divider->attach(this);
		}
	}

	AudioStreamBlipKit::unlock();
}
