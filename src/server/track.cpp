#include "track.hpp"
#include "server/blipkit_server.hpp"

using namespace BlipKit;
using namespace godot;

bool Track::initialize() {
	BKInt result = BKTrackInit(&track, BK_SQUARE);
	ERR_FAIL_COND_V_MSG(false, result != BK_SUCCESS, vformat("Failed to initialize BKTrack: %s.", BKStatusGetName(result)));

	return true;
}

Track::~Track() {
	BKDispose(&track);
}

TypedArray<RID> Track::get_dividers() const {
	return {};
}

bool Track::has_divider(RID p_rid) {
	// TODO: Implement.

	return false;
}

RID Track::add_divider(int p_tick_interval, const Callable &p_callable) {
	// TODO: Implement.

	return RID();
}

void Track::remove_divider(RID p_rid) {
	// TODO: Implement.
}

void Track::reset_divider(RID p_rid, int p_tick_interval) {
	// TODO: Implement.
}

void Track::clear_dividers() {
	// TODO: Implement.
}
