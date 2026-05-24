#include "instrument.hpp"

using namespace BlipKit;
using namespace godot;

bool Instrument::initialize() {
	const BKInt result = BKInstrumentInit(&instrument);
	ERR_FAIL_COND_V_MSG(false, result != BK_SUCCESS, vformat("Failed to initialize BKInstrument: %s.", BKStatusGetName(result)));

	return true;
}

Instrument::~Instrument() {
	BKDispose(&instrument);
}
