extends Control

var saw := BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_SAWTOOTH)
var bass := BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_TRIANGLE)
var noise := BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_NOISE)
var saw_interp := BlipKitInterpreter.new()
var bass_interp := BlipKitInterpreter.new()
var noise_interp := BlipKitInterpreter.new()

@onready var _audio_stream_player: AudioStreamPlayer = %AudioStreamPlayer


func _ready() -> void:
	var assem := _init_byte_code()

	if assem.compile() != BlipKitAssembler.OK:
		printerr(assem.get_error_message())
		return

	var bytecode := assem.get_byte_code()

	_init_saw(saw, bytecode, saw_interp)
	_init_bass(bass, bytecode, bass_interp)
	_init_noise(noise, bytecode, noise_interp)

	var stream: AudioStreamBlipKit = _audio_stream_player.stream

	# Ensure tracks are attached at the same time.
	stream.call_synced(func () -> void:
		saw.attach(stream)
		bass.attach(stream)
		noise.attach(stream)
	)


func _init_byte_code() -> BlipKitAssembler:
	var assem := BlipKitAssembler.new()

	# --- LEAD ---

	assem.put_label("saw:init", true)

	assem.put(BlipKitAssembler.OP_VOLUME, 0.85)
	assem.put(BlipKitAssembler.OP_INSTRUMENT, 0)

	assem.put_label("saw:start")

	assem.put(BlipKitAssembler.OP_CALL, "saw:a")
	assem.put(BlipKitAssembler.OP_CALL, "saw:a")
	assem.put(BlipKitAssembler.OP_CALL, "saw:b")
	assem.put(BlipKitAssembler.OP_CALL, "saw:c")

	assem.put(BlipKitAssembler.OP_CALL, "saw:a")
	assem.put(BlipKitAssembler.OP_CALL, "saw:a")
	assem.put(BlipKitAssembler.OP_CALL, "saw:b")
	assem.put(BlipKitAssembler.OP_CALL, "saw:c")

	assem.put(BlipKitAssembler.OP_JUMP, "saw:start")

	assem.put_label("saw:a")

	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_C_1)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_D_SH_1)
	assem.put(BlipKitAssembler.OP_STEP, 1)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_G_1)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 0.33)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("saw:b")

	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_D_SH_1)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_G_1)
	assem.put(BlipKitAssembler.OP_STEP, 1)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_A_SH_1)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 0.33)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("saw:c")

	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_D_1)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_F_SH_1)
	assem.put(BlipKitAssembler.OP_STEP, 1)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_A_1)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 0.33)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_RETURN)

	# --- BASS ---

	assem.put_label("bass:init", true)

	# Slightly detune bass to reduce clash with same notes on other tracks.
	assem.put(BlipKitAssembler.OP_PITCH, 0.07)

	assem.put_label("bass:start")

	assem.put(BlipKitAssembler.OP_CALL, "bass:a")
	assem.put(BlipKitAssembler.OP_CALL, "bass:a")
	assem.put(BlipKitAssembler.OP_CALL, "bass:b")
	assem.put(BlipKitAssembler.OP_CALL, "bass:c")

	assem.put(BlipKitAssembler.OP_CALL, "bass:a")
	assem.put(BlipKitAssembler.OP_CALL, "bass:a")
	assem.put(BlipKitAssembler.OP_CALL, "bass:b")
	assem.put(BlipKitAssembler.OP_CALL, "bass:c2")

	assem.put(BlipKitAssembler.OP_JUMP, "bass:start")

	assem.put_label("bass:a")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 0)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_C_1)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 1.5)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 4)
	assem.put(BlipKitAssembler.OP_INSTRUMENT, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_C_1)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 1.5)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 4)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("bass:b")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 0)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_D_SH_1)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 1.5)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 4)
	assem.put(BlipKitAssembler.OP_INSTRUMENT, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_D_SH_1)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 1.5)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 4)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("bass:c")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 0)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_D_1)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 1.5)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 4)
	assem.put(BlipKitAssembler.OP_INSTRUMENT, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_D_1)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 1.5)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 4)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("bass:c2")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 0)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_D_1)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 1.5)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 0.5)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_D_1)
	assem.put(BlipKitAssembler.OP_STEP, 1)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 1)
	assem.put(BlipKitAssembler.OP_INSTRUMENT, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_D_1)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_RETURN)

	# --- NOISE ---

	assem.put_label("noise:init", true)

	assem.put(BlipKitAssembler.OP_VOLUME, 0.6)

	assem.put_label("noise:start")

	assem.put(BlipKitAssembler.OP_CALL, "noise:a")
	assem.put(BlipKitAssembler.OP_CALL, "noise:a")
	assem.put(BlipKitAssembler.OP_CALL, "noise:a")
	assem.put(BlipKitAssembler.OP_CALL, "noise:a")

	assem.put(BlipKitAssembler.OP_CALL, "noise:a")
	assem.put(BlipKitAssembler.OP_CALL, "noise:a")
	assem.put(BlipKitAssembler.OP_CALL, "noise:a")
	assem.put(BlipKitAssembler.OP_CALL, "noise:b")

	assem.put(BlipKitAssembler.OP_JUMP, "noise:start")

	assem.put_label("noise:snare")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 0)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_F_5)
	assem.put(BlipKitAssembler.OP_DELAY_STEP, 1.5)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("noise:hihat")

	assem.put(BlipKitAssembler.OP_INSTRUMENT, 1)
	assem.put(BlipKitAssembler.OP_ATTACK, BlipKitTrack.NOTE_F_6)
	assem.put(BlipKitAssembler.OP_STEP, 2)
	assem.put(BlipKitAssembler.OP_RELEASE)
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("noise:a")

	assem.put(BlipKitAssembler.OP_CALL, "noise:hihat")
	assem.put(BlipKitAssembler.OP_CALL, "noise:hihat")
	assem.put(BlipKitAssembler.OP_CALL, "noise:snare")
	assem.put(BlipKitAssembler.OP_CALL, "noise:hihat")
	assem.put(BlipKitAssembler.OP_RETURN)

	assem.put_label("noise:b")

	assem.put(BlipKitAssembler.OP_CALL, "noise:hihat")
	assem.put(BlipKitAssembler.OP_CALL, "noise:hihat")
	assem.put(BlipKitAssembler.OP_CALL, "noise:snare")
	assem.put(BlipKitAssembler.OP_CALL, "noise:snare")
	assem.put(BlipKitAssembler.OP_RETURN)

	return assem


