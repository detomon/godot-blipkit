extends Node2D

var _notes: Array[int] = []
var _track1: BlipKitTrack
var _track2: BlipKitTrack
var _track3: BlipKitTrack
var _tracks: Array[BlipKitTrack] = []

@onready var _audio_stream_player: AudioStreamPlayer = %AudioStreamPlayer


func _ready() -> void:
	_audio_stream_player.play()
	_init_track()

	OS.open_midi_inputs()


func _init_track() -> void:
	var playback: AudioStreamBlipKitPlayback = _audio_stream_player.get_stream_playback()

	_track1 = _create_track(BlipKitTrack.WAVEFORM_SQUARE, 12.0)
	#_track1.attach(playback)

	_track2 = _create_track(BlipKitTrack.WAVEFORM_TRIANGLE, -0.07)
	_track2.master_volume = 0.2
	#_track2.attach(playback)

	_track3 = _create_track(BlipKitTrack.WAVEFORM_NOISE, 0.0)
	var ni := BlipKitInstrument.new()
	ni.set_envelope_adsr(0, 16, 0.1, 36)
	_track3.instrument = ni
	_track3.attach(playback)

	#var instrument := BlipKitInstrument.new()
	##instrument.set_sequence_pitch([12, 0], 1, 1)
	#instrument.set_envelope_adsr(2, 8, 0.75, 40)
	#_track1.instrument = instrument

	var instrument2 := BlipKitInstrument.new()
	instrument2.set_envelope_pitch([4, 8], [12.0, 0.0], 1, 1)
	_track2.instrument = instrument2

	_tracks = [_track1, _track2, _track3]


func _create_track(waveform: BlipKitTrack.Waveform, pitch: float) -> BlipKitTrack:
	var track := BlipKitTrack.new()

	#track.master_volume = 0.25
	#track.waveform = BlipKitTrack.WAVEFORM_TRIANGLE
	track.waveform = waveform
	track.duty_cycle = 8
	track.portamento = 8
	track.pitch = pitch

	#track.arpeggio = [0, 3, 7]
	#track.arpeggio_divider = 2

	return track


func _input(event: InputEvent) -> void:
	if event is InputEventMIDI:
		_input_midi_event(event)


func _input_midi_event(event: InputEventMIDI) -> void:
	if not event.velocity:
		_notes.erase(event.pitch)

	elif event.pitch not in _notes:
		_notes.append(event.pitch)

	if not _notes:
		for track in _tracks:
			track.release()

	else:
		for track in _tracks:
			track.note = _notes.back()

		#if len(_notes) > 1:
			#_track.note = _notes.back()
			#var arpeggio := PackedFloat32Array()
			#arpeggio.resize(len(_notes))
			#for i in len(_notes):
				#arpeggio[i] = _notes[i] - _notes.back()
			#_track.arpeggio = arpeggio

		#else:
			#_track.note = _notes[0]
			#_track.arpeggio = []
