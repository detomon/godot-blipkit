# Format description of `.blipc` files

- [Data type](#data-types)
- [File](#file)
	- [Struct `File`](#struct-file)
	- [Struct `Bytecode`](#struct-bytecode)
	- [Struct `LabelList`](#struct-labellist)
	- [Struct `Label`](#struct-label)
- [Instructions](#instructions)
	- [Struct `Instruction`](#struct-instruction)
	- [Enum `Opcode`](#enum-opcode)

## Data types

| Type | Description |
|---|---|
| `u8` | Unsigned 8 bit integer |
| `u16` | Unsigned 16 bit integer |
| `f16` | 16 bit float (IEEE 754-2008) |
| `u32` | Unsigned 32 bit integer |
| `s32` | Signed 32 bit integer |

**Note:** All types are *little endian*.

## File

A File is defined as [`File`](#struct-file).

### `Struct File`

 Sections are optional and can be in any order. The `code` section is an exception, and is required to be at the first position.

| Field | Type | Value | Description |
|---|---|---|---|
| magic | `u8[4]` | `"BLIP"` | Constant |
| version | `u8` | `0` | Binary version |
| flags | `u8[3]` | `{ 0, 0, 0 }` | File flags |
| code | [`Bytecode`](#struct-bytecode) | `*` | Code section |
| labels | [`LabelList`](#struct-labellist) | `*` | Label section (optional) |

### `Struct Bytecode`

Defines the byte code. The `bytes` field contains a stream of [`Instruction`](#struct-instruction). This section is required.

| Field | Type | Value | Description |
|---|---|---|---|
| magic | `u8[4]` | `"code"` | Constant |
| size | `u32` | `0xNNNNNNNN` | Byte code size in bytes |
| bytes | `u8[size]` | `*` | Byte code |

### `Struct LabelList`

Defines a list of named jump addresses in the byte code section.

| Field | Type | Value | Description |
|---|---|---|---|
| magic | `u8[4]` | `"labl"` | Constant |
| size | `u32` | `0xNNNNNNNN` | Section size in bytes including field `count` |
| count | `u32` | `0xNNNNNNNN` | Label count |
| labels |  [`Label`](#struct-labellist)`[count]` | `*` | Labels |

### `Struct Label`

Defines a jump address in the byte code section.

| Field | Type | Value | Description |
|---|---|---|---|
| address | `u32` | `0xNNNNNNNN` | Address relative to `Bytecode.bytes` |
| length | `u8` | `0xNN` | Name length without terminating `NUL` |
| name | `u8[length]` | `*` | Label name without terminating `NUL` |

## Instructions

### `Struct Instruction`

Instructions have a variable amount and types of arguments.

| Field | Type | Value | Description |
|---|---|---|---|
| opcode | `u8` | `0xNN` | [`Enum Opcode`](#enum-opcode) |
| args | `*` | `*` | Opcode arguments |

### `Enum Opcode`

Opcodes grouped by argument types.

---

| Opcode (`u8`) | Description |
|---|---|
| `OP_NOOP` | No operation |
| `OP_HALT` | Stop execution |
| `OP_RELEASE` | Release note |
| `OP_MUTE` | Mute note |
| `OP_RETURN` | Return from call |
| `OP_RESET` | Reset track |

---

| Opcode (`u8`) | Arg 1 (`u8`) | Description |
|---|---|---|
| `OP_WAVEFORM` | Waveform type | Set waveform |
| `OP_DUTY_CYCLE` | Duty cycle | Set square wave duty cycle |
| `OP_CUSTOM_WAVEFORM` | Custom waveform slot | Set custom waveform |
| `OP_INSTRUMENT` | Instrument slot | Set instrument |
| `OP_SAMPLE` | Sample slot | Set sample |
| `OP_PHASE_WRAP` | Phase wrap | Set wave phase wrap |

---

| Opcode (`u8`) | Arg 1 (`u16`) | Description |
|---|---|---|
| `OP_EFFECT_DIV` | Effect divider ticks | Set effect divider |
| `OP_ARPEGGIO_DIV` | Arpeggio divider ticks | Set arpeggio divider ticks |
| `OP_INSTRUMENT_DIV` | Instrument divider ticks | Set instrument divider |
| `OP_TICK` | Number of ticks to wait | Wait for number of ticks |
| `OP_STEP` | Number of steps to wait | Wait for number of steps |
| `OP_DELAY_TICKS` | Number of ticks to delay the following instructions | Delay following instructions for number if ticks |
| `OP_STEP_TICKS` | Number of ticks per step | Set number of ticks per step |
| `OP_INTERRUPT` | Trigger interrupt | Calls interrupt handler |

---

| Opcode (`u8`) | Arg 1 (`f16`) | Description |
|---|---|---|
| `OP_ATTACK` | Note | Attack note |
| `OP_VOLUME` | Value between `0.0` and `1.0` | Set volume |
| `OP_MASTER_VOLUME` | Value between `0.0` and `1.0` | Set master volume |
| `OP_PANNING` | Value between `-1.0` and `+1.0` | Set panning |
| `OP_PITCH` | Note pitch | Set pitch relative to current note |
| `OP_SAMPLE_PITCH` | Sample pitch | Set sample pitch |
| `OP_VOLUME_SLIDE` | Volume slide steps | Set volume slide steps |
| `OP_PANNING_SLIDE` | Panning slide steps | Set panning slide steps |
| `OP_PORTAMENTO` | Portamento steps | Set portamento steps |
| `OP_DELAY_STEP` | Number of steps to delay the following instructions | Delay following instructions for number if steps |

---

| Opcode (`u8`) | Arg 1 (`s32`) | Description |
|---|---|---|
| `OP_CALL` | Relative byte position to current instruction | Function call |
| `OP_JUMP` | Relative byte position to current instruction | Jump to position |

---

| Opcode (`u8`) | Arg 1 (`f16`) | Arg 2 (`f16`) | Arg 3 (`f16`) | Description |
|---|---|---|---|---|
| `OP_TREMOLO` | Delta pitch | Number of steps for a 1/2 cycle | Number of steps to slide to the newly set values | Set tremolo effect |
| `OP_VIBRATO` | Delta volume | Number of steps for a 1/4 cycle | Number of steps to slide to the newly set values | Set Vibrato effect |
