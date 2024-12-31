extends Control

@onready var _player: AudioStreamPlayer = %AudioStreamPlayer

var _interp := BlipKitInterpreter.new()
var _track := BlipKitTrack.new()


func _ready() -> void:
	var assem := BlipKitAssembler.new()

	assem.put(BlipKitAssembler.INSTR_WAVEFORM, BlipKitTrack.WAVEFORM_SQUARE)
	assem.put(BlipKitAssembler.INSTR_DUTY_CYCLE, 8)

	assem.put_label("start")
	assem.put(BlipKitAssembler.INSTR_ATTACK, float(BlipKitTrack.NOTE_C_5))
	assem.put(BlipKitAssembler.INSTR_TICK, 18)
	assem.put(BlipKitAssembler.INSTR_ATTACK, float(BlipKitTrack.NOTE_C_6))
	assem.put(BlipKitAssembler.INSTR_VOLUME_SLIDE, 162)
	assem.put(BlipKitAssembler.INSTR_VOLUME, 0.0)
	assem.put(BlipKitAssembler.INSTR_TICK, 162)
	assem.put(BlipKitAssembler.INSTR_RELEASE)
	assem.put(BlipKitAssembler.INSTR_VOLUME_SLIDE, 0)
	assem.put(BlipKitAssembler.INSTR_VOLUME, 1.0)
	assem.put(BlipKitAssembler.INSTR_TICK, 180)
	assem.put(BlipKitAssembler.INSTR_JUMP, "start")
#
	if assem.compile() != BlipKitAssembler.OK:
		printerr(assem.get_error_message())
		return

	var bytes := assem.get_byte_code()

	_interp.set_byte_code(bytes)

	var stream: AudioStreamBlipKit = _player.stream
	_track.attach(stream)
	_track.add_divider(&"run", 1, func () -> int:
		var result := _interp.advance(_track)
		return result
	)


func _on_check_button_toggled(toggled_on: bool) -> void:
	if not toggled_on:
		return

	var stream: AudioStreamBlipKit = _player.stream

	stream.call_synced(func () -> void:
		_interp.reset()
		_track.reset_divider(&"run")
		_track.reset()
	)
