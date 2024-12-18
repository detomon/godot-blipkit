#include "blipkit_track.hpp"
#include "audio_stream_blipkit.hpp"
#include "divider.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace detomon::BlipKit;
using namespace godot;

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
	ClassDB::bind_method(D_METHOD("attach", "playback"), &BlipKitTrack::attach);
	ClassDB::bind_method(D_METHOD("detach"), &BlipKitTrack::detach);
	ClassDB::bind_method(D_METHOD("release"), &BlipKitTrack::release);
	ClassDB::bind_method(D_METHOD("mute"), &BlipKitTrack::mute);
	ClassDB::bind_method(D_METHOD("reset"), &BlipKitTrack::reset);
	ClassDB::bind_method(D_METHOD("get_divider_names"), &BlipKitTrack::get_divider_names);
	ClassDB::bind_method(D_METHOD("add_divider", "name", "tick_interval", "callable"), &BlipKitTrack::add_divider);
	ClassDB::bind_method(D_METHOD("remove_divider", "name"), &BlipKitTrack::remove_divider);
	ClassDB::bind_method(D_METHOD("clear_dividers"), &BlipKitTrack::clear_dividers);
	ClassDB::bind_method(D_METHOD("reset_divider", "name", "tick_interval"), &BlipKitTrack::reset_divider, DEFVAL(0));

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

	BIND_ENUM_CONSTANT(WAVEFORM_SQUARE);
	BIND_ENUM_CONSTANT(WAVEFORM_TRIANGLE);
	BIND_ENUM_CONSTANT(WAVEFORM_NOISE);
	BIND_ENUM_CONSTANT(WAVEFORM_SAWTOOTH);
	BIND_ENUM_CONSTANT(WAVEFORM_SINE);
	BIND_ENUM_CONSTANT(WAVEFORM_CUSTOM);

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
}

BlipKitTrack::BlipKitTrack() {
	BKInt result = BKTrackInit(&track, BK_SQUARE);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKTrack: %s.", BKStatusGetName(result)));

	// Set default waveform and master volume.
	set_waveform(WAVEFORM_SQUARE);
	// Allow setting volume of triangle wave.
	BKSetAttr(&track, BK_TRIANGLE_IGNORES_VOLUME, 0);
}

BlipKitTrack::~BlipKitTrack() {
	detach();

	AudioStreamBlipKit::lock();
	BKDispose(&track);
	AudioStreamBlipKit::unlock();
}

Ref<BlipKitTrack> BlipKitTrack::create_with_waveform(BlipKitTrack::Waveform p_waveform) {
	Ref<BlipKitTrack> instance;
	instance.instantiate();
	instance->set_waveform(p_waveform);

	return instance;
}

String BlipKitTrack::_to_string() const {
	return vformat("BlipKitTrack: waveform=%d, volume=%f (%f), note=%f", (int)get_waveform(), get_volume(), get_master_volume(), get_note());
}

real_t BlipKitTrack::get_master_volume() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_MASTER_VOLUME, &value);
	AudioStreamBlipKit::unlock();

	return real_t(value) / real_t(BK_MAX_VOLUME);
}

void BlipKitTrack::set_master_volume(real_t p_master_volume) {
	p_master_volume = CLAMP(p_master_volume, 0.0, 1.0);
	const BKInt value = BKInt(p_master_volume * real_t(BK_MAX_VOLUME));

	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_MASTER_VOLUME, value);
	AudioStreamBlipKit::unlock();
}

real_t BlipKitTrack::get_volume() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_VOLUME, &value);
	AudioStreamBlipKit::unlock();

	return real_t(value) / real_t(BK_MAX_VOLUME);
}

void BlipKitTrack::set_volume(real_t p_volume) {
	p_volume = CLAMP(p_volume, 0.0, 1.0);
	const BKInt value = BKInt(p_volume * real_t(BK_MAX_VOLUME));

	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_VOLUME, value);
	AudioStreamBlipKit::unlock();
}

