#include "blipkit_instrument.hpp"
#include "audio_stream_blipkit.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/array.hpp>

using namespace BlipKit;
using namespace godot;

void BlipKitInstrument::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_envelope", "type", "steps", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_envelope);
	ClassDB::bind_method(D_METHOD("set_adsr", "attack", "decay", "sustain", "release"), &BlipKitInstrument::set_adsr);
	ClassDB::bind_method(D_METHOD("has_envelope", "type"), &BlipKitInstrument::has_envelope);
	ClassDB::bind_method(D_METHOD("get_envelope_steps", "type"), &BlipKitInstrument::get_envelope_steps);
	ClassDB::bind_method(D_METHOD("get_envelope_values", "type"), &BlipKitInstrument::get_envelope_values);
	ClassDB::bind_method(D_METHOD("get_envelope_sustain_offset", "type"), &BlipKitInstrument::get_envelope_sustain_offset);
	ClassDB::bind_method(D_METHOD("get_envelope_sustain_length", "type"), &BlipKitInstrument::get_envelope_sustain_length);
	ClassDB::bind_method(D_METHOD("clear_envelope", "type"), &BlipKitInstrument::clear_envelope);

	BIND_ENUM_CONSTANT(ENVELOPE_VOLUME);
	BIND_ENUM_CONSTANT(ENVELOPE_PANNING);
	BIND_ENUM_CONSTANT(ENVELOPE_PITCH);
	BIND_ENUM_CONSTANT(ENVELOPE_DUTY_CYCLE);
}

String BlipKitInstrument::_to_string() const {
	return vformat("BlipKitInstrument: volume=%s, panning=%s, pitch=%s, duty_cycle=%s",
			get("envelope/volume"),
			get("envelope/panning"),
			get("envelope/pitch"),
			get("envelope/duty_cycle"));
}

void BlipKitInstrument::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::ARRAY, "envelope/volume", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "envelope/panning", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "envelope/pitch", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "envelope/duty_cycle", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
}

bool BlipKitInstrument::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name.begins_with("envelope/")) {
		const String name = p_name;
		EnvelopeType type;

		if (name == "envelope/volume") {
			type = ENVELOPE_VOLUME;
		} else if (name == "envelope/panning") {
			type = ENVELOPE_PANNING;
		} else if (name == "envelope/pitch") {
			type = ENVELOPE_PITCH;
		} else if (name == "envelope/duty_cycle") {
			type = ENVELOPE_DUTY_CYCLE;
		} else {
			return false;
		}

		Array data = p_value;

		if (data.size() == 4) {
			PackedInt32Array steps = data[0];
			PackedFloat32Array values = data[1];
			int sustain_offset = data[2];
			int sustain_length = data[3];

			set_envelope(type, steps, values, sustain_offset, sustain_length);
		}

		return true;
	}

	return false;
}

bool BlipKitInstrument::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name.begins_with("envelope/")) {
		const String name = p_name;
		EnvelopeType type;

		if (name == "envelope/volume") {
			type = ENVELOPE_VOLUME;
		} else if (name == "envelope/panning") {
			type = ENVELOPE_PANNING;
		} else if (name == "envelope/pitch") {
			type = ENVELOPE_PITCH;
		} else if (name == "envelope/duty_cycle") {
			type = ENVELOPE_DUTY_CYCLE;
		} else {
			return false;
		}

		Array data;
		const Sequence &seq = sequences[type];

		if (!seq.values.is_empty()) {
			data.append(seq.steps);
			data.append(seq.values);
			data.append(seq.sustain_offset);
			data.append(seq.sustain_length);
		}

		r_ret = data;

		return true;
	}

	return false;
}

BlipKitInstrument::BlipKitInstrument() {
	const BKInt result = BKInstrumentInit(&instrument);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKInstrument: %s.", BKStatusGetName(result)));
}

BlipKitInstrument::~BlipKitInstrument() {
	AudioStreamBlipKit::lock();
	BKDispose(&instrument);
	AudioStreamBlipKit::unlock();
}

