#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/math.hpp>

#include "blipkit_context.hpp"

using namespace detomon::BlipKit;
using namespace godot;

BlipKitContext::BlipKitContext() {
	int sample_rate = ProjectSettings::get_singleton()->get_setting_with_override("audio/driver/mix_rate");
	BKInt result = BKContextInit(&context, NUM_CHANNELS, sample_rate);

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Could not initialize BKContext");
}

void BlipKitContext::_bind_methods() {
	//ClassDB::bind_method(D_METHOD("skeleton_set_bone_pose_positions", "skeleton", "bones", "positions"), &Context::skeleton_set_bone_pose_positions);
	//ClassDB::bind_method(D_METHOD("skeleton_set_bone_pose_rotations", "skeleton", "bones", "rotations"), &Context::skeleton_set_bone_pose_rotations);
	//ClassDB::bind_method(D_METHOD("skeleton_set_bone_pose_scales", "skeleton", "bones", "scales"), &Context::skeleton_set_bone_pose_scales);
}
