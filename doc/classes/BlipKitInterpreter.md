# Class: BlipKitInterpreter

Inherits: *RefCounted*

**Executes bytecode generated with [`BlipKitAssembler`](BlipKitAssembler.md).**

## Description

Executes bytecode generated with [`BlipKitAssembler`](BlipKitAssembler.md) to modify properties of a [`BlipKitTrack`](BlipKitTrack.md).

**Example:** Play instructions:

```gdscript
# Create the interpreter.
var interp := BlipKitInterpreter.new()

# Get the bytecode from a [BlipKitAssembler].
var bytecode := assem.get_byte_code()

# Load the bytecode.
interp.load_byte_code(bytecode)

# Add a divider and run the interpreter on the track.
track.add_divider(1, func () -> int:
    return interp.advance(track)
)
```
## Properties

- *int* [**`step_ticks`**](#int-step_ticks) `[default: 24]`

## Methods

- *int* [**`advance`**](#int-advancetrack-blipkittrack)(track: BlipKitTrack)
- *String* [**`get_error_message`**](#string-get_error_message-const)() const
- *BlipKitInstrument* [**`get_instrument`**](#blipkitinstrument-get_instrumentslot-int-const)(slot: int) const
- *BlipKitSample* [**`get_sample`**](#blipkitsample-get_sampleslot-int-const)(slot: int) const
- *int* [**`get_state`**](#int-get_state-const)() const
- *BlipKitWaveform* [**`get_waveform`**](#blipkitwaveform-get_waveformslot-int-const)(slot: int) const
- *bool* [**`load_byte_code`**](#bool-load_byte_codebyte_code-blipkitbytecode-start_label-string--)(byte_code: BlipKitBytecode, start_label: String = "")
- *void* [**`reset`**](#void-resetstart_label-string--)(start_label: String = "")
- *void* [**`set_instrument`**](#void-set_instrumentslot-int-instrument-blipkitinstrument)(slot: int, instrument: BlipKitInstrument)
- *void* [**`set_sample`**](#void-set_sampleslot-int-sample-blipkitsample)(slot: int, sample: BlipKitSample)
- *void* [**`set_waveform`**](#void-set_waveformslot-int-waveform-blipkitwaveform)(slot: int, waveform: BlipKitWaveform)

## Enumerations

### enum `State`

- `OK_RUNNING` = `0`
	- More instructions are available to execute.
- `OK_FINISHED` = `1`
	- There are no more instructions to execute.
- `ERR_INVALID_BINARY` = `2`
	- The bytecode is not valid.
- `ERR_INVALID_OPCODE` = `3`
	- An invalid opcode was encountered.
- `ERR_INVALID_LABEL` = `4`
	- A label with that name does not exist.
- `ERR_STACK_OVERFLOW` = `5`
	- A stack overflow occurred.
- `ERR_STACK_UNDERFLOW` = `6`
	- A stack underflow occurred.

## Constants

- `STACK_SIZE_MAX` = `16`
	- The maximum function call depth.
- `SLOT_COUNT` = `256`
	- The number of slots for instruments, waveforms and samples.
- `STEP_TICKS_DEFAULT` = `24`
	- Default value of number of ticks per step instruction.

## Property Descriptions

### `int step_ticks`

*Default*: `24`

The number of *ticks* per `BlipKitAssembler.OP_STEP` instruction. The value is clamped between `1` and `65535`.


## Method Descriptions

### `int advance(track: BlipKitTrack)`

Runs the interpreter and executes instructions on `track` until a `BlipKitAssembler.OP_TICK` or `BlipKitAssembler.OP_STEP` instruction is encountered, or no more instructions are available.

Returns a value greater than `0` indicating the number of *ticks* to wait before [`advance()`](#int-advancetrack-blipkittrack) should be called again.

Returns `0` if no more instructions are available.

Returns `-1` if an error occured. In this case, [`get_state()`](#int-get_state-const) returns the state and [`get_error_message()`](#string-get_error_message-const) returns the error message.

### `String get_error_message() const`

Returns the last error message.

Returns an empty string if no error occurred.

### `BlipKitInstrument get_instrument(slot: int) const`

Returns the instrument in `slot`. This is a number between `0` and `255`.

Returns `null` if no instrument is set in `slot`.

### `BlipKitSample get_sample(slot: int) const`

Returns the sample in `slot`. This is a number between `0` and `255`.

Returns `null` if no sample is set in `slot`.

### `int get_state() const`

Returns the current execution state.

### `BlipKitWaveform get_waveform(slot: int) const`

Returns the waveform in `slot`. This is a number between `0` and `255`.

Returns `null` if no waveform is set in `slot`.

### `bool load_byte_code(byte_code: BlipKitBytecode, start_label: String = "")`

Sets the bytecode to interpret and resets all registers and errors.

If `start_label` is not empty, starts executing bytecode from the label's position. The label must be set `public` when adding it with `BlipKitAssembler.put_label()`.

Returns `false` if the bytecode is not valid or the label does not exist. The error message can be get with [`get_error_message()`](#string-get_error_message-const).

### `void reset(start_label: String = "")`

Resets the instruction pointer to the beginning of the bytecode, and resets all registers and errors. This does not clear instrument, waveform or sample slots.

If `start_label` is not empty, starts executing bytecode from the label's position. The label must be set `public` when adding it with `BlipKitAssembler.put_label()`.

**Note:** This does not reset [`BlipKitTrack`](BlipKitTrack.md). Call `BlipKitTrack.reset()` to reset the corresponding track.

### `void set_instrument(slot: int, instrument: BlipKitInstrument)`

Sets the instrument in `slot`. This is a number between `0` and `255`.

### `void set_sample(slot: int, sample: BlipKitSample)`

Sets the sample in `slot`. This is a number between `0` and `255`.

### `void set_waveform(slot: int, waveform: BlipKitWaveform)`

Sets the waveform in `slot`. This is a number between `0` and `255`.


