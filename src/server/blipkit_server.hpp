#pragma once

#include "BKBase.h"
#include "audio_stream_blipkit.hpp"
#include "blipkit_instrument.hpp"
#include "fixed_vector.hpp"
#include <BlipKit.h>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/templates/rid_owner.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitServer : public Object {
	GDCLASS(BlipKitServer, Object)

public:
	static constexpr int SAMPLE_RATE = BK_DEFAULT_SAMPLE_RATE;
	static constexpr int CHANNEL_COUNT = 2;
	static constexpr int CHUNK_SIZE = 512;

private:
	struct Context {
		BKContext ctx = {};
		FixedVector<BKFrame, CHUNK_SIZE * CHANNEL_COUNT> buffer;
		LocalVector<Callable> sync_callables;
		struct {
			bool is_calling_callbacks : 1 = false;
		};

		Context();
		~Context();
	};

	struct Track {
		BKTrack track = {};
		PackedFloat32Array arpeggio;
		AudioStreamBlipKitPlayback *playback = nullptr;
		RID divider_group;
		RID instrument;
		RID custom_waveform;
		RID sample;
		struct {
			bool master_volume_changed : 1 = false;
		};

		Track();
		~Track();
	};

	struct Instrument {
		struct Sequence {
			PackedInt32Array steps;
			PackedFloat32Array values;
			int sustain_offset = 0;
			int sustain_length = 0;
		};

		BKInstrument instrument = {};
		FixedVector<Sequence, BlipKitInstrument::ENVELOPE_MAX> sequences;

		Instrument();
		~Instrument();
	};

	struct Waveform {
		BKData data = {};
		FixedVector<BKFrame, BK_WAVE_MAX_LENGTH> frames;

		Waveform();
		~Waveform();
	};

	struct Sample {
		BKData data = {};

		Sample();
		~Sample();
	};

	struct Divider {
		RID divider_group;
		Callable callable;
		int divider = 0;
		int counter = 0;
	};

	struct DividerGroup {
		BKDivider divider = {};
		HashSet<RID> dividers;

		DividerGroup();
		~DividerGroup();

		static BKEnum divider_callback(BKCallbackInfo *p_info, void *p_user_info);
	};

	static inline BlipKitServer *singleton = nullptr;

	mutable RID_Owner<Context, true> context_owner;
	mutable RID_Owner<Track, true> track_owner;
	mutable RID_Owner<Instrument, true> instrument_owner;
	mutable RID_Owner<Waveform, true> waveform_owner;
	mutable RID_Owner<Sample, true> sample_owner;
	mutable RID_Owner<Divider, true> divider_owner;
	mutable RID_Owner<DividerGroup, true> divider_group_owner;

	RID divider_create(const RID &p_track_rid, int p_tick_interval, const Callable &p_callable);

public:
	static _FORCE_INLINE_ BlipKitServer *get_singleton() { return singleton; }

	static void create();
	static void free();

	RID context_create();
	int32_t context_generate(const RID &p_rid, AudioFrame *r_buffer, int32_t p_frames);
	int32_t context_generate_samples(const RID &p_rid, PackedFloat32Array &r_buffer, int32_t p_frames);

	RID track_create();
	void track_attach(const RID &p_rid, const RID &p_ctx_rid);
	void track_detach(const RID &p_rid);
	void track_set_arpeggio(const RID &p_rid, const PackedFloat32Array &p_arpeggio);
	TypedArray<RID> track_get_dividers(const RID &p_track_rid) const;
	bool track_has_divider(const RID &p_track_rid, const RID &p_divider_rid) const;
	RID track_add_divider(const RID &p_track_rid, int p_tick_interval, const Callable &p_callable);
	void track_clear_dividers(const RID &p_track_rid);

	void divider_group_set_active(const RID &p_rid);
	bool divider_group_is_active(const RID &p_rid) const;

	void divider_reset(const RID &p_rid, int p_tick_interval = 0);

	RID instrument_create();

	RID waveform_create();
	bool waveform_set_frames(const RID &p_rid, const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);
	PackedFloat32Array waveform_get_frames(const RID &p_rid) const;
	int waveform_get_size(const RID &p_rid) const;
	bool waveform_get_is_valid(const RID &p_rid) const;
	BKData *waveform_get_data(const RID &p_rid) const;

	RID sample_create();

	void free_rid(const RID &p_rid);

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit
