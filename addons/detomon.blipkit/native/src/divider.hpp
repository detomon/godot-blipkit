#pragma once

#include <BlipKit.h>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace BlipKit {

class AudioStreamBlipKitPlayback;

class Divider {
private:
	Callable callable;
	int divider = 0;
	int counter = 0;

public:
	Divider() = default;
	~Divider() = default;

	void initialize(const Callable &p_callable, int p_tick_interval);
	int tick();
	void reset(int p_tick_interval = 0);
};

class DividerGroup {
private:
	HashMap<String, Divider> dividers;
	BKDivider divider = { 0 };

	static BKEnum divider_callback(BKCallbackInfo *p_info, void *p_user_info);

public:
	DividerGroup();
	~DividerGroup();

	PackedStringArray get_divider_names() const;
	void add_divider(const String &p_name, int p_tick_interval, const Callable &p_callable);
	void remove_divider(const String &p_name);
	bool has_divider(const String &p_name);
	void reset_divider(const String &p_name, int p_tick_interval = 0);
	void clear();

	void attach(AudioStreamBlipKitPlayback *p_playback);
	void detach();
	void reset();
};

} // namespace BlipKit
