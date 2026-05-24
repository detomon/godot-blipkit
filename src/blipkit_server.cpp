#include "blipkit_server.hpp"
#include "audio_stream_blipkit.hpp"
#include <godot_cpp/core/error_macros.hpp>

using namespace BlipKit;
using namespace godot;

BKEnum BlipKitServer::DividerGroup::divider_callback(BKCallbackInfo *p_info, void *p_user_info) {
	DividerGroup *group = static_cast<DividerGroup *>(p_user_info);

	// TODO: Implement.

	return BK_SUCCESS;
}

BlipKitServer::DividerGroup::DividerGroup() {
	BKCallback callback = {
		.func = divider_callback,
		.userInfo = static_cast<void *>(this),
	};
	BKDividerInit(&divider, 1, &callback);
}

BlipKitServer::DividerGroup::~DividerGroup() {
	BKDispose(&divider);
}

BlipKitServer::Context::Context() {
	BKInt result = BKContextInit(&ctx, CHANNEL_COUNT, SAMPLE_RATE);
	CRASH_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKContext: %s.", BKStatusGetName(result)));

	const uint32_t buffer_size = CHUNK_SIZE * CHANNEL_COUNT;
	buffer.resize(buffer_size);
}

BlipKitServer::Context::~Context() {
	BKDispose(&ctx);
}

BlipKitServer::Track::Track() {
	BKTrackInit(&track, BK_SQUARE);
}

BlipKitServer::Track::~Track() {
	BKDispose(&track);
}

BlipKitServer::Instrument::Instrument() {
	BKInstrumentInit(&instrument);
	sequences.resize(sequences.capacity());
}

BlipKitServer::Instrument::~Instrument() {
	BKDispose(&instrument);
}

BlipKitServer::Waveform::Waveform() {
	BKDataInit(&data);
}

BlipKitServer::Waveform::~Waveform() {
	BKDispose(&data);
}

BlipKitServer::Sample::Sample() {
	BKDataInit(&data);
}

BlipKitServer::Sample::~Sample() {
	BKDispose(&data);
}

void BlipKitServer::create() {
	if (not singleton) {
		singleton = memnew(BlipKitServer);
	}
}

void BlipKitServer::free() {
	if (singleton) {
		memdelete(singleton);
		singleton = nullptr;
	}
}

RID BlipKitServer::context_create() {
	return context_owner.make_rid();
}

int32_t BlipKitServer::context_generate(const RID &p_ctx, AudioFrame *r_buffer, int32_t p_frames) {
	Context *ctx = context_owner.get_or_null(p_ctx);
	ERR_FAIL_NULL_V(ctx, 0);

	BK_THREAD_SAFE_METHOD

	// Call sync callbacks.
	if (not ctx->sync_callables.is_empty()) {
		ctx->is_calling_callbacks = true;

		for (const Callable &callable : ctx->sync_callables) {
			callable.call();
		}

		ctx->sync_callables.clear();
		ctx->is_calling_callbacks = false;
	}

	int32_t out_count = 0;
	AudioFrame *out_buffer = r_buffer;
	auto &buffer = ctx->buffer;
	BKFrame *chunk_buffer = buffer.ptr();
	constexpr float frame_scale = 1.0 / float(BK_FRAME_MAX);

	while (out_count < p_frames) {
		BKInt chunk_size = MIN(p_frames - out_count, CHUNK_SIZE);
		// Generate frames; produces no errors.
		chunk_size = BKContextGenerate(&ctx->ctx, chunk_buffer, chunk_size);
		// Nothing more to generate.
		if (chunk_size <= 0) {
			break;
		}

		// Fill output buffer.
		for (uint32_t i = 0; i < chunk_size; i++) {
			const float left = float(buffer[i * CHANNEL_COUNT + 0]) * frame_scale;
			const float right = float(buffer[i * CHANNEL_COUNT + 1]) * frame_scale;
			*out_buffer++ = { left, right };
		}

		out_count += chunk_size;
	}

	// Fill rest of output buffer if too few frames are generated.
	for (; out_count < p_frames; out_count++) {
		*out_buffer++ = { 0, 0 };
	}

	return out_count;
}

