#include "track.hpp"
#include <BKBase.h>

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