real_t BlipKitTrack::get_panning() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_PANNING, &value);
	AudioStreamBlipKit::unlock();

	return real_t(value) / real_t(BK_MAX_VOLUME);
}

void BlipKitTrack::set_panning(real_t p_panning) {
	p_panning = CLAMP(p_panning, -1.0, +1.0);
	BKInt value = BKInt(p_panning * real_t(BK_MAX_VOLUME));

	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_PANNING, value);
	AudioStreamBlipKit::unlock();
}

BlipKitTrack::Waveform BlipKitTrack::get_waveform() const {
	BKInt value = 0;
	Waveform waveform = WAVEFORM_SQUARE;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_WAVEFORM, &value);
	AudioStreamBlipKit::unlock();

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

void BlipKitTrack::set_waveform(BlipKitTrack::Waveform p_waveform) {
	BKInt waveform = 0;
	real_t master_volume = 0.0;

	switch (p_waveform) {
		case WAVEFORM_SQUARE: {
			waveform = BK_SQUARE;
			master_volume = 0.15;
		} break;
		case WAVEFORM_TRIANGLE: {
			waveform = BK_TRIANGLE;
			master_volume = 0.3;
		} break;
		case WAVEFORM_NOISE: {
			waveform = BK_NOISE;
			master_volume = 0.15;
		} break;
		case WAVEFORM_SAWTOOTH: {
			waveform = BK_SAWTOOTH;
			master_volume = 0.15;
		} break;
		case WAVEFORM_SINE: {
			waveform = BK_SINE;
			master_volume = 0.3;
		} break;
		default: {
			ERR_FAIL_MSG(vformat("Cannot set waveform %d directly.", p_waveform));
		} break;
	}

	custom_waveform.unref();

	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_WAVEFORM, waveform);
	set_master_volume(master_volume);
	AudioStreamBlipKit::unlock();
}

int BlipKitTrack::get_duty_cycle() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_DUTY_CYCLE, &value);
	AudioStreamBlipKit::unlock();

	return value;
}

void BlipKitTrack::set_duty_cycle(int p_duty_cycle) {
	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_DUTY_CYCLE, p_duty_cycle);
	AudioStreamBlipKit::unlock();
}

real_t BlipKitTrack::get_note() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_NOTE, &value);
	AudioStreamBlipKit::unlock();

	// No note set.
	if (value < 0) {
		return real_t(NOTE_RELEASE);
	}

	return real_t(value) / real_t(BK_FINT20_UNIT);
}

void BlipKitTrack::set_note(real_t p_note) {
	BKInt value;

	if (p_note >= 0.0) {
		p_note = CLAMP(p_note, real_t(BK_MIN_NOTE), real_t(BK_MAX_NOTE));
		value = BKInt(p_note * real_t(BK_FINT20_UNIT));
	} else if (p_note <= -2.0) {
		value = NOTE_MUTE;
	} else {
		value = NOTE_RELEASE;
	}

	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_NOTE, value);
	AudioStreamBlipKit::unlock();
}

real_t BlipKitTrack::get_pitch() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_PITCH, &value);
	AudioStreamBlipKit::unlock();

	return real_t(value) / real_t(BK_FINT20_UNIT);
}

void BlipKitTrack::set_pitch(real_t p_pitch) {
	p_pitch = CLAMP(p_pitch, -real_t(BK_MAX_NOTE), +real_t(BK_MAX_NOTE));
	BKInt value = BKInt(p_pitch * real_t(BK_FINT20_UNIT));

	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_PITCH, value);
	AudioStreamBlipKit::unlock();
}

int BlipKitTrack::get_phase_wrap() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_PHASE_WRAP, &value);
	AudioStreamBlipKit::unlock();

	return value;
}

void BlipKitTrack::set_phase_wrap(int p_phase_wrap) {
	if (p_phase_wrap > 0) {
		p_phase_wrap = MAX(2, p_phase_wrap);
	} else {
		p_phase_wrap = 0;
	}

	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_PHASE_WRAP, p_phase_wrap);
	AudioStreamBlipKit::unlock();
}

