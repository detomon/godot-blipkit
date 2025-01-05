#pragma once

#include <BlipKit.h>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

namespace BlipKit {

class AudioStreamBlipKitPlayback;

class Divider {
private:
	Callable callable;
	BKDivider divider = { 0 };

	static BKEnum divider_callback(BKCallbackInfo *p_info, void *p_user_info);

public:
	Divider() = default;
	~Divider();

	void initialize(int p_ticks, const Callable &p_callable);
	void attach(AudioStreamBlipKitPlayback *p_playback);
	void detach();
	void reset(int p_ticks = 0);
};

} // namespace BlipKit
