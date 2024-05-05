extends Control

const AAH: BlipKitWaveform = preload("aah.tres")
const INSTRUMENT: BlipKitInstrument = preload("instrument.tres")

var _active_tracks := {}
var _roll: Array[Dictionary] = []
var _playing := false
var _play_index := 0
var _inactive_tracks: Array[BlipKitTrack] = []
#var _instrument := BlipKitInstrument.new()
#var _waveform := BlipKitWaveform.new()

@onready var _audio_stream_player: AudioStreamPlayer = %AudioStreamPlayer
@onready var _visualizer: Node2D = %Visualizer
@onready var _visualizer2: Node2D = %Visualizer2

@onready var _timer: Timer = %Timer
@onready var _progress: HSlider = %Progress


func _ready() -> void:
	_init_track()

	#_waveform.set_frames_normalized([
		#-255, -163, -154, -100, 45, 127, 9, -163, -163,
		#-27, 63, 72, 63, 9, -100, -154, -127,
		#-91, -91, -91, -91, -127, -154, -100, 45,
		#127, 9, -163, -163, 9, 127, 45,
	#])

	#ResourceSaver.save(_waveform, "res://aah.tres")


func _process(_delta: float) -> void:
	_progress.value = 1.0 - (_timer.time_left / _timer.wait_time)

	if _playing:
		if _play_index >= len(_roll):
			return

		var time := _timer.wait_time - _timer.time_left
		var item := _roll[_play_index]

		while time >= item.time:
			_on_midi_input_notes_changes(item.notes)

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
	INSTRUMENT.set_sequence_duty_cycle([8, 0, 2], 1, 1)

	#ResourceSaver.save(_instrument, "res://instrument.tres")

	var playback: AudioStreamBlipKitPlayback = _audio_stream_player.get_stream_playback()
	var track := BlipKitTrack.new()
	track.waveform = BlipKitTrack.WAVEFORM_TRIANGLE
	track.master_volume = 0.3
	track.portamento = 8
	track.attach(playback)

	playback.add_tick_function(_on_tick.bind(track), 24)


var _index := 0
var _notes := PackedFloat32Array([
	12, 12, -1, -1, 15, -1, 19, -1,
	12, 12, -1, -1, 15, -1, 19, -1,
	15, 15, -1, -1, 19, -1, 22, -1,
	14, 14, -1, -1, 18, -1, 21, -1,
])

func _on_tick(ticks: int, track: BlipKitTrack) -> void:
	track.note = _notes[_index]
	_index = wrapi(_index + 1, 0, len(_notes))

	#prints("ticks", ticks)


func _attach(track: BlipKitTrack) -> void:
	var playback: AudioStreamBlipKitPlayback = _audio_stream_player.get_stream_playback()
	track.attach(playback)


func _on_midi_input_notes_changes(notes: Dictionary) -> void:
	for note in notes:
		if note not in _active_tracks:
			var track: BlipKitTrack = _inactive_tracks.pop_back()
			if not track:
				track = BlipKitTrack.new()
				_attach(track)
			track.duty_cycle = 4
			#track.set_tremolo(16, 0.2)
			#track.arpeggio = [0, 12]
			#track.arpeggio_divider = 8
			track.instrument = INSTRUMENT
			#track.custom_waveform = AAH

			_active_tracks[note] = track

	for note in _active_tracks:
		if note not in notes:
			var track: BlipKitTrack = _active_tracks[note]
			track.release()
			#track.detach()
			_inactive_tracks.append(track)
			_active_tracks.erase(note)

	for note in _active_tracks:
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

	_visualizer.notes = PackedInt32Array(notes.keys())

	var time := _timer.wait_time - _timer.time_left

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
