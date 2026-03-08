#pragma once

#include "divider.hpp"
#include "fixed_vector.hpp"
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
	struct Instrument {
		enum EnvelopeType {
			ENVELOPE_VOLUME,
			ENVELOPE_PANNING,
			ENVELOPE_PITCH,
			ENVELOPE_DUTY_CYCLE,
			ENVELOPE_MAX,
		};

		struct Sequence {
			PackedInt32Array steps;
			PackedFloat32Array values;
			int sustain_offset = 0;
			int sustain_length = 0;
		};

		BKInstrument instrument = { { 0 } };
		Sequence sequences[ENVELOPE_MAX];
	};

	struct Sample {
	};

	struct Track {
		BKTrack track = { { { 0 } } };
		PackedFloat32Array arpeggio;
		DividerGroup dividers;
		AudioStreamBlipKitPlayback *playback = nullptr;
		RID instrument;
		RID custom_waveform;
		RID sample;
		bool master_volume_changed = false;
	};

	struct Waveform {
		static constexpr int WAVE_SIZE_MAX = BK_WAVE_MAX_LENGTH;

		BKData data = { { 0 } };
		FixedVector<BKFrame, WAVE_SIZE_MAX> frames;

		~Waveform();

		bool initialize();
		bool set_frames(const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);
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

	RID create_instrument();
	RID create_sample();
	RID create_track();
	RID create_waveform();
	RID create_divider(const RID &p_track_rid, int p_tick_interval, const Callable &p_callable);

	bool waveform_set_frames(const RID &p_rid, const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);

	void free_rid(const RID &p_rid);

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit
