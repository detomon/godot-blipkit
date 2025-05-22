#pragma once

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/string_name.hpp>

#define BKStringName(m_name) Strings::get_singleton()->m_name

using namespace godot;

namespace BlipKit {

struct Strings {
private:
	static Strings *strings;

public:
	StringName _bytes;
	StringName _frames;
	StringName BlipKitBytecode;
	StringName delta;
	StringName envelope_;
	StringName envelope_duty_cycle;
	StringName envelope_panning;
	StringName envelope_pitch;
	StringName envelope_volume;
	StringName repeat_mode;
	StringName slide_ticks;
	StringName sustain_end;
	StringName sustain_offset;
	StringName ticks;

	static void initialize();
	static void uninitialize();
	static _ALWAYS_INLINE_ const Strings *get_singleton() { return strings; }

	Strings();
	~Strings() = default;
};

} //namespace BlipKit
