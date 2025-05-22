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
