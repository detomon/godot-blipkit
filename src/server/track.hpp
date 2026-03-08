#pragma once

#include "divider.hpp"
#include <BlipKit.h>

using namespace godot;

namespace BlipKit {

class Track {
private:
	BKTrack track = { { { 0 } } };
	PackedFloat32Array arpeggio;
	DividerGroup dividers;
	AudioStreamBlipKitPlayback *playback = nullptr;
	RID instrument;
	RID custom_waveform;
	RID sample;
	bool master_volume_changed = false;
};

} //namespace BlipKit
