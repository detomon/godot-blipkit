#include "blipkit_track.hpp"
#include "BKBase.h"
#include "audio_stream_blipkit.hpp"
#include "divider.hpp"
#include "strings.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace BlipKit;
using namespace godot;

static constexpr float MASTER_VOLUME_DEFAULT = 0.15;
static constexpr float MASTER_VOLUME_BASS = 0.3;

BlipKitTrack::BlipKitTrack() {
	BKInt result = BKTrackInit(&track, BK_SQUARE);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKTrack: %s.", BKStatusGetName(result)));

	// Allow setting volume of triangle wave.
	BKSetAttr(&track, BK_TRIANGLE_IGNORES_VOLUME, 0);

	// Set default waveform.
	set_waveform(WAVEFORM_SQUARE);
}

BlipKitTrack::~BlipKitTrack() {
	BK_THREAD_SAFE_METHOD

	detach();
	BKDispose(&track);
}

Ref<BlipKitTrack> BlipKitTrack::create_with_waveform(BlipKitTrack::Waveform p_waveform) {
	Ref<BlipKitTrack> instance;
	instance.instantiate();
	instance->set_waveform(p_waveform);

	return instance;
}

void BlipKitTrack::set_master_volume(float p_master_volume) {
	BK_THREAD_SAFE_METHOD

	p_master_volume = CLAMP(p_master_volume, 0.0, 1.0);
	const BKInt value = BKInt(p_master_volume * float(BK_MAX_VOLUME));

	BKSetAttr(&track, BK_MASTER_VOLUME, value);

	master_volume_changed = true;
}

float BlipKitTrack::get_master_volume() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_MASTER_VOLUME, &value);

	return float(value) / float(BK_MAX_VOLUME);
}

void BlipKitTrack::set_volume(float p_volume) {
	BK_THREAD_SAFE_METHOD

	p_volume = CLAMP(p_volume, 0.0, 1.0);
	const BKInt value = BKInt(p_volume * float(BK_MAX_VOLUME));

	BKSetAttr(&track, BK_VOLUME, value);
}

float BlipKitTrack::get_volume() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_VOLUME, &value);

	return float(value) / float(BK_MAX_VOLUME);
}

void BlipKitTrack::set_panning(float p_panning) {
	BK_THREAD_SAFE_METHOD

	p_panning = CLAMP(p_panning, -1.0, +1.0);
	BKInt value = BKInt(p_panning * float(BK_MAX_VOLUME));

	BKSetAttr(&track, BK_PANNING, value);
}

float BlipKitTrack::get_panning() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_PANNING, &value);

	return float(value) / float(BK_MAX_VOLUME);
}

void BlipKitTrack::set_waveform(BlipKitTrack::Waveform p_waveform) {
	ERR_FAIL_INDEX(p_waveform, WAVEFORM_MAX);

	switch (p_waveform) {
		case WAVEFORM_SQUARE:
		case WAVEFORM_TRIANGLE:
		case WAVEFORM_NOISE:
		case WAVEFORM_SAWTOOTH:
		case WAVEFORM_SINE: {
			// OK.
		} break;
		default: {
			ERR_FAIL_MSG(vformat("Cannot set waveform %d directly.", p_waveform));
		} break;
	}

	mute();
	update_waveform(p_waveform);
}

BlipKitTrack::Waveform BlipKitTrack::get_waveform() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	Waveform waveform = WAVEFORM_SQUARE;

	BKGetAttr(&track, BK_WAVEFORM, &value);

	switch (value) {
		case BK_SQUARE: {
			waveform = WAVEFORM_SQUARE;
		} break;
		case BK_TRIANGLE: {
			waveform = WAVEFORM_TRIANGLE;
		} break;
		case BK_NOISE: {
			waveform = WAVEFORM_NOISE;
		} break;
		case BK_SAWTOOTH: {
			waveform = WAVEFORM_SAWTOOTH;
		} break;
		case BK_SINE: {
			waveform = WAVEFORM_SINE;
		} break;
		case BK_CUSTOM: {
			waveform = WAVEFORM_CUSTOM;
		} break;
		case BK_SAMPLE: {
			waveform = WAVEFORM_SAMPLE;
		} break;
	}

	return waveform;
}

void BlipKitTrack::set_duty_cycle(int p_duty_cycle) {
	BK_THREAD_SAFE_METHOD

	BKSetAttr(&track, BK_DUTY_CYCLE, p_duty_cycle);
}

int BlipKitTrack::get_duty_cycle() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_DUTY_CYCLE, &value);

	return value;
}

