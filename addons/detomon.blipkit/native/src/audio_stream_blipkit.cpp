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

	ADD_PROPERTY(PropertyInfo(Variant::INT, "clock_rate", PROPERTY_HINT_RANGE, vformat("%d,%d,1", MIN_CLOCK_RATE, MAX_CLOCK_RATE)), "set_clock_rate", "get_clock_rate");
}

String AudioStreamBlipKit::_to_string() const {
	return vformat("AudioStreamBlipKit: clock_rate=%d", clock_rate);
}

Ref<AudioStreamPlayback> AudioStreamBlipKit::_instantiate_playback() const {
	Ref<AudioStreamBlipKitPlayback> playback;
	playback.instantiate();

	if (!playback->initialize(Ref<AudioStreamBlipKit>(this))) {
		return nullptr;
	}

	return playback;
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

int AudioStreamBlipKit::get_clock_rate() {
	return clock_rate;
}

void AudioStreamBlipKit::set_clock_rate(int p_clock_rate) {
	clock_rate = CLAMP(p_clock_rate, MIN_CLOCK_RATE, MAX_CLOCK_RATE);
}

int AudioStreamBlipKitPlayback::divider_id = 0;

AudioStreamBlipKitPlayback::AudioStreamBlipKitPlayback() {
	int sample_rate = ProjectSettings::get_singleton()->get_setting_with_override("audio/driver/mix_rate");
	BKInt result = BKContextInit(&context, NUM_CHANNELS, sample_rate);

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKContext: %s.", BKStatusGetName(result)));
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
	ClassDB::bind_method(D_METHOD("call_synced", "callback"), &AudioStreamBlipKitPlayback::call_synced);
	// ClassDB::bind_method(D_METHOD("add_divider", "callable", "tick_interval"), &AudioStreamBlipKitPlayback::add_divider);
	// ClassDB::bind_method(D_METHOD("remove_divider", "id"), &AudioStreamBlipKitPlayback::remove_divider);
	// ClassDB::bind_method(D_METHOD("clear_dividers"), &AudioStreamBlipKitPlayback::clear_dividers);
	// ClassDB::bind_method(D_METHOD("reset_divider", "id", "tick_interval"), &AudioStreamBlipKitPlayback::reset_divider, DEFVAL(0));
}

String AudioStreamBlipKitPlayback::_to_string() const {
	return "AudioStreamBlipKitPlayback";
}

bool AudioStreamBlipKitPlayback::initialize(Ref<AudioStreamBlipKit> p_stream) {
	stream = p_stream;

	int clock_rate = stream->get_clock_rate();
	BKTime tick_rate = BKTimeFromSeconds(&context, 1.0 / double(clock_rate));

	BKInt result = BKSetPtr(&context, BK_CLOCK_PERIOD, &tick_rate, sizeof(tick_rate));
	ERR_FAIL_COND_V_MSG(result != BK_SUCCESS, false, vformat("Failed to set clock period: %s.", BKStatusGetName(result)));

	return true;
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

	// AudioStreamBlipKit::lock();
	// BKContextReset(&context);
	// AudioStreamBlipKit::unlock();
}

bool AudioStreamBlipKitPlayback::_is_playing() const {
	return active;
}

int32_t AudioStreamBlipKitPlayback::_mix(AudioFrame *p_buffer, double p_rate_scale, int32_t p_frames) {
	ERR_FAIL_COND_V(!active, 0);

	int channel_size = 1024;
	int buffer_size = channel_size * NUM_CHANNELS;
	int out_count = 0;

	thread_local LocalVector<BKFrame> buffer;
	buffer.resize(buffer_size);
	AudioFrame *out_buffer = p_buffer;

	while (out_count < p_frames) {
		BKInt chunk_size = MIN(p_frames - out_count, channel_size);

		AudioStreamBlipKit::lock();

		// Generate frames; produces no errors.
		chunk_size = BKContextGenerate(&context, buffer.ptr(), chunk_size);

		AudioStreamBlipKit::unlock();

		// Nothing was generated.
		if (chunk_size <= 0) {
			break;
		}

		out_count += chunk_size;

		// Fill output buffer.
		for (int i = 0; i < chunk_size; i++) {
			float left = real_t(buffer[i * NUM_CHANNELS + 0]) / real_t(BK_FRAME_MAX);
			float right = real_t(buffer[i * NUM_CHANNELS + 1]) / real_t(BK_FRAME_MAX);
			*out_buffer++ = { left, right };
		}
	}

	// Fill rest of output buffer if too few frames are generated.
	for (; out_count < p_frames; out_count++) {
		*out_buffer++ = { 0, 0 };
	}

	return out_count;
}

void AudioStreamBlipKitPlayback::call_synced(Callable p_callable) const {
	ERR_FAIL_COND(!p_callable.is_valid());

	AudioStreamBlipKit::lock();
	p_callable.call();
	AudioStreamBlipKit::unlock();
}

int AudioStreamBlipKitPlayback::add_divider(Callable p_callable, int p_tick_interval) {
	ERR_FAIL_COND_V(p_tick_interval <= 0, -1);

	AudioStreamBlipKit::lock();

	Divider *divider = memnew(Divider);
	divider->initialize(p_callable, p_tick_interval);
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
