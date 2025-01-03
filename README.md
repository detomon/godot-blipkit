# BlipKit for Godot Engine 4.3

A GDExtension for creating the beautiful sound of old sound chips.

- Provides an [`AudioStream`](doc/classes/AudioStreamBlipKit.md) to generate waveforms: square, triangle, noise, sawtooth, sine, and [custom ones](doc/classes/BlipKitWaveform.md)
- Allows to add multiple [`BlipKitTrack`](doc/classes/BlipKitTrack.md) objects to a stream to generate waveforms in parallel
- [Instruments](doc/classes/BlipKitInstrument.md) allow to change track properties when a note is playing, e.g., to create ADSR envelopes
- Provides a basic [byte code interpreter](#byte_code_interpreter) to generate more complex melodies

The [`BlipKitTrack`](doc/classes/BlipKitTrack.md) object generates the waveform. The [`AudioStreamBlipKit`](doc/classes/AudioStreamBlipKit.md) audio stream resource mixes the audio from multiple [`BlipKitTrack`](doc/classes/BlipKitTrack.md)s.
It can be used on `AudioStreamPlayer`, `AudioStreamPlayer2D` or `AudioStreamPlayer3D` nodes.

Time is measured in *ticks*. Every *tick* allows properties of the waveform to be updated.

- [Installation](#installation)
- [Usage](#usage)
- [Byte code interpreter](#byte_code_interpreter)

*Uses the [BlipKit](https://github.com/detomon/BlipKit) library to generate audio.*

## Installation

1. Clone this repository or download the archive
2. Copy the folder `addons/detomon.blipkit` to your project's `addons` folder (create it if needed)
3. Enable the plugin in the project settings via the `Plugins` tab

## Usage

- Create an `AudioStreamPlayer` node and add an [`AudioStreamBlipKit`](doc/classes/AudioStreamBlipKit.md) resource
- Attach [`BlipKitTrack`](doc/classes/BlipKitTrack.md) objects to the stream resource
- Change properties of the [`BlipKitTrack`](doc/classes/BlipKitTrack.md) to play notes, change the volume etc.

> [!TIP]
> See the [examples](examples) directory for some examples.
>
> Class descriptions are available in the Editor via the reference documentation after the extension is loaded,
or in the [doc/classes](doc/classes) directory.

**Example**: Play a note:

```gdscript
extends Control

# Create track.
var _track := BlipKitTrack.new()

# An audio player with a `AudioStreamBlipKit` resource.
@onready var _player: AudioStreamPlayer = %AudioStreamPlayer

func ready() -> void:
	# Get the stream.
	var stream: AudioStreamBlipKit = _player.stream

	# Attach the track to the stream.
	_track.attach(stream)
	# Play a note.
	_track.note = BlipKitTrack.NOTE_A_3

	# Ensure the player is playing or has `autoplay` enabled.
	_player.play()
```

## Byte code interpreter

`BlipKitInterpreter` allows to execute commands on a `BlipKitTrack` and change its properties over time.

`BlipKitAssembler` generate byte code from instructions which is executed by a `BlipKitInterpreter`.
Parts can be reused using function calls. The jump instruction allows to create loops.

**Example**: Loop two notes:

```gdscript
extends Control

# Create track.
var _track := BlipKitTrack.new()
# Create interpreter.
var _interp := BlipKitInterpreter.new()

# An audio player with an `AudioStreamBlipKit` resource.
@onready var _player: AudioStreamPlayer = $AudioStreamPlayer


func _ready() -> void:
	# Create assembler.
	var assem := BlipKitAssembler.new()

	# Set the duty cycle of the square wave.
	assem.put(BlipKitAssembler.OP_DUTY_CYCLE, 2)

	# Add a label "start" to loop back.
	assem.put_label("start")

	# Play two notes.
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_C_1))
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_C_2))
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 48)

	# Loop back to "start".
	assem.put(BlipKitAssembler.OP_JUMP, "start")

	# Compile and check for errors.
	if assem.compile() != BlipKitAssembler.OK:
		printerr(assem.get_error_message())
		return

	# Get and load the byte code.
	var bytes := assem.get_byte_code()
	_interp.load_byte_code(bytes)

	# Get the audio stream.
	var stream: AudioStreamBlipKit = _player.stream

	# Attach the track to the audio stream.
	_track.attach(stream

	# Add a divider and run the interpreter on the track.
	_track.add_divider(&"run", 1, func () -> int:
		return _interp.advance(_track)
	)
```
