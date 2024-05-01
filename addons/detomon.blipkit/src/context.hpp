#pragma once

#include <godot_cpp/classes/ref_counted.hpp>

using namespace godot;

namespace detomon::BlipKit {

class Context : public RefCounted {
	GDCLASS(DMBKContext, RefCounted)

private:
	int bla;

protected:
	static void _bind_methods();

public:
	Context();

};

}
