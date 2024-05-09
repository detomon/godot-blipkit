#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/array.hpp>

#include "audio_stream_blipkit.hpp"
#include "blipkit_instrument.hpp"

using namespace detomon::BlipKit;
using namespace godot;

void BlipKitInstrument::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_sequence_volume", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_sequence_volume);
	ClassDB::bind_method(D_METHOD("set_sequence_panning", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_sequence_panning);
	ClassDB::bind_method(D_METHOD("set_sequence_pitch", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_sequence_pitch);
	ClassDB::bind_method(D_METHOD("set_sequence_duty_cycle", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_sequence_duty_cycle);
	ClassDB::bind_method(D_METHOD("set_envelope_volume", "steps", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_envelope_volume);
	ClassDB::bind_method(D_METHOD("set_envelope_panning", "steps", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_envelope_panning);
	ClassDB::bind_method(D_METHOD("set_envelope_pitch", "steps", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_envelope_pitch);
	ClassDB::bind_method(D_METHOD("set_envelope_duty_cycle", "steps", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_envelope_duty_cycle);
	ClassDB::bind_method(D_METHOD("set_envelope_adsr", "attack", "decay", "sustain", "release"), &BlipKitInstrument::set_envelope_adsr);
}

String BlipKitInstrument::_to_string() const {
	return "BlipKitInstrument";
}

void BlipKitInstrument::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::ARRAY, "sequence/volume", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "sequence/panning", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "sequence/pitch", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "sequence/duty_cycle", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
}

bool BlipKitInstrument::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name.begins_with("sequence/")) {
		String name = p_name;
		Array data = p_value;
		PackedInt32Array steps;
		Variant values;
		int sustain_offset = 0;
		int sustain_length = 0;
		bool is_envelope = false;
		SequenceType type;

		if (name == "sequence/volume") {
			type = SEQUENCE_VOLUME;
		} else if (name == "sequence/panning") {
			type = SEQUENCE_PANNING;
		} else if (name == "sequence/pitch") {
			type = SEQUENCE_PITCH;
		} else if (name == "sequence/duty_cycle") {
			type = SEQUENCE_DUTY_CYCLE;
		} else {
			return false;
		}

		if (data.size() == 3) { // Sequence.
			values = data[0];
			sustain_offset = data[1];
			sustain_length = data[2];
			is_envelope = false;
		} else if (data.size() == 4) { // Envelope.
			steps = data[0];
			values = data[1];
			sustain_offset = data[2];
			sustain_length = data[3];
			is_envelope = true;
		} else {
			return false;
		}

		switch (type) {
			case SEQUENCE_VOLUME: {
				if (is_envelope) {
					set_envelope_volume(steps, values, sustain_offset, sustain_length);
				} else {
					set_sequence_volume(values, sustain_offset, sustain_length);
				}
			} break;
			case SEQUENCE_PANNING: {
				if (is_envelope) {
					set_envelope_panning(steps, values, sustain_offset, sustain_length);
				} else {
					set_sequence_panning(values, sustain_offset, sustain_length);
				}
			} break;
			case SEQUENCE_PITCH: {
				if (is_envelope) {
					set_envelope_pitch(steps, values, sustain_offset, sustain_length);
				} else {
					set_sequence_pitch(values, sustain_offset, sustain_length);
				}
			} break;
			case SEQUENCE_DUTY_CYCLE: {
				if (is_envelope) {
					set_envelope_duty_cycle(steps, values, sustain_offset, sustain_length);
				} else {
					set_sequence_duty_cycle(values, sustain_offset, sustain_length);
				}
			} break;
		}
	} else {
		return false;
	}

	return true;
}

bool BlipKitInstrument::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name.begins_with("sequence/")) {
		String name = p_name;
		SequenceType type;

		if (name == "sequence/volume") {
			type = SEQUENCE_VOLUME;
		} else if (name == "sequence/panning") {
			type = SEQUENCE_PANNING;
		} else if (name == "sequence/pitch") {
			type = SEQUENCE_PITCH;
		} else if (name == "sequence/duty_cycle") {
			type = SEQUENCE_DUTY_CYCLE;
		} else {
			return false;
		}

		Array data;
		const Sequence &sequence = sequences[type];

		if (sequence.values != Variant()) {
			if (!sequence.steps.is_empty()) {
				data.append(sequence.steps);
			}

			data.append(sequence.values);
			data.append(sequence.sustain_offset);
			data.append(sequence.sustain_length);
		}

		r_ret = data;
	} else {
		return false;
	}

	return true;
}

BlipKitInstrument::BlipKitInstrument() {
	BKInt result = BKInstrumentInit(&instrument);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKInstrument: %s.", BKStatusGetName(result)));
}

BlipKitInstrument::~BlipKitInstrument() {
	AudioStreamBlipKit::lock();
	BKDispose(&instrument);
	AudioStreamBlipKit::unlock();
}

void BlipKitInstrument::set_sequence_float(SequenceType p_sequence, PackedFloat32Array &p_values, int p_sustain_offset, int p_sustain_length, real_t p_multiplier) {
	p_sustain_offset = CLAMP(p_sustain_offset, 0, p_values.size());
	p_sustain_length = CLAMP(p_sustain_length, 0, p_values.size() - p_sustain_offset);

	switch (p_sequence) {
		case SEQUENCE_VOLUME:
		case SEQUENCE_PANNING:
		case SEQUENCE_PITCH: {
			// Ok.
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

	AudioStreamBlipKit::lock();

	// TODO: Copy/clear data outside of lock.
	Sequence &sequence = sequences[p_sequence];
	sequence.steps.clear();
	sequence.values = p_values.is_empty() ? Variant() : Variant(p_values.duplicate());
	sequence.sustain_offset = p_sustain_offset;
	sequence.sustain_length = p_sustain_length;

	BKInt result = BKInstrumentSetSequence(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);

	AudioStreamBlipKit::unlock();

	if (result == BK_INVALID_ATTRIBUTE) {
		ERR_FAIL_MSG("Failed to set sequence: Sustain loop has zero steps.");
	} else if (result != BK_SUCCESS) {
		ERR_FAIL_MSG(vformat("Failed to set sequence: %s.", BKStatusGetName(result)));
	}

	emit_changed();
}

void BlipKitInstrument::set_sequence_int(SequenceType p_sequence, PackedInt32Array &p_values, int p_sustain_offset, int p_sustain_length) {
	p_sustain_offset = CLAMP(p_sustain_offset, 0, p_values.size());
	p_sustain_length = CLAMP(p_sustain_length, 0, p_values.size() - p_sustain_offset);

	switch (p_sequence) {
		case SEQUENCE_DUTY_CYCLE: {
			// Ok.
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

	AudioStreamBlipKit::lock();

	// TODO: Copy/clear data outside of lock.
	Sequence &sequence = sequences[p_sequence];
	sequence.steps.clear();
	sequence.values = p_values.is_empty() ? Variant() : Variant(p_values.duplicate());
	sequence.sustain_offset = p_sustain_offset;
	sequence.sustain_length = p_sustain_length;

	BKInt result = BKInstrumentSetSequence(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);

	AudioStreamBlipKit::unlock();

	if (result == BK_INVALID_ATTRIBUTE) {
		ERR_FAIL_MSG("Failed to set sequence: Sustain loop has zero steps.");
	} else if (result != BK_SUCCESS) {
		ERR_FAIL_MSG(vformat("Failed to set sequence: %s.", BKStatusGetName(result)));
	}

	emit_changed();
}

void BlipKitInstrument::set_sequence_volume(PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_sequence_float(SEQUENCE_VOLUME, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_MAX_VOLUME);
}

void BlipKitInstrument::set_sequence_panning(PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_sequence_float(SEQUENCE_PANNING, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_MAX_VOLUME);
}

void BlipKitInstrument::set_sequence_pitch(PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_sequence_float(SEQUENCE_PITCH, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_FINT20_UNIT);
}

void BlipKitInstrument::set_sequence_duty_cycle(PackedInt32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_sequence_int(SEQUENCE_DUTY_CYCLE, p_values, p_sustain_offset, p_sustain_length);
}

void BlipKitInstrument::set_envelope_float(SequenceType p_sequence, PackedInt32Array &p_steps, PackedFloat32Array &p_values, int p_sustain_offset, int p_sustain_length, real_t p_multiplier) {
	ERR_FAIL_COND(p_steps.size() != p_values.size());

	p_sustain_offset = CLAMP(p_sustain_offset, 0, p_values.size());
	p_sustain_length = CLAMP(p_sustain_length, 0, p_values.size() - p_sustain_offset);

	switch (p_sequence) {
		case SEQUENCE_PITCH:
		case SEQUENCE_VOLUME:
		case SEQUENCE_PANNING: {
			// Ok.
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

	AudioStreamBlipKit::lock();

	// TODO: Copy/clear data outside of lock.
	Sequence &sequence = sequences[p_sequence];
	sequence.steps = p_steps.duplicate();
	sequence.values = p_values.is_empty() ? Variant() : Variant(p_values.duplicate());
	sequence.sustain_offset = p_sustain_offset;
	sequence.sustain_length = p_sustain_length;

	BKInt result = BKInstrumentSetEnvelope(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);

	AudioStreamBlipKit::unlock();

	if (result == BK_INVALID_ATTRIBUTE) {
		ERR_FAIL_MSG("Failed to set envelope: Sustain loop has zero steps.");
	} else if (result != BK_SUCCESS) {
		ERR_FAIL_MSG(vformat("Failed to set envelope: %s.", BKStatusGetName(result)));
	}

	emit_changed();
}

void BlipKitInstrument::set_envelope_int(SequenceType p_sequence, PackedInt32Array &p_steps, PackedInt32Array &p_values, int p_sustain_offset, int p_sustain_length) {
	ERR_FAIL_COND(p_steps.size() != p_values.size());

	p_sustain_offset = CLAMP(p_sustain_offset, 0, p_values.size());
	p_sustain_length = CLAMP(p_sustain_length, 0, p_values.size() - p_sustain_offset);

	switch (p_sequence) {
		case SEQUENCE_DUTY_CYCLE: {
			// Ok.
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

	AudioStreamBlipKit::lock();

	// TODO: Copy/clear data outside of lock.
	Sequence &sequence = sequences[p_sequence];
	sequence.steps = p_steps.duplicate();
	sequence.values = p_values.is_empty() ? Variant() : Variant(p_values.duplicate());
	sequence.sustain_offset = p_sustain_offset;
	sequence.sustain_length = p_sustain_length;

	BKInt result = BKInstrumentSetEnvelope(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);

	AudioStreamBlipKit::unlock();

	if (result == BK_INVALID_ATTRIBUTE) {
		ERR_FAIL_MSG("Failed to set envelope: Sustain loop has zero steps.");
	} else if (result != BK_SUCCESS) {
		ERR_FAIL_MSG(vformat("Failed to set envelope: %s.", BKStatusGetName(result)));
	}

	emit_changed();
}

void BlipKitInstrument::set_envelope_volume(PackedInt32Array p_steps, PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_envelope_float(SEQUENCE_VOLUME, p_steps, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_MAX_VOLUME);
}

void BlipKitInstrument::set_envelope_panning(PackedInt32Array p_steps, PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_envelope_float(SEQUENCE_PANNING, p_steps, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_MAX_VOLUME);
}

void BlipKitInstrument::set_envelope_pitch(PackedInt32Array p_steps, PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_envelope_float(SEQUENCE_PITCH, p_steps, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_FINT20_UNIT);
}

void BlipKitInstrument::set_envelope_duty_cycle(PackedInt32Array p_steps, PackedInt32Array p_values, int p_sustain_offset, int p_sustain_length) {
	set_envelope_int(SEQUENCE_DUTY_CYCLE, p_steps, p_values, p_sustain_offset, p_sustain_length);
}

void BlipKitInstrument::set_envelope_adsr(int p_attack, int p_decay, real_t p_sustain, int p_release) {
	BKInt sustain = CLAMP(p_sustain, 0.0, 1.0) * BK_MAX_VOLUME;
	set_envelope_volume({ p_attack, p_decay, 240, p_release }, { 1.0, p_sustain, p_sustain, 0.0 }, 2, 1);
}
