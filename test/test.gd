extends Control

var saw := BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_SAWTOOTH)
var bass := BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_TRIANGLE)
var noise := BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_NOISE)
var saw_interp := BlipKitInterpreter.new()
var bass_interp := BlipKitInterpreter.new()
var noise_interp := BlipKitInterpreter.new()

@onready var _audio_stream_player: AudioStreamPlayer = %AudioStreamPlayer


func _ready() -> void:
	_init_saw(saw, saw_interp)
	_init_bass(bass, bass_interp)
	_init_noise(noise, noise_interp)

	var stream: AudioStreamBlipKit = _audio_stream_player.stream

	# Ensure tracks are attached at the same time.
	stream.call_synced(func () -> void:
		saw.attach(stream)
		bass.attach(stream)
		noise.attach(stream)
	)


func _init_saw(track: BlipKitTrack, interp: BlipKitInterpreter) -> void:
	var assem := BlipKitAssembler.new()

	assem.put(BlipKitAssembler.OP_VOLUME, 0.85)

	assem.put_label("start")

	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "b")
	assem.put(BlipKitAssembler.OP_CALL, "c")

	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "b")
	assem.put(BlipKitAssembler.OP_CALL, "c")

	assem.put(BlipKitAssembler.OP_JUMP, "start")

	assem.put_label("a")

	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_C_1))
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_D_SH_1))
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_G_1))
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("b")

	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_D_SH_1))
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_G_1))
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_A_SH_1))
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("c")

	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_D_1))
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_F_SH_1))
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_A_1))
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_RETURN)

	if assem.compile() != BlipKitAssembler.OK:
		printerr(assem.get_error_message())
		return

	var bytes := assem.get_byte_code()
	interp.load_byte_code(bytes)

	var instr := BlipKitInstrument.new()
	instr.set_envelope(BlipKitInstrument.ENVELOPE_VOLUME, [0, 24, 36, 24, 12], [1.0, 1.0, 0.25, 0.25, 0.0], 1, 1)
	instr.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [], [24, 24, 0, 12], 2, 1)

	track.instrument = instr

	track.add_divider("run", 1, func () -> int:
		return interp.advance(track)
	)


func _init_bass(track: BlipKitTrack, interp: BlipKitInterpreter) -> void:
	var assem := BlipKitAssembler.new()

	# Slightly detune bass to reduce clash with same notes on other tracks.
	assem.put(BlipKitAssembler.OP_PITCH, 0.07)

	assem.put_label("start")

	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "b")
	assem.put(BlipKitAssembler.OP_CALL, "c")

	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "b")
	assem.put(BlipKitAssembler.OP_CALL, "c2")

	assem.put(BlipKitAssembler.OP_JUMP, "start")

	assem.put_label("a")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 0)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_C_1))
	assem.put(BlipKitAssembler.OP_TICK, 36)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 60)
	assem.put(BlipKitAssembler.OP_INSTRUMENT, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_C_1))
	assem.put(BlipKitAssembler.OP_TICK, 36)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 60)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("b")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 0)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_D_SH_1))
	assem.put(BlipKitAssembler.OP_TICK, 36)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 60)
	assem.put(BlipKitAssembler.OP_INSTRUMENT, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_D_SH_1))
	assem.put(BlipKitAssembler.OP_TICK, 36)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 60)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("c")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 0)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_D_1))
	assem.put(BlipKitAssembler.OP_TICK, 36)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 60)
	assem.put(BlipKitAssembler.OP_INSTRUMENT, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_D_1))
	assem.put(BlipKitAssembler.OP_TICK, 36)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 60)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("c2")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 0)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_D_1))
	assem.put(BlipKitAssembler.OP_TICK, 36)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 12)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_D_1))
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 24)
	assem.put(BlipKitAssembler.OP_INSTRUMENT, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_D_1))
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_RETURN)

	if assem.compile() != BlipKitAssembler.OK:
		printerr(assem.get_error_message())
		return

	var bytes := assem.get_byte_code()
	interp.load_byte_code(bytes)

	var drum := BlipKitInstrument.new()
	drum.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [0, 16], [24.0, 0.0], 1, 1)
	interp.set_instrument(0, drum)

	var drum2 := BlipKitInstrument.new()
	drum2.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [], [24.0, 0.0], 1, 1)
	interp.set_instrument(1, drum2)

	track.add_divider("run", 1, func () -> int:
		return interp.advance(track)
	)


func _init_noise(track: BlipKitTrack, interp: BlipKitInterpreter) -> void:
	var assem := BlipKitAssembler.new()

	assem.put(BlipKitAssembler.OP_VOLUME, 0.5)

	assem.put_label("start")

	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "a")

	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "a")
	assem.put(BlipKitAssembler.OP_CALL, "b")

	assem.put(BlipKitAssembler.OP_JUMP, "start")

	assem.put_label("snare")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 0)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_F_5))
	assem.put(BlipKitAssembler.OP_TICK, 36)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_TICK, 12)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("hihat")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, float(BlipKitTrack.NOTE_F_6))
	assem.put(BlipKitAssembler.OP_TICK, 48)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("a")

	assem.put(BlipKitAssembler.OP_CALL, "hihat")
	assem.put(BlipKitAssembler.OP_CALL, "hihat")
	assem.put(BlipKitAssembler.OP_CALL, "snare")
	assem.put(BlipKitAssembler.OP_CALL, "hihat")
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("b")

	assem.put(BlipKitAssembler.OP_CALL, "hihat")
	assem.put(BlipKitAssembler.OP_CALL, "hihat")
	assem.put(BlipKitAssembler.OP_CALL, "snare")
	assem.put(BlipKitAssembler.OP_CALL, "snare")
	assem.put(BlipKitAssembler.OP_RETURN)

	if assem.compile() != BlipKitAssembler.OK:
		printerr(assem.get_error_message())
		return

	var snare := BlipKitInstrument.create_with_adsr(0, 24, 0.5, 12)
	snare.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [0, 24], [0.0, -12.0], 1, 1)
	interp.set_instrument(0, snare)

	var hihat := BlipKitInstrument.create_with_adsr(0, 12, 0.0, 0)
	interp.set_instrument(1, hihat)

	var bytes := assem.get_byte_code()
	interp.load_byte_code(bytes)

	track.add_divider("run", 1, func () -> int:
		return interp.advance(track)
	)
