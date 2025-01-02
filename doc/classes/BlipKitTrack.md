# Class: BlipKitTrack

Inherits: *RefCounted*

**Generates a single waveform.**

## Description

This class generates a single waveform and plays a `note`. Method calls and property changes are thread-safe.

**Note:** When a [`BlipKitTrack`](BlipKitTrack.md) is freed, it is automatically detached from [`AudioStreamBlipKit`](AudioStreamBlipKit.md) and all dividers are removed.

**Example:** Create a [`BlipKitTrack`](BlipKitTrack.md) and attach it to an [`AudioStreamBlipKit`](AudioStreamBlipKit.md):

```gdscript
# Create a track with the default waveform [BlipKitTrack.WAVEFORM_SQUARE].
# Ensure it is playing or has `autoplay` enabled.
var _track := BlipKitTrack.new()

# An audio stream player with an [AudioStreamBlipKit] resource and `autoplay` enabled.
@onready var stream_player: AudioStreamPlayer = $AudioStreamPlayer

func _ready() -> void:
    # Get the audio stream.
    var stream: AudioStreamBlipKit = stream_player.stream

    # Attach the track to the audio stream.
    _track.attach(stream)
    # Play a note.
_track.note = BlipKitTrack.NOTE_A_3
```
## Online Tutorials

