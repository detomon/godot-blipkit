@tool
extends "../resource_editor.gd"

@onready var _frame_count: SpinBox = %FrameCount
@onready var _edit_button: MenuButton = %EditButton
@onready var _presets_button: MenuButton = %PresetsButton
@onready var _snap_button: Button = %SnapButton
@onready var _snap_steps: SpinBox = %SnapSteps
@onready var _play_button: Button = %PlayButton
@onready var _waveform_editor: Control = %WaveformEditor
@onready var _audio_stream_player: AudioStreamPlayer = %AudioStreamPlayer


var waveform: BlipKitWaveform: set = set_waveform

var _track := BlipKitTrack.new()
var _theme_cache := {
	snap_icon = null,
	play_icon = null,
}


func _init() -> void:
	theme_changed.connect(_update_theme)


func _enter_tree() -> void:
	_update_theme_cache()


func _ready() -> void:
	_edit_button.get_popup().id_pressed.connect(_on_edit_id_pressed)
	_presets_button.get_popup().id_pressed.connect(_on_presets_id_pressed)

	_snap_steps.visible = false

	_update_theme_icons()


func set_waveform(value: BlipKitWaveform) -> void:
	waveform = value

	if waveform:
		_frame_count.value = len(waveform.frames)


func _update_theme() -> void:
	_update_theme_cache()
	_update_theme_icons()


func _update_theme_cache() -> void:
	if has_theme_icon(&"SnapGrid", &"EditorIcons"):
		_theme_cache.snap_icon = get_theme_icon(&"SnapGrid", &"EditorIcons")
	if has_theme_icon(&"Play", &"EditorIcons"):
		_theme_cache.play_icon = get_theme_icon(&"Play", &"EditorIcons")


func _update_theme_icons() -> void:
	if _snap_button:
		_snap_button.icon = _theme_cache.snap_icon
	if _play_button:
		_play_button.icon = _theme_cache.play_icon


func _handles(object: Object) -> bool:
	return object is BlipKitWaveform


func _edit(object: Object) -> void:
	waveform = object


func _get_panel_title() -> String:
	return "BlipKitWaveform"


func _on_frame_count_value_changed(value: float) -> void:
	pass


func _on_snap_button_toggled(toggled_on: bool) -> void:
	_snap_steps.visible = toggled_on
	_waveform_editor.snap_steps = _snap_steps.value \
		if toggled_on \
		else 0


func _on_snap_steps_value_changed(value: float) -> void:
	_waveform_editor.snap_steps = _snap_steps.value


func _on_play_button_button_down() -> void:
	_audio_stream_player.play()

	var playback: AudioStreamBlipKitPlayback = _audio_stream_player.get_stream_playback()
	_track.attach(playback)
	 # Set custom waveform after attaching.
	_track.custom_waveform = waveform
	_track.note = BlipKitTrack.NOTE_A_3


func _on_play_button_button_up() -> void:
	_track.release()
	_track.detach()
	_audio_stream_player.stop()


func _on_edit_id_pressed(id: int) -> void:
	match id:
		0:
			pass


func _on_presets_id_pressed(id: int) -> void:
	match id:
		0:
			pass

		1:
			pass

		2:
			pass

		3:
			pass