int BlipKitTrack::get_volume_slide() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_EFFECT_VOLUME_SLIDE, &value);
	AudioStreamBlipKit::unlock();

	return value;
}

void BlipKitTrack::set_volume_slide(int p_volume_slide) {
	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_EFFECT_VOLUME_SLIDE, p_volume_slide);
	AudioStreamBlipKit::unlock();
}

int BlipKitTrack::get_panning_slide() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_EFFECT_VOLUME_SLIDE, &value);
	AudioStreamBlipKit::unlock();

	return value;
}

void BlipKitTrack::set_panning_slide(int p_panning_slide) {
	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_EFFECT_PANNING_SLIDE, p_panning_slide);
	AudioStreamBlipKit::unlock();
}

int BlipKitTrack::get_portamento() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_EFFECT_PORTAMENTO, &value);
	AudioStreamBlipKit::unlock();

	return value;
}

void BlipKitTrack::set_portamento(int p_portamento) {
	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_EFFECT_PORTAMENTO, p_portamento);
	AudioStreamBlipKit::unlock();
}

void BlipKitTrack::set_tremolo(int p_ticks, float p_delta, int p_slide_ticks) {
	p_delta = CLAMP(p_delta, 0.0, 1.0);
	p_slide_ticks = MAX(p_slide_ticks, 0);
	const BKInt delta = BKInt(p_delta * real_t(BK_MAX_VOLUME));
	BKInt values[3] = { p_ticks, delta, p_slide_ticks };

	AudioStreamBlipKit::lock();
	BKSetPtr(&track, BK_EFFECT_TREMOLO, values, sizeof(values));
	AudioStreamBlipKit::unlock();
}

Dictionary BlipKitTrack::get_tremolo() const {
	BKInt values[3] = { 0 };

	AudioStreamBlipKit::lock();
	BKGetPtr(&track, BK_EFFECT_TREMOLO, values, sizeof(values));
	AudioStreamBlipKit::unlock();

	const int ticks = values[0];
	const real_t delta = real_t(values[1]) / real_t(BK_MAX_VOLUME);
	const int slide_ticks = values[2];

	Dictionary ret;
	ret[StringName("ticks")] = ticks;
	ret[StringName("delta")] = delta;
	ret[StringName("slide_ticks")] = slide_ticks;

	return ret;
}

Dictionary BlipKitTrack::get_vibrato() const {
	BKInt values[3] = { 0 };

	AudioStreamBlipKit::lock();
	BKGetPtr(&track, BK_EFFECT_VIBRATO, values, sizeof(values));
	AudioStreamBlipKit::unlock();

	const int ticks = values[0];
	const real_t delta = real_t(values[1]) / real_t(BK_FINT20_UNIT);
	const int slide_ticks = values[2];

	Dictionary ret;
	ret[StringName("ticks")] = ticks;
	ret[StringName("delta")] = delta;
	ret[StringName("slide_ticks")] = slide_ticks;

	return ret;
}

void BlipKitTrack::set_vibrato(int p_ticks, float p_delta, int p_slide_ticks) {
	p_delta = CLAMP(p_delta, -real_t(BK_MAX_NOTE), +real_t(BK_MAX_NOTE));
	p_slide_ticks = MAX(p_slide_ticks, 0);
	const BKInt delta = BKInt(p_delta * real_t(BK_FINT20_UNIT));
	BKInt values[3] = { p_ticks, delta, p_slide_ticks };

	AudioStreamBlipKit::lock();
	BKSetPtr(&track, BK_EFFECT_VIBRATO, values, sizeof(values));
	AudioStreamBlipKit::unlock();
}

int BlipKitTrack::get_effect_divider() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_EFFECT_DIVIDER, &value);
	AudioStreamBlipKit::unlock();

	return value;
}

