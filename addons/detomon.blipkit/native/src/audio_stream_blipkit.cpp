#include "audio_stream_blipkit.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/project_settings.hpp>

#include <atomic>

using namespace detomon::BlipKit;
using namespace godot;

static thread_local int lock_thread_id;
static struct {
	std::atomic<int> owner = 0;
	int count = 0;
	std::atomic<int> next_thread_id = 0;
} lock_data;

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
	playback->initialize(Ref<AudioStreamBlipKit>(this), always_generate);

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

void AudioStreamBlipKit::lock() {
	int id = lock_thread_id;

	if (unlikely(id == 0)) {
		id = lock_data.next_thread_id.fetch_add(1, std::memory_order_relaxed) + 1;
		lock_thread_id = id;
	}

	if (lock_data.owner.load() != id) {
		int free_id = 0;
		while (!lock_data.owner.compare_exchange_weak(free_id, id, std::memory_order_acquire, std::memory_order_relaxed)) {
			;
		}
	}

	lock_data.count++;
}

void AudioStreamBlipKit::unlock() {
	if (unlikely(lock_data.owner.load() != lock_thread_id)) {
		return;
	}

	if (lock_data.count > 0) {
		lock_data.count--;
		if (lock_data.count <= 0) {
			lock_data.owner.store(0, std::memory_order_release);
		}
	}
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
	// ...
}

String AudioStreamBlipKitPlayback::_to_string() const {
	return "AudioStreamBlipKitPlayback";
}

void AudioStreamBlipKitPlayback::initialize(Ref<AudioStreamBlipKit> p_stream, bool p_always_generate) {
	stream = p_stream;
	always_generate = p_always_generate;

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

	AudioStreamBlipKit::lock();
	BKContextReset(&context);
	AudioStreamBlipKit::unlock();
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

	AudioStreamBlipKit::lock();

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

	AudioStreamBlipKit::unlock();

	// Fill rest of output buffer, even if nothing was generated.
	if (out_count < p_frames && always_generate) {
		while (out_count < p_frames) {
			*out_buffer = { 0, 0 };
			out_buffer++;
			out_count++;
		}
	}

	return out_count;
}

Ref<BlipKitTrack> AudioStreamBlipKitPlayback::create_track(BlipKitTrack::Waveform p_waveform) {
	Ref<BlipKitTrack> track;

	return track;
}

Ref<BlipKitInstrument> AudioStreamBlipKitPlayback::create_instrument() {
	Ref<BlipKitInstrument> instrument;

	return instrument;
}

Ref<BlipKitWaveform> AudioStreamBlipKitPlayback::create_custom_waveform(PackedFloat32Array p_frames, bool p_normalize) {
	Ref<BlipKitWaveform> waveform;

	return waveform;
}
