#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/core/math.hpp>

#include "blipkit_track.hpp"

using namespace detomon::BlipKit;
using namespace godot;

BlipKitTrack::BlipKitTrack() {
	BKInt result = BKTrackInit(&track, BK_SQUARE);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to initialize BKTrack.");

	// Set default master volume.
	BKSetAttr(&track, BK_MASTER_VOLUME, (BKInt)(0.1 * (real_t)BK_MAX_VOLUME));
	// Allow setting volume of triangle wave.
	BKSetAttr(&track, BK_TRIANGLE_IGNORES_VOLUME, 0);
}

BlipKitTrack::~BlipKitTrack() {
	AudioServer::get_singleton()->lock();
	BKDispose(&track);
	AudioServer::get_singleton()->unlock();
}

real_t BlipKitTrack::get_master_volume() const {
	BKInt value = 0;
	AudioServer::get_singleton()->lock();
	BKGetAttr(&track, BK_MASTER_VOLUME, &value);
	AudioServer::get_singleton()->unlock();

	return (real_t)value / (real_t)BK_MAX_VOLUME;
}

void BlipKitTrack::set_master_volume(real_t p_master_volume) {
	p_master_volume = CLAMP(p_master_volume, 0.0, 1.0);
	BKInt value = (BKInt)(p_master_volume * (real_t)BK_MAX_VOLUME);
	AudioServer::get_singleton()->lock();
	BKInt result = BKSetAttr(&track, BK_MASTER_VOLUME, value);
	AudioServer::get_singleton()->unlock();

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set master volume.");
}

real_t BlipKitTrack::get_volume() const {
	BKInt value = 0;
	AudioServer::get_singleton()->lock();
	BKGetAttr(&track, BK_VOLUME, &value);
	AudioServer::get_singleton()->unlock();

	return (real_t)value / (real_t)BK_MAX_VOLUME;
}

void BlipKitTrack::set_volume(real_t p_volume) {
	p_volume = CLAMP(p_volume, 0.0, 1.0);
	BKInt value = (BKInt)(p_volume * (real_t)BK_MAX_VOLUME);
	AudioServer::get_singleton()->lock();
	BKInt result = BKSetAttr(&track, BK_VOLUME, value);
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set volume.");
}

real_t BlipKitTrack::get_panning() const {
	BKInt value = 0;
	AudioServer::get_singleton()->lock();
	BKGetAttr(&track, BK_PANNING, &value);
	AudioServer::get_singleton()->unlock();

	return (real_t)value / (real_t)BK_MAX_VOLUME;
}

void BlipKitTrack::set_panning(real_t p_panning) {
	p_panning = CLAMP(p_panning, -1.0, +1.0);
	BKInt value = (BKInt)(p_panning * (real_t)BK_MAX_VOLUME);
	AudioServer::get_singleton()->lock();
	BKInt result = BKSetAttr(&track, BK_PANNING, value);
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set panning.");
}

BlipKitTrack::Waveform BlipKitTrack::get_waveform() const {
	BKInt value = 0;
	AudioServer::get_singleton()->lock();
	BKGetAttr(&track, BK_WAVEFORM, &value);
	AudioServer::get_singleton()->unlock();

	return static_cast<BlipKitTrack::Waveform>(value);
}

void BlipKitTrack::set_waveform(BlipKitTrack::Waveform p_waveform) {
	AudioServer::get_singleton()->lock();
	BKInt result = BKSetAttr(&track, BK_WAVEFORM, p_waveform);
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set waveform.");
}

int BlipKitTrack::get_duty_cycle() const {
	BKInt value = 0;
	AudioServer::get_singleton()->lock();
	BKGetAttr(&track, BK_DUTY_CYCLE, &value);
	AudioServer::get_singleton()->unlock();

	return value;
}

void BlipKitTrack::set_duty_cycle(int p_duty_cycle) {
	AudioServer::get_singleton()->lock();
	BKInt result = BKSetAttr(&track, BK_DUTY_CYCLE, p_duty_cycle);
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set duty cycle.");
}

real_t BlipKitTrack::get_note() const {
	BKInt value = 0;
	AudioServer::get_singleton()->lock();
	BKGetAttr(&track, BK_NOTE, &value);
	AudioServer::get_singleton()->unlock();

	// No note set.
	if (value < 0) {
		return (real_t)NOTE_RELEASE;
	}

	return (real_t)value / (real_t)BK_FINT20_UNIT;
}

void BlipKitTrack::set_note(real_t p_note) {
	BKInt value = 0;

	if (p_note >= 0.0) {
		p_note = CLAMP(p_note, (real_t)BK_MIN_NOTE, (real_t)BK_MAX_NOTE);
		value = p_note * (real_t)BK_FINT20_UNIT;
	} else {
		// NOTE_RELEASE or NOTE_MUTE
		value = (BKInt)Math::round(p_note);
	}

	AudioServer::get_singleton()->lock();
	BKInt result = BKSetAttr(&track, BK_NOTE, value);
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set note.");
}

