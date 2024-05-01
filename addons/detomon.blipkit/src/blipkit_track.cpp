#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/math.hpp>

#include "blipkit_track.hpp"

using namespace detomon::BlipKit;
using namespace godot;

BlipKitTrack::BlipKitTrack() {
	BKInt result = BKTrackInit(&track, BK_SQUARE);

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Could not initialize BKTrack");
}

void BlipKitTrack::_bind_methods() {
	ClassDB::bind_method(D_METHOD("attach", "context"), &BlipKitTrack::attach);
	ClassDB::bind_method(D_METHOD("dettach"), &BlipKitTrack::detach);

}

void BlipKitTrack::attach(BlipKitContext *p_context) {
	ERR_FAIL_COND(!p_context);

	BKContext* context = p_context->get_context();
	BKInt result = BKTrackAttach(&track, context);

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to attach BKTrack");
}

void BlipKitTrack::detach() {
	BKTrackDetach(&track);
}
