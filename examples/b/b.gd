extends Node

var _track := BlipKitTrack.new()
var _interp := BlipKitInterpreter.new()
var _midi_track := BlipKitTrack.new()

@onready var _player: AudioStreamPlayer = $AudioStreamPlayer


func _ready() -> void:
	var assem := BlipKitAssembler.new()

	assem.put(BlipKitAssembler.OP_DUTY_CYCLE, 4)

	assem.put_label("start")
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_C_1))
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_C_2))
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_JUMP, "start")

	if assem.compile() != BlipKitAssembler.OK:
		printerr(assem.get_error_message())
		return

	var bytes := assem.get_byte_code()
	_interp.load_byte_code(bytes)

	ResourceSaver.save(bytes, "res://examples/b/b.blipc")

	var stream: AudioStreamBlipKit = _player.stream

	var instr0 := BlipKitInstrument.create_with_adsr(2, 16, 0.85, 36)
	_track.instrument = instr0

	_track.volume = 0.9
	_track.pitch = 0.07
	_track.attach(stream)
	_track.add_divider("run", 1, func () -> int:
		var result := _interp.advance(_track)
		return result
	)

	var instr := BlipKitInstrument.create_with_adsr(4, 8, 0.75, 24)

	_midi_track.portamento = 8
	_midi_track.instrument = instr
	_midi_track.attach(stream)


func _on_blip_kit_midi_input_notes_changes(notes: Dictionary) -> void:
	var values := notes.keys()

	if values:
		_midi_track.note = values[-1]
	else:
		_midi_track.release()