real_t BlipKitTrack::get_pitch() const {
	BKInt value = 0;
	AudioServer::get_singleton()->lock();
	BKGetAttr(&track, BK_PITCH, &value);
	AudioServer::get_singleton()->unlock();

	return (real_t)value / (real_t)BK_FINT20_UNIT;
}

void BlipKitTrack::set_pitch(real_t p_pitch) {
	p_pitch = CLAMP(p_pitch, -(real_t)BK_MAX_NOTE, +(real_t)BK_MAX_NOTE);
	BKInt value = (BKInt)(p_pitch * (real_t)BK_FINT20_UNIT);
	AudioServer::get_singleton()->lock();
	BKInt result = BKSetAttr(&track, BK_PITCH, value);
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set pitch.");
}

int BlipKitTrack::get_volume_slide() const {
	BKInt value[1] = { 0 };
	AudioServer::get_singleton()->lock();
	BKGetPtr(&track, BK_EFFECT_VOLUME_SLIDE, value, sizeof(value));
	AudioServer::get_singleton()->unlock();

	return value[0];
}

void BlipKitTrack::set_volume_slide(int p_volume_slide) {
	BKInt value[1] = { p_volume_slide };
	AudioServer::get_singleton()->lock();
	BKInt result = BKSetPtr(&track, BK_EFFECT_VOLUME_SLIDE, value, sizeof(value));
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set volume slide.");
}

int BlipKitTrack::get_panning_slide() const {
	BKInt value[1] = { 0 };
	AudioServer::get_singleton()->lock();
	BKGetPtr(&track, BK_EFFECT_VOLUME_SLIDE, value, sizeof(value));
	AudioServer::get_singleton()->unlock();

	return value[0];
}

void BlipKitTrack::set_panning_slide(int p_panning_slide) {
	BKInt value[1] = { p_panning_slide };
	AudioServer::get_singleton()->lock();
	BKInt result = BKSetPtr(&track, BK_EFFECT_PANNING_SLIDE, value, sizeof(value));
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set panning slide.");
}

int BlipKitTrack::get_portamento() const {
	BKInt value[1] = { 0 };
	AudioServer::get_singleton()->lock();
	BKGetPtr(&track, BK_EFFECT_PORTAMENTO, value, sizeof(value));
	AudioServer::get_singleton()->unlock();

	return value[0];
}

void BlipKitTrack::set_portamento(int p_portamento) {
	BKInt value[1] = { p_portamento };
	AudioServer::get_singleton()->lock();
	BKInt result = BKSetPtr(&track, BK_EFFECT_PORTAMENTO, value, sizeof(value));
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set portamento.");
}

void BlipKitTrack::set_tremolo(int p_ticks, float p_delta) {
	p_delta = MAX(p_delta, 0);
	BKInt delta = (BKInt)(p_delta * (real_t)BK_MAX_VOLUME);
	BKInt values[2] = { p_ticks, delta };

	AudioServer::get_singleton()->lock();
	BKInt result = BKSetPtr(&track, BK_EFFECT_TREMOLO, values, sizeof(values));
	AudioServer::get_singleton()->unlock();
}

void BlipKitTrack::set_vibrato(int p_ticks, float p_delta) {
	p_delta = CLAMP(p_delta, -(real_t)BK_MAX_NOTE, +(real_t)BK_MAX_NOTE);
	BKInt delta = (BKInt)(p_delta * (real_t)BK_FINT20_UNIT);
	BKInt values[2] = { p_ticks, delta };

	AudioServer::get_singleton()->lock();
	BKInt result = BKSetPtr(&track, BK_EFFECT_VIBRATO, values, sizeof(values));
	AudioServer::get_singleton()->unlock();
}

PackedFloat32Array BlipKitTrack::get_arpeggio() const {
	BKInt value[BK_MAX_ARPEGGIO + 1] = { 0 };

	AudioServer::get_singleton()->lock();
	BKGetPtr(&track, BK_ARPEGGIO, value, (BK_MAX_ARPEGGIO + 1) * sizeof(BKInt));
	AudioServer::get_singleton()->unlock();

	BKInt count = value[0];
	PackedFloat32Array arpeggio;
	arpeggio.resize(count);

	for (int i = 0; i < count; i++) {
		arpeggio[i] = (real_t)value[i + 1] / (real_t)BK_FINT20_UNIT;
	}

	return arpeggio;
}

int BlipKitTrack::get_arpeggio_divider() const {
	BKInt value = 0;
	AudioServer::get_singleton()->lock();
	BKGetAttr(&track, BK_ARPEGGIO_DIVIDER, &value);
	AudioServer::get_singleton()->unlock();

	return value;
}

void BlipKitTrack::set_arpeggio_divider(int p_arpeggio_divider) {
	p_arpeggio_divider = MAX(0, p_arpeggio_divider);
	AudioServer::get_singleton()->lock();
	BKInt result = BKSetAttr(&track, BK_ARPEGGIO_DIVIDER, p_arpeggio_divider);
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set arpeggio divider.");
}

