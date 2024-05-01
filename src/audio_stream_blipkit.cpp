#include "audio_stream_blipkit.hpp"
#include "blipkit_track.hpp"

using namespace BlipKit;
using namespace godot;

RecursiveMutex AudioStreamBlipKit::mutex;

AudioStreamBlipKit::AudioStreamBlipKit() {
	set_local_to_scene(true);
}

Ref<AudioStreamBlipKitPlayback> AudioStreamBlipKit::get_playback() {
	if (playback.is_valid()) {
		return playback;
	}

	playback.instantiate();

	if (not playback->initialize(clock_rate)) {
		playback.unref();
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

void AudioStreamBlipKit::set_clock_rate(int p_clock_rate) {
	clock_rate = CLAMP(p_clock_rate, CLOCK_RATE_MIN, CLOCK_RATE_MAX);

	if (playback.is_valid()) {
		playback->set_clock_rate(clock_rate);
	}
}

int AudioStreamBlipKit::get_clock_rate() const {
	return clock_rate;
}

void AudioStreamBlipKit::attach(BlipKitTrack *p_track) {
	get_playback()->attach(p_track);
}

void AudioStreamBlipKit::detach(BlipKitTrack *p_track) {
	get_playback()->detach(p_track);
}

void AudioStreamBlipKit::call_synced(const Callable &p_callable) {
	ERR_FAIL_COND(not p_callable.is_valid());

	get_playback()->call_synced(p_callable);
}

void AudioStreamBlipKit::_bind_methods() {
	ClassDB::bind_method(D_METHOD("call_synced", "callback"), &AudioStreamBlipKit::call_synced);
}

String AudioStreamBlipKit::_to_string() const {
	return vformat("<AudioStreamBlipKit#%d>", get_instance_id());
}

AudioStreamBlipKitPlayback::AudioStreamBlipKitPlayback() {
	const BKInt result = BKContextInit(&context, CHANNEL_COUNT, SAMPLE_RATE);

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKContext: %s.", BKStatusGetName(result)));

	const uint32_t buffer_size = CHANNEL_SIZE * CHANNEL_COUNT;
	buffer.resize(buffer_size);
}

AudioStreamBlipKitPlayback::~AudioStreamBlipKitPlayback() {
	BK_THREAD_SAFE_METHOD

	active = false;
	BKDispose(&context);

	for (BlipKitTrack *track : tracks) {
		track->detach();
	}
}

void AudioStreamBlipKitPlayback::_bind_methods() {
}

String AudioStreamBlipKitPlayback::_to_string() const {
	return vformat("<AudioStreamBlipKitPlayback#%d>", get_instance_id());
}

bool AudioStreamBlipKitPlayback::initialize(int p_clock_rate) {
	set_clock_rate(p_clock_rate);

	return true;
}

void AudioStreamBlipKitPlayback::set_clock_rate(int p_clock_rate) {
	BK_THREAD_SAFE_METHOD

	clock_rate = CLAMP(p_clock_rate, AudioStreamBlipKit::CLOCK_RATE_MIN, AudioStreamBlipKit::CLOCK_RATE_MAX);

	BKTime tick_rate = BKTimeFromSeconds(&context, 1.0 / double(p_clock_rate));
	const BKInt result = BKSetPtr(&context, BK_CLOCK_PERIOD, &tick_rate, sizeof(tick_rate));

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to set clock period: %s.", BKStatusGetName(result)));
}

int AudioStreamBlipKitPlayback::get_clock_rate() const {
	return clock_rate;
}

void AudioStreamBlipKitPlayback::call_synced(const Callable &p_callable) {
	BK_THREAD_SAFE_METHOD

	ERR_FAIL_COND(not p_callable.is_valid());

	if (active && not is_calling_callbacks) {
		sync_callables.push_back(p_callable);
	} else {
		p_callable.call();
	}
}

void AudioStreamBlipKitPlayback::attach(BlipKitTrack *p_track) {
	if (not tracks.has(p_track)) {
		tracks.push_back(p_track);
	}
}

void AudioStreamBlipKitPlayback::detach(BlipKitTrack *p_track) {
	tracks.erase(p_track);
}

void AudioStreamBlipKitPlayback::_start(double p_from_pos) {
	active = true;
}

void AudioStreamBlipKitPlayback::_stop() {
	active = false;
}

bool AudioStreamBlipKitPlayback::_is_playing() const {
	return active;
}

int32_t AudioStreamBlipKitPlayback::_mix_resampled(AudioFrame *p_buffer, int32_t p_frames) {
	BK_THREAD_SAFE_METHOD

	if (not active) {
		return 0;
	}

	// Call sync callbacks.
	if (not sync_callables.is_empty()) {
		is_calling_callbacks = true;

		for (const Callable &callable : sync_callables) {
			callable.call();
		}

		is_calling_callbacks = false;
		sync_callables.clear();
	}

	int32_t out_count = 0;
	AudioFrame *out_buffer = p_buffer;
	BKFrame *chunk_buffer = buffer.ptr();
	constexpr float frame_scale = 1.0 / float(BK_FRAME_MAX);

	while (out_count < p_frames) {
		BKInt chunk_size = MIN(p_frames - out_count, CHANNEL_SIZE);

		// Generate frames; produces no errors.
		chunk_size = BKContextGenerate(&context, chunk_buffer, chunk_size);

		// Nothing more to generate.
		if (chunk_size <= 0) {
			break;
		}

		// Fill output buffer.
		for (uint32_t i = 0; i < chunk_size; i++) {
			float left = float(buffer[i * CHANNEL_COUNT + 0]) * frame_scale;
			float right = float(buffer[i * CHANNEL_COUNT + 1]) * frame_scale;
			*out_buffer++ = { left, right };
		}

		out_count += chunk_size;
	}

	// Fill rest of output buffer if too few frames are generated.
	for (; out_count < p_frames; out_count++) {
		*out_buffer++ = { 0, 0 };
	}

	return out_count;
}

double AudioStreamBlipKitPlayback::_get_stream_sampling_rate() const {
	return double(SAMPLE_RATE);
}
