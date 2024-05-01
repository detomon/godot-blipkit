#pragma once

#include <BlipKit.h>
#include <atomic>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace BlipKit {

class AudioStreamBlipKitPlayback;

class DividerGroup {
public:
	typedef uint32_t ID;

private:
	class Divider {
	private:
		Callable callable;
		int divider = 0;
		int counter = 0;

	public:
		void initialize(const Callable &p_callable, int p_tick_interval);
		void reset(int p_tick_interval = 0);

		_ALWAYS_INLINE_ int tick() {
			counter--;

			if (counter > 0) [[likely]] {
				return 0;
			} else {
				int ticks = callable.call();

				// Set new divider value.
				if (ticks > 0) {
					divider = ticks;
				}
				counter = divider;

				return ticks;
			}
		}
	};

	static std::atomic<ID> id;
	HashMap<ID, Divider> dividers;
	BKDivider divider = { { 0 } };

	static BKEnum divider_callback(BKCallbackInfo *p_info, void *p_user_info);

public:
	DividerGroup();
	~DividerGroup();

	PackedInt32Array get_dividers() const;
	ID add_divider(int p_tick_interval, const Callable &p_callable);
	void remove_divider(ID p_id);
	bool has_divider(ID p_id);
	void reset_divider(ID p_id, int p_tick_interval = 0);
	void clear();

	void attach(AudioStreamBlipKitPlayback *p_playback);
	void detach();
	void reset();
};

} // namespace BlipKit
