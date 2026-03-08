#pragma once

#include "divider.hpp"
#include <BlipKit.h>

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
	BKInstrument instrument = { { 0 } };
	Sequence sequences[ENVELOPE_MAX];
};

} //namespace BlipKit
