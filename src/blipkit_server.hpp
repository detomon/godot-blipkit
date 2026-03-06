#pragma once

#include "blipkit_instrument.hpp"
#include "blipkit_sample.hpp"
#include "blipkit_track.hpp"
#include "blipkit_waveform.hpp"
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/templates/rid_owner.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitServer : public Object {
	GDCLASS(BlipKitServer, Object)

private:
	mutable RID_PtrOwner<BlipKitInstrument, true> instrument_owner;
	mutable RID_PtrOwner<BlipKitSample, true> sample_owner;
	mutable RID_PtrOwner<BlipKitTrack, true> track_owner;
	mutable RID_PtrOwner<BlipKitWaveform, true> waveform_owner;

	inline static BlipKitServer *singleton = nullptr;

public:
	_FORCE_INLINE_ BlipKitServer *get_singletone() {
		return singleton;
	}

	RID create_instrument();
	RID create_sample();
	RID create_track();
	RID create_waveform();

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit
