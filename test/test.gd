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

	stream.call_synced(func () -> void:
		_init_track()
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

	#ResourceSaver.save(_instrument, "res://instrument.tres")

	var stream: AudioStreamBlipKit = _audio_stream_player.stream

	saw = BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_SAWTOOTH)
	#track.waveform = BlipKitTrack.WAVEFORM_SAWTOOTH
	#track.master_volume = 0.15
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
	saw.attach(stream)

	#print_debug.call_deferred(saw.get_tremolo())

	#prints("has_sequence", saw_instr.has_envelope(BlipKitInstrument.ENVELOPE_PITCH))
	#prints("values", saw_instr.get_envelope_values(BlipKitInstrument.ENVELOPE_PITCH))
	#prints("sustain",
		#saw_instr.get_envelope_sustain_offset(BlipKitInstrument.ENVELOPE_PITCH),
		#saw_instr.get_envelope_sustain_length(BlipKitInstrument.ENVELOPE_PITCH)
	#)

	lead = BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_SQUARE)
	##lead.waveform = BlipKitTrack.WAVEFORM_SQUARE
	#lead.duty_cycle = 2
	##lead.master_volume = 0.0
	#lead.panning = -0.25
	#lead.portamento = 8
#
	#var lead_instr := BlipKitInstrument.new()
	#lead_instr.set_adsr(1, 4, 0.75, 8)
	#lead.instrument = lead_instr
#
	#lead.attach(playback)
	#playback.add_divider(_on_tick_2.bind(lead), 24)

	bass = BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_TRIANGLE)
	#bass.waveform = BlipKitTrack.WAVEFORM_TRIANGLE
	#bass.master_volume = 0.0
	bass.portamento = 4
	bass.pitch = 0.07 # Slightly detune bass to reduce clash with same notes on other tracks.

	var bass_instr := BlipKitInstrument.new()
	bass_instr.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [0, 8], [24, 0], 1, 1)
	bass.instrument = bass_instr

	bass.add_divider(&"beat", 24, _on_tick_3.bind(bass))
	bass.attach(stream)
	#playback.add_divider(_on_tick_3.bind(bass), 24)

	#print_debug.call_deferred("dividers: ", bass.get_divider_names())


#var _index := 0
#var _notes := PackedFloat32Array([
	#12, 12, -1, -1, 15, -1, 19, -1,
	#12, 12, -1, -1, 15, -1, 19, -1,
	#15, 15, -1, -1, 19, -1, 22, -1,
	#14, 14, -1, -1, 18, -1, 21, -1,
#])

#var _index2 := 0
#var _notes2 := PackedFloat32Array([
	#-1, -1, -1, 24, -1, 24, -1, -1, -1, -1, -1, 24, -1, -1, -1, -1,
	#-1, -1, -1, 27, -1, 27, -1, -1, -1, -1, -1, 26, -1, -1, -1, -1,
#])

var _index3 := 0
var _notes3 := PackedFloat32Array([
	12, 12, -1, -1, 12, -1, -1, -1, 12, 12, -1, -1, 12, -1, -1, -1,
	15, 15, -1, -1, 15, -1, -1, -1, 14, 14, -1, -1, 14, -1, -1, -1,
])

#var _notes2 := [
	#-1, -1, -1, -1, [24, 27, 31], -1, [24, 27, 31], -1, -1, -1, -1, -1, [24, 27, 31], -1, -1, -1,
	#-1, -1, -1, -1, [27, 31, 34], -1, [27, 31, 34], -1, -1, -1, -1, -1, [26, 31, 33], -1, -1, -1,
#]

#func _on_tick(track: BlipKitTrack) -> void:
	#track.note = _notes[_index]
	#_index = wrapi(_index + 1, 0, len(_notes))


#func _on_tick_2(track: BlipKitTrack) -> void:
	#track.note = _notes2[_index2]
	#_index2 = wrapi(_index2 + 1, 0, len(_notes2))


func _on_tick_3(track: BlipKitTrack) -> void:
	track.note = _notes3[_index3]
	_index3 = wrapi(_index3 + 1, 0, len(_notes3))


#func _on_tick2(_ticks: int, _track: BlipKitTrack) -> void:
	#var note = _notes2[_index2]
	#_index2 = wrapi(_index2 + 1, 0, len(_notes2))
#
	#if note is Array:
		#var d := {}
		#for n in note:
			#d[n + 12] = 1.0
		#_on_midi_input_notes_changes(d)
#
	#else:
		#_on_midi_input_notes_changes({})


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