void BlipKitTrack::set_note(float p_note) {
	BKInt value;

	if (p_note >= 0.0) {
		p_note = CLAMP(p_note, float(BK_MIN_NOTE), float(BK_MAX_NOTE));
		value = BKInt(p_note * float(BK_FINT20_UNIT));
	} else if (p_note <= -2.0) {
		value = NOTE_MUTE;
	} else {
		value = NOTE_RELEASE;
	}

	BK_THREAD_SAFE_METHOD

	BKSetAttr(&track, BK_NOTE, value);
}

float BlipKitTrack::get_note() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_NOTE, &value);

	// No note set.
	if (value < 0) {
		return float(NOTE_RELEASE);
	}

	return float(value) / float(BK_FINT20_UNIT);
}

void BlipKitTrack::set_pitch(float p_pitch) {
	BK_THREAD_SAFE_METHOD

	p_pitch = CLAMP(p_pitch, -float(BK_MAX_NOTE), +float(BK_MAX_NOTE));
	BKInt value = BKInt(p_pitch * float(BK_FINT20_UNIT));

	BKSetAttr(&track, BK_PITCH, value);
}

float BlipKitTrack::get_pitch() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_PITCH, &value);

	return float(value) / float(BK_FINT20_UNIT);
}

void BlipKitTrack::set_phase_wrap(int p_phase_wrap) {
	BK_THREAD_SAFE_METHOD

	if (p_phase_wrap > 0) {
		p_phase_wrap = MAX(2, p_phase_wrap);
	} else {
		p_phase_wrap = 0;
	}

	BKSetAttr(&track, BK_PHASE_WRAP, p_phase_wrap);
}

int BlipKitTrack::get_phase_wrap() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_PHASE_WRAP, &value);

	return value;
}

void BlipKitTrack::set_volume_slide(int p_volume_slide) {
	BK_THREAD_SAFE_METHOD

	BKSetAttr(&track, BK_EFFECT_VOLUME_SLIDE, p_volume_slide);
}

int BlipKitTrack::get_volume_slide() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_EFFECT_VOLUME_SLIDE, &value);

	return value;
}

void BlipKitTrack::set_panning_slide(int p_panning_slide) {
	BK_THREAD_SAFE_METHOD

	BKSetAttr(&track, BK_EFFECT_PANNING_SLIDE, p_panning_slide);
}

int BlipKitTrack::get_panning_slide() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_EFFECT_VOLUME_SLIDE, &value);

	return value;
}

void BlipKitTrack::set_portamento(int p_portamento) {
	BK_THREAD_SAFE_METHOD

	BKSetAttr(&track, BK_EFFECT_PORTAMENTO, p_portamento);
}

int BlipKitTrack::get_portamento() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_EFFECT_PORTAMENTO, &value);

	return value;
}

void BlipKitTrack::set_tremolo(int p_ticks, float p_delta, int p_slide_ticks) {
	BK_THREAD_SAFE_METHOD

	p_delta = CLAMP(p_delta, 0.0, 1.0);
	p_slide_ticks = MAX(p_slide_ticks, 0);
	const BKInt delta = BKInt(p_delta * float(BK_MAX_VOLUME));
	BKInt values[3] = { p_ticks, delta, p_slide_ticks };

	BKSetPtr(&track, BK_EFFECT_TREMOLO, values, sizeof(values));
}

Dictionary BlipKitTrack::get_tremolo() const {
	Dictionary ret;
	Variant &ticks_value = ret[Strings::get().ticks];
	Variant &delta_value = ret[Strings::get().delta];
	Variant &slide_ticks_value = ret[Strings::get().slide_ticks];

	BK_THREAD_SAFE_METHOD

	BKInt values[3] = { 0 };
	BKGetPtr(&track, BK_EFFECT_TREMOLO, values, sizeof(values));

	ticks_value = values[0];
	delta_value = float(values[1]) / float(BK_MAX_VOLUME);
	slide_ticks_value = values[2];

	return ret;
}

void BlipKitTrack::set_vibrato(int p_ticks, float p_delta, int p_slide_ticks) {
	BK_THREAD_SAFE_METHOD

	p_delta = CLAMP(p_delta, -float(BK_MAX_NOTE), +float(BK_MAX_NOTE));
	p_slide_ticks = MAX(p_slide_ticks, 0);
	const BKInt delta = BKInt(p_delta * float(BK_FINT20_UNIT));
	BKInt values[3] = { p_ticks, delta, p_slide_ticks };

	BKSetPtr(&track, BK_EFFECT_VIBRATO, values, sizeof(values));
}

