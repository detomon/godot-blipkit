#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/templates/vector.hpp>

#include "blipkit_instrument.hpp"

using namespace detomon::BlipKit;
using namespace godot;

void BlipKitInstrument::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_sequence_pitch", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_sequence_pitch);
	ClassDB::bind_method(D_METHOD("set_sequence_volume", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_sequence_volume);
	ClassDB::bind_method(D_METHOD("set_sequence_panning", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_sequence_panning);
	ClassDB::bind_method(D_METHOD("set_sequence_duty_cycle", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_sequence_duty_cycle);
	ClassDB::bind_method(D_METHOD("set_envelope_pitch", "steps", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_envelope_pitch);
	ClassDB::bind_method(D_METHOD("set_envelope_volume", "steps", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_envelope_volume);
	ClassDB::bind_method(D_METHOD("set_envelope_panning", "steps", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_envelope_panning);
	ClassDB::bind_method(D_METHOD("set_envelope_duty_cycle", "steps", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_envelope_duty_cycle);
	ClassDB::bind_method(D_METHOD("set_envelope_adsr", "attack", "decay", "sustain", "release"), &BlipKitInstrument::set_envelope_adsr);
}

String BlipKitInstrument::_to_string() const {
	return "BlipKitInstrument";
}

BlipKitInstrument::BlipKitInstrument() {
	BKInt result = BKInstrumentInit(&instrument);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKInstrument: %s.", BKStatusGetName(result)));
}

BlipKitInstrument::~BlipKitInstrument() {
	AudioServer::get_singleton()->lock();
	BKDispose(&instrument);
	AudioServer::get_singleton()->unlock();
}

void BlipKitInstrument::set_sequence_float(Sequence p_sequence, const PackedFloat32Array &p_values, int p_sustain_offset, int p_sustain_length, real_t p_multiplier) {
	p_sustain_offset = CLAMP(0, p_sustain_offset, p_values.size());
	p_sustain_length = CLAMP(0, p_sustain_length, p_values.size());

	switch (p_sequence) {
		case SEQUENCE_PITCH:
		case SEQUENCE_VOLUME:
		case SEQUENCE_PANNING: {
			// ok
		} break;
		default: {
			return;
		} break;
	}

	Vector<BKInt> values;
	values.resize(p_values.size());
	BKInt *ptrw = values.ptrw();

	for (int i = 0; i < p_values.size(); i++) {
		ptrw[i] = (BKInt)(p_values[i] * p_multiplier);
	}

	AudioServer::get_singleton()->lock();
	BKInstrumentSetSequence(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);
	AudioServer::get_singleton()->unlock();
}

void BlipKitInstrument::set_sequence_int(Sequence p_sequence, const PackedInt32Array &p_values, int p_sustain_offset, int p_sustain_length) {
	p_sustain_offset = CLAMP(0, p_sustain_offset, p_values.size());
	p_sustain_length = CLAMP(0, p_sustain_length, p_values.size());

	switch (p_sequence) {
		case SEQUENCE_DUTY_CYCLE: {
			// ok
		} break;
		default: {
			return;
		} break;
	}

	Vector<BKInt> values;
	values.resize(p_values.size());
	BKInt *ptrw = values.ptrw();

	for (int i = 0; i < p_values.size(); i++) {
		ptrw[i] = p_values[i];
	}

	AudioServer::get_singleton()->lock();
	BKInstrumentSetSequence(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);
	AudioServer::get_singleton()->unlock();
}

void BlipKitInstrument::set_sequence_pitch(PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_sequence_float(SEQUENCE_PITCH, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_FINT20_UNIT);
}

void BlipKitInstrument::set_sequence_volume(PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_sequence_float(SEQUENCE_VOLUME, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_MAX_VOLUME);
}

void BlipKitInstrument::set_sequence_panning(PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_sequence_float(SEQUENCE_PANNING, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_MAX_VOLUME);
}

void BlipKitInstrument::set_sequence_duty_cycle(PackedInt32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_sequence_int(SEQUENCE_PITCH, p_values, p_sustain_offset, p_sustain_length);
}


void BlipKitInstrument::set_envelope_float(Sequence p_sequence, const PackedInt32Array &p_steps, const PackedFloat32Array &p_values, int p_sustain_offset, int p_sustain_length, real_t p_multiplier) {
	ERR_FAIL_COND(p_steps.size() != p_values.size());

	p_sustain_offset = CLAMP(0, p_sustain_offset, p_values.size());
	p_sustain_length = CLAMP(0, p_sustain_length, p_values.size());

	switch (p_sequence) {
		case SEQUENCE_PITCH:
		case SEQUENCE_VOLUME:
		case SEQUENCE_PANNING: {
			// ok
		} break;
		default: {
			return;
		} break;
	}

	Vector<BKSequencePhase> values;
	values.resize(p_values.size());
	BKSequencePhase *ptrw = values.ptrw();

	for (int i = 0; i < p_values.size(); i++) {
		ptrw[i].steps = p_steps[i];
		ptrw[i].value = (BKInt)(p_values[i] * p_multiplier);
	}

	AudioServer::get_singleton()->lock();
	BKInt result = BKInstrumentSetEnvelope(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);
	AudioServer::get_singleton()->unlock();

	ERR_FAIL_COND_MSG(result != BK_SUCCESS, "Sustain loop has zero steps.");
}

void BlipKitInstrument::set_envelope_int(Sequence p_sequence, const PackedInt32Array &p_steps, const PackedInt32Array &p_values, int p_sustain_offset, int p_sustain_length) {
	ERR_FAIL_COND(p_steps.size() != p_values.size());

	p_sustain_offset = CLAMP(0, p_sustain_offset, p_values.size());
	p_sustain_length = CLAMP(0, p_sustain_length, p_values.size());

	switch (p_sequence) {
		case SEQUENCE_DUTY_CYCLE: {
			// ok
		} break;
		default: {
			return;
		} break;
	}

	Vector<BKSequencePhase> values;
	values.resize(p_values.size());
	BKSequencePhase *ptrw = values.ptrw();

	for (int i = 0; i < p_values.size(); i++) {
		ptrw[i].steps = p_steps[i];
		ptrw[i].value = p_values[i];
	}

	AudioServer::get_singleton()->lock();
	BKInt result = BKInstrumentSetEnvelope(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);
	AudioServer::get_singleton()->unlock();

	ERR_FAIL_COND(result != BK_SUCCESS);
}

void BlipKitInstrument::set_envelope_pitch(PackedInt32Array p_steps, PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_envelope_float(SEQUENCE_PITCH, p_steps, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_FINT20_UNIT);
}

void BlipKitInstrument::set_envelope_volume(PackedInt32Array p_steps, PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_envelope_float(SEQUENCE_VOLUME, p_steps, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_MAX_VOLUME);
}

void BlipKitInstrument::set_envelope_panning(PackedInt32Array p_steps, PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_envelope_float(SEQUENCE_PANNING, p_steps, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_MAX_VOLUME);
}

void BlipKitInstrument::set_envelope_duty_cycle(PackedInt32Array p_steps, PackedInt32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_envelope_int(SEQUENCE_DUTY_CYCLE, p_steps, p_values, p_sustain_offset, p_sustain_length);
}

void BlipKitInstrument::set_envelope_adsr(int p_attack, int p_decay, real_t p_sustain, int p_release) {
	BKInt sustain = CLAMP(p_sustain, 0.0, 1.0) * BK_MAX_VOLUME;

	AudioServer::get_singleton()->lock();
	BKInstrumentSetEnvelopeADSR(&instrument, p_attack, p_decay, sustain, p_release);
	AudioServer::get_singleton()->unlock();
}
