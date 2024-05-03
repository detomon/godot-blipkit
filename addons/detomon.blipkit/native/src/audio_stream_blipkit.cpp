#include "audio_stream_blipkit.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/project_settings.hpp>

using namespace godot;

void AudioStreamBlipKit::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_clock_rate"), &AudioStreamBlipKit::get_clock_rate);
	ClassDB::bind_method(D_METHOD("set_clock_rate"), &AudioStreamBlipKit::set_clock_rate);
	ClassDB::bind_method(D_METHOD("set_generate_always"), &AudioStreamBlipKit::set_generate_always);
	ClassDB::bind_method(D_METHOD("is_always_generating"), &AudioStreamBlipKit::is_always_generating);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "clock_rate", PROPERTY_HINT_RANGE, vformat("%d,%d,1", MIN_CLOCK_RATE, MAX_CLOCK_RATE)), "set_clock_rate", "get_clock_rate");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "always_generate"), "set_generate_always", "is_always_generating");
}

String AudioStreamBlipKit::_to_string() const {
	return "AudioStreamBlipKit";
}

Ref<AudioStreamPlayback> AudioStreamBlipKit::_instantiate_playback() const {
	Ref<AudioStreamBlipKitPlayback> playback;
	playback.instantiate();
	playback->set_stream(Ref<AudioStreamBlipKit>(this));
	playback->always_generate = always_generate;

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

bool AudioStreamBlipKit::is_always_generating() {
	return always_generate;
}

void AudioStreamBlipKit::set_generate_always(bool p_always_generate) {
	always_generate = p_always_generate;
}

AudioStreamBlipKitPlayback::AudioStreamBlipKitPlayback() {
	int sample_rate = ProjectSettings::get_singleton()->get_setting_with_override("audio/driver/mix_rate");

	BKInt result = BKContextInit(&context, NUM_CHANNELS, sample_rate);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKContext: %s.", BKStatusGetName(result)));
}

AudioStreamBlipKitPlayback::~AudioStreamBlipKitPlayback() {
	active = false;
	AudioServer::get_singleton()->lock();
	BKDispose(&context);
	AudioServer::get_singleton()->unlock();
}

void AudioStreamBlipKitPlayback::_bind_methods() {
	// ...
}

String AudioStreamBlipKitPlayback::_to_string() const {
	return "AudioStreamBlipKitPlayback";
}

void AudioStreamBlipKitPlayback::set_stream(Ref<AudioStreamBlipKit> p_stream) {
	stream = p_stream;

	int clock_rate = stream->get_clock_rate();
	BKTime tick_rate = BKTimeFromSeconds(&context, 1.0 / (real_t)clock_rate);

	BKInt result = BKSetPtr(&context, BK_CLOCK_PERIOD, &tick_rate, sizeof(tick_rate));
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to set clock period: %s.", BKStatusGetName(result)));
}

void AudioStreamBlipKitPlayback::_start(double p_from_pos) {
	active = true;
}

void AudioStreamBlipKitPlayback::_stop() {
	active = false;

	AudioServer::get_singleton()->lock();
	BKContextReset(&context);
	AudioServer::get_singleton()->unlock();
}

bool AudioStreamBlipKitPlayback::_is_playing() const {
	return active;
}

int32_t AudioStreamBlipKitPlayback::_mix(AudioFrame *p_buffer, double p_rate_scale, int32_t p_frames) {
	ERR_FAIL_COND_V(!active, 0);

	int channel_size = 2048;
	int buffer_size = channel_size * NUM_CHANNELS;
	int out_count = 0;
	AudioFrame *out_buffer = p_buffer;
	BKFrame buffer[buffer_size];

	while (out_count < p_frames) {
		int32_t chunk_size = MIN(p_frames - out_count, channel_size);

		// Produces no error.
		BKInt generated = BKContextGenerate(&context, buffer, chunk_size);
		out_count += generated;

		// Nothing was generated.
		if (generated == 0) {
			break;
		}

		// Fill output buffer.
		for (int i = 0; i < generated; i++) {
			out_buffer->left = (real_t)buffer[i * NUM_CHANNELS + 0] / (real_t)BK_FRAME_MAX;
			out_buffer->right = (real_t)buffer[i * NUM_CHANNELS + 1] / (real_t)BK_FRAME_MAX;
			out_buffer++;
		}
	}

	// Fill rest of output buffer, even if nothing was produced.
	if (out_count < p_frames && always_generate) {
		while (out_count < p_frames) {
			*out_buffer = { 0, 0 };
			out_buffer++;
			out_count++;
		}
	}

	return out_count;
}
