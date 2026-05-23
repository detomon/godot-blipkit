#pragma once

#include <BlipKit.h>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace BlipKit {

class Sample {
private:
	BKData data = {};

public:
	~Sample();

	bool initialize();
};

} //namespace BlipKit
