#pragma once

#include "audio_stream_blipkit.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_waveform.hpp"
#include "divider.hpp"
#include <BlipKit.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

using namespace godot;

namespace detomon::BlipKit {

class AudioStreamBlipKitPlayback;

class BlipKitTrack : public RefCounted {
	GDCLASS(BlipKitTrack, RefCounted)

public:
	enum Waveform {
		WAVEFORM_SQUARE,
		WAVEFORM_TRIANGLE,
		WAVEFORM_NOISE,
		WAVEFORM_SAWTOOTH,
		WAVEFORM_SINE,
		WAVEFORM_CUSTOM,
		WAVEFORM_SAMPLE,
	};

	enum Note {
		NOTE_C_0 = BK_C_0,
		NOTE_C_SH_0 = BK_C_SH_0,
		NOTE_D_0 = BK_D_0,
		NOTE_D_SH_0 = BK_D_SH_0,
		NOTE_E_0 = BK_E_0,
		NOTE_F_0 = BK_F_0,
		NOTE_F_SH_0 = BK_F_SH_0,
		NOTE_G_0 = BK_G_0,
		NOTE_G_SH_0 = BK_G_SH_0,
		NOTE_A_0 = BK_A_0,
		NOTE_A_SH_0 = BK_A_SH_0,
		NOTE_B_0 = BK_B_0,
		NOTE_C_1 = BK_C_1,
		NOTE_C_SH_1 = BK_C_SH_1,
		NOTE_D_1 = BK_D_1,
		NOTE_D_SH_1 = BK_D_SH_1,
		NOTE_E_1 = BK_E_1,
		NOTE_F_1 = BK_F_1,
		NOTE_F_SH_1 = BK_F_SH_1,
		NOTE_G_1 = BK_G_1,
		NOTE_G_SH_1 = BK_G_SH_1,
		NOTE_A_1 = BK_A_1,
		NOTE_A_SH_1 = BK_A_SH_1,
		NOTE_B_1 = BK_B_1,
		NOTE_C_2 = BK_C_2,
		NOTE_C_SH_2 = BK_C_SH_2,
		NOTE_D_2 = BK_D_2,
		NOTE_D_SH_2 = BK_D_SH_2,
		NOTE_E_2 = BK_E_2,
		NOTE_F_2 = BK_F_2,
		NOTE_F_SH_2 = BK_F_SH_2,
		NOTE_G_2 = BK_G_2,
		NOTE_G_SH_2 = BK_G_SH_2,
		NOTE_A_2 = BK_A_2,
		NOTE_A_SH_2 = BK_A_SH_2,
		NOTE_B_2 = BK_B_2,
		NOTE_C_3 = BK_C_3,
		NOTE_C_SH_3 = BK_C_SH_3,
		NOTE_D_3 = BK_D_3,
		NOTE_D_SH_3 = BK_D_SH_3,
		NOTE_E_3 = BK_E_3,
		NOTE_F_3 = BK_F_3,
		NOTE_F_SH_3 = BK_F_SH_3,
		NOTE_G_3 = BK_G_3,
		NOTE_G_SH_3 = BK_G_SH_3,
		NOTE_A_3 = BK_A_3,
		NOTE_A_SH_3 = BK_A_SH_3,
		NOTE_B_3 = BK_B_3,
		NOTE_C_4 = BK_C_4,
		NOTE_C_SH_4 = BK_C_SH_4,
		NOTE_D_4 = BK_D_4,
		NOTE_D_SH_4 = BK_D_SH_4,
		NOTE_E_4 = BK_E_4,
		NOTE_F_4 = BK_F_4,
		NOTE_F_SH_4 = BK_F_SH_4,
		NOTE_G_4 = BK_G_4,
		NOTE_G_SH_4 = BK_G_SH_4,
		NOTE_A_4 = BK_A_4,
		NOTE_A_SH_4 = BK_A_SH_4,
		NOTE_B_4 = BK_B_4,
		NOTE_C_5 = BK_C_5,
		NOTE_C_SH_5 = BK_C_SH_5,
		NOTE_D_5 = BK_D_5,
		NOTE_D_SH_5 = BK_D_SH_5,
		NOTE_E_5 = BK_E_5,
		NOTE_F_5 = BK_F_5,
		NOTE_F_SH_5 = BK_F_SH_5,
		NOTE_G_5 = BK_G_5,
		NOTE_G_SH_5 = BK_G_SH_5,
		NOTE_A_5 = BK_A_5,
		NOTE_A_SH_5 = BK_A_SH_5,
		NOTE_B_5 = BK_B_5,
		NOTE_C_6 = BK_C_6,
		NOTE_C_SH_6 = BK_C_SH_6,
		NOTE_D_6 = BK_D_6,
		NOTE_D_SH_6 = BK_D_SH_6,
		NOTE_E_6 = BK_E_6,
		NOTE_F_6 = BK_F_6,
		NOTE_F_SH_6 = BK_F_SH_6,
		NOTE_G_6 = BK_G_6,
		NOTE_G_SH_6 = BK_G_SH_6,
		NOTE_A_6 = BK_A_6,
		NOTE_A_SH_6 = BK_A_SH_6,
		NOTE_B_6 = BK_B_6,
		NOTE_C_7 = BK_C_7,
		NOTE_C_SH_7 = BK_C_SH_7,
		NOTE_D_7 = BK_D_7,
		NOTE_D_SH_7 = BK_D_SH_7,
		NOTE_E_7 = BK_E_7,
		NOTE_F_7 = BK_F_7,
		NOTE_F_SH_7 = BK_F_SH_7,
		NOTE_G_7 = BK_G_7,
		NOTE_G_SH_7 = BK_G_SH_7,
		NOTE_A_7 = BK_A_7,
		NOTE_A_SH_7 = BK_A_SH_7,
		NOTE_B_7 = BK_B_7,
		NOTE_C_8 = BK_C_8,
		NOTE_RELEASE = BK_NOTE_RELEASE,
		NOTE_MUTE = BK_NOTE_MUTE,
	};

private:
	struct DividerItem {
		StringName name;
		Divider divider;
	};

