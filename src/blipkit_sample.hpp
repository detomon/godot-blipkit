#pragma once

#include <BlipKit.h>
#include <godot_cpp/classes/audio_stream_wav.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitSample : public Resource {
	GDCLASS(BlipKitSample, Resource)

public:
	enum RepeatMode {
		REPEAT_NONE,
		REPEAT_FORWARD,
		REPEAT_PING_PONG,
		REPEAT_BACKWARD,
		REPEAT_MAX,
	};

private:
	BKData data;
	LocalVector<BKFrame> frames;
	uint32_t sustain_offset = 0;
	uint32_t sustain_end = 0;
	RepeatMode repeat_mode = RepeatMode::REPEAT_NONE;

public:
	BlipKitSample();
	~BlipKitSample();

	static Ref<BlipKitSample> create_with_wav(const Ref<AudioStreamWAV> &p_wav, bool p_normalize = false, float p_amplitude = 1.0);

	_ALWAYS_INLINE_ BKData *get_data() { return &data; };
	_ALWAYS_INLINE_ int size() const { return frames.size(); };
	_ALWAYS_INLINE_ bool is_valid() const { return !frames.is_empty(); };

	void set_frames(const PackedFloat32Array &p_frames, bool p_normalize = false, float p_amplitude = 1.0);
	PackedFloat32Array get_frames() const;

	void set_sustain_offset(int p_sustain_offset);
	int get_sustain_offset() const;
	void set_sustain_end(int p_sustain_end);
	int get_sustain_end() const;
	void set_repeat_mode(RepeatMode p_repeat_mode);
	RepeatMode get_repeat_mode() const;

protected:
	void set_frame_bytes(const PackedByteArray &p_frames);
	PackedByteArray get_frame_bytes() const;

	static void _bind_methods();
	String _to_string() const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
};

} // namespace BlipKit

VARIANT_ENUM_CAST(BlipKit::BlipKitSample::RepeatMode);
