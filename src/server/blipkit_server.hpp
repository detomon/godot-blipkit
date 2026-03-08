#pragma once

#include "divider.hpp"
#include "server/instrument.hpp"
#include "server/track.hpp"
#include "server/waveform.hpp"
#include <BlipKit.h>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/templates/rid_owner.hpp>

using namespace godot;

namespace BlipKit {

class AudioStreamBlipKitPlayback;
class BlipKitInstrument;
class BlipKitSample;
class BlipKitTrack;
class BlipKitWaveform;

class BlipKitServer : public Object {
	GDCLASS(BlipKitServer, Object)
	friend class BlipKitInstrument;

	using Divider = DividerGroup::Divider;

private:
	struct Sample {
	};

	static inline BlipKitServer *singleton = nullptr;

	mutable RID_Owner<Instrument, true> instrument_owner;
	mutable RID_Owner<Sample, true> sample_owner;
	mutable RID_Owner<Track, true> track_owner;
	mutable RID_Owner<Waveform, true> waveform_owner;
	mutable RID_Owner<Divider, true> divider_owner;

public:
	static _FORCE_INLINE_ BlipKitServer *get_singleton() {
		return singleton;
	}

	static void create();
	static void free();

	RID create_instrument();
	RID create_sample();
	RID create_track();
	RID create_waveform();
	RID create_divider(const RID &p_track_rid, int p_tick_interval, const Callable &p_callable);

	bool waveform_set_frames(const RID &p_rid, const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);
	PackedFloat32Array waveform_get_frames(const RID &p_rid) const;
	int waveform_get_size(const RID &p_rid) const;
	bool waveform_get_is_valid(const RID &p_rid) const;
	BKData *waveform_get_data(const RID &p_rid) const;

	void free_rid(const RID &p_rid);

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit
