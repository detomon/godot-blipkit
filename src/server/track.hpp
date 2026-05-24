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

	TypedArray<RID> get_dividers() const;
	bool has_divider(RID p_rid);
	RID add_divider(int p_tick_interval, const Callable &p_callable);
	void remove_divider(RID p_rid);
	void reset_divider(RID p_rid, int p_tick_interval = 0);
	void clear_dividers();
};

} //namespace BlipKit
