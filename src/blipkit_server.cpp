#include "blipkit_server.hpp"

using namespace BlipKit;
using namespace godot;

RID BlipKitServer::create_instrument() {
	return RID();
}

RID BlipKitServer::create_sample() {
	return RID();
}

RID BlipKitServer::create_track() {
	BlipKitTrack *track = memnew(BlipKitTrack);

	RID id = track_owner.make_rid(track);
	track->set_self(id);

	return id;
}

RID BlipKitServer::create_waveform() {
	return RID();
}

void BlipKitServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_instrument"), &BlipKitServer::create_instrument);
	ClassDB::bind_method(D_METHOD("create_sample"), &BlipKitServer::create_sample);
	ClassDB::bind_method(D_METHOD("create_track"), &BlipKitServer::create_track);
	ClassDB::bind_method(D_METHOD("create_waveform"), &BlipKitServer::create_waveform);
}

String BlipKitServer::_to_string() const {
	return vformat("<BlipKitServer#%d>", get_instance_id());
}
