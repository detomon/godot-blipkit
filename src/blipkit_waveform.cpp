#include "blipkit_waveform.hpp"
#include "string_names.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>

using namespace BlipKit;
using namespace godot;

BlipKitWaveform::BlipKitWaveform() {
	rid = BlipKitServer::get_singleton()->create_waveform();
	ERR_FAIL_COND(rid == RID());
}

BlipKitWaveform::~BlipKitWaveform() {
	BlipKitServer::get_singleton()->free_rid(rid);
}

Ref<BlipKitWaveform> BlipKitWaveform::create_with_frames(const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
	const uint32_t frames_size = p_frames.size();
	ERR_FAIL_COND_V(frames_size < 2, nullptr);
	ERR_FAIL_COND_V(frames_size > WAVE_SIZE_MAX, nullptr);

	Ref<BlipKitWaveform> instance;
	instance.instantiate();
	instance->set_frames(p_frames, p_normalize, p_amplitude);

	return instance;
}

void BlipKitWaveform::set_frames(const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
	if (not BlipKitServer::get_singleton()->waveform_set_frames(rid, p_frames, p_normalize, p_amplitude)) {
		return;
	}

	emit_changed();
}

PackedFloat32Array BlipKitWaveform::get_frames() const {
	return BlipKitServer::get_singleton()->waveform_get_frames(rid);
}

void BlipKitWaveform::_bind_methods() {
	ClassDB::bind_static_method("BlipKitWaveform", D_METHOD("create_with_frames", "frames", "normalize", "amplitude"), &BlipKitWaveform::create_with_frames, DEFVAL(false), DEFVAL(1.0));

	ClassDB::bind_method(D_METHOD("size"), &BlipKitWaveform::size);
	ClassDB::bind_method(D_METHOD("is_valid"), &BlipKitWaveform::is_valid);
	ClassDB::bind_method(D_METHOD("set_frames", "frames", "normalize", "amplitude"), &BlipKitWaveform::set_frames, DEFVAL(false), DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("get_frames"), &BlipKitWaveform::get_frames);
	ClassDB::bind_method(D_METHOD("get_rid"), &BlipKitWaveform::get_rid);

	BIND_CONSTANT(WAVE_SIZE_MAX);
}

String BlipKitWaveform::_to_string() const {
	return vformat("<BlipKitWaveform#%d>", get_instance_id());
}

void BlipKitWaveform::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "_frames", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
}

bool BlipKitWaveform::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == BKStringName(_frames)) {
		set_frames(p_value);
	} else {
		return false;
	}

	return true;
}

bool BlipKitWaveform::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == BKStringName(_frames)) {
		r_ret = get_frames();
	} else {
		return false;
	}

	return true;
}