int32_t BlipKitServer::context_generate_samples(const RID &p_ctx, PackedFloat32Array r_buffer, int32_t p_frames) {
	{ // Check if AudioFrame == float[2].
		constexpr AudioFrame frame = {};
		static_assert(std::is_same<decltype(frame.left), float>::value);
		static_assert(std::is_same<decltype(frame.right), float>::value);
		static_assert(sizeof(frame) == sizeof(float) * 2);
	}

	r_buffer.resize(p_frames * CHANNEL_COUNT);
	AudioFrame *buffer = reinterpret_cast<AudioFrame *>(r_buffer.ptrw());

	return context_generate(p_ctx, buffer, p_frames);
}

RID BlipKitServer::track_create() {
	return track_owner.make_rid();
}

void BlipKitServer::track_attach(const RID &p_track, const RID &p_ctx) {
	Track *track = track_owner.get_or_null(p_track);
	ERR_FAIL_NULL(track);
	Context *ctx = context_owner.get_or_null(p_ctx);
	ERR_FAIL_NULL(ctx);

	BKInt result = BKTrackAttach(&track->track, &ctx->ctx);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to attach BKTrack: %s.", BKStatusGetName(result)));
}

void BlipKitServer::track_detach(const RID &p_track) {
	Track *track = track_owner.get_or_null(p_track);
	ERR_FAIL_NULL(track);

	BKTrackDetach(&track->track);
}

void BlipKitServer::track_set_arpeggio(const RID &p_track, const PackedFloat32Array &p_arpeggio) {
	Track *track = track_owner.get_or_null(p_track);
	ERR_FAIL_NULL(track);

	BK_THREAD_SAFE_METHOD

	const int count = MIN(p_arpeggio.size(), BK_MAX_ARPEGGIO);
	const float *ptr = p_arpeggio.ptr();

	BKInt value[BK_MAX_ARPEGGIO + 1] = { count };
	for (uint32_t i = 0; i < count; i++) {
		value[i + 1] = BKInt(CLAMP(ptr[i], -float(BK_MAX_NOTE), +float(BK_MAX_NOTE)) * float(BK_FINT20_UNIT));
	}

	track->arpeggio = p_arpeggio;
	BKSetPtr(&track, BK_ARPEGGIO, value, (count + 1) * sizeof(BKInt));
}

TypedArray<RID> BlipKitServer::track_get_dividers(const RID &p_track) const {
	Track *track = track_owner.get_or_null(p_track);
	ERR_FAIL_NULL_V(track, {});

	// TODO: Implement.
	return {};
}

bool BlipKitServer::track_has_divider(const RID &p_track, const RID &p_divider) const {
	// TODO: Implement.
	return false;
}

RID BlipKitServer::track_add_divider(const RID &p_track, int p_tick_interval, const Callable &p_callable) {
	// TODO: Implement.
	return RID();
}

void BlipKitServer::track_clear_dividers(const RID &p_track) {
	// TODO: Implement.
}

