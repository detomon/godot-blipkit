#pragma once

#include "blipkit_instrument.hpp"
#include "blipkit_track.hpp"
#include "blipkit_waveform.hpp"
#include <BlipKit.h>
#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>

using namespace detomon::BlipKit;
using namespace godot;

class AudioStreamBlipKit : public AudioStream {
	GDCLASS(AudioStreamBlipKit, AudioStream);

private:
	static const int MIN_CLOCK_RATE = 1;
	static const int MAX_CLOCK_RATE = 1200;

	int clock_rate = BK_DEFAULT_CLOCK_RATE;
	bool always_generate = true;

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

	bool is_always_generating();
	void set_generate_always(bool p_always_generate);

	static void lock();
	static void unlock();
};

class AudioStreamBlipKitPlayback : public AudioStreamPlayback {
	GDCLASS(AudioStreamBlipKitPlayback, AudioStreamPlayback)
	friend class AudioStreamBlipKit;

private:
	static const int NUM_CHANNELS = 2;

	Ref<AudioStreamBlipKit> stream;
	bool active = false;
	bool always_generate = false;
	BKContext context;

protected:
	static void _bind_methods();
	String _to_string() const;

	void initialize(Ref<AudioStreamBlipKit> p_stream, bool p_always_generate);

public:
	AudioStreamBlipKitPlayback();
	~AudioStreamBlipKitPlayback();

	_FORCE_INLINE_ BKContext *get_context() { return &context; };

	void _start(double p_from_pos) override;
	void _stop() override;
	bool _is_playing() const override;
	int32_t _mix(AudioFrame *p_buffer, double p_rate_scale, int32_t p_frames) override;

	Ref<BlipKitTrack> create_track(BlipKitTrack::Waveform p_waveform);
	Ref<BlipKitInstrument> create_instrument();
	Ref<BlipKitWaveform> create_custom_waveform(PackedFloat32Array p_frames, bool p_normalize = true);
};
