extends Node

# Load WAV.
const DRUM := preload("res://examples/shared/samples/drum2.wav")
const SNARE := preload("res://examples/shared/samples/snare1.wav")
const HIHAT := preload("res://examples/shared/samples/hat2.wav")

# Create a track.
var _track := BlipKitTrack.new()
# Create samples.
var _samples: Array[BlipKitSample] = [
	BlipKitSample.create_with_wav(DRUM),
	BlipKitSample.create_with_wav(SNARE),
	BlipKitSample.create_with_wav(HIHAT),
]

# An audio player with an `AudioStreamBlipKit` resource.
@onready var _player: AudioStreamPlayer = $AudioStreamPlayer


func _ready() -> void:
	# Set master volume.
	_track.master_volume = 0.4

	# Get the audio stream.
	var stream: AudioStreamBlipKit = _player.stream
	# Attach the track to the audio stream.
	_track.attach(stream)


func _on_pressed(index: int) -> void:
	# Set the sample from the button index.
	_track.sample = _samples[index]
	# Play note C on octave 4 (original sample speed).
	_track.note = BlipKitTrack.NOTE_C_4
