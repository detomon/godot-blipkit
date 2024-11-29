#pragma once

#include "blipkit_instrument.hpp"
#include "blipkit_track.hpp"
#include "blipkit_waveform.hpp"
#include "recursive_spin_lock.hpp"
#include <BlipKit.h>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace detomon::BlipKit;
using namespace godot;

class AudioStreamBlipKit : public AudioStream {
	GDCLASS(AudioStreamBlipKit, AudioStream);

private:
	static const int MIN_CLOCK_RATE = 1;
	static const int MAX_CLOCK_RATE = 1200;

	int clock_rate = BK_DEFAULT_CLOCK_RATE;
	static RecursiveSpinLock spin_lock;

protected:
	static void _bind_methods();
	String _to_string() const;

public:
	Ref<AudioStreamPlayback> _instantiate_playback() const override;
	String _get_stream_name() const override;

	double _get_length() const override;
	bool _is_monophonic() const override;

	int get_clock_rate();
	void set_clock_rate(int p_clock_rate);

	_FORCE_INLINE_ static void lock() { spin_lock.lock(); }
	_FORCE_INLINE_ static void unlock() { spin_lock.unlock(); }
	_FORCE_INLINE_ static RecursiveSpinLock::Autolock autolock() { return spin_lock.autolock(); }
};

class AudioStreamBlipKitPlayback : public AudioStreamPlayback {
	GDCLASS(AudioStreamBlipKitPlayback, AudioStreamPlayback)
	friend class AudioStreamBlipKit;

private:
	struct TickFunction {
	private:
		Callable callable;
		BKDivider divider;

		static BKEnum divider_callback(BKCallbackInfo *p_info, void *p_user_info);

	public:
		TickFunction() = default;
		~TickFunction();

		void initialize(Callable &p_callable, int p_ticks, AudioStreamBlipKitPlayback *p_playback);
		void detach();
		void reset();
	};

	static const int NUM_CHANNELS = 2;

	Ref<AudioStreamBlipKit> stream;
	bool active = false;
	BKContext context;
	HashMap<uint32_t, TickFunction> tick_functions;

protected:
	static void _bind_methods();
	String _to_string() const;

	bool initialize(Ref<AudioStreamBlipKit> p_stream);

public:
	AudioStreamBlipKitPlayback();
	~AudioStreamBlipKitPlayback();

	_FORCE_INLINE_ BKContext *get_context() { return &context; };

	void _start(double p_from_pos) override;
	void _stop() override;
	bool _is_playing() const override;
	int32_t _mix(AudioFrame *p_buffer, double p_rate_scale, int32_t p_frames) override;

	void add_tick_function(Callable p_callable, int p_ticks);
	void remove_tick_function(Callable p_callable);

	// Ref<BlipKitTrack> create_track(BlipKitTrack::Waveform p_waveform);
	// Ref<BlipKitInstrument> create_instrument();
	// Ref<BlipKitWaveform> create_custom_waveform(PackedFloat32Array p_frames, bool p_normalize = true);
};
