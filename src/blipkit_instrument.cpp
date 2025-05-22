#include "blipkit_instrument.hpp"
#include "audio_stream_blipkit.hpp"
#include "string_names.hpp"
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/core/mutex_lock.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/array.hpp>

using namespace BlipKit;
using namespace godot;

BlipKitInstrument::BlipKitInstrument() {
	const BKInt result = BKInstrumentInit(&instrument);
	ERR_FAIL_COND_MSG(result != BK_SUCCESS, vformat("Failed to initialize BKInstrument: %s.", BKStatusGetName(result)));
}

BlipKitInstrument::~BlipKitInstrument() {
	BK_THREAD_SAFE_METHOD

	BKDispose(&instrument);
}

Ref<BlipKitInstrument> BlipKitInstrument::create_with_adsr(int p_attack, int p_decay, float p_sustain, int p_release) {
	Ref<BlipKitInstrument> instance;
	instance.instantiate();
	instance->set_adsr(p_attack, p_decay, p_sustain, p_release);

	return instance;
}

void BlipKitInstrument::set_envelope(EnvelopeType p_type, const PackedFloat32Array &p_values, const PackedInt32Array &p_steps, int p_sustain_offset, int p_sustain_length) {
	ERR_FAIL_INDEX(p_type, ENVELOPE_MAX);

	float multiplier = 1.0;
	BKEnum sequence = 0;

	switch (p_type) {
		case ENVELOPE_VOLUME: {
			multiplier = float(BK_MAX_VOLUME);
			sequence = BK_SEQUENCE_VOLUME;
		} break;
		case ENVELOPE_PANNING: {
			multiplier = float(BK_MAX_VOLUME);
			sequence = BK_SEQUENCE_PANNING;
		} break;
		case ENVELOPE_PITCH: {
			multiplier = float(BK_FINT20_UNIT);
			sequence = BK_SEQUENCE_PITCH;
		} break;
		case ENVELOPE_DUTY_CYCLE: {
			multiplier = 1.0;
			sequence = BK_SEQUENCE_DUTY_CYCLE;
		} break;
		default: {
			ERR_FAIL_MSG(vformat("Invalid instrument sequence: %d.", p_type));
		} break;
	}

	const uint32_t steps_size = p_steps.size();
	const uint32_t values_size = p_values.size();
	const bool has_steps = steps_size > 0;

	if (has_steps) {
		ERR_FAIL_COND(steps_size != values_size);
	}

	if (p_sustain_offset < 0) {
		p_sustain_offset += values_size + 1;
	}

	p_sustain_offset = CLAMP(p_sustain_offset, 0, values_size);
	p_sustain_length = CLAMP(p_sustain_length, 0, values_size - p_sustain_offset);

	PackedInt32Array steps_copy;
	steps_copy.resize(steps_size);
	int32_t *steps_ptrw = steps_copy.ptrw();
	for (uint32_t i = 0; i < steps_size; i++) {
		steps_ptrw[i] = p_steps[i];
	}

	PackedFloat32Array values_copy;
	values_copy.resize(values_size);
	float *values_ptrw = values_copy.ptrw();
	for (uint32_t i = 0; i < values_size; i++) {
		values_ptrw[i] = p_values[i];
	}

	LocalVector<BKSequencePhase> seq_phases;
	LocalVector<BKInt> step_phases;

	if (has_steps) {
		seq_phases.resize(values_size);

		for (uint32_t i = 0; i < values_size; i++) {
			seq_phases[i].steps = BKUInt(p_steps[i]);
			seq_phases[i].value = BKInt(p_values[i] * multiplier);
		}
	} else {
		step_phases.resize(values_size);

		for (uint32_t i = 0; i < values_size; i++) {
			step_phases[i] = BKInt(p_values[i] * multiplier);
		}
	}

	BK_THREAD_SAFE_METHOD

	BKInt result = 0;

	if (has_steps) {
		result = BKInstrumentSetEnvelope(&instrument, sequence, seq_phases.ptr(), seq_phases.size(), p_sustain_offset, p_sustain_length);
	} else {
		result = BKInstrumentSetSequence(&instrument, sequence, step_phases.ptr(), step_phases.size(), p_sustain_offset, p_sustain_length);
	}

	if (result == BK_INVALID_VALUE) {
		ERR_FAIL_MSG("Failed to set envelope: Sustain cycle has zero steps.");
	} else if (result != BK_SUCCESS) {
		ERR_FAIL_MSG(vformat("Failed to set envelope: %s.", BKStatusGetName(result)));
	}

	Sequence &seq = sequences[p_type];
	seq.steps = steps_copy;
	seq.values = values_copy;
	seq.sustain_offset = p_sustain_offset;
	seq.sustain_length = p_sustain_length;

	emit_changed();
}

