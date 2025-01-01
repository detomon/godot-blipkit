# Class: BlipKitInterpreter

Inherits: *RefCounted*

**Interprets and executes byte code.**

## Description

Interprets and executes byte code generated with [`BlipKitAssembler`](BlipKitAssembler.md) to modify properties of a [`BlipKitTrack`](BlipKitTrack.md).

**Example:** Play instructions:

```gdscript
# Create interpreter.
var interp := BlipKitInterpreter.new()

# Get byte from a [BlipKitAssembler].
var byte_code := assem.get_byte_code()

# Set the byte code.
interp.set_byte_code(byte_code)

# Run the byte code on a track using a divider.
track.add_divider(&"run", 1, func () -> int:
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
- *void* [**`reset`**](#void-reset)()
- *void* [**`set_byte_code`**](#void-set_byte_codebyte_code-packedbytearray)(byte_code: PackedByteArray)
- *void* [**`set_instrument`**](#void-set_instrumentslot-int-instruments-blipkitinstrument)(slot: int, instruments: BlipKitInstrument)
- *void* [**`set_register`**](#void-set_registerregister-int-value-int)(register: int, value: int)
- *void* [**`set_waveform`**](#void-set_waveformslot-int-waveforms-blipkitwaveform)(slot: int, waveforms: BlipKitWaveform)

## Enumerations

### enum `Status`

- `OK_RUNNING` = `0`
	- More instructions are available to execute.
- `OK_FINISHED` = `1`
	- There are no more instructions to execute.
- `ERR_INVALID_ARGUMENT` = `2`
	- An invalid argument was provided.
- `ERR_INVALID_INSTRUCTION` = `3`
	- An invalid instruction was found.
- `ERR_STACK_OVERFLOW` = `4`
	- A stack overflow occurred.
- `ERR_STACK_UNDERFLOW` = `5`
	- A stack underflow occurred.
- `ERR_RECURSION` = `6`
	- An infinite recursion was detected.

## Method Descriptions

### `int advance(track: BlipKitTrack)`

Advances the interpreter until a `BlipKitAssembler.INSTR_WAIT` instruction is encountered or no more instruction are available.

Returns a value greater than `0` indicating the number of *ticks* to wait before [`advance()`](#int-advancetrack-blipkittrack) should be called again.

Returns `0` if no more instructions are available.

Returns `-1` if an error occured. The status can be get with [`get_status()`](#int-get_status-const), and the error message can be get with [`get_error_message()`](#string-get_error_message-const).

### `String get_error_message() const`

Returns the last error message.

Returns an empty string if no error occurred.

### `BlipKitInstrument get_instrument(slot: int) const`

Returns the instrument in `slot`. This is a number between `0` and `255`.

Returns `null` if no instrument is set in `slot`.

### `int get_register(register: int) const`

Returns the value in register `number`. `number` is a value between `0` and `15`.

### `int get_status() const`

Returns the current execution status.

### `BlipKitWaveform get_waveform(slot: int) const`

Returns the waveform in `slot`. This is a number between `0` and `255`.

Returns `null` if no waveform is set in `slot`.

### `void reset()`

Resets the instruction pointer to the beginning of the byte code, and clears all registers and errors.

**Note:** This does not reset [`BlipKitTrack`](BlipKitTrack.md). Call `BlipKitTrack.reset()` to reset the corresponding track.

### `void set_byte_code(byte_code: PackedByteArray)`

Sets the byte code to interpret and calls [`reset()`](#void-reset).

### `void set_instrument(slot: int, instruments: BlipKitInstrument)`

Sets the instrument in `slot`. This is a number between `0` and `255`.

### `void set_register(register: int, value: int)`

Stores `value` in `register`. `register` is a number between `0` and `15`.

### `void set_waveform(slot: int, waveforms: BlipKitWaveform)`

Sets the waveform in `slot`. This is a number between `0` and `255`.


