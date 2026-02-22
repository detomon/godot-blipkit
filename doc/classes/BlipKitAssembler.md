# Class: BlipKitAssembler

Inherits: *RefCounted*

**Generates byte code from instructions.**

## Description

Generates byte code from instructions which can be executed with [`BlipKitInterpreter`](BlipKitInterpreter.md).

Named labels can be used to create function calls and loops.

**Example:** Add instructions to play an iconic startup sound:

```gdscript
var assem := BlipKitAssembler.new()

# Initialize track.
assem.put(BlipKitAssembler.OP_WAVEFORM, BlipKitTrack.WAVEFORM_SQUARE)
assem.put(BlipKitAssembler.OP_DUTY_CYCLE, 8)

# Define a label "start" to loop back.
assem.put_label("start")

# Play notes.
assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_C_5)
assem.put(BlipKitAssembler.OP_STEP, 1)
assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_C_6)
assem.put(BlipKitAssembler.OP_VOLUME_SLIDE, 9)
assem.put(BlipKitAssembler.OP_VOLUME, 0.0)
assem.put(BlipKitAssembler.OP_STEP, 9)
assem.put(BlipKitAssembler.OP_RELEASE)
assem.put(BlipKitAssembler.OP_VOLUME_SLIDE, 0)
assem.put(BlipKitAssembler.OP_VOLUME, 1.0)
assem.put(BlipKitAssembler.OP_STEP, 10)

# Jump back to label "start".
assem.put(BlipKitAssembler.OP_JUMP, "start")

# Compile byte code and check for errors.
if assem.compile() != BlipKitAssembler.OK:
    printerr(assem.get_error_message())
    return

# Get the byte code.
var bytes := assem.get_byte_code()
```
## Methods