void BlipKitTrack::set_effect_divider(int p_effect_divider) {
	p_effect_divider = MAX(0, p_effect_divider);

	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_EFFECT_DIVIDER, p_effect_divider);
	AudioStreamBlipKit::unlock();
}

PackedFloat32Array BlipKitTrack::get_arpeggio() const {
	BKInt value[BK_MAX_ARPEGGIO + 1] = { 0 };

	AudioStreamBlipKit::lock();
	BKGetPtr(&track, BK_ARPEGGIO, value, (BK_MAX_ARPEGGIO + 1) * sizeof(BKInt));
	AudioStreamBlipKit::unlock();

	BKInt count = value[0];
	PackedFloat32Array arpeggio;
	arpeggio.resize(count);

	for (int i = 0; i < count; i++) {
		arpeggio[i] = real_t(value[i + 1]) / real_t(BK_FINT20_UNIT);
	}

	return arpeggio;
}

void BlipKitTrack::set_arpeggio(const PackedFloat32Array &p_arpeggio) {
	BKInt value[BK_MAX_ARPEGGIO + 1] = { 0 };
	const int count = MIN(p_arpeggio.size(), BK_MAX_ARPEGGIO);

	value[0] = count;
	for (int i = 0; i < count; i++) {
		value[i + 1] = BKInt(CLAMP(p_arpeggio[i], -real_t(BK_MAX_NOTE), +real_t(BK_MAX_NOTE)) * double(BK_FINT20_UNIT));
	}

	AudioStreamBlipKit::lock();
	BKSetPtr(&track, BK_ARPEGGIO, value, (count + 1) * sizeof(BKInt));
	AudioStreamBlipKit::unlock();
}

int BlipKitTrack::get_arpeggio_divider() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_ARPEGGIO_DIVIDER, &value);
	AudioStreamBlipKit::unlock();

	return value;
}

void BlipKitTrack::set_arpeggio_divider(int p_arpeggio_divider) {
	p_arpeggio_divider = MAX(0, p_arpeggio_divider);

	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_ARPEGGIO_DIVIDER, p_arpeggio_divider);
	AudioStreamBlipKit::unlock();
}

Ref<BlipKitInstrument> BlipKitTrack::get_instrument() {
	return instrument;
}

void BlipKitTrack::set_instrument(Ref<BlipKitInstrument> p_instrument) {
	AudioStreamBlipKit::lock();

	instrument = p_instrument;

	if (instrument.is_valid()) {
		BKSetPtr(&track, BK_INSTRUMENT, instrument->get_instrument(), 0);
	} else {
		BKSetPtr(&track, BK_INSTRUMENT, nullptr, 0);
	}

	AudioStreamBlipKit::unlock();
}

int BlipKitTrack::get_instrument_divider() const {
	BKInt value = 0;

	AudioStreamBlipKit::lock();
	BKGetAttr(&track, BK_INSTRUMENT_DIVIDER, &value);
	AudioStreamBlipKit::unlock();

	return value;
}

void BlipKitTrack::set_instrument_divider(int p_instrument_divider) {
	p_instrument_divider = MAX(0, p_instrument_divider);

	AudioStreamBlipKit::lock();
	BKSetAttr(&track, BK_INSTRUMENT_DIVIDER, p_instrument_divider);
	AudioStreamBlipKit::unlock();
}

Ref<BlipKitWaveform> BlipKitTrack::get_custom_waveform() {
	return custom_waveform;
}

void BlipKitTrack::set_custom_waveform(Ref<BlipKitWaveform> p_waveform) {
	if (p_waveform.is_null()) {
		set_waveform(WAVEFORM_SQUARE);
		return;
	}

	ERR_FAIL_COND(!p_waveform->is_valid());

	BKData *data = p_waveform->get_waveform();

	AudioStreamBlipKit::lock();
	custom_waveform = p_waveform;
	BKInt result = BKSetPtr(&track, BK_WAVEFORM, data, 0);
	AudioStreamBlipKit::unlock();

	if (result == BK_INVALID_STATE) {
		// OK. Track is not attached yet.
	} else {
		ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to set custom waveform: %s.", BKStatusGetName(result)));
	}
}

