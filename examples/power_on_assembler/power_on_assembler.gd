extends Node

# Create a track.
var _track := BlipKitTrack.new()
# Create an interpreter.
var _interp := BlipKitInterpreter.new()

# An audio player with an `AudioStreamBlipKit` resource.
@onready var _player: AudioStreamPlayer = $AudioStreamPlayer


func _ready() -> void:
	# Create assembler.
	var assem := BlipKitAssembler.new()

	# Set number of ticks per OP_STEP instruction.
	assem.put(BlipKitAssembler.OP_STEP_TICKS, 18)
	# Set duty cycle of square wave to 50%.
	assem.put(BlipKitAssembler.OP_DUTY_CYCLE, 8)

	# Add label to loop back.
	assem.put_label("start")

	# Play note on octave 5.
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_C_5)
	# Wait for one step.
	assem.put(BlipKitAssembler.OP_STEP, 1)
	# Play note on octave 5.
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_C_6)
	# Set volume slide effect to slide to new within 9 steps.
	assem.put(BlipKitAssembler.OP_VOLUME_SLIDE, 9)
	# Slides volume to 0.0 within 9 steps.
	assem.put(BlipKitAssembler.OP_VOLUME, 0.0)
	# Wait for 9 steps.
	assem.put(BlipKitAssembler.OP_STEP, 9)
	# Release note.
	assem.put(BlipKitAssembler.OP_RELEASE)
	# Disable volume slide effect.
	assem.put(BlipKitAssembler.OP_VOLUME_SLIDE, 0)
	# Set volume to 1.0.
	assem.put(BlipKitAssembler.OP_VOLUME, 1.0)
	# Wait for 10 steps.
	assem.put(BlipKitAssembler.OP_STEP, 10)

	# Jump back to start label.
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
	_track.attach(stream)

	# Add a divider and run the interpreter on the track.
	_track.add_divider(1, func () -> int:
		# Returns number of ticks to wait.
		return _interp.advance(_track)
	)
