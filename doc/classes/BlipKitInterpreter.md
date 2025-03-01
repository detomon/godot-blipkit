# Class: BlipKitInterpreter

Inherits: *RefCounted*

**Executes byte code.**

## Description

Executes byte code generated with [`BlipKitAssembler`](BlipKitAssembler.md) to modify properties of a [`BlipKitTrack`](BlipKitTrack.md).

**Example:** Play instructions:

```gdscript
# Create the interpreter.
var interp := BlipKitInterpreter.new()

# Get the byte from a [BlipKitAssembler].
var byte_code := assem.get_byte_code()

# Load the byte code.
interp.load_byte_code(byte_code)

# Add a divider and run the interpreter on the track.
track.add_divider("run", 1, func () -> int:
    return interp.advance(track)
)
```
## Methods

- *int* [**`advance`**](#int-advancetrack-blipkittrack)(track: BlipKitTrack)
- *String* [**`get_error_message`**](#string-get_error_message-const)() const
- *BlipKitInstrument* [**`get_instrument`**](#blipkitinstrument-get_instrumentslot-int-const)(slot: int) const
- *int* [**`get_register`**](#int-get_registerregister-int-const)(register: int) const
- *int* [**`get_status`**](#int-get_status-const)() const
- *BlipKitWaveform* [**`get_waveform`**](#blipkitwaveform-get_waveformslot-int-const)(slot: int) const
- *bool* [**`load_byte_code`**](#bool-load_byte_codebyte_code-blipkitbytecode-start_label-string--)(byte_code: BlipKitBytecode, start_label: String = "")
- *void* [**`reset`**](#void-resetstart_label-string--)(start_label: String = "")
- *void* [**`set_instrument`**](#void-set_instrumentslot-int-instruments-blipkitinstrument)(slot: int, instruments: BlipKitInstrument)
- *void* [**`set_register`**](#void-set_registerregister-int-value-int)(register: int, value: int)
- *void* [**`set_waveform`**](#void-set_waveformslot-int-waveforms-blipkitwaveform)(slot: int, waveforms: BlipKitWaveform)

## Enumerations

### enum `Status`

- `OK_RUNNING` = `0`
	- More instructions are available to execute.
- `OK_FINISHED` = `1`
	- There are no more instructions to execute.
- `ERR_INVALID_BINARY` = `2`
	- The byte code is not valid.
- `ERR_INVALID_OPCODE` = `3`
	- An invalid opcode was encountered.
- `ERR_STACK_OVERFLOW` = `4`
	- A stack overflow occurred.
- `ERR_STACK_UNDERFLOW` = `5`
	- A stack underflow occurred.

## Constants

- `REGISTER_COUNT` = `16`
	- The number of usable registers.
- `SLOT_COUNT` = `256`
	- The number of slots for instruments and waveforms.

## Method Descriptions

### `int advance(track: BlipKitTrack)`

Advances the interpreter until a `BlipKitAssembler.OP_TICK` instruction is encountered, or no more instructions are available.

Returns a value greater than `0` indicating the number of *ticks* to wait before [`advance()`](#int-advancetrack-blipkittrack) should be called again.

Returns `0` if no more instructions are available.

Returns `-1` if an error occured. In this case, [`get_status()`](#int-get_status-const) returns the status and [`get_error_message()`](#string-get_error_message-const) returns the error message.

### `String get_error_message() const`

Returns the last error message.

Returns an empty string if no error occurred.

### `BlipKitInstrument get_instrument(slot: int) const`

Returns the instrument in `slot`. This is a number between `0` and `255`.

Returns `null` if no instrument is set in `slot`.

### `int get_register(register: int) const`

Returns the value in register `number`. This is a value between `0` and `15`.

### `int get_status() const`

Returns the current execution status.

### `BlipKitWaveform get_waveform(slot: int) const`

Returns the waveform in `slot`. This is a number between `0` and `255`.

Returns `null` if no waveform is set in `slot`.

### `bool load_byte_code(byte_code: BlipKitBytecode, start_label: String = "")`

Sets the byte code to interpret and clears all registers and errors.

If `start_label` is not empty, starts executing byte code from the label's position.

Returns `false` if the byte code is not valid or the label does not exist. The error message can be get with [`get_error_message()`](#string-get_error_message-const).

### `void reset(start_label: String = "")`

Resets the instruction pointer to the beginning of the byte code, and clears all registers and errors. This does not clear instrument and waveform slots.

**Note:** This does not reset [`BlipKitTrack`](BlipKitTrack.md). Call `BlipKitTrack.reset()` to reset the corresponding track.

### `void set_instrument(slot: int, instruments: BlipKitInstrument)`

Sets the instrument in `slot`. This is a number between `0` and `255`.

### `void set_register(register: int, value: int)`

Stores `value` in `register`. `register` is a number between `0` and `15`.

### `void set_waveform(slot: int, waveforms: BlipKitWaveform)`

Sets the waveform in `slot`. This is a number between `0` and `255`.


