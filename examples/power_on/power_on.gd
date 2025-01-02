extends Control

var _track := BlipKitTrack.new()
var _interp := BlipKitInterpreter.new()

@onready var _player: AudioStreamPlayer = $AudioStreamPlayer


func _ready() -> void:
	var assem := BlipKitAssembler.new()

	assem.put(BlipKitAssembler.OP_WAVEFORM, BlipKitTrack.WAVEFORM_SQUARE)
	assem.put(BlipKitAssembler.OP_DUTY_CYCLE, 8)

	assem.put_label("start")
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_C_5))
	assem.put(BlipKitAssembler.OP_TICK, 18)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_C_6))
	assem.put(BlipKitAssembler.OP_VOLUME_SLIDE, 162)
	assem.put(BlipKitAssembler.OP_VOLUME, 0.0)
	assem.put(BlipKitAssembler.OP_TICK, 162)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_VOLUME_SLIDE, 0)
	assem.put(BlipKitAssembler.OP_VOLUME, 1.0)
	assem.put(BlipKitAssembler.OP_TICK, 180)
	assem.put(BlipKitAssembler.OP_JUMP, "start")

	if assem.compile() != BlipKitAssembler.OK:
		printerr(assem.get_error_message())
		return

	var bytes := assem.get_byte_code()
	_interp.load_byte_code(bytes)

	var stream: AudioStreamBlipKit = _player.stream

	_track.attach(stream)
	_track.add_divider("run", 1, func () -> int:
		var result := _interp.advance(_track)
		return result
	)
