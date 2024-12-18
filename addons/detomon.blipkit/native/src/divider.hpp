#pragma once

#include <BlipKit.h>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

namespace detomon::BlipKit {

class AudioStreamBlipKitPlayback;

class Divider {
private:
	BKDivider divider = { 0 };
	Callable callable;

	static BKEnum divider_callback(BKCallbackInfo *p_info, void *p_user_info);

public:
	Divider() = default;
	~Divider();

	void initialize(int p_ticks, Callable &p_callable);
	void attach(AudioStreamBlipKitPlayback *p_playback);
	void detach();
	void reset(int p_ticks = 0);
};

} // namespace detomon::BlipKit