- [Power On!](https://github.com/detomon/godot-blipkit/blob/master/examples/power_on/power_on.md)

## Properties

- *PackedFloat32Array* [**`arpeggio`**](#packedfloat32array-arpeggio)
- *int* [**`arpeggio_divider`**](#int-arpeggio_divider)
- *BlipKitWaveform* [**`custom_waveform`**](#blipkitwaveform-custom_waveform)
- *int* [**`duty_cycle`**](#int-duty_cycle)
- *int* [**`effect_divider`**](#int-effect_divider)
- *BlipKitInstrument* [**`instrument`**](#blipkitinstrument-instrument)
- *int* [**`instrument_divider`**](#int-instrument_divider)
- *float* [**`master_volume`**](#float-master_volume)
- *float* [**`note`**](#float-note)
- *float* [**`panning`**](#float-panning)
- *int* [**`panning_slide`**](#int-panning_slide)
- *int* [**`phase_wrap`**](#int-phase_wrap)
- *float* [**`pitch`**](#float-pitch)
- *int* [**`portamento`**](#int-portamento)
- *float* [**`volume`**](#float-volume)
- *int* [**`volume_slide`**](#int-volume_slide)
- *int* [**`waveform`**](#int-waveform)

## Methods

- *void* [**`add_divider`**](#void-add_dividername-string-tick_interval-int-callable-callable)(name: String, tick_interval: int, callable: Callable)
- *void* [**`attach`**](#void-attachplayback-audiostreamblipkit)(playback: AudioStreamBlipKit)
- *void* [**`clear_dividers`**](#void-clear_dividers)()
- *BlipKitTrack* [**`create_with_waveform`**](#blipkittrack-create_with_waveformwaveform-int-static)(waveform: int) static
- *void* [**`detach`**](#void-detach)()
- *PackedStringArray* [**`get_divider_names`**](#packedstringarray-get_divider_names-const)() const
- *Dictionary* [**`get_tremolo`**](#dictionary-get_tremolo-const)() const
- *Dictionary* [**`get_vibrato`**](#dictionary-get_vibrato-const)() const
- *void* [**`mute`**](#void-mute)()
- *void* [**`release`**](#void-release)()
- *void* [**`remove_divider`**](#void-remove_dividername-string)(name: String)
- *void* [**`reset`**](#void-reset)()
- *void* [**`reset_divider`**](#void-reset_dividername-string-tick_interval-int--0)(name: String, tick_interval: int = 0)
- *void* [**`set_tremolo`**](#void-set_tremoloticks-int-delta-float-slide_ticks-int--0)(ticks: int, delta: float, slide_ticks: int = 0)
- *void* [**`set_vibrato`**](#void-set_vibratoticks-int-delta-float-slide_ticks-int--0)(ticks: int, delta: float, slide_ticks: int = 0)

## Enumerations

### enum `Waveform`

- `WAVEFORM_SQUARE` = `0`
	- A square wave with 16 phases. The duty cycle (number of high amplitudes) can be set with `duty_cycle`. This waveform has a default `master_volume` of `0.15`.
- `WAVEFORM_TRIANGLE` = `1`
	- A triangle wave with 32 phases. This waveform has a default `master_volume` of `0.3`.
- `WAVEFORM_NOISE` = `2`
	- Generates noise using a 16-bit random generator. Looks like a square wave but with random low or high amplitudes. This waveform has a default `master_volume` of `0.15`.
- `WAVEFORM_SAWTOOTH` = `3`
	- A sawtooth wave with 7 phases. This waveform has a default `master_volume` of `0.15`.
- `WAVEFORM_SINE` = `4`
	- A sine wave with 32 phases. This waveform has a default `master_volume` of `0.3`.
- `WAVEFORM_CUSTOM` = `5`
	- Set when a `custom_waveform` is set. Cannot be set directly.

### enum `Note`

- `NOTE_C_0` = `0`
	- Note C on octave `0`.
- `NOTE_C_SH_0` = `1`
	- Note C# on octave `0`.
- `NOTE_D_0` = `2`
	- Note D on octave `0`.
- `NOTE_D_SH_0` = `3`
	- Note D# on octave `0`.
- `NOTE_E_0` = `4`
	- Note E on octave `0`.
- `NOTE_F_0` = `5`
	- Note F on octave `0`.
- `NOTE_F_SH_0` = `6`
	- Note F# on octave `0`.
- `NOTE_G_0` = `7`
	- Note G# on octave `0`.
- `NOTE_G_SH_0` = `8`
	- Note G on octave `0`.
- `NOTE_A_0` = `9`
	- Note A on octave `0`.
- `NOTE_A_SH_0` = `10`
	- Note A# on octave `0`.
- `NOTE_B_0` = `11`
	- Note B on octave `0`.
- `NOTE_C_1` = `12`
	- Note C on octave `1`.
- `NOTE_C_SH_1` = `13`
	- Note C# on octave `1`.
- `NOTE_D_1` = `14`
	- Note D on octave `1`.
- `NOTE_D_SH_1` = `15`
	- Note D# on octave `1`.
- `NOTE_E_1` = `16`
	- Note E on octave `1`.
- `NOTE_F_1` = `17`
	- Note F on octave `1`.
- `NOTE_F_SH_1` = `18`
	- Note F# on octave `1`.
- `NOTE_G_1` = `19`
	- Note G on octave `1`.
- `NOTE_G_SH_1` = `20`
	- Note G# on octave `1`.
- `NOTE_A_1` = `21`
	- Note A on octave `1`.
- `NOTE_A_SH_1` = `22`
	- Note A# on octave `1`.
- `NOTE_B_1` = `23`
	- Note B on octave `1`.
- `NOTE_C_2` = `24`
	- Note C on octave `2`.
- `NOTE_C_SH_2` = `25`
	- Note C on octave `2`.
- `NOTE_D_2` = `26`
	- Note D on octave `2`.
- `NOTE_D_SH_2` = `27`
	- Note D# on octave `2`.
- `NOTE_E_2` = `28`
	- Note E on octave `2`.
- `NOTE_F_2` = `29`
	- Note F on octave `2`.
- `NOTE_F_SH_2` = `30`
	- Note F# on octave `2`.
- `NOTE_G_2` = `31`
	- Note G on octave `2`.
- `NOTE_G_SH_2` = `32`
	- Note G# on octave `2`.
- `NOTE_A_2` = `33`
	- Note A on octave `2`.
- `NOTE_A_SH_2` = `34`
	- Note A# on octave `2`.
- `NOTE_B_2` = `35`
	- Note B on octave `2`.
- `NOTE_C_3` = `36`
	- Note C on octave `3`.
- `NOTE_C_SH_3` = `37`
	- Note C on octave `3`.
- `NOTE_D_3` = `38`
	- Note C# on octave `3`.
- `NOTE_D_SH_3` = `39`
	- Note D# on octave `3`.
- `NOTE_E_3` = `40`
	- Note E on octave `3`.
- `NOTE_F_3` = `41`
	- Note F on octave `3`.
- `NOTE_F_SH_3` = `42`
	- Note F# on octave `3`.
- `NOTE_G_3` = `43`
	- Note G on octave `3`.
- `NOTE_G_SH_3` = `44`
	- Note G# on octave `3`.
- `NOTE_A_3` = `45`
	- Note A on octave `3`.
- `NOTE_A_SH_3` = `46`
	- Note A# on octave `3`.
- `NOTE_B_3` = `47`
	- Note B on octave `3`.
- `NOTE_C_4` = `48`
	- Note C on octave `4`.
- `NOTE_C_SH_4` = `49`
	- Note C# on octave `4`.
- `NOTE_D_4` = `50`
	- Note D on octave `4`.
- `NOTE_D_SH_4` = `51`
	- Note D# on octave `4`.
- `NOTE_E_4` = `52`
	- Note E on octave `4`.
- `NOTE_F_4` = `53`
	- Note F on octave `4`.
- `NOTE_F_SH_4` = `54`
	- Note F# on octave `4`.
- `NOTE_G_4` = `55`
	- Note G on octave `4`.
- `NOTE_G_SH_4` = `56`
	- Note G# on octave `4`.
- `NOTE_A_4` = `57`
	- Note A on octave `4`.
- `NOTE_A_SH_4` = `58`
	- Note A# on octave `4`.
- `NOTE_B_4` = `59`
	- Note B on octave `4`.
- `NOTE_C_5` = `60`
	- Note C on octave `5`.
- `NOTE_C_SH_5` = `61`
	- Note C# on octave `5`.
- `NOTE_D_5` = `62`
	- Note D on octave `5`.
- `NOTE_D_SH_5` = `63`
	- Note D# on octave `5`.
- `NOTE_E_5` = `64`
	- Note E on octave `5`.
- `NOTE_F_5` = `65`
	- Note F on octave `5`.
- `NOTE_F_SH_5` = `66`
	- Note F# on octave `5`.
- `NOTE_G_5` = `67`
	- Note G on octave `5`.
- `NOTE_G_SH_5` = `68`
	- Note G# on octave `5`.
- `NOTE_A_5` = `69`
	- Note A on octave `5`.
- `NOTE_A_SH_5` = `70`
	- Note A# on octave `5`.
- `NOTE_B_5` = `71`
	- Note B on octave `5`.
- `NOTE_C_6` = `72`
	- Note C on octave `6`.
- `NOTE_C_SH_6` = `73`
	- Note C# on octave `6`.
- `NOTE_D_6` = `74`
	- Note D on octave `6`.
- `NOTE_D_SH_6` = `75`
	- Note D# on octave `6`.
- `NOTE_E_6` = `76`
	- Note E on octave `6`.
- `NOTE_F_6` = `77`
	- Note F on octave `6`.
- `NOTE_F_SH_6` = `78`
	- Note F# on octave `6`.
- `NOTE_G_6` = `79`
	- Note G on octave `6`.
- `NOTE_G_SH_6` = `80`
	- Note G# on octave `6`.
- `NOTE_A_6` = `81`
	- Note A on octave `6`.
- `NOTE_A_SH_6` = `82`
	- Note A# on octave `6`.
- `NOTE_B_6` = `83`
	- Note B on octave `6`.
- `NOTE_C_7` = `84`
	- Note C on octave `7`.
- `NOTE_C_SH_7` = `85`
	- Note C# on octave `7`.
- `NOTE_D_7` = `86`
	- Note D on octave `7`.
- `NOTE_D_SH_7` = `87`
	- Note D# on octave `7`.
- `NOTE_E_7` = `88`
	- Note E on octave `7`.
- `NOTE_F_7` = `89`
	- Note F on octave `7`.
- `NOTE_F_SH_7` = `90`
	- Note F# on octave `7`.
- `NOTE_G_7` = `91`
	- Note G on octave `7`.
- `NOTE_G_SH_7` = `92`
	- Note G on octave `7`.
- `NOTE_A_7` = `93`
	- Note A on octave `7`.
- `NOTE_A_SH_7` = `94`
	- Note A# on octave `7`.
- `NOTE_B_7` = `95`
	- Note B on octave `7`.
- `NOTE_C_8` = `96`
	- Note C on octave `8`.
- `NOTE_RELEASE` = `-1`
	- Releases `note`. Has the same effect as calling [`release()`](#void-release).
- `NOTE_MUTE` = `-2`
	- Mutes `note` immediately. Has the same effect as calling [`mute()`](#void-mute).

## Property Descriptions

### `PackedFloat32Array arpeggio`

*Default*: `PackedFloat32Array()`

Sets the arpeggio sequence consisting of an array of pitch changes relative to the currently playing `note`. The array can have a maximum size of `8` and is truncated if it is larger.

The duration for which each arpeggio note is defined with `arpeggio_divider`.

**Example:** Play a major chord:

```gdscript
track.arpeggio = [0.0, 4.0, 7.0]
```
### `int arpeggio_divider`

*Default*: `4`

Sets the number of *ticks* each `arpeggio` note is played.

### `BlipKitWaveform custom_waveform`

Sets a custom waveform. If set, `waveform` returns [`WAVEFORM_CUSTOM`](#waveform_custom). If set to `null` `waveform` is reset to [`WAVEFORM_SQUARE`](#waveform_square).

### `int duty_cycle`

*Default*: `4`

Sets the duty cycle when `waveform` is set to [`WAVEFORM_SQUARE`](#waveform_square). This defines the number of high amplitudes and is a value between `1` and `15`. Has no effect on other waveforms.

### `int effect_divider`

*Default*: `1`

Sets the number of *ticks* in which effects change the value of the corresponding property.

### `BlipKitInstrument instrument`

Sets the instrument when playing a note.

### `int instrument_divider`

*Default*: `4`

Sets the number of *ticks* each instrument envelope value is played when no steps are defined.

### `float master_volume`

*Default*: `0.149998`

Sets the mix volume. This is multiplied with `volume` to be used as the output volume.

**Note:** This is also changed when setting the waveform (see [`Waveform`](#enum-waveform) for the default values).

### `float note`

*Default*: `-1.0`

Sets the note to play between `0.0` (note C on octave `0`) and `96.0` (note C on octave `8`) (see [`Note`](#enum-note)).

Setting [`NOTE_RELEASE`](#note_release) releases the note (see also [`release()`](#void-release)). Setting [`NOTE_MUTE`](#note_mute) mutes the note (see also [`mute()`](#void-mute)).

If `instrument` is set, setting a note plays the attack and sustain part of the envelopes, whereas releasing the note plays the release part of the envelopes.

### `float panning`

*Default*: `0.0`

Sets the stereo panning between `-1.0` and `+1.0`. Negative values pan to the left channel and positive values to the right channel.

### `int panning_slide`

*Default*: `0`

Sets the number of *ticks* in which `panning` changes to a new value.

### `int phase_wrap`

*Default*: `0`

Sets the number of wave phases after which the waveform is reset to the beginning. This has only an effect on [`WAVEFORM_NOISE`](#waveform_noise) or when `custom_waveform` is set.

If `phase_wrap` is `0`, this behaviour is disabled.

**Example:** Limit noise to the first 32 phases:

```gdscript
var track := BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_NOISE)
track.phase_wrap = 32
```
**Note:** Non-power-of-two values change the pitch.

### `float pitch`

*Default*: `0.0`

Sets the pitch values which is added to `note`.

**Example:** Play all notes one octave lower:

```gdscript
track.pitch = -12.0
```
### `int portamento`

*Default*: `0`

Sets the number of *ticks* in which `note` changes to a new value.

### `float volume`

*Default*: `1.0`

Sets the note volume. This is multiplied with `master_volume` to be used as the output volume.

### `int volume_slide`

*Default*: `0`

Sets the number of *ticks* in which `volume` changes to a new value.

### `int waveform`

*Default*: `0`

Sets the waveform and its `master_volume` accordingly (see [`Waveform`](#enum-waveform)). The default is [`WAVEFORM_SQUARE`](#waveform_square).


## Method Descriptions

### `void add_divider(name: String, tick_interval: int, callable: Callable)`

Adds a divider with `name` which is called every multiple number of *ticks*. For callbacks to be called, [`BlipKitTrack`](BlipKitTrack.md) has to be attached to an [`AudioStreamBlipKit`](AudioStreamBlipKit.md) (see [`attach()`](#void-attachplayback-audiostreamblipkit)). Callbacks are called in the same order as they are added.

`callable` does not receive any arguments and should return an `int` indicating whether to change the tick interval. If `callback` returns `0`, the tick interval is not changed and `callable` is called again after the same number of ticks. If `callback` returns a value greater than `0`, the tick interval is permanently changed and `callable` is called next after the returned number of ticks.

**Note:** Callbacks are called from the audio thread, they should run as fast as possible to prevent distorted audio. Consider using `Object.call_deferred()` for functions which are expensive to run or not time critical.

**Note:** `callback` is called for the first time on the next tick.

**Example:** Add a divider and play a higher note on each call:

```gdscript
track.add_divider("run", 60, func () -> int:
    track.note += 1.0
    # Keep tick interval.
    return 0
)
```
**Example:** Add a divider and change the tick interval to `90` after the first call:

```gdscript
track.add_divider("run", 180, func () -> int:
    track.note += 1.0
    # Change tick interval to 90.
    return 90
)
```
### `void attach(playback: AudioStreamBlipKit)`

Attaches the track to an [`AudioStreamBlipKit`](AudioStreamBlipKit.md) and resumes all dividers from their last state.

### `void clear_dividers()`

Removes all divider callbacks.

### `BlipKitTrack create_with_waveform(waveform: int) static`

Creates a [`BlipKitTrack`](BlipKitTrack.md) with the given `waveform` and sets `master_volume` according to [`Waveform`](#enum-waveform).

### `void detach()`

Detaches the track from its [`AudioStreamBlipKit`](AudioStreamBlipKit.md) and pauses all dividers.

### `PackedStringArray get_divider_names() const`

Returns the names of all added dividers.

### `Dictionary get_tremolo() const`

Returns the tremolo values as [`Dictionary`](https://docs.godotengine.org/en/stable/classes/class_dictionary.html). Contains the keys `ticks`, `delta`, and `slide_ticks`.

```gdscript
{ ticks = 0, delta = 0.0, slide_ticks = 0 }
```
### `Dictionary get_vibrato() const`

Returns the vibrato values as [`Dictionary`](https://docs.godotengine.org/en/stable/classes/class_dictionary.html). Contains the keys `ticks`, `delta`, and `slide_ticks`.

```gdscript
{ ticks = 0, delta = 0.0, slide_ticks = 0 }
```
### `void mute()`

Mutes `note` immediately without playing the release part of `instrument` envelopes. Has the same effect as setting `note` to [`NOTE_MUTE`](#note_mute).

### `void release()`

Releases `note`. When `instrument` is set, plays the release part of `instrument` envelopes. Has the same effect as setting `note` to [`NOTE_RELEASE`](#note_release).

### `void remove_divider(name: String)`

Removes the divider with `name`.

### `void reset()`

Resets the track properties to the initial values (except `waveform` and `master_volume`) and mutes `note`.

### `void reset_divider(name: String, tick_interval: int = 0)`

Resets the counter of a divider with `name`. If `tick_interval` is greater than `0`, the tick interval is changed to that value.

**Note:** The divider callback is called the first time on the next tick.

### `void set_tremolo(ticks: int, delta: float, slide_ticks: int = 0)`

Enables the tremolo effect. This decreases `volume` periodically by `delta` within the given number of `ticks`.

If `slide_ticks` is greater than `0`, the current `ticks` and `delta` values are interpolated to the values within the given number of *ticks* .

If `ticks` is `0`, the effect is disabled.

### `void set_vibrato(ticks: int, delta: float, slide_ticks: int = 0)`

Enables the vibrato effect. This increases and decreases `note` periodically by `delta` within the given number of `ticks`.

If `slide_ticks` is greater than `0`, the current `ticks` and `delta` values are interpolated to the values within the given number of *ticks* .

If `ticks` is `0`, the effect is disabled.


