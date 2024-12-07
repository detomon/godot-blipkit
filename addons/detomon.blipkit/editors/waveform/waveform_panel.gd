@tool
extends "../resource_editor.gd"

const WaveformEditor := preload("waveform_editor.gd")

@export var snap_steps_visible := false: set = set_snap_steps_visible
@export var snap_steps := 0: set = set_snap_steps

var editor_lock := false: set = set_editor_lock
var waveform: BlipKitWaveform: set = set_waveform

var _track := BlipKitTrack.new()
var _instrument := BlipKitInstrument.new()


@onready var _lock_button: Button = %LockButton
@onready var _frame_count: SpinBox = %FrameCount
@onready var _edit_button: MenuButton = %EditButton
@onready var _presets_button: MenuButton = %PresetsButton
@onready var _snap_button: Button = %SnapButton
@onready var _snap_steps: SpinBox = %SnapSteps
@onready var _play_button: Button = %PlayButton
@onready var _frame_values: LineEdit = %FrameValues
@onready var _waveform_editor: WaveformEditor = %WaveformEditor
@onready var _test_audio: AudioStreamPlayer = %TestAudio


func _enter_tree() -> void:
	set_editor_lock(editor_lock)
	_update_theme_cache()


func _ready() -> void:
	super()

	_instrument.set_adsr(8, 0, 1.0, 8)

	_edit_button.get_popup().id_pressed.connect(_on_edit_id_pressed)
	_presets_button.get_popup().id_pressed.connect(_on_presets_id_pressed)
	_waveform_editor.undo_redo = undo_redo

	set_snap_steps_visible(snap_steps_visible)
	set_snap_steps(snap_steps)


func _handles(object: Object) -> bool:
	return object is BlipKitWaveform


func _edit(object: Object) -> void:
	waveform = object


func _get_panel_title() -> String:
	return "BlipKit Waveform"


func set_editor_lock(value: bool) -> void:
	editor_lock = value
	_update_editor_lock()


func set_waveform(value: BlipKitWaveform) -> void:
	if waveform:
		waveform.changed.disconnect(_on_waveform_changed)
	waveform = value
	if waveform:
		waveform.changed.connect(_on_waveform_changed)

	if waveform:
		editor_lock = waveform.get_meta(&"editor_lock", false)
		_waveform_editor.frames = waveform.frames
		_frame_count.value = waveform.size()

	_update_frame_values()


func set_snap_steps_visible(value: bool) -> void:
	snap_steps_visible = value

	if _snap_steps:
		_snap_steps.visible = snap_steps_visible
	if _waveform_editor:
		_waveform_editor.snap_steps = int(_snap_steps.value) \
			if snap_steps_visible \
			else 0


func set_snap_steps(value: int) -> void:
	snap_steps = value
	if _waveform_editor:
		_waveform_editor.snap_steps = value


func _update_theme_cache() -> void:
	if has_theme_icon(&"SnapGrid", &"EditorIcons"):
		_theme_cache.snap_icon = get_theme_icon(&"SnapGrid", &"EditorIcons")
	if has_theme_icon(&"Play", &"EditorIcons"):
		_theme_cache.play_icon = get_theme_icon(&"Play", &"EditorIcons")
	if has_theme_icon(&"Lock", &"EditorIcons"):
		_theme_cache.lock_icon = get_theme_icon(&"Lock", &"EditorIcons")
	if has_theme_icon(&"Unlock", &"EditorIcons"):
		_theme_cache.unlock_icon = get_theme_icon(&"Unlock", &"EditorIcons")


func _update_theme_icons() -> void:
	if not is_inside_tree():
		return

	_snap_button.icon = _theme_cache.snap_icon
	_play_button.icon = _theme_cache.play_icon
	_lock_button.icon = _theme_cache.lock_icon \
		if editor_lock \
		else _theme_cache.unlock_icon


func _update_editor_lock() -> void:
	if not is_inside_tree():
		return

	var locked: bool = waveform.get_meta(&"editor_lock", false)

	_waveform_editor.editor_lock = locked
	_lock_button.set_pressed_no_signal(locked)
	_lock_button.icon = _theme_cache.lock_icon \
		if locked \
		else _theme_cache.unlock_icon
	_frame_values.editable = not locked
	_frame_count.editable = not locked
	_edit_button.disabled = locked
	_presets_button.disabled = locked


func _update_frame_values() -> void:
	if not _frame_values:
		return

	if not waveform:
		_frame_values.text = ""
		return

	var frames := waveform.frames
	var values := PackedStringArray()
	values.resize(len(frames))

	for i in len(frames):
		values[i] = str(roundi(frames[i] * 255.0))

	_frame_values.text = ", ".join(values)