Dictionary BlipKitTrack::get_vibrato() const {
	Dictionary ret;
	Variant &ticks_value = ret[Strings::get().ticks];
	Variant &delta_value = ret[Strings::get().delta];
	Variant &slide_ticks_value = ret[Strings::get().slide_ticks];

	BK_THREAD_SAFE_METHOD

	BKInt values[3] = { 0 };
	BKGetPtr(&track, BK_EFFECT_VIBRATO, values, sizeof(values));

	ticks_value = values[0];
	delta_value = float(values[1]) / float(BK_MAX_VOLUME);
	slide_ticks_value = values[2];

	return ret;
}

void BlipKitTrack::set_effect_divider(int p_effect_divider) {
	BK_THREAD_SAFE_METHOD

	p_effect_divider = MAX(0, p_effect_divider);
	BKSetAttr(&track, BK_EFFECT_DIVIDER, p_effect_divider);
}

int BlipKitTrack::get_effect_divider() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_EFFECT_DIVIDER, &value);

	return value;
}

void BlipKitTrack::set_arpeggio(const PackedFloat32Array &p_arpeggio) {
	BK_THREAD_SAFE_METHOD

	BKInt value[BK_MAX_ARPEGGIO + 1] = { 0 };
	const int count = MIN(p_arpeggio.size(), BK_MAX_ARPEGGIO);
	const float *ptr = p_arpeggio.ptr();

	value[0] = count;
	for (uint32_t i = 0; i < count; i++) {
		value[i + 1] = BKInt(CLAMP(ptr[i], -float(BK_MAX_NOTE), +float(BK_MAX_NOTE)) * float(BK_FINT20_UNIT));
	}

	arpeggio = p_arpeggio;
	BKSetPtr(&track, BK_ARPEGGIO, value, (count + 1) * sizeof(BKInt));
}

PackedFloat32Array BlipKitTrack::get_arpeggio() const {
	return arpeggio;
}

void BlipKitTrack::set_arpeggio_divider(int p_arpeggio_divider) {
	BK_THREAD_SAFE_METHOD

	p_arpeggio_divider = MAX(0, p_arpeggio_divider);
	BKSetAttr(&track, BK_ARPEGGIO_DIVIDER, p_arpeggio_divider);
}

int BlipKitTrack::get_arpeggio_divider() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_ARPEGGIO_DIVIDER, &value);

	return value;
}

void BlipKitTrack::set_instrument(const Ref<BlipKitInstrument> &p_instrument) {
	BK_THREAD_SAFE_METHOD

	instrument = p_instrument;

	if (instrument.is_valid()) {
		BKSetPtr(&track, BK_INSTRUMENT, instrument->get_instrument(), 0);
	} else {
		BKSetPtr(&track, BK_INSTRUMENT, nullptr, 0);
	}
}

Ref<BlipKitInstrument> BlipKitTrack::get_instrument() {
	return instrument;
}

void BlipKitTrack::set_instrument_divider(int p_instrument_divider) {
	BK_THREAD_SAFE_METHOD

	p_instrument_divider = MAX(0, p_instrument_divider);
	BKSetAttr(&track, BK_INSTRUMENT_DIVIDER, p_instrument_divider);
}

int BlipKitTrack::get_instrument_divider() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_INSTRUMENT_DIVIDER, &value);

	return value;
}

void BlipKitTrack::set_custom_waveform(const Ref<BlipKitWaveform> &p_waveform) {
	BK_THREAD_SAFE_METHOD

	const bool is_set = p_waveform.is_valid();

	if (is_set) {
		ERR_FAIL_COND(not p_waveform->is_valid());
	}

	custom_waveform = p_waveform;
	sample.unref();

	const Waveform waveform = is_set ? WAVEFORM_CUSTOM : WAVEFORM_SQUARE;
	mute();
	update_waveform(waveform);
}

Ref<BlipKitWaveform> BlipKitTrack::get_custom_waveform() {
	return custom_waveform;
}

void BlipKitTrack::set_sample(const Ref<BlipKitSample> &p_sample) {
	BK_THREAD_SAFE_METHOD

	const bool is_set = p_sample.is_valid();

	if (is_set) {
		ERR_FAIL_COND(not p_sample->is_valid());
	}

	sample = p_sample;
	custom_waveform.unref();

	const Waveform waveform = is_set ? WAVEFORM_SAMPLE : WAVEFORM_SQUARE;
	mute();
	update_waveform(waveform);
}

Ref<BlipKitSample> BlipKitTrack::get_sample() {
	return sample;
}

void BlipKitTrack::set_sample_pitch(float p_pitch) {
	p_pitch = CLAMP(p_pitch, -float(BK_MAX_NOTE), +float(BK_MAX_NOTE));
	BKInt value = BKInt(p_pitch * float(BK_FINT20_UNIT));

	BK_THREAD_SAFE_METHOD

	BKSetAttr(&track, BK_SAMPLE_PITCH, value);
}

