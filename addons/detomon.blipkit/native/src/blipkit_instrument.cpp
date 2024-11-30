#include "blipkit_instrument.hpp"
#include "audio_stream_blipkit.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/array.hpp>

using namespace detomon::BlipKit;
using namespace godot;

void BlipKitInstrument::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_sequence", "type", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_sequence);
	ClassDB::bind_method(D_METHOD("set_envelope", "type", "steps", "values", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_envelope);
	ClassDB::bind_method(D_METHOD("set_adsr", "attack", "decay", "sustain", "release"), &BlipKitInstrument::set_adsr);

	BIND_ENUM_CONSTANT(SEQUENCE_VOLUME);
	BIND_ENUM_CONSTANT(SEQUENCE_PANNING);
	BIND_ENUM_CONSTANT(SEQUENCE_PITCH);
	BIND_ENUM_CONSTANT(SEQUENCE_DUTY_CYCLE);
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
			ERR_FAIL_V_MSG(false, vformat("Invalid instrument sequence: %s.", name));
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
			ERR_FAIL_V_MSG(false, "Invalid number of array elements in instrument sequence.");
		}

		switch (type) {
			case SEQUENCE_VOLUME: {
				if (is_envelope) {
					set_envelope(SEQUENCE_VOLUME, steps, values, sustain_offset, sustain_length);
				} else {
					set_sequence(SEQUENCE_VOLUME, values, sustain_offset, sustain_length);
				}
			} break;
			case SEQUENCE_PANNING: {
				if (is_envelope) {
					set_envelope(SEQUENCE_PANNING, steps, values, sustain_offset, sustain_length);
				} else {
					set_sequence(SEQUENCE_PANNING, values, sustain_offset, sustain_length);
				}
			} break;
			case SEQUENCE_PITCH: {
				if (is_envelope) {
					set_envelope(SEQUENCE_PITCH, steps, values, sustain_offset, sustain_length);
				} else {
					set_sequence(SEQUENCE_PITCH, values, sustain_offset, sustain_length);
				}
			} break;
			case SEQUENCE_DUTY_CYCLE: {
				if (is_envelope) {
					set_envelope(SEQUENCE_DUTY_CYCLE, steps, values, sustain_offset, sustain_length);
				} else {
					set_sequence(SEQUENCE_DUTY_CYCLE, values, sustain_offset, sustain_length);
				}
			} break;
			default: {
				ERR_FAIL_V_MSG(false, vformat("Invalid instrument sequence type: %d.", type));
			} break;
		}

		return true;
	}

	return false;
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
			ERR_FAIL_V_MSG(false, vformat("Invalid instrument sequence: %s.", name));
		}

		Array data;
		const Sequence &seq = sequences[type];

		if (seq.values != Variant()) {
			if (!seq.steps.is_empty()) {
				data.append(seq.steps);
			}

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
			// OK.
		} break;
		default: {
			return;
		} break;
	}

	LocalVector<BKInt> values;
	values.resize(p_values.size());

	for (int i = 0; i < p_values.size(); i++) {
		values[i] = BKInt(p_values[i] * p_multiplier);
	}

	Variant values_copy = p_values.is_empty() ? Variant() : Variant(p_values.duplicate());

	RecursiveSpinLock::Autolock lock = AudioStreamBlipKit::autolock();

	BKInt result = BKInstrumentSetSequence(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);

	if (result == BK_INVALID_ATTRIBUTE) {
		ERR_FAIL_MSG("Failed to set sequence: Sustain loop has zero steps.");
	} else if (result != BK_SUCCESS) {
		ERR_FAIL_MSG(vformat("Failed to set sequence: %s.", BKStatusGetName(result)));
	}

	Sequence &seq = sequences[p_sequence];
	seq.steps.clear();
	seq.values = values_copy;
	seq.sustain_offset = p_sustain_offset;
	seq.sustain_length = p_sustain_length;

	lock.unlock();

	emit_changed();
}

