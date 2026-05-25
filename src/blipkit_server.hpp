#pragma once

#include "audio_stream_blipkit.hpp"
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
	enum EnvelopeType {
		ENVELOPE_VOLUME,
		ENVELOPE_PANNING,
		ENVELOPE_PITCH,
		ENVELOPE_DUTY_CYCLE,
		ENVELOPE_MAX,
	};

	static constexpr int CLOCK_RATE_MIN = 60;
	static constexpr int CLOCK_RATE_MAX = 960;

	static constexpr int SAMPLE_RATE = BK_DEFAULT_SAMPLE_RATE;
	static constexpr int CHANNEL_COUNT = 2;
	static constexpr int CHUNK_SIZE = 512;

	static constexpr int SEQUENCE_SIZE_MAX = 32;

private:
	struct Context {
		BKContext ctx = {};
		FixedVector<BKFrame, CHUNK_SIZE * CHANNEL_COUNT> buffer;
		LocalVector<Callable> sync_callables;
		int clock_rate = BK_DEFAULT_CLOCK_RATE;
		bool is_calling_callbacks : 1 = false;

		Context();
		~Context();
	};

	struct Track {
		BKTrack track = {};
		PackedFloat32Array arpeggio;
		// FixedVector<float, BK_MAX_ARPEGGIO> arpeggio;
		AudioStreamBlipKitPlayback *playback = nullptr;
		RID divider_group;
		RID instrument;
		RID custom_waveform;
		RID sample;
		bool master_volume_changed : 1 = false;

		Track();
		~Track();
	};

	struct Instrument {
		struct Sequence {
			FixedVector<uint32_t, SEQUENCE_SIZE_MAX> steps;
			FixedVector<float, SEQUENCE_SIZE_MAX> values;
			int sustain_offset = 0;
			int sustain_length = 0;
		};

		BKInstrument instrument = {};
		FixedVector<Sequence, ENVELOPE_MAX> sequences;

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
		BKEnum tick();
	};

	static inline BlipKitServer *singleton = nullptr;

	mutable RID_Owner<Context, true> context_owner;
	mutable RID_Owner<Track, true> track_owner;
	mutable RID_Owner<Instrument, true> instrument_owner;
	mutable RID_Owner<Waveform, true> waveform_owner;
	mutable RID_Owner<Sample, true> sample_owner;
	mutable RID_Owner<Divider, true> divider_owner;
	mutable RID_Owner<DividerGroup, true> divider_group_owner;

	RID divider_create(const RID &p_track, int p_tick_interval, const Callable &p_callable);

public:
	static _FORCE_INLINE_ BlipKitServer *get_singleton() { return singleton; }

	static void create();
	static void free();

	RID context_create();
	void context_set_clock_rate(const RID &p_ctx, int p_clock_rate);
	int context_get_clock_rate(const RID &p_ctx) const;
	int32_t context_generate(const RID &p_ctx, AudioFrame *r_buffer, int32_t p_frames);
	int32_t context_generate_samples(const RID &p_ctx, PackedFloat32Array r_buffer, int32_t p_frames);

	RID track_create();
	void track_attach(const RID &p_track, const RID &p_ctx_rid);
	void track_detach(const RID &p_track);
	void track_set_arpeggio(const RID &p_track, const PackedFloat32Array &p_arpeggio);
	TypedArray<RID> track_get_dividers(const RID &p_track) const;
	bool track_has_divider(const RID &p_track, const RID &p_divider) const;
	RID track_add_divider(const RID &p_track, int p_tick_interval, const Callable &p_callable);
	void track_clear_dividers(const RID &p_track);

	void divider_group_set_active(const RID &divider_group, bool p_active);
	bool divider_group_is_active(const RID &divider_group) const;
	void divider_reset(const RID &divider, int p_tick_interval = 0);

	RID instrument_create();

	RID waveform_create();
	bool waveform_set_frames(const RID &p_waveform, const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);
	PackedFloat32Array waveform_get_frames(const RID &p_waveform) const;
	int waveform_get_size(const RID &p_waveform) const;
	bool waveform_is_valid(const RID &p_waveform) const;
	BKData *waveform_get_data(const RID &p_waveform) const;

	RID sample_create();

	void free_rid(const RID &p_rid);

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit
