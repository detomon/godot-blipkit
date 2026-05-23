#pragma once

#include <BlipKit.h>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace BlipKit {

class Instrument {
public:
	enum EnvelopeType {
		ENVELOPE_VOLUME,
		ENVELOPE_PANNING,
		ENVELOPE_PITCH,
		ENVELOPE_DUTY_CYCLE,
		ENVELOPE_MAX,
	};

	struct Sequence {
		PackedInt32Array steps;
		PackedFloat32Array values;
		int sustain_offset = 0;
		int sustain_length = 0;
	};

private:
	BKInstrument instrument = {};
	Sequence sequences[ENVELOPE_MAX];

public:
	~Instrument();

	bool initialize();
	_ALWAYS_INLINE_ BKInstrument *get_instrument() { return &instrument; };
};

} //namespace BlipKit
