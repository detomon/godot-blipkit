#include "divider.hpp"
#include "audio_stream_blipkit.hpp"

using namespace BlipKit;
using namespace godot;

void Divider::initialize(const Callable &p_callable, int p_tick_interval) {
	callable = p_callable;
	divider = p_tick_interval;
}

int Divider::tick() {
	int ticks = 0;

	counter--;
	if (counter <= 0) {
		ticks = callable.call();
		// Set new divider value.
		if (ticks > 0) {
			divider = ticks;
		}
		counter = divider;
	}

	return ticks;
}

void Divider::reset(int p_tick_interval) {
	counter = 0;
	if (p_tick_interval > 0) {
		divider = p_tick_interval;
	}
}

BKEnum DividerGroup::divider_callback(BKCallbackInfo *p_info, void *p_user_info) {
	DividerGroup *group = static_cast<DividerGroup *>(p_user_info);
	HashMap<String, Divider> &dividers = group->dividers;
	LocalVector<String> remove_dividers;

	for (KeyValue<String, Divider> &E : dividers) {
		Divider &divider = E.value;
		int ticks = divider.tick();

		// Remove divider.
		if (ticks < 0) {
			remove_dividers.push_back(E.key);
		}
	}

	for (const String &name : remove_dividers) {
		dividers.erase(name);
	}

	return BK_SUCCESS;
}

DividerGroup::DividerGroup() {
	BKCallback callback = {
		.func = divider_callback,
		.userInfo = (void *)this,
	};
	BKDividerInit(&divider, 1, &callback);
}

DividerGroup::~DividerGroup() {
	detach();
}

PackedStringArray DividerGroup::get_divider_names() const {
	PackedStringArray names;
	names.resize(dividers.size());
	int i = 0;

	for (const KeyValue<String, Divider> &E : dividers) {
		names[i] = E.key;
		i++;
	}

	return names;
}

void DividerGroup::add_divider(const String &p_name, int p_tick_interval, const Callable &p_callable) {
	ERR_FAIL_COND(has_divider(p_name));

	Divider &divider = dividers[p_name];
	divider.initialize(p_callable, p_tick_interval);
}

void DividerGroup::remove_divider(const String &p_name) {
	dividers.erase(p_name);
}

bool DividerGroup::has_divider(const String &p_name) {
	return dividers.has(p_name);
}

void DividerGroup::reset_divider(const String &p_name, int p_tick_interval) {
	ERR_FAIL_COND(!has_divider(p_name));

	dividers[p_name].reset(p_tick_interval);
}

void DividerGroup::attach(AudioStreamBlipKitPlayback *p_playback) {
	ERR_FAIL_NULL(p_playback);

	BKContext *context = p_playback->get_context();

	AudioStreamBlipKit::lock();
	BKContextAttachDivider(context, &divider, BK_CLOCK_TYPE_BEAT);
	AudioStreamBlipKit::unlock();
}

void DividerGroup::detach() {
	AudioStreamBlipKit::lock();
	BKDividerDetach(&divider);
	AudioStreamBlipKit::unlock();
}

void DividerGroup::reset() {
	AudioStreamBlipKit::lock();

	for (KeyValue<String, Divider> &E : dividers) {
		Divider &divider = E.value;
		divider.reset();
	}

	AudioStreamBlipKit::unlock();
}

void DividerGroup::clear() {
	AudioStreamBlipKit::lock();
	dividers.clear();
	AudioStreamBlipKit::unlock();
}