func _init_saw(track: BlipKitTrack, bytecode: BlipKitBytecode, interp: BlipKitInterpreter) -> void:
	var lead := BlipKitInstrument.new()
	lead.set_envelope(BlipKitInstrument.ENVELOPE_VOLUME, [1.0, 1.0, 0.25, 0.25, 0.0], [0, 24, 36, 24, 12], 1, 1)
	lead.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [24, 24, 0, 12], [], 2, 1)
	interp.set_instrument(0, lead)

	interp.load_byte_code(bytecode, "saw:init")

	track.add_divider(1, func () -> int:
		return interp.advance(track)
	)


func _init_bass(track: BlipKitTrack, bytecode: BlipKitBytecode, interp: BlipKitInterpreter) -> void:
	var drum1 := BlipKitInstrument.new()
	drum1.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [24.0, 0.0], [0, 16], 1, 1)
	interp.set_instrument(0, drum1)

	var drum2 := BlipKitInstrument.new()
	drum2.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [24.0, 0.0], [], 1, 1)
	interp.set_instrument(1, drum2)

	interp.load_byte_code(bytecode, "bass:init")

	track.add_divider(1, func () -> int:
		return interp.advance(track)
	)


func _init_noise(track: BlipKitTrack, bytecode: BlipKitBytecode, interp: BlipKitInterpreter) -> void:
	var snare := BlipKitInstrument.create_with_adsr(0, 24, 0.5, 12)
	snare.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [0.0, -12.0], [0, 24], 1, 1)
	interp.set_instrument(0, snare)

	var hihat := BlipKitInstrument.create_with_adsr(0, 12, 0.0, 0)
	interp.set_instrument(1, hihat)

	interp.load_byte_code(bytecode, "noise:init")

	track.add_divider(1, func () -> int:
		return interp.advance(track)
	)
