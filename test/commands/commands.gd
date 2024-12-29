extends Control

@onready var _player: AudioStreamPlayer = %AudioStreamPlayer

const Instr := BlipKitAssemblerTest.Instruction

var _track := BlipKitTrack.new()
var _instrument := BlipKitInstrument.new()


func _ready() -> void:
	_instrument.set_adsr(8, 24, 0.5, 24)

	#var assembler := BlipKitAssemblerTest.new()
#
	#assembler.put_1(Instr.WAVEFORM, BlipKitTrack.WAVEFORM_SQUARE)
	#assembler.put_1(Instr.DUTY_CYCLE, 8)
#
	#assembler.put_label("start")
	#assembler.put_1(Instr.NOTE, float(BlipKitTrack.NOTE_C_5))
	#assembler.put_1(Instr.WAIT, 18)
	#assembler.put_1(Instr.NOTE, float(BlipKitTrack.NOTE_C_6))
	#assembler.put_1(Instr.VOLUME_SLIDE, 162)
	#assembler.put_1(Instr.VOLUME, 0.0)
	#assembler.put_1(Instr.WAIT, 162)
	#assembler.put_1(Instr.NOTE, float(BlipKitTrack.NOTE_RELEASE))
	#assembler.put_1(Instr.VOLUME_SLIDE, 0)
	#assembler.put_1(Instr.VOLUME, 1.0)
	#assembler.put_1(Instr.WAIT, 180)
	#assembler.put_1(Instr.CALL, "tone")
	#assembler.put_1(Instr.JUMP, "start")
##
	#assembler.put_label("tone")
	#assembler.put_1(Instr.NOTE, float(BlipKitTrack.NOTE_C_2))
	#assembler.put_3(Instr.VIBRATO, 12, 0.3, 0)
	#assembler.put_1(Instr.WAIT, 100)
	#assembler.put_3(Instr.VIBRATO, 0, 0.0, 0)
	#assembler.put_1(Instr.NOTE, float(BlipKitTrack.NOTE_RELEASE))
	#assembler.put_0(Instr.RETURN)
#
	#var bytes1 := assembler.compile()
	#print(bytes1)
	#print(assembler.get_compile_error())


	var assem := BlipKitAssembler.new()

	assem.put(BlipKitAssembler.INSTR_WAVEFORM, BlipKitTrack.WAVEFORM_SQUARE)
	assem.put(BlipKitAssembler.INSTR_DUTY_CYCLE, 8)

	assem.put_label("start")
	assem.put(BlipKitAssembler.INSTR_NOTE, float(BlipKitTrack.NOTE_C_5))
	assem.put(BlipKitAssembler.INSTR_WAIT, 18)
	assem.put(BlipKitAssembler.INSTR_NOTE, float(BlipKitTrack.NOTE_C_6))
	assem.put(BlipKitAssembler.INSTR_VOLUME_SLIDE, 162)
	assem.put(BlipKitAssembler.INSTR_VOLUME, 0.0)
	assem.put(BlipKitAssembler.INSTR_WAIT, 162)
	assem.put(BlipKitAssembler.INSTR_NOTE, float(BlipKitTrack.NOTE_RELEASE))
	assem.put(BlipKitAssembler.INSTR_VOLUME_SLIDE, 0)
	assem.put(BlipKitAssembler.INSTR_VOLUME, 1.0)
	assem.put(BlipKitAssembler.INSTR_WAIT, 180)
	assem.put(BlipKitAssembler.INSTR_CALL, "tone")
	assem.put(BlipKitAssembler.INSTR_JUMP, "start")
#
	assem.put_label("tone")
	assem.put(BlipKitAssembler.INSTR_NOTE, float(BlipKitTrack.NOTE_C_2))
	assem.put(BlipKitAssembler.INSTR_VIBRATO, 12, 0.3, 0)
	assem.put(BlipKitAssembler.INSTR_WAIT, 100)
	assem.put(BlipKitAssembler.INSTR_VIBRATO, 0, 0.0, 0)
	assem.put(BlipKitAssembler.INSTR_NOTE, float(BlipKitTrack.NOTE_RELEASE))
	assem.put(BlipKitAssembler.INSTR_RETURN)

	if assem.compile() != BlipKitAssembler.OK:
		printerr(assem.get_error_message())
		return

	var bytes := assem.get_byte_code()
	print(bytes)

	var interpreter := BlipKitInterpreter.new()
	#interpreter.set_instrument_at(0, _instrument)
	interpreter.set_byte_code(bytes)

	var stream: AudioStreamBlipKit = _player.stream

	_track.attach(stream)
	_track.add_divider(&"cmd", 1, func () -> int:
		var result := interpreter.advance(_track)
		prints(result, interpreter.get_status(), interpreter.get_error_message())
		return result
	)