void BlipKitInstrument::set_sequence_int(SequenceType p_sequence, PackedInt32Array &p_values, int p_sustain_offset, int p_sustain_length) {
	p_sustain_offset = CLAMP(p_sustain_offset, 0, p_values.size());
	p_sustain_length = CLAMP(p_sustain_length, 0, p_values.size() - p_sustain_offset);

	switch (p_sequence) {
		case SEQUENCE_DUTY_CYCLE: {
			// OK.
		} break;
		default: {
			return;
		} break;
	}

	LocalVector<BKInt> values;
	values.resize(p_values.size());

	for (int i = 0; i < p_values.size(); i++) {
		values[i] = p_values[i];
	}

	Variant values_copy = p_values.is_empty() ? Variant() : Variant(p_values.duplicate());

	RecursiveSpinLock::Autolock lock = AudioStreamBlipKit::autolock();

	BKInt result = BKInstrumentSetSequence(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);

	if (result == BK_INVALID_ATTRIBUTE) {
		ERR_FAIL_MSG("Failed to set sequence: Sustain loop has zero steps.");
	} else if (result != BK_SUCCESS) {
		ERR_FAIL_MSG(vformat("Failed to set sequence: %s.", BKStatusGetName(result)));
	}

	Sequence &seq = sequences[p_sequence];
	seq.steps.clear();
	seq.values = values_copy;
	seq.sustain_offset = p_sustain_offset;
	seq.sustain_length = p_sustain_length;

	lock.unlock();

	emit_changed();
}

void BlipKitInstrument::set_envelope_float(SequenceType p_sequence, PackedInt32Array &p_steps, PackedFloat32Array &p_values, int p_sustain_offset, int p_sustain_length, real_t p_multiplier) {
	ERR_FAIL_COND(p_steps.size() != p_values.size());

	p_sustain_offset = CLAMP(p_sustain_offset, 0, p_values.size());
	p_sustain_length = CLAMP(p_sustain_length, 0, p_values.size() - p_sustain_offset);

	switch (p_sequence) {
		case SEQUENCE_PITCH:
		case SEQUENCE_VOLUME:
		case SEQUENCE_PANNING: {
			// OK.
		} break;
		default: {
			return;
		} break;
	}

	LocalVector<BKSequencePhase> values;
	values.resize(p_values.size());

	for (int i = 0; i < p_values.size(); i++) {
		values[i] = {
			.steps = BKUInt(p_steps[i]),
			.value = BKInt(p_values[i] * p_multiplier),
		};
	}

	PackedInt32Array steps_copy = p_steps.duplicate();
	Variant values_copy = p_values.is_empty() ? Variant() : Variant(p_values.duplicate());

	RecursiveSpinLock::Autolock lock = AudioStreamBlipKit::autolock();

	BKInt result = BKInstrumentSetEnvelope(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);

	if (result == BK_INVALID_ATTRIBUTE) {
		ERR_FAIL_MSG("Failed to set envelope: Sustain loop has zero steps.");
	} else if (result != BK_SUCCESS) {
		ERR_FAIL_MSG(vformat("Failed to set envelope: %s.", BKStatusGetName(result)));
	}

	Sequence &seq = sequences[p_sequence];
	seq.steps = steps_copy;
	seq.values = values_copy;
	seq.sustain_offset = p_sustain_offset;
	seq.sustain_length = p_sustain_length;

	lock.unlock();

	emit_changed();
}

