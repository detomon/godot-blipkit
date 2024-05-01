#pragma once

#include "mutex.hpp"
#include <BlipKit.h>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback_resampled.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

#define BK_THREAD_SAFE_METHOD MutexLock _mutex_lock_ = AudioStreamBlipKit::mutex_lock();

namespace BlipKit {

class AudioStreamBlipKitPlayback;
class BlipKitTrack;

class AudioStreamBlipKit : public AudioStream {
	GDCLASS(AudioStreamBlipKit, AudioStream);
	friend class AudioStreamBlipKitPlayback;

private:
	static constexpr int CLOCK_RATE_MIN = 60;
	static constexpr int CLOCK_RATE_MAX = 960;

	int clock_rate = BK_DEFAULT_CLOCK_RATE;
	Ref<AudioStreamBlipKitPlayback> playback;

	static RecursiveMutex mutex;

	void set_clock_rate(int p_clock_rate);
	int get_clock_rate() const;

public:
	AudioStreamBlipKit();

	Ref<AudioStreamPlayback> _instantiate_playback() const override;
	String _get_stream_name() const override;

	double _get_length() const override;
	bool _is_monophonic() const override;

	Ref<AudioStreamBlipKitPlayback> get_playback();

	void attach(BlipKitTrack *p_track);
	void detach(BlipKitTrack *p_track);

	void call_synced(const Callable &p_callable);

	_ALWAYS_INLINE_ static void lock() { mutex.lock(); }
	_ALWAYS_INLINE_ static void unlock() { mutex.unlock(); }
	_ALWAYS_INLINE_ static MutexLock<RecursiveMutex> mutex_lock() { return BlipKit::MutexLock(mutex); }

protected:
	static void _bind_methods();
	String _to_string() const;
};

class AudioStreamBlipKitPlayback : public AudioStreamPlaybackResampled {
	GDCLASS(AudioStreamBlipKitPlayback, AudioStreamPlaybackResampled)
	friend class AudioStreamBlipKit;
	friend class BlipKitTrack;

private:
	static constexpr int SAMPLE_RATE = BK_DEFAULT_SAMPLE_RATE;
	static constexpr int CHANNEL_COUNT = 2;
	static constexpr int CHANNEL_SIZE = 1024;

	BKContext context;
	LocalVector<BKFrame> buffer;
	LocalVector<BlipKitTrack *> tracks;
	LocalVector<Callable> sync_callables;
	int clock_rate = BK_DEFAULT_CLOCK_RATE;
	bool active = false;
	bool is_calling_callbacks = false;

protected:
	bool initialize(int p_clock_rate);
	int get_clock_rate() const;
	void set_clock_rate(int p_clock_rate);

	void call_synced(const Callable &p_callable);

	void attach(BlipKitTrack *p_track);
	void detach(BlipKitTrack *p_track);

public:
	AudioStreamBlipKitPlayback();
	~AudioStreamBlipKitPlayback();

	_ALWAYS_INLINE_ BKContext *get_context() { return &context; }

	void _start(double p_from_pos) override;
	void _stop() override;
	bool _is_playing() const override;
	int32_t _mix_resampled(AudioFrame *p_buffer, int32_t p_frames) override;
	double _get_stream_sampling_rate() const override;

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit
