# Class: BlipKitSample

Inherits: *Resource*

**Contains audio frames.**

## Description

```gdscript
# Load WAV from resource.
var wav := preload("res://sample.wav")
var sample := BlipKitSample.create_from_wav(wav)
# Set sample to track.
track.sample = sample
# Play sample.
track.note = BlipKitTrack.NOTE_C_4
```
## Properties

- *int* [**`repeat_mode`**](#int-repeat_mode) `[default: 0]`
- *int* [**`sustain_end`**](#int-sustain_end) `[default: 0]`
- *int* [**`sustain_offset`**](#int-sustain_offset) `[default: 0]`

## Methods

- *BlipKitSample* [**`create_with_wav`**](#blipkitsample-create_with_wavwav-audiostreamwav-normalize-bool--false-amplitude-float--10-static)(wav: AudioStreamWAV, normalize: bool = false, amplitude: float = 1.0) static
- *PackedFloat32Array* [**`get_frames`**](#packedfloat32array-get_frames-const)() const
- *bool* [**`is_valid`**](#bool-is_valid-const)() const
- *void* [**`set_frames`**](#void-set_framesframes-packedfloat32array-normalize-bool--false-amplitude-float--10)(frames: PackedFloat32Array, normalize: bool = false, amplitude: float = 1.0)
- *int* [**`size`**](#int-size-const)() const

## Enumerations

### enum `RepeatMode`

- `REPEAT_NONE` = `0`
	- Does not repeat the sample.
- `REPEAT_FORWARD` = `1`
	- Repeats the sample forward.
- `REPEAT_PING_PONG` = `2`
	- Repeats the sample forward and backward. Changes the direction when it reaches the end or the beginning.
- `REPEAT_BACKWARD` = `3`
	- Repeats the sample backward from the end to the beginning.

## Property Descriptions

### `int repeat_mode`

*Default*: `0`

Sets the sample repeat mode.

### `int sustain_end`

*Default*: `0`

Sets the end of the sustain range in samples.

### `int sustain_offset`

*Default*: `0`

Sets the beginning of the sustain range in samples.


## Method Descriptions

### `BlipKitSample create_with_wav(wav: AudioStreamWAV, normalize: bool = false, amplitude: float = 1.0) static`

Creates a [`BlipKitSample`](BlipKitSample.md) from a [`AudioStreamWAV`](https://docs.godotengine.org/en/stable/classes/class_audiostreamwav.html). Stereo channels are merged into a single mono channel.

If `normalize` is `true`, frames values are normalized between negative and positive `amplitude`. `amplitude` is clamped between `0.0` and `1.0`.

The sample rate of [`AudioStreamWAV`](https://docs.godotengine.org/en/stable/classes/class_audiostreamwav.html) is expected to be 44100 Hz, as the internal sample rate of [`AudioStreamBlipKit`](AudioStreamBlipKit.md) is fixed to this value. When playing the sample with `BlipKitTrack.NOTE_C_4`, it is played with it's original speed.

Copies `AudioStreamWAV.loop_mode`, `AudioStreamWAV.loop_begin`, and `AudioStreamWAV.loop_end` into `repeat_mode`, `sustain_offset`, and `sustain_end`, respectively.

**Note:** Only supports the formats `AudioStreamWAV.FORMAT_8_BITS` and `AudioStreamWAV.FORMAT_16_BITS`.

### `PackedFloat32Array get_frames() const`

Returns the sample frames as values between `-1.0` and `+1.0`.

### `bool is_valid() const`

Returns `true` if the sample was initialized with frames.

### `void set_frames(frames: PackedFloat32Array, normalize: bool = false, amplitude: float = 1.0)`

Sets the sample frames. If `normalize` is `false`, values in `frames` are clamped between `-1.0` and `+1.0`. If `normalize` is `true`, values in `frames` are normalized between negative and positive `amplitude`. `amplitude` is clamped between `0.0` and `1.0`.

**Note:** Only supports a mono channel.

### `int size() const`

Returns the number of frames in the sample.


