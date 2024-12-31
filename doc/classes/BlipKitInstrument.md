# Class: BlipKitInstrument

Inherits: *Resource*

**Changes parameters of a [`BlipKitTrack`](BlipKitTrack.md) while a note is playing or after it is released.**

## Description

```gdscript
var instrument := BlipKitInstrument.new()
instrument.set_adsr(4, 16, 0.75, 36)
instrument.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [], [12, 0], 1, 1)

var track := BlipKitTrack.new()
track.instrument = instrument
track.note = BlipKitTrack.NOTE_A_2
```
## Methods

- *void* [**`clear_envelope`**](#void-clear_envelopetype-int)(type: int)
- *PackedInt32Array* [**`get_envelope_steps`**](#packedint32array-get_envelope_stepstype-int-const)(type: int) const
- *int* [**`get_envelope_sustain_length`**](#int-get_envelope_sustain_lengthtype-int-const)(type: int) const
- *int* [**`get_envelope_sustain_offset`**](#int-get_envelope_sustain_offsettype-int-const)(type: int) const
- *PackedFloat32Array* [**`get_envelope_values`**](#packedfloat32array-get_envelope_valuestype-int-const)(type: int) const
- *bool* [**`has_envelope`**](#bool-has_envelopetype-int-const)(type: int) const
- *void* [**`set_adsr`**](#void-set_adsrattack-int-decay-int-sustain-float-release-int)(attack: int, decay: int, sustain: float, release: int)
- *void* [**`set_envelope`**](#void-set_envelopetype-int-steps-packedint32array-values-packedfloat32array-sustain_offset-int-sustain_length-int)(type: int, steps: PackedInt32Array, values: PackedFloat32Array, sustain_offset: int, sustain_length: int)

## Enumerations

### enum `EnvelopeType`

- `ENVELOPE_VOLUME` = `0`
	- Changes the output value of `BlipKitTrack.volume` by multiplying it with values from the envelope.
- `ENVELOPE_PANNING` = `1`
	- Changes the output value of `BlipKitTrack.panning` by adding values from the envelope.
- `ENVELOPE_PITCH` = `2`
	- Changes the output value of `BlipKitTrack.note` by adding values from the envelope.
- `ENVELOPE_DUTY_CYCLE` = `3`
	- Changes the output value of `BlipKitTrack.duty_cycle` by overriding it with values from the envelope.

If an envelope value is `0`, `BlipKitTrack.duty_cycle` is not changed.

**Note:** Values are cast to integers and clamped between `0` and `15`.

## Method Descriptions

### `void clear_envelope(type: int)`

Removes the envelope with `type`.

### `PackedInt32Array get_envelope_steps(type: int) const`

Returns the steps for the envelope with `type`.

Returns an empty array if the envelope is not set or has no steps.

### `int get_envelope_sustain_length(type: int) const`

Returns the length of the sustain cycle.

Returns `0` if the envelope is not set.

### `int get_envelope_sustain_offset(type: int) const`

Returns the offset of the sustain cycle.

Returns `0` if the envelope is not set.

### `PackedFloat32Array get_envelope_values(type: int) const`

Returns the values for the envelope with `type`.

Returns an empty array if the envelope is not set.

### `bool has_envelope(type: int) const`

Returns `true` if the envelope with `type` was set.

### `void set_adsr(attack: int, decay: int, sustain: float, release: int)`

Initializes the [`ENVELOPE_VOLUME`](#envelope_volume) envelope with an ADSR envelope. This first raises the volume to `1.0` within `attack` *ticks*, then lowers it to `sustain` within `decay` *ticks*. This value is kept until the note is released, after which the volume is lowered to `0.0` within `release` *ticks*.

This corresponds to the following call:

```gdscript
set_envelope(BlipKitInstrument.ENVELOPE_VOLUME,
    [attack, decay, 240, release],
    [1.0, sustain, sustain, 0.0],
    2, 1)
```
### `void set_envelope(type: int, steps: PackedInt32Array, values: PackedFloat32Array, sustain_offset: int, sustain_length: int)`

Sets the envelope with `type`. `values` define the individual values for each phase.

If `steps` is not empty, it has to have the same size as `values`. In this case, `values` are linearly interpolated. The duration of each phase is defined by the value at the corresponding index in `steps` as number of *ticks*. Values in `steps` are allowed to be `0` to instantly change a value.

If `steps` is empty, `values` is used as a sequence and values are changed instantly. In this case, the duration of each phase is defined by `BlipKitTrack.instrument_divider`.

The parameters `sustain_offset` and `sustain_length` define the range of the sustain cycle which is repeated while the note is playing.


