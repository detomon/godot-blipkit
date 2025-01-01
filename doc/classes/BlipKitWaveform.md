# Class: BlipKitWaveform

Inherits: *Resource*

**Defines a waveform consisting of amplitude values.**

## Description

```gdscript
var aah := BlipKitWaveform.new()
aah.set_frames_normalized([
    -255, -163, -154, -100, 45, 127, 9, -163,
    -163, -27, 63, 72, 63, 9, -100, -154,
    -127, -91, -91, -91, -91, -127, -154, -100,
    45, 127, 9, -163, -163, 9, 127, 45,
])
```
## Methods

- *BlipKitWaveform* [**`create_with_frames`**](#blipkitwaveform-create_with_framesframes-packedfloat32array-normalize-bool--false-amplitude-float--10-static)(frames: PackedFloat32Array, normalize: bool = false, amplitude: float = 1.0) static
- *PackedFloat32Array* [**`get_frames`**](#packedfloat32array-get_frames-const)() const
- *bool* [**`is_valid`**](#bool-is_valid-const)() const
- *void* [**`set_frames`**](#void-set_framesframes-packedfloat32array-normalize-bool--false-amplitude-float--10)(frames: PackedFloat32Array, normalize: bool = false, amplitude: float = 1.0)
- *int* [**`size`**](#int-size-const)() const

## Method Descriptions

### `BlipKitWaveform create_with_frames(frames: PackedFloat32Array, normalize: bool = false, amplitude: float = 1.0) static`

Creates a waveform and set its amplitudes to the given `frames`. If `normalize` is `false`, values of `frames` are clamped between `-1.0` and `+1.0`. If `normalize` is `true`, values of `frames` are normalized between negative and positive `amplitude`. `amplitude` is clamped between `0.0` and `1.0`.

**Note:** The number of frames must be between `2` and `64`.

### `PackedFloat32Array get_frames() const`

Get the waveform amplitudes as values between `-1.0` and `+1.0`.

### `bool is_valid() const`

Returns `true` if the waveform was initialized with frames.

### `void set_frames(frames: PackedFloat32Array, normalize: bool = false, amplitude: float = 1.0)`

Set the waveform amplitudes. If `normalize` is `false`, values of `frames` are clamped between `-1.0` and `+1.0`. If `normalize` is `true`, values of `frames` are normalized between negative and positive `amplitude`. `amplitude` is clamped between `0.0` and `1.0`.

**Note:** The number of frames must be between `2` and `64`.

### `int size() const`

Returns the number of frames in the waveform.