	static constexpr float MASTER_VOLUME_DEFAULT = 0.15;
	static constexpr float MASTER_VOLUME_DEFAULT_2 = 0.3;

	BKTrack track;
	Ref<BlipKitInstrument> instrument;
	Ref<BlipKitWaveform> custom_waveform;
	AudioStreamBlipKitPlayback *playback = nullptr;
	LocalVector<int> divider_ids;
	LocalVector<DividerItem> dividers;

	DividerItem *find_divider(const StringName &p_name);
	bool has_divider(const StringName &p_name);

protected:
	static void _bind_methods();
	String _to_string() const;

public:
	BlipKitTrack();
	~BlipKitTrack();

	static Ref<BlipKitTrack> create_with_waveform(Waveform p_waveform);

	Waveform get_waveform() const;
	void set_waveform(Waveform p_waveform);
	int get_duty_cycle() const;
	void set_duty_cycle(int p_duty_cycle);
	float get_master_volume() const;
	void set_master_volume(float p_master_volume);
	float get_volume() const;
	void set_volume(float p_volume);
	float get_panning() const;
	void set_panning(float p_panning);
	float get_note() const;
	void set_note(float p_note);
	float get_pitch() const;
	void set_pitch(float p_pitch);
	int get_phase_wrap() const;
	void set_phase_wrap(int p_phase_wrap);

	int get_volume_slide() const;
	void set_volume_slide(int p_volume_slide);
	int get_panning_slide() const;
	void set_panning_slide(int p_panning_slide);
	int get_portamento() const;
	void set_portamento(int p_portamento);
	Dictionary get_tremolo() const;
	void set_tremolo(int p_ticks, float p_delta, int p_slide_ticks = 0);
	Dictionary get_vibrato() const;
	void set_vibrato(int p_ticks, float p_delta, int p_slide_ticks = 0);
	int get_effect_divider() const;
	void set_effect_divider(int p_effect_divider);

	PackedFloat32Array get_arpeggio() const;
	void set_arpeggio(const PackedFloat32Array &p_arpeggio);
	int get_arpeggio_divider() const;
	void set_arpeggio_divider(int p_arpeggio_divider);

	Ref<BlipKitInstrument> get_instrument();
	void set_instrument(const Ref<BlipKitInstrument> &p_instrument);
	int get_instrument_divider() const;
	void set_instrument_divider(int p_instrument_divider);

	Ref<BlipKitWaveform> get_custom_waveform();
	void set_custom_waveform(const Ref<BlipKitWaveform> &p_waveform);

	void attach(AudioStreamBlipKit *p_stream);
	void detach();

	void release();
	void mute();

	void reset();

	PackedStringArray get_divider_names() const;
	void add_divider(const StringName &p_name, int p_tick_interval, Callable p_callable);
	void remove_divider(const StringName &p_name);
	void reset_divider(const StringName &p_name, int p_tick_interval = 0);
	void clear_dividers();
};

} // namespace detomon::BlipKit

VARIANT_ENUM_CAST(detomon::BlipKit::BlipKitTrack::Waveform);
VARIANT_ENUM_CAST(detomon::BlipKit::BlipKitTrack::Note);
