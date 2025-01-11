extends Control

const AAH2: BlipKitWaveform = preload("waveforms/aah.tres")
const INSTRUMENT: BlipKitInstrument = preload("instruments/simple.tres")

var _active_tracks := {}
var _inactive_tracks: Array[BlipKitTrack] = []

var bass: BlipKitTrack
var saw: BlipKitTrack
var lead: BlipKitTrack

@onready var _audio_stream_player: AudioStreamPlayer = %AudioStreamPlayer

var interp := BlipKitInterpreter.new()

func _ready() -> void:
	var stream: AudioStreamBlipKit = _audio_stream_player.stream

	_init_track()

	stream.call_synced(func () -> void:
		saw.attach(stream)
		bass.attach(stream)
	)


func _init_track() -> void:
	var assem := BlipKitAssembler.new()

	#assem.put_code("a:c1;s:2;r;s:2;a:d#1;s:1;r;s:1;a:g1;s:2;r;s:1")
	#assem.put_code("a:c1;s:2;r;s:2;a:d#1;s:1;r;s:1;a:g1;s:2;r;s:1")
	#assem.put_code("a:d#1;s:2;r;s:2;a:g1;s:1;r;s:1;a:b2;s:2;r;s:1")
	#assem.put_code("a:d1;s:2;r;s:2;a:f#1;s:1;r;s:1;a:a2;s:2;r;s:1")

	assem.put(BlipKitAssembler.OP_JUMP, "start")

	assem.put_label("part1")
	assem.put(BlipKitAssembler.OP_STORE, 0, 0)
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

	assem.put_label("start")

	assem.put(BlipKitAssembler.OP_CALL, "part1")
	assem.put(BlipKitAssembler.OP_CALL, "part1")

	assem.put(BlipKitAssembler.OP_STORE, 0, 2)
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

	assem.put(BlipKitAssembler.OP_STORE, 0, 3)
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

	assem.put(BlipKitAssembler.OP_JUMP, "start")

	assem.compile()

	var bytes := assem.get_byte_code()
	interp.load_byte_code(bytes)
	prints(len(bytes), bytes)

	INSTRUMENT.set_envelope(BlipKitInstrument.ENVELOPE_DUTY_CYCLE, [], [8, 0, 2], 1, 1)

	var stream: AudioStreamBlipKit = _audio_stream_player.stream

	saw = BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_SAWTOOTH)
	saw.portamento = 8

	var saw_instr := BlipKitInstrument.create_with_adsr(0, 0, 1.0, 12)
	saw_instr.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [], [24, 0, 12], 1, 1)
	saw.instrument = saw_instr
	#saw.custom_waveform = preload("res://test/waveforms/aah2.tres")

	saw.add_divider("run", 1, func () -> int:
		var result := interp.advance(saw)
		# Remove divider when finished.
		if result == 0:
			return -1
		return result
	)

	lead = BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_SQUARE)

	bass = BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_TRIANGLE)
	bass.portamento = 4
	bass.pitch = 0.07 # Slightly detune bass to reduce clash with same notes on other tracks.

	var bass_instr := BlipKitInstrument.new()
	bass_instr.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [0, 8], [24, 0], 1, 1)
	bass.instrument = bass_instr

	bass.add_divider(&"beat", 24, _on_tick_3.bind(bass))


var _index3 := 0
var _notes3 := PackedFloat32Array([
	12, 12, -1, -1, 12, -1, -1, -1, 12, 12, -1, -1, 12, -1, -1, -1,
	15, 15, -1, -1, 15, -1, -1, -1, 14, 14, -1, -1, 14, -1, -1, -1,
])

func _on_tick_3(track: BlipKitTrack) -> void:
	track.note = _notes3[_index3]
	_index3 = wrapi(_index3 + 1, 0, len(_notes3))


func _attach(track: BlipKitTrack) -> void:
	var stream: AudioStreamBlipKit = _audio_stream_player.stream
	track.attach(stream)


func _on_midi_input_notes_changes(notes: Dictionary) -> void:
	for note: int in notes:
		if note not in _active_tracks:
			var track: BlipKitTrack = _inactive_tracks.pop_back()
			if not track:
				track = BlipKitTrack.new()
				_attach(track)
			track.duty_cycle = 4
			track.set_vibrato(12, 0.1)
			track.instrument = INSTRUMENT
			track.custom_waveform = AAH2

			_active_tracks[note] = track

	for note: int in _active_tracks:
		if note not in notes:
			var track: BlipKitTrack = _active_tracks[note]
			track.release()
			_inactive_tracks.append(track)
			_active_tracks.erase(note)

	for note: int in _active_tracks:
		var track: BlipKitTrack = _active_tracks[note]
		track.note = note
