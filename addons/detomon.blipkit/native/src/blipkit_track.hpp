#pragma once

#include <BlipKit.h>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/core/gdvirtual.gen.inc>
#include <godot_cpp/variant/packed_float32_array.hpp>

#include "blipkit_instrument.hpp"
#include "blipkit_waveform.hpp"

using namespace godot;

class AudioStreamBlipKitPlayback;

namespace detomon::BlipKit {

class BlipKitTrack : public RefCounted {
	GDCLASS(BlipKitTrack, RefCounted)

private:
	const real_t DEFAULT_MASTER_VOLUME = 0.125;

	BKTrack track;
	Ref<BlipKitInstrument> instrument;
	Ref<BlipKitWaveform> custom_waveform;

	int tick(int p_divider_index, int p_ticks);

protected:
	static void _bind_methods();
	String _to_string() const;

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

	BlipKitTrack();
	~BlipKitTrack();

	BlipKitTrack::Waveform get_waveform() const;
	void set_waveform(BlipKitTrack::Waveform p_waveform);

	int get_duty_cycle() const;
	void set_duty_cycle(int p_duty_cycle);

	real_t get_master_volume() const;
	void set_master_volume(real_t p_master_volume);

	real_t get_volume() const;
	void set_volume(real_t p_volume);

	real_t get_panning() const;
	void set_panning(real_t p_panning);

	real_t get_note() const;
	void set_note(real_t p_note);

	real_t get_pitch() const;
	void set_pitch(real_t p_pitch);

	int get_volume_slide() const;
	void set_volume_slide(int p_volume_slide);
	int get_panning_slide() const;
	void set_panning_slide(int p_panning_slide);
	int get_portamento() const;
	void set_portamento(int p_portamento);
	void set_tremolo(int p_ticks, float p_delta, int p_slide_ticks = 0);
	void set_vibrato(int p_ticks, float p_delta, int p_slide_ticks = 0);
	int get_effect_divider() const;
	void set_effect_divider(int p_effect_divider);

	PackedFloat32Array get_arpeggio() const;
	void set_arpeggio(const PackedFloat32Array &p_arpeggio);
	int get_arpeggio_divider() const;
	void set_arpeggio_divider(int p_arpeggio_divider);

	Ref<BlipKitInstrument> get_instrument();
	void set_instrument(Ref<BlipKitInstrument> p_instrument);
	int get_instrument_divider() const;
	void set_instrument_divider(int p_instrument_divider);

	Ref<BlipKitWaveform> get_custom_waveform();
	void set_custom_waveform(Ref<BlipKitWaveform> p_waveform);

	void attach(AudioStreamBlipKitPlayback *p_playback);
	void detach();

	// This is the same as setting set_note(BK_NOTE_RELEASE)
	void release();
	// This is the same as setting set_note(BK_NOTE_MUTE)
	void mute();

	void reset();

	int add_divider(int p_ticks);
	void remove_divider(int p_divider_index);

	GDVIRTUAL2R(int, _tick, int, int);
};

} // namespace detomon::BlipKit

VARIANT_ENUM_CAST(detomon::BlipKit::BlipKitTrack::Waveform);
VARIANT_ENUM_CAST(detomon::BlipKit::BlipKitTrack::Note);
