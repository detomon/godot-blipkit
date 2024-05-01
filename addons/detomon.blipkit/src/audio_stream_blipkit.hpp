#pragma once

#include <godot_cpp/classes/audio_stream.hpp>
#include <godot_cpp/classes/audio_stream_playback.hpp>
#include <BlipKit.h>

using namespace godot;

class AudioStreamBlipKit : public AudioStream {
	GDCLASS(AudioStreamBlipKit, AudioStream);

protected:
	static void _bind_methods();

public:
	Ref<AudioStreamPlayback> _instantiate_playback() const override;
	String _get_stream_name() const override;

	double _get_length() const override;
	bool _is_monophonic() const override;
};

class AudioStreamBlipKitPlayback : public AudioStreamPlayback {
	GDCLASS(AudioStreamBlipKitPlayback, AudioStreamPlayback)
	friend class AudioStreamBlipKit;

private:
	const int NUM_CHANNELS = 2;

	Ref<AudioStreamBlipKit> stream;
	bool active = false;
	BKContext context;

protected:
	static void _bind_methods();

public:
	AudioStreamBlipKitPlayback();
	~AudioStreamBlipKitPlayback();

	_FORCE_INLINE_ BKContext *get_context() { return &context; };

	void _start(double p_from_pos) override;
	void _stop() override;
	bool _is_playing() const override;
	void _seek(double p_position) override;
	int32_t _mix(AudioFrame *p_buffer, double p_rate_scale, int32_t p_frames) override;
};