float BlipKitTrack::get_sample_pitch() const {
	BK_THREAD_SAFE_METHOD

	BKInt value = 0;
	BKGetAttr(&track, BK_SAMPLE_PITCH, &value);

	return float(value) / float(BK_FINT20_UNIT);
}

void BlipKitTrack::attach(AudioStreamBlipKit *p_stream) {
	BK_THREAD_SAFE_METHOD

	ERR_FAIL_NULL(p_stream);

	Ref<AudioStreamBlipKitPlayback> stream_playback = p_stream->get_playback();

	ERR_FAIL_COND(stream_playback.is_null());

	BKContext *context = stream_playback->get_context();

	BKTrackAttach(&track, context);
	playback = stream_playback.ptr();
	playback->attach(this);

	if (custom_waveform.is_valid()) {
		// Custom waveform needs to be set again after attaching.
		set_custom_waveform(custom_waveform);
	} else if (sample.is_valid()) {
		// Sample needs to be set again after attaching.
		set_sample(sample);
	}

	// TODO: Make better.
	set_note(get_note());

	dividers.attach(playback);
}

void BlipKitTrack::detach() {
	BK_THREAD_SAFE_METHOD

	if (not playback) {
		return;
	}

	dividers.detach();

	mute();
	BKTrackDetach(&track);

	playback->detach(this);
	playback = nullptr;
}

void BlipKitTrack::release() {
	set_note(float(NOTE_RELEASE));
}

void BlipKitTrack::mute() {
	set_note(float(NOTE_MUTE));
}

void BlipKitTrack::reset() {
	BK_THREAD_SAFE_METHOD

	const Waveform waveform = get_waveform();
	const float master_volume = get_master_volume();

	BKTrackReset(&track);
	instrument.unref();
	arpeggio.clear();

	// TODO: Reset custom waveform and sample?

	update_waveform(waveform);
	set_master_volume(master_volume);
}

bool BlipKitTrack::has_divider(DividerGroup::ID p_id) {
	BK_THREAD_SAFE_METHOD

	return dividers.has_divider(p_id);
}

PackedInt32Array BlipKitTrack::get_dividers() const {
	BK_THREAD_SAFE_METHOD

	return dividers.get_dividers();
}

DividerGroup::ID BlipKitTrack::add_divider(int p_tick_interval, Callable p_callable) {
	ERR_FAIL_COND_V(p_tick_interval <= 0, 0);
	ERR_FAIL_COND_V(p_callable.is_null(), 0);

	BK_THREAD_SAFE_METHOD

	return dividers.add_divider(p_tick_interval, p_callable);
}

void BlipKitTrack::remove_divider(DividerGroup::ID p_id) {
	BK_THREAD_SAFE_METHOD

	ERR_FAIL_COND(not has_divider(p_id));

	dividers.remove_divider(p_id);
}

void BlipKitTrack::reset_divider(DividerGroup::ID p_id, int p_tick_interval) {
	BK_THREAD_SAFE_METHOD

	dividers.reset_divider(p_id, p_tick_interval);
}

void BlipKitTrack::clear_dividers() {
	BK_THREAD_SAFE_METHOD

	dividers.clear();
}