bool BlipKitServer::waveform_set_frames(const RID &p_waveform, const PackedFloat32Array &p_frames, bool p_normalize, float p_amplitude) {
	Waveform *waveform = waveform_owner.get_or_null(p_waveform);
	ERR_FAIL_NULL_V(waveform, false);

	const uint32_t frames_size = p_frames.size();
	ERR_FAIL_COND_V(frames_size < 2, false);
	ERR_FAIL_COND_V(frames_size > BK_WAVE_MAX_LENGTH, false);

	p_amplitude = CLAMP(p_amplitude, 0.0, 1.0);

	const float *ptr = p_frames.ptr();
	const uint32_t size = MIN(frames_size, BK_WAVE_MAX_LENGTH);
	float scale = 1.0;

	if (p_normalize) {
		float max_value = 0.0;
		for (uint32_t i = 0; i < size; i++) {
			max_value = MAX(max_value, ABS(ptr[i]));
		}

		scale = 0.0;
		if (not Math::is_zero_approx(max_value)) {
			scale = p_amplitude / max_value;
		}
	}

	BK_THREAD_SAFE_METHOD

	auto &frames = waveform->frames;
	frames.resize(size);

	for (uint32_t i = 0; i < size; i++) {
		const float value = CLAMP(ptr[i] * scale, -1.0, +1.0);
		frames[i] = BKFrame(value * float(BK_FRAME_MAX));
	}

	const BKInt result = BKDataSetFrames(&waveform->data, frames.ptr(), frames.size(), 1, false);
	ERR_FAIL_COND_V_MSG(result != BK_SUCCESS, false, vformat("Failed to update BKData: %s.", BKStatusGetName(result)));

	return true;
}

PackedFloat32Array BlipKitServer::waveform_get_frames(const RID &p_waveform) const {
	Waveform *waveform = waveform_owner.get_or_null(p_waveform);
	ERR_FAIL_NULL_V(waveform, {});

	auto &frames = waveform->frames;
	PackedFloat32Array ret;
	ret.resize(frames.size());

	float *ptrw = ret.ptrw();
	const float scale = 1.0 / float(BK_FRAME_MAX);

	for (uint32_t i = 0; i < frames.size(); i++) {
		ptrw[i] = float(frames[i]) * scale;
	}

	return ret;
}

int BlipKitServer::waveform_get_size(const RID &p_waveform) const {
	Waveform *waveform = waveform_owner.get_or_null(p_waveform);
	ERR_FAIL_NULL_V(waveform, 0);

	return waveform->frames.size();
}

bool BlipKitServer::waveform_is_valid(const RID &p_waveform) const {
	Waveform *waveform = waveform_owner.get_or_null(p_waveform);
	ERR_FAIL_NULL_V(waveform, false);

	return not waveform->frames.is_empty();
}

BKData *BlipKitServer::waveform_get_data(const RID &p_waveform) const {
	Waveform *waveform = waveform_owner.get_or_null(p_waveform);
	ERR_FAIL_NULL_V(waveform, nullptr);

	return &waveform->data;
}

RID BlipKitServer::divider_create(const RID &p_track, int p_tick_interval, const Callable &p_callable) {
	Track *track = track_owner.get_or_null(p_track);
	ERR_FAIL_NULL_V(track, RID());

	const RID rid = divider_owner.make_rid();
	Divider *divider = divider_owner.get_or_null(rid);

	divider->callable = p_callable;
	divider->counter = p_tick_interval;

	// TODO: Add divider to track.

	return rid;
}

void BlipKitServer::divider_group_set_active(const RID &p_divider_group, bool p_active) {
	DividerGroup *group = divider_group_owner.get_or_null(p_divider_group);
	ERR_FAIL_NULL(group);

	// TODO: Implement.
}

bool BlipKitServer::divider_group_is_active(const RID &p_divider_group) const {
	DividerGroup *group = divider_group_owner.get_or_null(p_divider_group);
	ERR_FAIL_NULL_V(group, false);

	// TODO: Implement.

	return false;
}

void BlipKitServer::divider_reset(const RID &p_divider, int p_tick_interval) {
	// TODO: Implement.
}

RID BlipKitServer::instrument_create() {
	return instrument_owner.make_rid();
}

RID BlipKitServer::waveform_create() {
	return waveform_owner.make_rid();
}

RID BlipKitServer::sample_create() {
	return sample_owner.make_rid();
}

