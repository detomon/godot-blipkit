#pragma once

#include "blipkit_instrument.hpp"
#include "blipkit_waveform.hpp"
#include "divider.hpp"
#include "recursive_spin_lock.hpp"
#include <BlipKit.h>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

namespace detomon::BlipKit {

class BlipKitTrack;

class AudioStreamBlipKit : public AudioStream {
	GDCLASS(AudioStreamBlipKit, AudioStream);
	friend class AudioStreamBlipKitPlayback;

private:
	static const int MIN_CLOCK_RATE = 1;
	static const int MAX_CLOCK_RATE = 1200;

	int clock_rate = BK_DEFAULT_CLOCK_RATE;
	Ref<AudioStreamBlipKitPlayback> playback;
	static RecursiveSpinLock spin_lock;

protected:
	static void _bind_methods();
	String _to_string() const;

public:
	Ref<AudioStreamPlayback> _instantiate_playback() const override;
	String _get_stream_name() const override;

	double _get_length() const override;
	bool _is_monophonic() const override;

	int get_clock_rate() const;
	void set_clock_rate(int p_clock_rate);

	_FORCE_INLINE_ static void lock() { spin_lock.lock(); }
	_FORCE_INLINE_ static void unlock() { spin_lock.unlock(); }
	_FORCE_INLINE_ static RecursiveSpinLock::Autolock autolock() { return spin_lock.autolock(); }
};

class AudioStreamBlipKitPlayback : public AudioStreamPlayback {
	GDCLASS(AudioStreamBlipKitPlayback, AudioStreamPlayback)
	friend class AudioStreamBlipKit;
	friend class BlipKitTrack;

	static const int NUM_CHANNELS = 2;
	static const int CHANNEL_SIZE = 1024;

	BKContext context;
	LocalVector<BKFrame> buffer;
	LocalVector<BlipKitTrack *> tracks;
	HashMap<int, Divider *> dividers;
	static int divider_id;
	LocalVector<Callable> sync_callables;
	int clock_rate = BK_DEFAULT_CLOCK_RATE;
	bool active = false;

protected:
	static void _bind_methods();
	String _to_string() const;

	bool initialize(int p_clock_rate);
	int get_clock_rate() const;
	void set_clock_rate(int p_clock_rate);

	void call_synced_callables();

	void attach(BlipKitTrack *p_track);
	void detach(BlipKitTrack *p_track);

public:
	AudioStreamBlipKitPlayback();
	~AudioStreamBlipKitPlayback();

	_FORCE_INLINE_ BKContext *get_context() { return &context; };

	void _start(double p_from_pos) override;
	void _stop() override;
	bool _is_playing() const override;
	int32_t _mix(AudioFrame *p_buffer, double p_rate_scale, int32_t p_frames) override;

	void call_synced(Callable p_callable);

	int add_divider(Callable p_callable, int p_tick_interval);
	void remove_divider(int p_id);
	void clear_dividers();
	void reset_divider(int p_id, int p_tick_interval = 0);
	void enable_divider(int p_id, bool p_enable);
};

} // namespace detomon::BlipKit