- *void* [**`clear`**](#void-clear)()
- *int* [**`compile`**](#int-compile)()
- *BlipKitBytecode* [**`get_byte_code`**](#blipkitbytecode-get_byte_code)()
- *String* [**`get_error_message`**](#string-get_error_message-const)() const
- *int* [**`put`**](#int-putopcode-int-arg1-variant--null-arg2-variant--null-arg3-variant--null)(opcode: int, arg1: Variant = null, arg2: Variant = null, arg3: Variant = null)
- *int* [**`put_byte_code`**](#int-put_byte_codebyte_code-blipkitbytecode-public-bool--false)(byte_code: BlipKitBytecode, public: bool = false)
- *int* [**`put_label`**](#int-put_labellabel-string-public-bool--false)(label: String, public: bool = false)

## Enumerations

### enum `Opcode`

- `OP_ATTACK` = `2`
	- Sets `BlipKitTrack.note`. Expects a `float` argument.
- `OP_RELEASE` = `3`
	- Calls `BlipKitTrack.release()`. Expects no arguments.
- `OP_MUTE` = `4`
	- Calls `BlipKitTrack.mute()`. Expects no arguments.
- `OP_VOLUME` = `5`
	- Sets `BlipKitTrack.volume`. Expects a `float` argument between `0.0` and `1.0`.
- `OP_MASTER_VOLUME` = `6`
	- Sets `BlipKitTrack.master_volume`. Expects a `float` argument between `0.0` and `1.0`.
- `OP_PANNING` = `7`
	- Sets `BlipKitTrack.panning`. Expects a `float` argument between `-1.0` and `+1.0`.
- `OP_WAVEFORM` = `8`
	- Sets `BlipKitTrack.waveform`. Expects [`BlipKitTrack.Waveform`](#enum-blipkittrackwaveform) as `int` argument.
- `OP_DUTY_CYCLE` = `9`
	- Sets `BlipKitTrack.duty_cycle`. Expects an `int` argument between `0` and `15`.
- `OP_PITCH` = `10`
	- Sets `BlipKitTrack.pitch`. Expects a `float` argument.
- `OP_PHASE_WRAP` = `11`
	- Sets `BlipKitTrack.phase_wrap`. Expects an `int` argument.
- `OP_PORTAMENTO` = `12`
	- Sets `BlipKitTrack.portamento`. Expects an `int` argument between `0` and `65535`.
- `OP_VIBRATO` = `13`
	- Calls `BlipKitTrack.set_vibrato()`. Expects `ticks` (`int` between `0` and `65535`), `delta` (`float`) and `slide_ticks` (`int` between `0` and `65535`).
- `OP_TREMOLO` = `14`
	- Calls `BlipKitTrack.set_tremolo()`. Expects `ticks` (`int` between `0` and `65535`), `delta` (`float`) and `slide_ticks` (`int` between `0` and `65535`).
- `OP_VOLUME_SLIDE` = `15`
	- Sets `BlipKitTrack.volume_slide`. Expects an `int` argument between `0` and `65535`.
- `OP_PANNING_SLIDE` = `16`
	- Sets `BlipKitTrack.panning_slide`. Expects an `int` argument between `0` and `65535`.
- `OP_EFFECT_DIV` = `17`
	- Sets `BlipKitTrack.effect_divider`. Expects an `int` argument between `0` and `65535`.
- `OP_ARPEGGIO` = `18`
	- Sets `BlipKitTrack.arpeggio`. Expects a [`PackedFloat32Array`](https://docs.godotengine.org/en/stable/classes/class_packedfloat32array.html) argument.
- `OP_ARPEGGIO_DIV` = `19`
	- Sets `BlipKitTrack.arpeggio_divider`. Expects an `int` argument between `0` and `65535`.
- `OP_TICK` = `20`
	- Interrupts the execution for a number of *ticks*. Expects an `int` argument between `0` and `65535`.
- `OP_STEP` = `21`
	- Interrupts the execution for a number of *ticks* multiplied with `BlipKitInterpreter.step_ticks`. Expects an `int` argument between `0` and `65535`.
- `OP_STEP_TICKS` = `22`
	- Sets `BlipKitInterpreter.step_ticks`. Expects an `int` argument between `1` and `65535`.
- `OP_DELAY_TICK` = `23`
	- Delays the execution of the following instructions by the given number of *ticks*. Expects an `int` argument between `0` and `65535`.

Must be followed by a [`OP_STEP`](#op_step) or [`OP_TICK`](#op_tick) instruction eventually. A maximum of `8` delay [`OP_DELAY_TICK`](#op_delay_tick) or [`OP_DELAY_STEP`](#op_delay_step) instruction sequences can be chained together.

**Note:** This instruction does not interrupt the execution.
- `OP_DELAY_STEP` = `24`
	- Delays the execution of the following instructions by the given number of *steps* (can be a fraction). Expects a `float` argument between `0.0` and `65535.0`.

Must be followed by a [`OP_STEP`](#op_step) or [`OP_TICK`](#op_tick) instruction eventually. A maximum of `8` delay [`OP_DELAY_TICK`](#op_delay_tick) or [`OP_DELAY_STEP`](#op_delay_step) instruction sequences can be chained together.

**Note:** This instruction does not interrupt the execution.
- `OP_JUMP` = `25`
	- Jumps to a named label. Expects the label name as [`String`](https://docs.godotengine.org/en/stable/classes/class_string.html) argument.

**Note:** The label is not required to be defined at this point and can also be set later with [`put_label()`](#int-put_labellabel-string-public-bool--false).
- `OP_CALL` = `26`
	- Calls a named label like a function. Expects a label name as [`String`](https://docs.godotengine.org/en/stable/classes/class_string.html) argument.

**Note:** The label is not required to be defined at this point and can also be set later with [`put_label()`](#int-put_labellabel-string-public-bool--false).
- `OP_RETURN` = `27`
	- Returns from a function call made with [`OP_CALL`](#op_call). Expects no arguments.
- `OP_RESET` = `28`
	- Calls `BlipKitTrack.reset()`. Expects no arguments.
- `OP_INSTRUMENT` = `29`
	- Sets `BlipKitTrack.instrument` from the given slot. Expects a slot index between `0` and `255` as `int` argument.

Which [`BlipKitInstrument`](BlipKitInstrument.md) is used is defined with `BlipKitInterpreter.set_instrument()`.

**Note:** An instrument cannot be unset. Keep a slot empty to be able to set an empty instrument (for example, `255`) if needed.
- `OP_INSTRUMENT_DIV` = `30`
	- Sets `BlipKitTrack.instrument_divider`. Expects an `int` argument between `0` and `65535`.
- `OP_CUSTOM_WAVEFORM` = `31`
	- Sets `BlipKitTrack.custom_waveform` from the given slot. Expects a slot index as `int` argument between `0` and `255`.

Which [`BlipKitWaveform`](BlipKitWaveform.md) is used is defined with `BlipKitInterpreter.set_waveform()`.

**Note:** If the custom waveform is set to an empty slot, the waveform is reset to `BlipKitTrack.WAVEFORM_SQUARE`.
- `OP_SAMPLE` = `32`
	- Sets `BlipKitTrack.sample` from the given slot. Expects a slot index between `0` and `255` as `int` argument.

Which [`BlipKitSample`](BlipKitSample.md) is used is defined with `BlipKitInterpreter.set_sample()`.

**Note:** If the sample is set to an empty slot, the waveform is reset to `BlipKitTrack.WAVEFORM_SQUARE`.
- `OP_SAMPLE_PITCH` = `33`
	- Sets `BlipKitTrack.sample_pitch`. Expects a `float` argument.

### enum `Error`

- `OK` = `0`
	- No error.
- `ERR_INVALID_STATE` = `1`
	- The assembler is not in the correct state.
- `ERR_INVALID_OPCODE` = `2`
	- The instruction is invalid.
- `ERR_INVALID_ARGUMENT` = `3`
	- An instruction argument is invalid.
- `ERR_DUPLICATE_LABEL` = `4`
	- The label already exists with the same name.
- `ERR_UNDEFINED_LABEL` = `5`
	- A label is not defined with the given name.
- `ERR_INVALID_LABEL` = `6`
	- The label has an invalid name.

## Method Descriptions

### `void clear()`

Clears all instructions, labels and errors.

### `int compile()`

Resolves label addresses and generates the byte code.

Call [`get_byte_code()`](#blipkitbytecode-get_byte_code) to get the byte code. Call [`clear()`](#void-clear) to generate new byte code.

Returns [`ERR_INVALID_STATE`](#err_invalid_state) if the byte code is already compiled.

### `BlipKitBytecode get_byte_code()`

Returns the generated byte code.

Returns an empty array if [`compile()`](#int-compile) was not called yet, or an error occurred.

### `String get_error_message() const`

Returns the last error as string.

Returns an empty string if no error occurred.

### `int put(opcode: int, arg1: Variant = null, arg2: Variant = null, arg3: Variant = null)`

Adds an instruction and returns [`OK`](#ok) on success. See [`Opcode`](#enum-opcode) for the required arguments.

Returns [`ERR_INVALID_OPCODE`](#err_invalid_opcode) if `opcode` is not valid.

Returns [`ERR_INVALID_ARGUMENT`](#err_invalid_argument) if the arguments are invalid for `opcode`.

Returns [`ERR_INVALID_STATE`](#err_invalid_state) if the byte code is already compiled.

### `int put_byte_code(byte_code: BlipKitBytecode, public: bool = false)`

Adds compiled [`BlipKitBytecode`](BlipKitBytecode.md) and returns [`OK`](#ok) on success. Public labels from `byte_code` are added to the existing labels.

If `public` is `true`, public labels from `byte_code` are also made public.

Returns [`ERR_DUPLICATE_LABEL`](#err_duplicate_label) if a label in `byte_code` already exists.

Returns [`ERR_INVALID_ARGUMENT`](#err_invalid_argument) if `byte_code` is not valid.

Returns [`ERR_INVALID_STATE`](#err_invalid_state) if the byte code is already compiled.

### `int put_label(label: String, public: bool = false)`

Adds a named label at the current position which can be referenced by [`OP_CALL`](#op_call) and [`OP_JUMP`](#op_jump) instructions.

If `public` is `true`, the label can be used as entry point when loading byte code with `BlipKitInterpreter.load_byte_code()`.

Returns [`ERR_DUPLICATE_LABEL`](#err_duplicate_label) if a label with the same name is already defined.

Returns [`ERR_INVALID_STATE`](#err_invalid_state) if the byte code is already compiled.

**Note:** The name is limited to `255` bytes. If the name is longer when converted to UTF-8, [`ERR_INVALID_LABEL`](#err_invalid_label) is returned.


