#pragma once

#include "divider.hpp"
#include <BlipKit.h>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

namespace BlipKit {

class Track {
private:
	BKTrack track = {};
	PackedFloat32Array arpeggio;
	DividerGroup dividers;
	AudioStreamBlipKitPlayback *playback = nullptr;
	RID instrument;
	RID custom_waveform;
	RID sample;
	struct {
		bool master_volume_changed : 1 = false;
	};

public:
	~Track();

	bool initialize();
};

} //namespace BlipKit