void BlipKitInstrument::set_adsr(int p_attack, int p_decay, float p_sustain, int p_release) {
	set_envelope(ENVELOPE_VOLUME, { 1.0, p_sustain, p_sustain, 0.0 }, { p_attack, p_decay, 240, p_release }, 2, 1);
}

bool BlipKitInstrument::has_envelope(EnvelopeType p_type) const {
	ERR_FAIL_INDEX_V(p_type, ENVELOPE_MAX, false);
	return not sequences[p_type].values.is_empty();
}

PackedInt32Array BlipKitInstrument::get_envelope_steps(EnvelopeType p_type) const {
	ERR_FAIL_INDEX_V(p_type, ENVELOPE_MAX, PackedInt32Array());
	return sequences[p_type].steps;
}

PackedFloat32Array BlipKitInstrument::get_envelope_values(EnvelopeType p_type) const {
	ERR_FAIL_INDEX_V(p_type, ENVELOPE_MAX, PackedFloat32Array());
	return sequences[p_type].values;
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
	set_envelope(p_type, {}, {}, 0, 0);
}

void BlipKitInstrument::_bind_methods() {
	ClassDB::bind_static_method("BlipKitInstrument", D_METHOD("create_with_adsr", "attack", "decay", "sustain", "release"), &BlipKitInstrument::create_with_adsr);

	ClassDB::bind_method(D_METHOD("set_envelope", "type", "values", "steps", "sustain_offset", "sustain_length"), &BlipKitInstrument::set_envelope, DEFVAL(PackedInt32Array()), DEFVAL(-1), DEFVAL(0));
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
	return vformat("<BlipKitInstrument#%d>", get_instance_id());
}

void BlipKitInstrument::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::ARRAY, "envelope/volume", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "envelope/panning", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "envelope/pitch", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
	p_list->push_back(PropertyInfo(Variant::ARRAY, "envelope/duty_cycle", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
}

bool BlipKitInstrument::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name.begins_with(BKStringName(envelope_))) {
		const String name = p_name;
		EnvelopeType type;

		if (p_name == BKStringName(envelope_volume)) {
			type = ENVELOPE_VOLUME;
		} else if (p_name == BKStringName(envelope_panning)) {
			type = ENVELOPE_PANNING;
		} else if (p_name == BKStringName(envelope_pitch)) {
			type = ENVELOPE_PITCH;
		} else if (p_name == BKStringName(envelope_duty_cycle)) {
			type = ENVELOPE_DUTY_CYCLE;
		} else {
			return false;
		}

		const Array &data = p_value;

		ERR_FAIL_COND_V(data.size() != 4, false);

		const PackedInt32Array &steps = data[0];
		const PackedFloat32Array &values = data[1];
		const int sustain_offset = data[2];
		const int sustain_length = data[3];

		set_envelope(type, values, steps, sustain_offset, sustain_length);
	} else {
		return false;
	}

	return true;
}

bool BlipKitInstrument::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name.begins_with(BKStringName(envelope_))) {
		EnvelopeType type;

		if (p_name == BKStringName(envelope_volume)) {
			type = ENVELOPE_VOLUME;
		} else if (p_name == BKStringName(envelope_panning)) {
			type = ENVELOPE_PANNING;
		} else if (p_name == BKStringName(envelope_pitch)) {
			type = ENVELOPE_PITCH;
		} else if (p_name == BKStringName(envelope_duty_cycle)) {
			type = ENVELOPE_DUTY_CYCLE;
		} else {
			return false;
		}

		Array data;
		const Sequence &seq = sequences[type];

		if (not seq.values.is_empty()) {
			data.resize(4);
			data[0] = seq.steps;
			data[1] = seq.values;
			data[2] = seq.sustain_offset;
			data[3] = seq.sustain_length;
		}

		r_ret = data;
	} else {
		return false;
	}

	return true;
}
