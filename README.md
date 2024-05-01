# BlipKit for Godot Engine 4.3

A GDExtension for creating the beautiful sound of old sound chips.

- Generate waveforms: square, triangle, noise, sawtooth, sine, and [custom ones](doc/classes/BlipKitWaveform.md) using [BlipKitTrack](doc/classes/BlipKitTrack.md)
- Create pitch, volume, panning, and duty cycle envelopes using [BlipKitInstrument](doc/classes/BlipKitInstrument.md)
- Load and play audio samples from `AudioStreamWAV` using [BlipKitSample](doc/classes/BlipKitSample.md)
- Provides a basic [byte code interpreter](#byte_code_interpreter) to generate more complex melodies and reuse patterns

## Contents

- [Installation](#installation)
- [Introduction](#introduction)
- [Usage](#usage)
- [Byte code interpreter](#byte_code_interpreter)
- [Classes](#classes)
- [References](#references)

<video width="480" height="320" controls src="https://github.com/user-attachments/assets/8850dadc-935f-46f6-804c-f757b9a2467b">Power On!</video>

## Installation

1. Clone this repository or download the archive
2. Copy the folder `addons/detomon.blipkit` to your project's `addons` folder (create it if needed)
3. Enable the plugin in the project settings via the `Plugins` tab

## Introduction

The [BlipKitTrack](doc/classes/BlipKitTrack.md) class generates a single waveform. Multiple tracks can be attached to a [AudioStreamBlipKit](doc/classes/AudioStreamBlipKit.md), which then mixes the audio. As with every audio stream, it can be used with `AudioStreamPlayer`, `AudioStreamPlayer2D` or `AudioStreamPlayer3D` nodes.

Time is measured in *ticks*. Every *tick* allows properties of the waveform to be updated. The tick rate is `240` ticks per second.

## Usage

- Create an `AudioStreamPlayer` node and add an [AudioStreamBlipKit](doc/classes/AudioStreamBlipKit.md) resource
- Attach [BlipKitTrack](doc/classes/BlipKitTrack.md) objects to the stream resource
- Change properties of the [BlipKitTrack](doc/classes/BlipKitTrack.md) to play notes, change the volume etc.

> [!TIP]
> See the [examples](examples) directory for some examples.
>
> Class descriptions are available in the Editor via the reference documentation after the extension is loaded, or in the [doc/classes](doc/classes) directory.

### Examples

- Play an iconic startup sound ([Power On!](examples/power_on)):

```gdscript
extends Node

# Create a track.
var _track := BlipKitTrack.new()
# Create an instrument.
var _instr := BlipKitInstrument.new()

# An audio player with an `AudioStreamBlipKit` resource.
@onready var _player: AudioStreamPlayer = $AudioStreamPlayer


func _ready() -> void:
	# Set pitch sequence:
	# - Play lower octave for 18 ticks (defined with `_track.instrument_divider`)
	# - Then play current note as long as the note is playing
	_instr.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [-12, 0])

	# Set volume envelope:
	# - Set volume to 1.0 for 0 ticks (do not slide from previous value)
	# - Keep volume on 1.0 for 18 ticks
	# - Slide volume to 0.0 for 162 ticks
	_instr.set_envelope(BlipKitInstrument.ENVELOPE_VOLUME, [1.0, 1.0, 0.0], [0, 18, 162])

	# Set duty cycle of square wave to 50%.
	_track.duty_cycle = 8
	# Set instrument.
	_track.instrument = _instr
	# Set number of ticks per envelope sequence value.
	_track.instrument_divider = 18

	# Get the audio stream.
	var stream: AudioStreamBlipKit = _player.stream
	# Attach the track to the audio stream.
	_track.attach(stream)

	# Add a divider and call it every 360 ticks (1.5 seconds).
	_track.add_divider(360, func () -> int:
		# Release previous note to start instrument again.
		_track.release()
		# Play note C on octave 6.
		_track.note = BlipKitTrack.NOTE_C_6

		# Do not change tick rate of divider.
		return 0
	)
```

## Byte code interpreter

The byte code assembler allows to create instructions to play patterns.

- [BlipKitAssembler](doc/classes/BlipKitAssembler.md) generate byte code from instructions
- [BlipKitInterpreter](doc/classes/BlipKitInterpreter.md) execute the byte code on a [BlipKitTrack](doc/classes/BlipKitTrack.md) to change its properties over time

### Examples

- Play an iconic startup sound ([Power On! Assembler version](examples/power_on_assembler)).
- Play on multiple tracks ([Bytecode](examples/bytecode)).

## Classes

[Overview](doc/classes)

- [AudioStreamBlipKit](doc/classes/AudioStreamBlipKit.md)
- [BlipKitAssembler](doc/classes/BlipKitAssembler.md)
- [BlipKitBytecode](doc/classes/BlipKitBytecode.md)
- [BlipKitInstrument](doc/classes/BlipKitInstrument.md)
- [BlipKitInterpreter](doc/classes/BlipKitInterpreter.md)
- [BlipKitSample](doc/classes/BlipKitSample.md)
- [BlipKitTrack](doc/classes/BlipKitTrack.md)
- [BlipKitWaveform](doc/classes/BlipKitWaveform.md)

## References

Uses the [BlipKit](https://github.com/detomon/BlipKit) library to generate audio.
