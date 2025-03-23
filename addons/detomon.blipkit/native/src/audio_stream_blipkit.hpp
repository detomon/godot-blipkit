#pragma once

#include <BlipKit.h>
#include "godot_cpp/core/defs.hpp"
#include "recursive_spin_lock.hpp"
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback_resampled.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

namespace BlipKit {

class AudioStreamBlipKitPlayback;
class BlipKitTrack;

class AudioStreamBlipKit : public AudioStream {
	GDCLASS(AudioStreamBlipKit, AudioStream);
	friend class AudioStreamBlipKitPlayback;

private:
	static constexpr int MIN_CLOCK_RATE = 1;
	static constexpr int MAX_CLOCK_RATE = 1200;

	int clock_rate = BK_DEFAULT_CLOCK_RATE;
	Ref<AudioStreamBlipKitPlayback> playback;
	static RecursiveSpinLock spin_lock;

public:
	Ref<AudioStreamPlayback> _instantiate_playback() const override;
	String _get_stream_name() const override;

	double _get_length() const override;
	bool _is_monophonic() const override;

	Ref<AudioStreamBlipKitPlayback> get_playback();

	int get_clock_rate() const;
	void set_clock_rate(int p_clock_rate);

	void attach(BlipKitTrack *p_track);
	void detach(BlipKitTrack *p_track);

	void call_synced(const Callable &p_callable);

	_FORCE_INLINE_ static void lock() { spin_lock.lock(); }
	_FORCE_INLINE_ static void unlock() { spin_lock.unlock(); }
	_FORCE_INLINE_ static RecursiveSpinLock::Autolock autolock() { return spin_lock.autolock(); }

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
	static constexpr int NUM_CHANNELS = 2;
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

	_FORCE_INLINE_ BKContext *get_context() { return &context; }

	void _start(double p_from_pos) override;
	void _stop() override;
	bool _is_playing() const override;
	int32_t _mix_resampled(AudioFrame *p_buffer, int32_t p_frames) override;
	double _get_stream_sampling_rate() const override { return double(SAMPLE_RATE); }

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit
