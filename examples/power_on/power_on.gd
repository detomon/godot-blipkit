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
	# - Set volume to 1.0 for 0 ticks (prevents sliding from previous value)
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
