#include "string_names.hpp"
#include <godot_cpp/core/memory.hpp>

using namespace BlipKit;
using namespace godot;

void StringNames::create() {
	singleton = memnew(StringNames);
}

void StringNames::free() {
	memdelete(singleton);
	singleton = nullptr;
}

StringNames::StringNames() {
	_bytes = StringName("_bytes");
	_frames = StringName("_frames");
	BlipKitBytecode = StringName("BlipKitBytecode");
	delta = StringName("delta");
	envelope_ = StringName("envelope");
	envelope_duty_cycle = StringName("envelope/duty_cycle");
	envelope_panning = StringName("envelope/panning");
	envelope_pitch = StringName("envelope/pitch");
	envelope_volume = StringName("envelope/volume");
	repeat_mode = StringName("repeat_mode");
	slide_ticks = StringName("slide_ticks");
	sustain_end = StringName("sustain_end");
	sustain_offset = StringName("sustain_offset");
	ticks = StringName("ticks");
}