void BlipKitTrack::update_waveform(Waveform p_waveform) {
	ERR_FAIL_INDEX(p_waveform, WAVEFORM_MAX);

	BK_THREAD_SAFE_METHOD

	switch (p_waveform) {
		case WAVEFORM_SQUARE:
		case WAVEFORM_TRIANGLE:
		case WAVEFORM_NOISE:
		case WAVEFORM_SAWTOOTH:
		case WAVEFORM_SINE: {
			BKInt waveform = BK_SQUARE;

			switch (p_waveform) {
				case WAVEFORM_SQUARE: {
					waveform = BK_SQUARE;
				} break;
				case WAVEFORM_TRIANGLE: {
					waveform = BK_TRIANGLE;
				} break;
				case WAVEFORM_NOISE: {
					waveform = BK_NOISE;
				} break;
				case WAVEFORM_SAWTOOTH: {
					waveform = BK_SAWTOOTH;
				} break;
				case WAVEFORM_SINE: {
					waveform = BK_SINE;
				} break;
				case WAVEFORM_CUSTOM: {
					waveform = BK_CUSTOM;
				} break;
				case WAVEFORM_SAMPLE: {
					waveform = BK_SAMPLE;
				} break;
				default: {
					// Ignore.
				} break;
			}

			BKSetAttr(&track, BK_WAVEFORM, waveform);
		} break;
		case WAVEFORM_CUSTOM: {
			BKData *data = custom_waveform->get_data();
			const BKInt result = BKSetPtr(&track, BK_WAVEFORM, data, 0);

			if (result == BK_INVALID_STATE) {
				// OK. Track is not attached yet.
			} else {
				ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to set custom waveform: %s.", BKStatusGetName(result)));
			}
		} break;
		case WAVEFORM_SAMPLE: {
			BKData *data = sample->get_data();
			const BKInt result = BKSetPtr(&track, BK_SAMPLE, data, 0);

			if (result == BK_INVALID_STATE) {
				// OK. Track is not attached yet.
			} else {
				ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to set sample: %s.", BKStatusGetName(result)));
			}

			const BlipKitSample::RepeatMode repeat_mode = sample->get_repeat_mode();

			if (repeat_mode == BlipKitSample::RepeatMode::REPEAT_BACKWARD) {
				BKInt range[2] = { -1, 0 };
				BKSetPtr(&track, BK_SAMPLE_RANGE, range, sizeof(range));
			}

			BKInt bk_repeat_mode = BK_NO_REPEAT;

			switch (repeat_mode) {
				case BlipKitSample::RepeatMode::REPEAT_NONE: {
					bk_repeat_mode = BK_NO_REPEAT;
				} break;
				case BlipKitSample::RepeatMode::REPEAT_FORWARD:
				case BlipKitSample::RepeatMode::REPEAT_BACKWARD: {
					bk_repeat_mode = BK_REPEAT;
				} break;
				case BlipKitSample::RepeatMode::REPEAT_PING_PONG: {
					bk_repeat_mode = BK_PALINDROME;
				} break;
				default: {
					// Ignore.
				} break;
			}

			BKInt sustain_range[2] = { sample->get_sustain_offset(), sample->get_sustain_end() };
			BKSetPtr(&track, BK_SAMPLE_SUSTAIN_RANGE, sustain_range, sizeof(sustain_range));

			BKSetAttr(&track, BK_SAMPLE_REPEAT, bk_repeat_mode);
		} break;
		default: {
			// Should not happen.
			return;
		} break;
	}

	// Only set master volume if not changed by user.
	if (not master_volume_changed) {
		float master_volume = 0.0;

		switch (p_waveform) {
			case WAVEFORM_SQUARE: {
				master_volume = MASTER_VOLUME_DEFAULT;
			} break;
			case WAVEFORM_TRIANGLE: {
				master_volume = MASTER_VOLUME_BASS;
			} break;
			case WAVEFORM_NOISE: {
				master_volume = MASTER_VOLUME_DEFAULT;
			} break;
			case WAVEFORM_SAWTOOTH: {
				master_volume = MASTER_VOLUME_DEFAULT;
			} break;
			case WAVEFORM_SINE: {
				master_volume = MASTER_VOLUME_BASS;
			} break;
			case WAVEFORM_CUSTOM: {
				master_volume = MASTER_VOLUME_DEFAULT;
			} break;
			case WAVEFORM_SAMPLE: {
				master_volume = MASTER_VOLUME_BASS;
			} break;
			default: {
				// Ignore.
			} break;
		}

		set_master_volume(master_volume);
		master_volume_changed = false;
	}
}

