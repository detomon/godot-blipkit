extends Control

const AAH2: BlipKitWaveform = preload("waveforms/aah.tres")
const INSTRUMENT: BlipKitInstrument = preload("instruments/simple.tres")

var _active_tracks := {}
var _roll: Array[Dictionary] = []
var _playing := false
var _play_index := 0
var _inactive_tracks: Array[BlipKitTrack] = []

@onready var _audio_stream_player: AudioStreamPlayer = %AudioStreamPlayer
@onready var _visualizer: Node2D = %Visualizer
#@onready var _visualizer2: Node2D = %Visualizer2

@onready var _timer: Timer = %Timer
@onready var _progress: HSlider = %Progress


func _ready() -> void:
	_init_track()

	#ResourceSaver.save(_waveform, "res://aah.tres")

	#INSTRUMENT.set_adsr(0, 12, 0.5, 24)
	#INSTRUMENT.set_envelope(BlipKitInstrument.ENVELOPE_VOLUME, [4, 12, 24], [1.0, 0.5, 0.0], 1, 1)
	#INSTRUMENT.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [], [12, 0, -0.2], 1, 1)
	#INSTRUMENT.set_envelope(BlipKitInstrument.ENVELOPE_DUTY_CYCLE, [], [8, 8, 0, 2], 2, 1)
	#INSTRUMENT.set_meta(&"name", "MIDI")
	#ResourceSaver.save(INSTRUMENT)

	#prints(INSTRUMENT)
	#prints(AAH)


func _process(_delta: float) -> void:
	_progress.value = 1.0 - (_timer.time_left / _timer.wait_time)

	if _playing:
		if _play_index >= len(_roll):
			return

		var time := _timer.wait_time - _timer.time_left
		var item := _roll[_play_index]

		while time >= item.time:
			var notes: Dictionary = item.notes
			_on_midi_input_notes_changes(notes)

			_play_index += 1
			if _play_index >= len(_roll):
				break

			item = _roll[_play_index]


#func _input(event: InputEvent) -> void:
	#if event.is_action_pressed(&"ui_accept"):
		#for note in _active_tracks:
			#var track: BlipKitTrack = _active_tracks[note]
			#track.set_tremolo(5, 0.75, 0)
			#track.set_tremolo(40, 0.75, 240)
			#track.set_vibrato(30, 0.1)


func _init_track() -> void:
	#var waveform := BlipKitWaveform.create_with_frames([
		#-255, -163, -154, -100, 45, 127, 9, -163,
		#-163, -27, 63, 72, 63, 9, -100, -154,
		#-127, -91, -91, -91, -91, -127, -154, -100,
		#45, 127, 9, -163, -163, 9, 127, 45,
	#], true, 0.5)

	INSTRUMENT.set_envelope(BlipKitInstrument.ENVELOPE_DUTY_CYCLE, [], [8, 0, 2], 1, 1)

	#ResourceSaver.save(_instrument, "res://instrument.tres")

	var playback: AudioStreamBlipKitPlayback = _audio_stream_player.get_stream_playback()

	var saw := BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_SAWTOOTH)
	#track.waveform = BlipKitTrack.WAVEFORM_SAWTOOTH
	#track.master_volume = 0.15
	saw.portamento = 8
#
	var saw_instr := BlipKitInstrument.new()
	saw_instr.set_adsr(0, 0, 1.0, 12)
	saw_instr.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [], [24, 0, 12], 1, 1)
	saw.instrument = saw_instr
	#saw.custom_waveform = waveform
	saw.attach(playback)
	playback.add_tick_function(_on_tick.bind(saw), 24)

	print_debug(saw.get_tremolo())

	prints("has_sequence", saw_instr.has_envelope(BlipKitInstrument.ENVELOPE_PITCH))
	prints("values", saw_instr.get_envelope_values(BlipKitInstrument.ENVELOPE_PITCH))
	prints("sustain",
		saw_instr.get_envelope_sustain_offset(BlipKitInstrument.ENVELOPE_PITCH),
		saw_instr.get_envelope_sustain_length(BlipKitInstrument.ENVELOPE_PITCH)
	)

	#var lead := BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_SQUARE)
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
	#playback.add_tick_function(_on_tick_2.bind(lead), 24)

	var bass := BlipKitTrack.create_with_waveform(BlipKitTrack.WAVEFORM_TRIANGLE)
	#bass.waveform = BlipKitTrack.WAVEFORM_TRIANGLE
	#bass.master_volume = 0.0
	bass.portamento = 4
	bass.pitch = 0.07 # Slightly detune bass to reduce clash with same notes on other tracks.

	var bass_instr := BlipKitInstrument.new()
	bass_instr.set_envelope(BlipKitInstrument.ENVELOPE_PITCH, [0, 8], [24, 0], 1, 1)
	bass.instrument = bass_instr

	bass.attach(playback)
	playback.add_tick_function(_on_tick_3.bind(bass), 24)


