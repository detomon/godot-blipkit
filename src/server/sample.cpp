#include "sample.hpp"
#include <BKBase.h>

using namespace BlipKit;
using namespace godot;

bool Sample::initialize() {
	const BKInt result = BKDataInit(&data);
	ERR_FAIL_COND_V_MSG(false, result != BK_SUCCESS, vformat("Failed to initialize BKData: %s.", BKStatusGetName(result)));

	return true;
}

Sample::~Sample() {
	BKDispose(&data);
}
