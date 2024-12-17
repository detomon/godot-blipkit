#include "divider.hpp"
#include "audio_stream_blipkit.hpp"

using namespace detomon::BlipKit;
using namespace godot;

Divider::~Divider() {
	AudioStreamBlipKit::lock();
	BKDispose(&divider);
	AudioStreamBlipKit::unlock();
}

BKEnum Divider::divider_callback(BKCallbackInfo *p_info, void *p_user_info) {
	Divider *divider = static_cast<Divider *>(p_user_info);
	int ticks = divider->callable.call();

	// Update ticks if callable returned value.
	if (ticks > 0) {
		p_info->divider = ticks;
	}

	return BK_SUCCESS;
}

void Divider::initialize(Callable &p_callable, int p_ticks) {
	callable = p_callable;

	BKCallback callback = { 0 };
	callback.func = divider_callback;
	callback.userInfo = (void *)this;
	BKDividerInit(&divider, p_ticks, &callback);
}

void Divider::attach(AudioStreamBlipKitPlayback *p_playback) {
	ERR_FAIL_NULL(p_playback);

	AudioStreamBlipKit::lock();

	BKContext *context = p_playback->get_context();
	BKContextAttachDivider(context, &divider, BK_CLOCK_TYPE_BEAT);

	AudioStreamBlipKit::unlock();
}

void Divider::detach() {
	AudioStreamBlipKit::lock();
	BKDividerDetach(&divider);
	AudioStreamBlipKit::unlock();
}

void Divider::reset(int p_ticks) {
	AudioStreamBlipKit::lock();

	BKDividerReset(&divider);
	if (p_ticks > 0) {
		// FIXME: There is not function for that.
		divider.divider = p_ticks;
	}

	AudioStreamBlipKit::unlock();
}