var _index := 0
var _notes := PackedFloat32Array([
	12, 12, -1, -1, 15, -1, 19, -1,
	12, 12, -1, -1, 15, -1, 19, -1,
	15, 15, -1, -1, 19, -1, 22, -1,
	14, 14, -1, -1, 18, -1, 21, -1,
])

var _index2 := 0
var _notes2 := PackedFloat32Array([
	-1, -1, -1, 24, -1, 24, -1, -1, -1, -1, -1, 24, -1, -1, -1, -1,
	-1, -1, -1, 27, -1, 27, -1, -1, -1, -1, -1, 26, -1, -1, -1, -1,
])

var _index3 := 0
var _notes3 := PackedFloat32Array([
	12, 12, -1, -1, 12, -1, -1, -1, 12, 12, -1, -1, 12, -1, -1, -1,
	15, 15, -1, -1, 15, -1, -1, -1, 14, 14, -1, -1, 14, -1, -1, -1,
])

#var _notes2 := [
	#-1, -1, -1, -1, [24, 27, 31], -1, [24, 27, 31], -1, -1, -1, -1, -1, [24, 27, 31], -1, -1, -1,
	#-1, -1, -1, -1, [27, 31, 34], -1, [27, 31, 34], -1, -1, -1, -1, -1, [26, 31, 33], -1, -1, -1,
#]

func _on_tick(_ticks: int, track: BlipKitTrack) -> void:
	track.note = _notes[_index]
	_index = wrapi(_index + 1, 0, len(_notes))


func _on_tick_2(_ticks: int, track: BlipKitTrack) -> void:
	track.note = _notes2[_index2]
	_index2 = wrapi(_index2 + 1, 0, len(_notes2))


func _on_tick_3(_ticks: int, track: BlipKitTrack) -> void:
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
	var playback: AudioStreamBlipKitPlayback = _audio_stream_player.get_stream_playback()
	track.attach(playback)


func _on_midi_input_notes_changes(notes: Dictionary) -> void:
	for note: int in notes:
		if note not in _active_tracks:
			var track: BlipKitTrack = _inactive_tracks.pop_back()
			if not track:
				track = BlipKitTrack.new()
				_attach(track)
			track.duty_cycle = 4
			#track.set_tremolo(24, 0.2)
			track.set_vibrato(12, 0.1)
			#track.arpeggio = [0, 12]
			#track.arpeggio_divider = 8
			track.instrument = INSTRUMENT
			track.custom_waveform = AAH2

			_active_tracks[note] = track

	for note: int in _active_tracks:
		if note not in notes:
			var track: BlipKitTrack = _active_tracks[note]
			track.release()
			#track.detach()
			_inactive_tracks.append(track)
			_active_tracks.erase(note)

	for note: int in _active_tracks:
		var track: BlipKitTrack = _active_tracks[note]

		track.note = note
		#if note >= 12 and note < 24:
			#track.master_volume = 0.2
			#track.waveform = BlipKitTrack.WAVEFORM_TRIANGLE
			#track.note = note
#
		#else:
			#track.master_volume = 0.1
			#track.waveform = BlipKitTrack.WAVEFORM_SAWTOOTH
			#track.note = note

	#_visualizer2.notes = PackedInt32Array(notes.keys())

	var values := PackedInt32Array(notes.keys())
	_visualizer.set_deferred(&"notes", values)

	#var time := _timer.wait_time - _timer.time_left

	#if not _playing:
		#_roll.append({
			#time = time,
			#notes = notes.duplicate(),
		#})

	#var last := {}
	#if _roll:
		#last = _roll.back()
#
	#if last and last.time == time:
		#last.notes.merge(notes)
#
	#else:
		#_roll.append({
			#time = time,
			#notes = notes.duplicate(),
		#})


func _on_record_button_pressed() -> void:
	_roll.clear()
	_playing = false
	_timer.start()

	_on_midi_input_notes_changes({})


func _on_play_button_pressed() -> void:
	_playing = true
	_play_index = 0
	_timer.start()

	print(_roll)


func _on_timer_timeout() -> void:
	_play_index = 0