void BlipKitInstrument::set_envelope(EnvelopeType p_type, const PackedInt32Array &p_steps, const PackedFloat32Array &p_values, int p_sustain_offset, int p_sustain_length) {
	ERR_FAIL_INDEX(p_type, ENVELOPE_MAX);

	float multiplier = 1.0;

	switch (p_type) {
		case ENVELOPE_VOLUME: {
			multiplier = float(BK_MAX_VOLUME);
		} break;
		case ENVELOPE_PANNING: {
			multiplier = float(BK_MAX_VOLUME);
		} break;
		case ENVELOPE_PITCH: {
			multiplier = float(BK_FINT20_UNIT);
		} break;
		case ENVELOPE_DUTY_CYCLE: {
			multiplier = 1.0;
		} break;
		default: {
			ERR_FAIL_MSG(vformat("Invalid instrument sequence: %d.", p_type));
		} break;
	}

	const bool is_envelope = p_steps.size() > 0;

	p_sustain_offset = CLAMP(p_sustain_offset, 0, p_values.size());
	p_sustain_length = CLAMP(p_sustain_length, 0, p_values.size() - p_sustain_offset);

	PackedInt32Array steps_copy;
	steps_copy.resize(p_steps.size());
	for (int i = 0; i < p_steps.size(); i++) {
		steps_copy[i] = p_steps[i];
	}

	PackedFloat32Array values_copy;
	values_copy.resize(p_values.size());
	for (int i = 0; i < p_values.size(); i++) {
		values_copy[i] = p_values[i];
	}

	RecursiveSpinLock::Autolock lock = AudioStreamBlipKit::autolock();
	BKInt result = 0;

	if (is_envelope) {
		LocalVector<BKSequencePhase> phases;
		phases.resize(p_values.size());

		for (int i = 0; i < p_values.size(); i++) {
			phases[i].steps = BKUInt(is_envelope ? p_steps[i] : 0);
			phases[i].value = BKInt(p_values[i] * multiplier);
		}

		result = BKInstrumentSetEnvelope(&instrument, p_type, phases.ptr(), phases.size(), p_sustain_offset, p_sustain_length);
	} else {
		LocalVector<BKInt> phases;
		phases.resize(p_values.size());

		for (int i = 0; i < p_values.size(); i++) {
			phases[i] = BKInt(p_values[i] * multiplier);
		}

		result = BKInstrumentSetSequence(&instrument, p_type, phases.ptr(), phases.size(), p_sustain_offset, p_sustain_length);
	}

	if (result == BK_INVALID_ATTRIBUTE) {
		ERR_FAIL_MSG("Failed to set envelope: Sustain cycle has zero steps.");
	} else if (result != BK_SUCCESS) {
		ERR_FAIL_MSG(vformat("Failed to set envelope: %s.", BKStatusGetName(result)));
	}

	Sequence &seq = sequences[p_type];
	seq.steps = steps_copy;
	seq.values = values_copy;
	seq.sustain_offset = p_sustain_offset;
	seq.sustain_length = p_sustain_length;

	lock.unlock();

	emit_changed();
}

void BlipKitInstrument::set_adsr(int p_attack, int p_decay, float p_sustain, int p_release) {
	BKInt sustain = BKInt(CLAMP(p_sustain, 0.0, 1.0) * float(BK_MAX_VOLUME));
	set_envelope(ENVELOPE_VOLUME, { p_attack, p_decay, 240, p_release }, { 1.0, p_sustain, p_sustain, 0.0 }, 2, 1);
}

bool BlipKitInstrument::has_envelope(EnvelopeType p_type) const {
	ERR_FAIL_INDEX_V(p_type, ENVELOPE_MAX, false);
	return !sequences[p_type].values.is_empty();
}

PackedInt32Array BlipKitInstrument::get_envelope_steps(EnvelopeType p_type) const {
	ERR_FAIL_INDEX_V(p_type, ENVELOPE_MAX, PackedInt32Array());

	Sequence &sequence = const_cast<BlipKitInstrument *>(this)->sequences[p_type];

	AudioStreamBlipKit::lock();
	PackedInt32Array steps = sequence.steps.duplicate();
	AudioStreamBlipKit::unlock();

	return steps;
}

PackedFloat32Array BlipKitInstrument::get_envelope_values(EnvelopeType p_type) const {
	ERR_FAIL_INDEX_V(p_type, ENVELOPE_MAX, Variant());

	Sequence &sequence = const_cast<BlipKitInstrument *>(this)->sequences[p_type];

	AudioStreamBlipKit::lock();
	PackedFloat32Array values = sequence.values.duplicate();
	AudioStreamBlipKit::unlock();

	return values;
}

int BlipKitInstrument::get_envelope_sustain_offset(EnvelopeType p_type) const {
	ERR_FAIL_INDEX_V(p_type, ENVELOPE_MAX, 0);
	return sequences[p_type].sustain_offset;
}

int BlipKitInstrument::get_envelope_sustain_length(EnvelopeType p_type) const {
	ERR_FAIL_INDEX_V(p_type, ENVELOPE_MAX, 0);
	return sequences[p_type].sustain_length;
}

void BlipKitInstrument::clear_envelope(EnvelopeType p_type) {
	set_envelope(p_type, PackedInt32Array(), PackedFloat32Array(), 0, 0);
}
