extends Control

# Create a track with the default waveform [BlipKitTrack.WAVEFORM_SQUARE].
var _track := BlipKitTrack.new()

# An audio stream player with an [AudioStreamBlipKit] resource.
# Ensure it is playing or has `autoplay` enabled.
@onready var stream_player: AudioStreamPlayer = $AudioStreamPlayer


func _ready() -> void:
	# Get the audio stream.
	var stream: AudioStreamBlipKit = stream_player.stream

	# Attach the track to the stream.
	_track.attach(stream)

	# Reduce volume.
	_track.volume = 0.5


func _on_button_button_down() -> void:
	# Play note.
	_track.note = BlipKitTrack.NOTE_A_3


func _on_button_button_up() -> void:
	# Release note.
	_track.note = BlipKitTrack.NOTE_RELEASE
