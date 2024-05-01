#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <BlipKit.h>

using namespace godot;

namespace detomon::BlipKit {

class BlipKitContext : public RefCounted {
	GDCLASS(BlipKitContext, RefCounted)

private:
	const int NUM_CHANNELS = 2;

	BKContext context;

protected:
	static void _bind_methods();

public:
	BlipKitContext();

	BKContext* get_context() { return &context; }

};

}