void BlipKitServer::free_rid(const RID &p_rid) {
	BK_THREAD_SAFE_METHOD

	if (context_owner.owns(p_rid)) {
		context_owner.free(p_rid);
	} else if (track_owner.owns(p_rid)) {
		track_owner.free(p_rid);
	} else if (instrument_owner.owns(p_rid)) {
		instrument_owner.free(p_rid);
	} else if (waveform_owner.owns(p_rid)) {
		waveform_owner.free(p_rid);
	} else if (sample_owner.owns(p_rid)) {
		sample_owner.free(p_rid);
	} else if (divider_owner.owns(p_rid)) {
		divider_owner.free(p_rid);
	} else if (divider_group_owner.owns(p_rid)) {
		divider_group_owner.free(p_rid);
	} else {
		ERR_FAIL_MSG("Invalid ID.");
	}
}

void BlipKitServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("context_create"), &BlipKitServer::context_create);
	ClassDB::bind_method(D_METHOD("context_generate", "ctx", "out_buffer", "frames"), &BlipKitServer::context_generate_samples);

	ClassDB::bind_method(D_METHOD("track_create"), &BlipKitServer::track_create);
	ClassDB::bind_method(D_METHOD("track_attach", "track", "ctx"), &BlipKitServer::track_attach);
	ClassDB::bind_method(D_METHOD("track_detach", "track"), &BlipKitServer::track_detach);
	ClassDB::bind_method(D_METHOD("track_set_arpeggio", "track", "arpeggio"), &BlipKitServer::track_set_arpeggio);
	ClassDB::bind_method(D_METHOD("track_has_divider", "track", "divider"), &BlipKitServer::track_has_divider);
	ClassDB::bind_method(D_METHOD("track_get_dividers", "track"), &BlipKitServer::track_get_dividers);
	ClassDB::bind_method(D_METHOD("track_add_divider", "track", "tick_interval", "callable"), &BlipKitServer::track_add_divider);
	ClassDB::bind_method(D_METHOD("track_clear_dividers", "track"), &BlipKitServer::track_clear_dividers);

	ClassDB::bind_method(D_METHOD("divider_group_set_active", "divider_group", "active"), &BlipKitServer::divider_group_set_active);
	ClassDB::bind_method(D_METHOD("divider_group_is_active", "divider_group"), &BlipKitServer::divider_group_is_active);
	ClassDB::bind_method(D_METHOD("divider_reset", "divider", "tick_interval"), &BlipKitServer::divider_reset);

	ClassDB::bind_method(D_METHOD("instrument_create"), &BlipKitServer::instrument_create);

	ClassDB::bind_method(D_METHOD("waveform_create"), &BlipKitServer::waveform_create);
	ClassDB::bind_method(D_METHOD("waveform_set_frames", "waveform", "frames", "normalize", "amplitude"), &BlipKitServer::waveform_set_frames, DEFVAL(false), DEFVAL(1.0));
	ClassDB::bind_method(D_METHOD("waveform_get_frames", "waveform"), &BlipKitServer::waveform_get_frames);
	ClassDB::bind_method(D_METHOD("waveform_get_size", "waveform"), &BlipKitServer::waveform_get_size);
	ClassDB::bind_method(D_METHOD("waveform_is_valid", "waveform"), &BlipKitServer::waveform_is_valid);

	ClassDB::bind_method(D_METHOD("sample_create"), &BlipKitServer::sample_create);

	ClassDB::bind_method(D_METHOD("free_rid", "rid"), &BlipKitServer::free_rid);

	// BIND_ENUM_CONSTANT(ENVELOPE_VOLUME);
	// BIND_ENUM_CONSTANT(ENVELOPE_PANNING);
	// BIND_ENUM_CONSTANT(ENVELOPE_PITCH);
	// BIND_ENUM_CONSTANT(ENVELOPE_DUTY_CYCLE);
}

String BlipKitServer::_to_string() const {
	return vformat("<BlipKitServer#%d>", get_instance_id());
}
