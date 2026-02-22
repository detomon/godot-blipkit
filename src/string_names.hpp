#pragma once

#include <godot_cpp/variant/string_name.hpp>

using namespace godot;

namespace BlipKit {

struct StringNames {
private:
	static inline StringNames *singleton = nullptr;

public:
	StringName _bytes = "_bytes";
	StringName _frames = "_frames";
	StringName BlipKitBytecode = "BlipKitBytecode";
	StringName delta = "delta";
	StringName envelope_ = "envelope";
	StringName envelope_duty_cycle = "envelope/duty_cycle";
	StringName envelope_panning = "envelope/panning";
	StringName envelope_pitch = "envelope/pitch";
	StringName envelope_volume = "envelope/volume";
	StringName repeat_mode = "repeat_mode";
	StringName slide_ticks = "slide_ticks";
	StringName sustain_end = "sustain_end";
	StringName sustain_offset = "sustain_offset";
	StringName ticks = "ticks";

	static void create();
	static void free();

	static _ALWAYS_INLINE_ const StringNames *get_singleton() { return singleton; }
};

} //namespace BlipKit

#define BKStringName(m_name) StringNames::get_singleton()->m_name
