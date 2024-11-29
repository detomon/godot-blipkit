#include "audio_stream_blipkit.hpp"
#include <atomic>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/project_settings.hpp>

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

AudioStreamBlipKitPlayback::AudioStreamBlipKitPlayback() {
	int sample_rate = ProjectSettings::get_singleton()->get_setting_with_override("audio/driver/mix_rate");
	BKInt result = BKContextInit(&context, NUM_CHANNELS, sample_rate);

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKContext: %s.", BKStatusGetName(result)));
}

AudioStreamBlipKitPlayback::~AudioStreamBlipKitPlayback() {
	active = false;
	AudioStreamBlipKit::lock();
	BKDispose(&context);
	AudioStreamBlipKit::unlock();
}

void AudioStreamBlipKitPlayback::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_tick_function", "callable", "ticks"), &AudioStreamBlipKitPlayback::add_tick_function);
	ClassDB::bind_method(D_METHOD("remove_tick_function", "callable"), &AudioStreamBlipKitPlayback::remove_tick_function);
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

	AudioStreamBlipKit::lock();
	BKContextReset(&context);
	AudioStreamBlipKit::unlock();
}

bool AudioStreamBlipKitPlayback::_is_playing() const {
	return active;
}

int32_t AudioStreamBlipKitPlayback::_mix(AudioFrame *p_buffer, double p_rate_scale, int32_t p_frames) {
	ERR_FAIL_COND_V(!active, 0);

	int channel_size = 1024;
	int buffer_size = channel_size * NUM_CHANNELS;
	int out_count = 0;

	BKFrame buffer[buffer_size];
	AudioFrame *out_buffer = p_buffer;

	AudioStreamBlipKit::lock();

	while (out_count < p_frames) {
		BKInt chunk_size = MIN(p_frames - out_count, channel_size);

		// Generate frames; produces no errors.
		chunk_size = BKContextGenerate(&context, buffer, chunk_size);

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

	AudioStreamBlipKit::unlock();

	// Fill rest of output buffer if too few frames are generated.
	for (; out_count < p_frames; out_count++) {
		*out_buffer++ = { 0, 0 };
	}

	return out_count;
}

void AudioStreamBlipKitPlayback::add_tick_function(Callable p_callable, int p_ticks) {
	ERR_FAIL_COND(p_ticks <= 0);

	uint32_t hash = p_callable.hash();

	AudioStreamBlipKit::lock();

	if (!tick_functions.has(hash)) {
		TickFunction &function = tick_functions[hash];
		function.initialize(p_callable, p_ticks, this);
	}

	AudioStreamBlipKit::unlock();
}

void AudioStreamBlipKitPlayback::remove_tick_function(Callable p_callable) {
	uint32_t hash = p_callable.hash();

	AudioStreamBlipKit::lock();
	tick_functions.erase(hash);
	AudioStreamBlipKit::unlock();
}

BKEnum AudioStreamBlipKitPlayback::TickFunction::divider_callback(BKCallbackInfo *p_info, void *p_user_info) {
	TickFunction *function = static_cast<TickFunction *>(p_user_info);
	int ticks = function->callable.call(p_info->divider);

	// Update ticks if callable returned value.
	if (ticks > 0) {
		p_info->divider = ticks;
	}

	return BK_SUCCESS;
}

void AudioStreamBlipKitPlayback::TickFunction::initialize(Callable &p_callable, int p_ticks, AudioStreamBlipKitPlayback *p_playback) {
	ERR_FAIL_COND(!p_playback);

	callable = p_callable;

	BKCallback callback = {
		.func = divider_callback,
		.userInfo = (void *)this,
	};
	BKDividerInit(&divider, p_ticks, &callback);

	BKContext *context = p_playback->get_context();

	AudioStreamBlipKit::lock();
	BKContextAttachDivider(context, &divider, BK_CLOCK_TYPE_BEAT);
	AudioStreamBlipKit::unlock();
}

AudioStreamBlipKitPlayback::TickFunction::~TickFunction() {
	AudioStreamBlipKit::lock();
	BKDispose(&divider);
	AudioStreamBlipKit::unlock();
}

void AudioStreamBlipKitPlayback::TickFunction::detach() {
	AudioStreamBlipKit::lock();
	BKDividerDetach(&divider);
	AudioStreamBlipKit::unlock();
}

void AudioStreamBlipKitPlayback::TickFunction::reset() {
	AudioStreamBlipKit::lock();
	BKDividerReset(&divider);
	AudioStreamBlipKit::unlock();
}
