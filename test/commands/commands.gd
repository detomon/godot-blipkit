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
	#assembler.put_1(Instr.WAIT, 162)
	##assembler.put_1(Instr.CALL, "tone")
	#assembler.put_1(Instr.JUMP, "start")
#
	##assembler.put_label("tone")
	##assembler.put_1(Instr.NOTE, float(BlipKitTrack.NOTE_C_2))
	##assembler.put_3(Instr.VIBRATO, 12, 0.3, 0)
	##assembler.put_1(Instr.WAIT, 100)
	##assembler.put_3(Instr.VIBRATO, 0, 0.0, 0)
	##assembler.put_1(Instr.NOTE, float(BlipKitTrack.NOTE_RELEASE))
	##assembler.put_0(Instr.RETURN)

	var assembler := BlipKitAssembler.new()

	#assembler.put_1(BlipKitAssembler.INSTR_WAVEFORM, BlipKitTrack.WAVEFORM_SQUARE)
	#assembler.put_1(BlipKitAssembler.INSTR_DUTY_CYCLE, 8)
#
	#assembler.put_label("start")
	#assembler.put_1(BlipKitAssembler.INSTR_NOTE, float(BlipKitTrack.NOTE_C_5))
	#assembler.put_1(BlipKitAssembler.INSTR_WAIT, 18)
	#assembler.put_1(BlipKitAssembler.INSTR_NOTE, float(BlipKitTrack.NOTE_C_6))
	#assembler.put_1(BlipKitAssembler.INSTR_VOLUME_SLIDE, 162)
	#assembler.put_1(BlipKitAssembler.INSTR_VOLUME, 0.0)
	#assembler.put_1(BlipKitAssembler.INSTR_WAIT, 162)
	#assembler.put_1(BlipKitAssembler.INSTR_NOTE, float(BlipKitTrack.NOTE_RELEASE))
	#assembler.put_1(BlipKitAssembler.INSTR_VOLUME_SLIDE, 0)
	#assembler.put_1(BlipKitAssembler.INSTR_VOLUME, 1.0)
	#assembler.put_1(BlipKitAssembler.INSTR_WAIT, 162)
	##assembler.put_1(BlipKitAssembler.INSTR_CALL, "tone")
	#assembler.put_1(BlipKitAssembler.INSTR_JUMP, "start")

	#assembler.put_label("tone")
	#assembler.put_1(BlipKitAssembler.INSTR_NOTE, float(BlipKitTrack.NOTE_C_2))
	#assembler.put_3(BlipKitAssembler.INSTR_VIBRATO, 12, 0.3, 0)
	#assembler.put_1(BlipKitAssembler.INSTR_WAIT, 100)
	#assembler.put_3(BlipKitAssembler.INSTR_VIBRATO, 0, 0.0, 0)
	#assembler.put_1(BlipKitAssembler.INSTR_NOTE, float(BlipKitTrack.NOTE_RELEASE))
	#assembler.put_0(BlipKitAssembler.INSTR_RETURN)

	#var bytes := assembler.compile()
#
	#print(assembler.get_compile_error())
	return
	var bytes := PackedByteArray()

	var interpreter := BlipKitInterpreterTest.new()
	interpreter.set_instrument_at(0, _instrument)
	interpreter.set_instructions(bytes)

	var stream: AudioStreamBlipKit = _player.stream

	_track.attach(stream)
	_track.add_divider(&"cmd", 1, func () -> int:
		return interpreter.advance(_track)
	)
