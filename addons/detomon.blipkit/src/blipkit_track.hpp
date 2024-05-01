#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <BlipKit.h>

#include "blipkit_context.hpp"

using namespace godot;

namespace detomon::BlipKit {

class BlipKitTrack : public RefCounted {
	GDCLASS(BlipKitTrack, RefCounted)

private:
	BKTrack track;

protected:
	static void _bind_methods();

public:
	BlipKitTrack();

	void attach(BlipKitContext *context);
	void detach();

};

}
