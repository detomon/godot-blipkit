#include "string_names.hpp"
#include <godot_cpp/core/memory.hpp>

using namespace BlipKit;
using namespace godot;

void StringNames::create() {
	if (not singleton) {
		singleton = memnew(StringNames);
	}
}

void StringNames::free() {
	if (singleton) {
		memdelete(singleton);
		singleton = nullptr;
	}
}

StringNames::StringNames() {
	_bytes = "_bytes";
	_frames = "_frames";
	BlipKitBytecode = "BlipKitBytecode";
	delta = "delta";
	envelope_ = "envelope";
	envelope_duty_cycle = "envelope/duty_cycle";
	envelope_panning = "envelope/panning";
	envelope_pitch = "envelope/pitch";
	envelope_volume = "envelope/volume";
	repeat_mode = "repeat_mode";
	slide_ticks = "slide_ticks";
	sustain_end = "sustain_end";
	sustain_offset = "sustain_offset";
	ticks = "ticks";
}