func _waveform_set_frames_undo(a_waveform: BlipKitWaveform, frames: PackedFloat32Array, action_name: StringName) -> void:
	var old_frames := a_waveform.frames

	undo_redo.create_action(action_name)
	undo_redo.add_undo_property(a_waveform, &"frames", old_frames)
	undo_redo.add_do_property(a_waveform, &"frames", frames)
	undo_redo.commit_action()


func _on_frame_count_value_changed(value: float) -> void:
	var frames := waveform.frames
	frames.resize(int(value))

	_waveform_set_frames_undo(waveform, frames, tr(&"Change Waveform Length", &"DMBK"))


func _on_snap_button_toggled(toggled_on: bool) -> void:
	snap_steps_visible = toggled_on


func _on_snap_steps_value_changed(value: float) -> void:
	snap_steps = int(value)


func _on_play_button_button_down() -> void:
	if not _test_audio.playing:
		_test_audio.play()

	var playback: AudioStreamBlipKitPlayback = _test_audio.get_stream_playback()
	_track.attach(playback)
	_track.instrument = _instrument
	_track.custom_waveform = waveform
	_track.note = BlipKitTrack.NOTE_A_3


func _on_play_button_button_up() -> void:
	_track.release()


func _on_edit_id_pressed(id: int) -> void:
	match id:
		0: # Normalize.
			var frames := waveform.frames

			var max_value := 0.0
			for frame in frames:
				max_value = maxf(max_value, absf(frame))

			if not is_zero_approx(max_value):
				var factor := 1.0 / max_value
				for i in len(frames):
					frames[i] *= factor

			_waveform_set_frames_undo(waveform, frames, tr(&"Normalize Waveform", &"DMBK"))


func _on_presets_id_pressed(id: int) -> void:
	var frames := PackedFloat32Array()
	var action_name := &""

	match id:
		0: # Square.
			frames = PackedFloat32Array([1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0])
			action_name = tr(&"Set Square Waveform", &"DMBK")

		1: # Triangle.
			frames = PackedFloat32Array([
				0.0, 0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875,
				0.875, 0.75, 0.625, 0.5, 0.375, 0.25, 0.125, 0.0,
				-0.125, -0.25, -0.375, -0.5, -0.625, -0.75, -0.875, -1.0,
				-1.0, -0.875, -0.75, -0.625, -0.5, -0.375, -0.25, -0.125,
			])
			action_name = tr(&"Set Triangle Waveform", &"DMBK")

		2: # Sawtooth.
			frames = PackedFloat32Array([1.0,  0.8333, 0.6667, 0.5, 0.3333, 0.1667, 0.0])
			action_name = tr(&"Set Sawtooth Waveform", &"DMBK")

		3: # Sine.
			frames = PackedFloat32Array([
				0.0, 0.195068, 0.38266, 0.555542, 0.707062, 0.831421, 0.923828, 0.980743,
				1.0, 0.980743, 0.923828, 0.831421, 0.707062, 0.555542, 0.38266, 0.195068,
				0.0, -0.195068, -0.38266, -0.555542, -0.707062, -0.831421, -0.923828, -0.980743,
				-1.0, -0.980743, -0.923828, -0.831421, -0.707062, -0.555542, -0.38266, -0.195068,
			])
			action_name = tr(&"Set Sine Waveform", &"DMBK")

		_:
			return

	_waveform_set_frames_undo(waveform, frames, action_name)


func _on_waveform_changed() -> void:
	if waveform:
		_frame_count.set_value_no_signal(waveform.size())
		_waveform_editor.frames = waveform.frames

	else:
		_frame_count.set_value_no_signal(0)
		_waveform_editor.frames = []

	_update_frame_values()


func _on_waveform_editor_frames_changed(frames: PackedFloat32Array) -> void:
	waveform.frames = frames


func _on_frame_values_text_submitted(new_text: String) -> void:
	var floats := new_text.split_floats(",", false)
	var new_frames := PackedFloat32Array()
	new_frames.resize(len(floats))

	for i in len(floats):
		new_frames[i] = clampf(floats[i] / 255.0, -1.0, +1.0)

	_waveform_set_frames_undo(waveform, new_frames, tr(&"Change Waveform Amplitudes", &"DMBK"))


func _on_lock_button_toggled(toggled_on: bool) -> void:
	var action_name := tr(&"Lock Waveform", &"DMBK") \
		if toggled_on \
		else tr(&"Unlock Waveform", &"DMBK")

	undo_redo.create_action(action_name)
	undo_redo.add_undo_method(waveform, &"set_meta", &"editor_lock", editor_lock)
	undo_redo.add_undo_method(self, &"_update_editor_lock")
	undo_redo.add_do_method(waveform, &"set_meta", &"editor_lock", toggled_on)
	undo_redo.add_do_method(self, &"_update_editor_lock")
	undo_redo.commit_action()

	editor_lock = toggled_on