void BlipKitInstrument::set_envelope_int(SequenceType p_sequence, PackedInt32Array &p_steps, PackedInt32Array &p_values, int p_sustain_offset, int p_sustain_length) {
	ERR_FAIL_COND(p_steps.size() != p_values.size());

	p_sustain_offset = CLAMP(p_sustain_offset, 0, p_values.size());
	p_sustain_length = CLAMP(p_sustain_length, 0, p_values.size() - p_sustain_offset);

	switch (p_sequence) {
		case SEQUENCE_DUTY_CYCLE: {
			// OK.
		} break;
		default: {
			return;
		} break;
	}

	LocalVector<BKSequencePhase> values;
	values.resize(p_values.size());

	for (int i = 0; i < p_values.size(); i++) {
		values[i].steps = p_steps[i];
		values[i].value = p_values[i];
	}

	PackedInt32Array steps_copy = p_steps.duplicate();
	Variant values_copy = p_values.is_empty() ? Variant() : Variant(p_values.duplicate());

	RecursiveSpinLock::Autolock lock = AudioStreamBlipKit::autolock();

	BKInt result = BKInstrumentSetEnvelope(&instrument, p_sequence, values.ptr(), values.size(), p_sustain_offset, p_sustain_length);

	if (result == BK_INVALID_ATTRIBUTE) {
		ERR_FAIL_MSG("Failed to set envelope: Sustain loop has zero steps.");
	} else if (result != BK_SUCCESS) {
		ERR_FAIL_MSG(vformat("Failed to set envelope: %s.", BKStatusGetName(result)));
	}

	Sequence &seq = sequences[p_sequence];
	seq.steps = steps_copy;
	seq.values = values_copy;
	seq.sustain_offset = p_sustain_offset;
	seq.sustain_length = p_sustain_length;

	lock.unlock();

	emit_changed();
}

void BlipKitInstrument::set_sequence(SequenceType p_type, PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	switch (p_type) {
		case SEQUENCE_VOLUME: {
			set_sequence_float(SEQUENCE_VOLUME, p_values, p_sustain_offset, p_sustain_length, real_t(BK_MAX_VOLUME));
		} break;
		case SEQUENCE_PANNING: {
			set_sequence_float(SEQUENCE_PANNING, p_values, p_sustain_offset, p_sustain_length, real_t(BK_MAX_VOLUME));
		} break;
		case SEQUENCE_PITCH: {
			set_sequence_float(SEQUENCE_PITCH, p_values, p_sustain_offset, p_sustain_length, real_t(BK_FINT20_UNIT));
		} break;
		case SEQUENCE_DUTY_CYCLE: {
			PackedInt32Array values;
			values.resize(p_values.size());
			for (int i = 0; i < p_values.size(); i++) {
				values[i] = int(p_values[i]);
			}

			set_sequence_int(SEQUENCE_DUTY_CYCLE, values, p_sustain_offset, p_sustain_length);
		} break;
		default: {
			ERR_FAIL_MSG(vformat("Invalid instrument sequence type: %d.", p_type));
		} break;
	}
}

void BlipKitInstrument::set_envelope(SequenceType p_type, PackedInt32Array p_steps, PackedFloat32Array p_values, int p_sustain_offset, int p_sustain_length) {
	switch (p_type) {
		case SEQUENCE_VOLUME: {
			set_envelope_float(SEQUENCE_VOLUME, p_steps, p_values, p_sustain_offset, p_sustain_length, real_t(BK_MAX_VOLUME));
		} break;
		case SEQUENCE_PANNING: {
			set_envelope_float(SEQUENCE_PANNING, p_steps, p_values, p_sustain_offset, p_sustain_length, real_t(BK_MAX_VOLUME));
		} break;
		case SEQUENCE_PITCH: {
			set_envelope_float(SEQUENCE_PITCH, p_steps, p_values, p_sustain_offset, p_sustain_length, (real_t)BK_FINT20_UNIT);
		} break;
		case SEQUENCE_DUTY_CYCLE: {
			PackedInt32Array values;
			values.resize(p_values.size());
			for (int i = 0; i < p_values.size(); i++) {
				values[i] = int(p_values[i]);
			}

			set_envelope_int(SEQUENCE_DUTY_CYCLE, p_steps, values, p_sustain_offset, p_sustain_length);
		} break;
		default: {
			ERR_FAIL_MSG(vformat("Invalid instrument envelope type: %d.", p_type));
		} break;
	}
}

void BlipKitInstrument::set_adsr(int p_attack, int p_decay, real_t p_sustain, int p_release) {
	BKInt sustain = BKInt(CLAMP(p_sustain, 0.0, 1.0) * real_t(BK_MAX_VOLUME));
	set_envelope(SEQUENCE_VOLUME, { p_attack, p_decay, 240, p_release }, { 1.0, p_sustain, p_sustain, 0.0 }, 2, 1);
}
