#pragma once

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/variant/string_name.hpp>

using namespace godot;

namespace BlipKit {

struct StringNames {
private:
	static inline StringNames *singleton = nullptr;

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

	static void create();
	static void free();
	static _ALWAYS_INLINE_ const StringNames *get_singleton() { return singleton; }

	StringNames();
	~StringNames() = default;
};

} //namespace BlipKit

#define BKStringName(m_name) StringNames::get_singleton()->m_name
