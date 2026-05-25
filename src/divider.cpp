#include "divider.hpp"
#include "audio_stream_blipkit.hpp"

using namespace BlipKit;
using namespace godot;

bool DividerGroup::Divider::initialize(const Callable &p_callable, int p_tick_interval) {
	callable = p_callable;
	divider = p_tick_interval;

	return true;
}

void DividerGroup::Divider::reset(int p_tick_interval) {
	if (p_tick_interval > 0) {
		divider = p_tick_interval;
	}
	counter = 0;
}

BKEnum DividerGroup::divider_callback(BKCallbackInfo *p_info, void *p_user_info) {
	return static_cast<DividerGroup *>(p_user_info)->tick();
}

DividerGroup::DividerGroup() {
	BKCallback callback = {
		.func = divider_callback,
		.userInfo = static_cast<void *>(this),
	};
	BKDividerInit(&divider, 1, &callback);
}

DividerGroup::~DividerGroup() {
	detach();
}

BKEnum DividerGroup::tick() {
	thread_local LocalVector<ID> removed_dividers;
	removed_dividers.clear();

	is_ticking = true;

	for (KeyValue<ID, Divider> &E : dividers) {
		Divider &divider = E.value;
		const int ticks = divider.tick();

		// Remove divider.
		if (ticks < 0) {
			removed_dividers.push_back(E.key);
		}
	}

	is_ticking = false;

	for (const ID &id : removed_dividers) {
		dividers.erase(id);
	}

	return BK_SUCCESS;
}

PackedInt32Array DividerGroup::get_dividers() const {
	PackedInt32Array ids;
	ids.resize(dividers.size());
	int *ptrw = ids.ptrw();
	uint32_t i = 0;

	for (const KeyValue<ID, Divider> &E : dividers) {
		ptrw[i++] = E.key;
	}

	return ids;
}

DividerGroup::ID DividerGroup::add_divider(int p_tick_interval, const Callable &p_callable, bool p_tick_now) {
	ID new_id = ++id;
	Divider &divider = dividers[new_id];
	divider.initialize(p_callable, p_tick_interval);

	if (p_tick_now && is_ticking) {
		const int ticks = divider.tick();

		// Remove divider.
		if (ticks < 0) {
			// TODO: Remove divider.
			// Return RID().

			//removed_dividers.push_back(E.key);
		}
	}

	return new_id;
}

void DividerGroup::remove_divider(ID p_id) {
	dividers.erase(p_id);
}

bool DividerGroup::has_divider(ID p_id) {
	return dividers.has(p_id);
}

void DividerGroup::reset_divider(ID p_id, int p_tick_interval) {
	ERR_FAIL_COND(not has_divider(p_id));

	dividers[p_id].reset(p_tick_interval);
}

void DividerGroup::attach(AudioStreamBlipKitPlayback *p_playback) {
	ERR_FAIL_NULL(p_playback);

	BKContext *context = p_playback->get_context();

	BKContextAttachDivider(context, &divider, BK_CLOCK_TYPE_BEAT);
}

void DividerGroup::detach() {
	BK_THREAD_SAFE_METHOD

	BKDividerDetach(&divider);
}

void DividerGroup::reset() {
	for (KeyValue<ID, Divider> &E : dividers) {
		Divider &divider = E.value;
		divider.reset();
	}
}

void DividerGroup::clear() {
	dividers.clear();
}