void BlipKitTrack::_bind_methods() {
	ClassDB::bind_static_method("BlipKitTrack", D_METHOD("create_with_waveform", "waveform"), &BlipKitTrack::create_with_waveform);

	ClassDB::bind_method(D_METHOD("set_waveform"), &BlipKitTrack::set_waveform);
	ClassDB::bind_method(D_METHOD("get_waveform"), &BlipKitTrack::get_waveform);
	ClassDB::bind_method(D_METHOD("set_duty_cycle"), &BlipKitTrack::set_duty_cycle);
	ClassDB::bind_method(D_METHOD("get_duty_cycle"), &BlipKitTrack::get_duty_cycle);
	ClassDB::bind_method(D_METHOD("set_master_volume"), &BlipKitTrack::set_master_volume);
	ClassDB::bind_method(D_METHOD("get_master_volume"), &BlipKitTrack::get_master_volume);
	ClassDB::bind_method(D_METHOD("set_volume"), &BlipKitTrack::set_volume);
	ClassDB::bind_method(D_METHOD("get_volume"), &BlipKitTrack::get_volume);
	ClassDB::bind_method(D_METHOD("set_panning"), &BlipKitTrack::set_panning);
	ClassDB::bind_method(D_METHOD("get_panning"), &BlipKitTrack::get_panning);
	ClassDB::bind_method(D_METHOD("set_note"), &BlipKitTrack::set_note);
	ClassDB::bind_method(D_METHOD("get_note"), &BlipKitTrack::get_note);
	ClassDB::bind_method(D_METHOD("set_pitch"), &BlipKitTrack::set_pitch);
	ClassDB::bind_method(D_METHOD("get_pitch"), &BlipKitTrack::get_pitch);
	ClassDB::bind_method(D_METHOD("set_phase_wrap"), &BlipKitTrack::set_phase_wrap);
	ClassDB::bind_method(D_METHOD("get_phase_wrap"), &BlipKitTrack::get_phase_wrap);
	ClassDB::bind_method(D_METHOD("set_volume_slide"), &BlipKitTrack::set_volume_slide);
	ClassDB::bind_method(D_METHOD("get_volume_slide"), &BlipKitTrack::get_volume_slide);
	ClassDB::bind_method(D_METHOD("set_panning_slide"), &BlipKitTrack::set_panning_slide);
	ClassDB::bind_method(D_METHOD("get_panning_slide"), &BlipKitTrack::get_panning_slide);
	ClassDB::bind_method(D_METHOD("set_portamento"), &BlipKitTrack::set_portamento);
	ClassDB::bind_method(D_METHOD("get_portamento"), &BlipKitTrack::get_portamento);
	ClassDB::bind_method(D_METHOD("set_tremolo", "ticks", "delta", "slide_ticks"), &BlipKitTrack::set_tremolo, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("set_vibrato", "ticks", "delta", "slide_ticks"), &BlipKitTrack::set_vibrato, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("get_tremolo"), &BlipKitTrack::get_tremolo);
	ClassDB::bind_method(D_METHOD("get_vibrato"), &BlipKitTrack::get_vibrato);
	ClassDB::bind_method(D_METHOD("set_effect_divider"), &BlipKitTrack::set_effect_divider);
	ClassDB::bind_method(D_METHOD("get_effect_divider"), &BlipKitTrack::get_effect_divider);
	ClassDB::bind_method(D_METHOD("set_arpeggio"), &BlipKitTrack::set_arpeggio);
	ClassDB::bind_method(D_METHOD("get_arpeggio"), &BlipKitTrack::get_arpeggio);
	ClassDB::bind_method(D_METHOD("set_arpeggio_divider"), &BlipKitTrack::set_arpeggio_divider);
	ClassDB::bind_method(D_METHOD("get_arpeggio_divider"), &BlipKitTrack::get_arpeggio_divider);
	ClassDB::bind_method(D_METHOD("set_instrument"), &BlipKitTrack::set_instrument);
	ClassDB::bind_method(D_METHOD("get_instrument"), &BlipKitTrack::get_instrument);
	ClassDB::bind_method(D_METHOD("set_instrument_divider"), &BlipKitTrack::set_instrument_divider);
	ClassDB::bind_method(D_METHOD("get_instrument_divider"), &BlipKitTrack::get_instrument_divider);
	ClassDB::bind_method(D_METHOD("set_custom_waveform"), &BlipKitTrack::set_custom_waveform);
	ClassDB::bind_method(D_METHOD("get_custom_waveform"), &BlipKitTrack::get_custom_waveform);
	ClassDB::bind_method(D_METHOD("set_sample"), &BlipKitTrack::set_sample);
	ClassDB::bind_method(D_METHOD("get_sample"), &BlipKitTrack::get_sample);
	ClassDB::bind_method(D_METHOD("set_sample_pitch"), &BlipKitTrack::set_sample_pitch);
	ClassDB::bind_method(D_METHOD("get_sample_pitch"), &BlipKitTrack::get_sample_pitch);
	ClassDB::bind_method(D_METHOD("attach", "playback"), &BlipKitTrack::attach);
	ClassDB::bind_method(D_METHOD("detach"), &BlipKitTrack::detach);
	ClassDB::bind_method(D_METHOD("release"), &BlipKitTrack::release);
	ClassDB::bind_method(D_METHOD("mute"), &BlipKitTrack::mute);
	ClassDB::bind_method(D_METHOD("reset"), &BlipKitTrack::reset);
	ClassDB::bind_method(D_METHOD("get_dividers"), &BlipKitTrack::get_dividers);
	ClassDB::bind_method(D_METHOD("has_divider", "id"), &BlipKitTrack::has_divider);
	ClassDB::bind_method(D_METHOD("add_divider", "tick_interval", "callback"), &BlipKitTrack::add_divider);
	ClassDB::bind_method(D_METHOD("remove_divider", "id"), &BlipKitTrack::remove_divider);
	ClassDB::bind_method(D_METHOD("clear_dividers"), &BlipKitTrack::clear_dividers);
	ClassDB::bind_method(D_METHOD("reset_divider", "id", "tick_interval"), &BlipKitTrack::reset_divider, DEFVAL(0));

	ADD_PROPERTY(PropertyInfo(Variant::INT, "waveform"), "set_waveform", "get_waveform");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "duty_cycle"), "set_duty_cycle", "get_duty_cycle");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "master_volume"), "set_master_volume", "get_master_volume");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "volume"), "set_volume", "get_volume");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "panning"), "set_panning", "get_panning");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "note"), "set_note", "get_note");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "pitch"), "set_pitch", "get_pitch");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "phase_wrap"), "set_phase_wrap", "get_phase_wrap");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "volume_slide"), "set_volume_slide", "get_volume_slide");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "panning_slide"), "set_panning_slide", "get_panning_slide");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "portamento"), "set_portamento", "get_portamento");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "effect_divider"), "set_effect_divider", "get_effect_divider");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "arpeggio"), "set_arpeggio", "get_arpeggio");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "arpeggio_divider"), "set_arpeggio_divider", "get_arpeggio_divider");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "instrument"), "set_instrument", "get_instrument");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "instrument_divider"), "set_instrument_divider", "get_instrument_divider");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "custom_waveform"), "set_custom_waveform", "get_custom_waveform");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "sample"), "set_sample", "get_sample");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sample_pitch"), "set_sample_pitch", "get_sample_pitch");

	BIND_ENUM_CONSTANT(WAVEFORM_SQUARE);
	BIND_ENUM_CONSTANT(WAVEFORM_TRIANGLE);
	BIND_ENUM_CONSTANT(WAVEFORM_NOISE);
	BIND_ENUM_CONSTANT(WAVEFORM_SAWTOOTH);
	BIND_ENUM_CONSTANT(WAVEFORM_SINE);
	BIND_ENUM_CONSTANT(WAVEFORM_CUSTOM);
	BIND_ENUM_CONSTANT(WAVEFORM_SAMPLE);

	BIND_ENUM_CONSTANT(NOTE_C_0);
	BIND_ENUM_CONSTANT(NOTE_C_SH_0);
	BIND_ENUM_CONSTANT(NOTE_D_0);
	BIND_ENUM_CONSTANT(NOTE_D_SH_0);
	BIND_ENUM_CONSTANT(NOTE_E_0);
	BIND_ENUM_CONSTANT(NOTE_F_0);
	BIND_ENUM_CONSTANT(NOTE_F_SH_0);
	BIND_ENUM_CONSTANT(NOTE_G_0);
	BIND_ENUM_CONSTANT(NOTE_G_SH_0);
	BIND_ENUM_CONSTANT(NOTE_A_0);
	BIND_ENUM_CONSTANT(NOTE_A_SH_0);
	BIND_ENUM_CONSTANT(NOTE_B_0);
	BIND_ENUM_CONSTANT(NOTE_C_1);
	BIND_ENUM_CONSTANT(NOTE_C_SH_1);
	BIND_ENUM_CONSTANT(NOTE_D_1);
	BIND_ENUM_CONSTANT(NOTE_D_SH_1);
	BIND_ENUM_CONSTANT(NOTE_E_1);
	BIND_ENUM_CONSTANT(NOTE_F_1);
	BIND_ENUM_CONSTANT(NOTE_F_SH_1);
	BIND_ENUM_CONSTANT(NOTE_G_1);
	BIND_ENUM_CONSTANT(NOTE_G_SH_1);
	BIND_ENUM_CONSTANT(NOTE_A_1);
	BIND_ENUM_CONSTANT(NOTE_A_SH_1);
	BIND_ENUM_CONSTANT(NOTE_B_1);
	BIND_ENUM_CONSTANT(NOTE_C_2);
	BIND_ENUM_CONSTANT(NOTE_C_SH_2);
	BIND_ENUM_CONSTANT(NOTE_D_2);
	BIND_ENUM_CONSTANT(NOTE_D_SH_2);
	BIND_ENUM_CONSTANT(NOTE_E_2);
	BIND_ENUM_CONSTANT(NOTE_F_2);
	BIND_ENUM_CONSTANT(NOTE_F_SH_2);
	BIND_ENUM_CONSTANT(NOTE_G_2);
	BIND_ENUM_CONSTANT(NOTE_G_SH_2);
	BIND_ENUM_CONSTANT(NOTE_A_2);
	BIND_ENUM_CONSTANT(NOTE_A_SH_2);
	BIND_ENUM_CONSTANT(NOTE_B_2);
	BIND_ENUM_CONSTANT(NOTE_C_3);
	BIND_ENUM_CONSTANT(NOTE_C_SH_3);
	BIND_ENUM_CONSTANT(NOTE_D_3);
	BIND_ENUM_CONSTANT(NOTE_D_SH_3);
	BIND_ENUM_CONSTANT(NOTE_E_3);
	BIND_ENUM_CONSTANT(NOTE_F_3);
	BIND_ENUM_CONSTANT(NOTE_F_SH_3);
	BIND_ENUM_CONSTANT(NOTE_G_3);
	BIND_ENUM_CONSTANT(NOTE_G_SH_3);
	BIND_ENUM_CONSTANT(NOTE_A_3);
	BIND_ENUM_CONSTANT(NOTE_A_SH_3);
	BIND_ENUM_CONSTANT(NOTE_B_3);
	BIND_ENUM_CONSTANT(NOTE_C_4);
	BIND_ENUM_CONSTANT(NOTE_C_SH_4);
	BIND_ENUM_CONSTANT(NOTE_D_4);
	BIND_ENUM_CONSTANT(NOTE_D_SH_4);
	BIND_ENUM_CONSTANT(NOTE_E_4);
	BIND_ENUM_CONSTANT(NOTE_F_4);
	BIND_ENUM_CONSTANT(NOTE_F_SH_4);
	BIND_ENUM_CONSTANT(NOTE_G_4);
	BIND_ENUM_CONSTANT(NOTE_G_SH_4);
	BIND_ENUM_CONSTANT(NOTE_A_4);
	BIND_ENUM_CONSTANT(NOTE_A_SH_4);
	BIND_ENUM_CONSTANT(NOTE_B_4);
	BIND_ENUM_CONSTANT(NOTE_C_5);
	BIND_ENUM_CONSTANT(NOTE_C_SH_5);
	BIND_ENUM_CONSTANT(NOTE_D_5);
	BIND_ENUM_CONSTANT(NOTE_D_SH_5);
	BIND_ENUM_CONSTANT(NOTE_E_5);
	BIND_ENUM_CONSTANT(NOTE_F_5);
	BIND_ENUM_CONSTANT(NOTE_F_SH_5);
	BIND_ENUM_CONSTANT(NOTE_G_5);
	BIND_ENUM_CONSTANT(NOTE_G_SH_5);
	BIND_ENUM_CONSTANT(NOTE_A_5);
	BIND_ENUM_CONSTANT(NOTE_A_SH_5);
	BIND_ENUM_CONSTANT(NOTE_B_5);
	BIND_ENUM_CONSTANT(NOTE_C_6);
	BIND_ENUM_CONSTANT(NOTE_C_SH_6);
	BIND_ENUM_CONSTANT(NOTE_D_6);
	BIND_ENUM_CONSTANT(NOTE_D_SH_6);
	BIND_ENUM_CONSTANT(NOTE_E_6);
	BIND_ENUM_CONSTANT(NOTE_F_6);
	BIND_ENUM_CONSTANT(NOTE_F_SH_6);
	BIND_ENUM_CONSTANT(NOTE_G_6);
	BIND_ENUM_CONSTANT(NOTE_G_SH_6);
	BIND_ENUM_CONSTANT(NOTE_A_6);
	BIND_ENUM_CONSTANT(NOTE_A_SH_6);
	BIND_ENUM_CONSTANT(NOTE_B_6);
	BIND_ENUM_CONSTANT(NOTE_C_7);
	BIND_ENUM_CONSTANT(NOTE_C_SH_7);
	BIND_ENUM_CONSTANT(NOTE_D_7);
	BIND_ENUM_CONSTANT(NOTE_D_SH_7);
	BIND_ENUM_CONSTANT(NOTE_E_7);
	BIND_ENUM_CONSTANT(NOTE_F_7);
	BIND_ENUM_CONSTANT(NOTE_F_SH_7);
	BIND_ENUM_CONSTANT(NOTE_G_7);
	BIND_ENUM_CONSTANT(NOTE_G_SH_7);
	BIND_ENUM_CONSTANT(NOTE_A_7);
	BIND_ENUM_CONSTANT(NOTE_A_SH_7);
	BIND_ENUM_CONSTANT(NOTE_B_7);
	BIND_ENUM_CONSTANT(NOTE_C_8);
	BIND_ENUM_CONSTANT(NOTE_RELEASE);
	BIND_ENUM_CONSTANT(NOTE_MUTE);

	BIND_CONSTANT(ARPEGGIO_MAX);
}

String BlipKitTrack::_to_string() const {
	return vformat("<BlipKitTrack#%d>", get_instance_id());
}
