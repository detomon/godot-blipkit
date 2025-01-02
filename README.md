# BlipKit for Godot Engine 4.3

A GDExtension for creating the beautiful sound of old sound chips.
This uses the [BlipKit](https://github.com/detomon/BlipKit) library to generate audio.

- [Installation](#installation)
- [Usage](#usage)
- [Byte code interpreter](#byte_code_interpreter)

## Installation

1. Clone this repository or download the archive
2. Copy the folder `addons/detomon.blipkit` to your project's `addons` folder (create it if needed)
3. Enable the plugin in the project settings via the `Plugins` tab

## Usage

- Create an `AudioStreamPlayer` node and add an `AudioStreamBlipKit` resource
- Attach `BlipKitTrack` objects to the stream resource
- Change properties to play notes, change the volume, and more

The `AudioStreamBlipKit` audio stream resource is used to generate audio.
It can be used on `AudioStreamPlayer`, `AudioStreamPlayer2D` or `AudioStreamPlayer3D` nodes.

The `BlipKitTrack` object generates a single waveform.
It has various properties to change the note, waveform, volume, effects and more.

See the [examples](examples) directory for some examples.

Class descriptions are available in the Editor via the reference documentation after the extension is loaded,
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

Byte code allows to execute commands on `BlipKitTrack`s and change the properties over time.

`BlipKitAssembler` generate byte code from instructions which is executed by `BlipKitInterpreter`.
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
	assem.put(BlipKitAssembler.INSTR_DUTY_CYCLE, 2)

	# Add a label "start" to loop back.
	assem.put_label("start")

	# Play two notes.
	assem.put(BlipKitAssembler.INSTR_ATTACK, float(BlipKitTrack.NOTE_C_1))
	assem.put(BlipKitAssembler.INSTR_TICK, 48)
	assem.put(BlipKitAssembler.INSTR_RELEASE)
	assem.put(BlipKitAssembler.INSTR_TICK, 48)
	assem.put(BlipKitAssembler.INSTR_ATTACK, float(BlipKitTrack.NOTE_C_2))
	assem.put(BlipKitAssembler.INSTR_TICK, 48)
	assem.put(BlipKitAssembler.INSTR_RELEASE)
	assem.put(BlipKitAssembler.INSTR_TICK, 48)

	# Loop back to "start".
	assem.put(BlipKitAssembler.INSTR_JUMP, "start")

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