void BlipKitTrack::set_arpeggio(const PackedFloat32Array &p_arpeggio) {
	BKInt value[BK_MAX_ARPEGGIO + 1] = { 0 };
	int count = MIN(BK_MAX_ARPEGGIO, p_arpeggio.size());

	value[0] = count;
	for (int i = 0; i < count; i++) {
		value[i + 1] = (BKInt)(CLAMP(p_arpeggio[i], -(real_t)BK_MAX_NOTE, +(real_t)BK_MAX_NOTE) * (real_t)BK_FINT20_UNIT);
	}

	AudioServer::get_singleton()->lock();
	BKInt result = BKSetPtr(&track, BK_ARPEGGIO, value, (count + 1) * sizeof(BKInt));
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set arpeggio.");
}

Ref<BlipKitInstrument> BlipKitTrack::get_instrument() {
	return instrument;
}

void BlipKitTrack::set_instrument(Ref<BlipKitInstrument> p_instrument) {
	BKInstrument *bk_instrument = p_instrument->get_instrument();
	instrument = p_instrument;

	AudioServer::get_singleton()->lock();
	BKInt result = BKSetPtr(&track, BK_INSTRUMENT, bk_instrument, sizeof(instrument));
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to set instrument.");
}

void BlipKitTrack::attach(AudioStreamBlipKitPlayback *p_playback) {
	ERR_FAIL_COND(!p_playback);

	BKContext* context = p_playback->get_context();
	AudioServer::get_singleton()->lock();
	BKInt result = BKTrackAttach(&track, context);
	AudioServer::get_singleton()->unlock();
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Failed to attach BKTrack.");
}

void BlipKitTrack::detach() {
	AudioServer::get_singleton()->lock();
	BKTrackDetach(&track);
	AudioServer::get_singleton()->unlock();
}

void BlipKitTrack::release() {
	set_note((real_t)NOTE_RELEASE);
}

void BlipKitTrack::mute() {
	set_note((real_t)NOTE_MUTE);
}

void BlipKitTrack::_bind_methods() {
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

	ClassDB::bind_method(D_METHOD("set_volume_slide"), &BlipKitTrack::set_volume_slide);
	ClassDB::bind_method(D_METHOD("get_volume_slide"), &BlipKitTrack::get_volume_slide);
	ClassDB::bind_method(D_METHOD("set_panning_slide"), &BlipKitTrack::set_panning_slide);
	ClassDB::bind_method(D_METHOD("get_panning_slide"), &BlipKitTrack::get_panning_slide);
	ClassDB::bind_method(D_METHOD("set_portamento"), &BlipKitTrack::set_portamento);
	ClassDB::bind_method(D_METHOD("get_portamento"), &BlipKitTrack::get_portamento);
	ClassDB::bind_method(D_METHOD("set_tremolo", "ticks", "delta"), &BlipKitTrack::set_tremolo);
	ClassDB::bind_method(D_METHOD("set_vibrato", "ticks", "delta"), &BlipKitTrack::set_vibrato);

	ClassDB::bind_method(D_METHOD("set_arpeggio"), &BlipKitTrack::set_arpeggio);
	ClassDB::bind_method(D_METHOD("get_arpeggio"), &BlipKitTrack::get_arpeggio);
	ClassDB::bind_method(D_METHOD("set_arpeggio_divider"), &BlipKitTrack::set_arpeggio_divider);
	ClassDB::bind_method(D_METHOD("get_arpeggio_divider"), &BlipKitTrack::get_arpeggio_divider);

	ClassDB::bind_method(D_METHOD("set_instrument"), &BlipKitTrack::set_instrument);
	ClassDB::bind_method(D_METHOD("get_instrument"), &BlipKitTrack::get_instrument);

	ClassDB::bind_method(D_METHOD("attach", "context"), &BlipKitTrack::attach);
	ClassDB::bind_method(D_METHOD("detach"), &BlipKitTrack::detach);
	ClassDB::bind_method(D_METHOD("release"), &BlipKitTrack::release);
	ClassDB::bind_method(D_METHOD("mute"), &BlipKitTrack::mute);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "waveform"), "set_waveform", "get_waveform");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "duty_cycle"), "set_duty_cycle", "get_duty_cycle");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "master_volume"), "set_master_volume", "get_master_volume");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "volume"), "set_volume", "get_volume");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "panning"), "set_panning", "get_panning");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "note"), "set_note", "get_note");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "pitch"), "set_pitch", "get_pitch");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "volume_slide"), "set_volume_slide", "get_volume_slide");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "panning_slide"), "set_panning_slide", "get_panning_slide");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "portamento"), "set_portamento", "get_portamento");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "arpeggio"), "set_arpeggio", "get_arpeggio");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "arpeggio_divider"), "set_arpeggio_divider", "get_arpeggio_divider");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "instrument"), "set_instrument", "get_instrument");

	BIND_ENUM_CONSTANT(WAVEFORM_SQUARE);
	BIND_ENUM_CONSTANT(WAVEFORM_TRIANGLE);
	BIND_ENUM_CONSTANT(WAVEFORM_NOISE);
	BIND_ENUM_CONSTANT(WAVEFORM_SAWTOOTH);
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
}