void BlipKitTrack::attach(AudioStreamBlipKitPlayback *p_playback) {
	ERR_FAIL_NULL(p_playback);

	RecursiveSpinLock::Autolock lock = AudioStreamBlipKit::autolock();
	BKContext *context = p_playback->get_context();

	ERR_FAIL_COND(playback != nullptr);

	BKTrackAttach(&track, context);
	playback = p_playback;
	playback->attach(this);

	// Custom waveform needs to be set again after attaching.
	if (custom_waveform.is_valid()) {
		set_custom_waveform(custom_waveform);
	}

	for (DividerItem &divider : dividers) {
		divider.divider.attach(p_playback);
	}

	lock.unlock();
}

void BlipKitTrack::detach() {
	RecursiveSpinLock::Autolock lock = AudioStreamBlipKit::autolock();

	if (!playback) {
		return;
	}

	for (DividerItem &divider : dividers) {
		divider.divider.detach();
	}

	BKTrackDetach(&track);
	playback->detach(this);
	playback = nullptr;

	lock.unlock();
}

void BlipKitTrack::release() {
	set_note(real_t(NOTE_RELEASE));
}

void BlipKitTrack::mute() {
	set_note(real_t(NOTE_MUTE));
}

void BlipKitTrack::reset() {
	const real_t master_volume = get_master_volume();
	const Waveform waveform = get_waveform();

	AudioStreamBlipKit::lock();

	BKTrackReset(&track);
	instrument.unref();

	AudioStreamBlipKit::unlock();

	set_waveform(waveform);
	set_master_volume(master_volume);
	if (custom_waveform.is_valid()) {
		set_custom_waveform(custom_waveform);
	}
}

BlipKitTrack::DividerItem *BlipKitTrack::find_divider(const StringName &p_name) {
	const int count = dividers.size();

	for (int i = 0; i < count; i++) {
		if (dividers[i].name == p_name) {
			return &dividers[i];
		}
	}

	return nullptr;
}

bool BlipKitTrack::has_divider(const StringName &p_name) {
	return find_divider(p_name) != nullptr;
}

PackedStringArray BlipKitTrack::get_divider_names() const {
	const int count = dividers.size();
	PackedStringArray names;
	names.resize(count);

	for (int i = 0; i < count; i++) {
		names[i] = dividers[i].name;
	}

	return names;
}

void BlipKitTrack::add_divider(const StringName &p_name, int p_tick_interval, Callable p_callable) {
	ERR_FAIL_COND(has_divider(p_name));

	AudioStreamBlipKit::lock();

	const int count = dividers.size();
	dividers.resize(count + 1);
	DividerItem &divider = dividers[count];

	divider.name = p_name;
	divider.divider.initialize(p_tick_interval, p_callable);

	if (playback) {
		divider.divider.attach(playback);
	}

	AudioStreamBlipKit::unlock();
}

void BlipKitTrack::remove_divider(const StringName &p_name) {
	AudioStreamBlipKit::lock();

	const int count = dividers.size();
	for (int i = 0; i < count; i++) {
		if (dividers[i].name == p_name) {
			dividers.remove_at(i);
			break;
		}
	}

	AudioStreamBlipKit::unlock();

	ERR_FAIL_MSG(vformat("Divider '%s' is not defined.", p_name));
}

void BlipKitTrack::reset_divider(const StringName &p_name, int p_tick_interval) {
	DividerItem *divider = find_divider(p_name);

	ERR_FAIL_NULL_MSG(divider, vformat("Divider '%s' is not defined.", p_name));

	divider->divider.reset(p_tick_interval);
}

void BlipKitTrack::clear_dividers() {
	AudioStreamBlipKit::lock();
	dividers.clear();
	AudioStreamBlipKit::unlock();
}
