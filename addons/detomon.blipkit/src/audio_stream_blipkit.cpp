#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <stdio.h>
#include "audio_stream_blipkit.hpp"

using namespace godot;

void AudioStreamBlipKit::_bind_methods() {
	// ...
}

Ref<AudioStreamPlayback> AudioStreamBlipKit::_instantiate_playback() const {
	Ref<AudioStreamBlipKitPlayback> playback;
	playback.instantiate();
	playback->stream = Ref<AudioStreamBlipKit>(this);

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

AudioStreamBlipKitPlayback::AudioStreamBlipKitPlayback() {
	int sample_rate = ProjectSettings::get_singleton()->get_setting_with_override("audio/driver/mix_rate");
	BKInt result = BKContextInit(&context, NUM_CHANNELS, sample_rate);

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to initialize BKContext.");
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

	int channel_size = 4096;
	int buffer_size = channel_size * NUM_CHANNELS;
	int out_count = 0;
	AudioFrame* out_buffer = p_buffer;
	BKFrame buffer[buffer_size];

	while (out_count < p_frames) {
		int32_t chunk_size = MIN(p_frames - out_count, channel_size);
		// Produces no errors.
		BKInt generated = BKContextGenerate(&context, buffer, chunk_size);
		out_count += generated;

		// Nothing was generated.
		if (generated == 0) {
			break;
		}

		for (int i = 0; i < generated; i++) {
			out_buffer->left = (real_t)buffer[i * NUM_CHANNELS + 0] / (real_t)BK_FRAME_MAX;
			out_buffer->right = (real_t)buffer[i * NUM_CHANNELS + 1] / (real_t)BK_FRAME_MAX;
			out_buffer++;
		}
	}

	return out_count;
}
