@tool
extends Control

signal frames_changed(frames: PackedFloat32Array)

enum State {
	NONE,
	DRAW_FRAMES,
}

var undo_redo: EditorUndoRedoManager

@export var frames: PackedFloat32Array: set = set_frames
@export var snap_steps := 0
@export var locked := false
@export var display_range := 100

var _frames_edit := PackedFloat32Array()
var _is_transform_dirty := false
var _frames_to_local_transform := Transform2D()
var _local_to_frames_transform := Transform2D()
var _state := State.NONE
var _index := -1
var _theme_cache := {
	font = null,
	font_size = 0,
	font_color = Color.WHITE,
	margin = 0,
	frame_margin = 0,
	margin_left = 0,
	grid_color = Color.WHITE,
	grid_color_light = Color.WHITE,
	grid_width = 1.0,
	line_color = Color.WHITE,
	line_width = 2.0,
	bar_color = Color.WHITE,
	bar_tip_color = Color.WHITE,
}


func _init() -> void:
	focus_mode = FOCUS_ALL

	resized.connect(_make_transform_dirty)
	theme_changed.connect(_update_theme_cache)


func _gui_input(event: InputEvent) -> void:
	if locked:
		return

	if event is InputEventMouseButton:
		var mouse_event: InputEventMouseButton = event

		if mouse_event.button_index == MOUSE_BUTTON_LEFT:
			if mouse_event.pressed:
				# Make copy before editting.
				_frames_edit = frames.duplicate()

				if _update_frame_at_position(mouse_event.position, false):
					_state = State.DRAW_FRAMES

			else:
				undo_redo.create_action(tr(&"Set Waveform Amplitudes", &"DMBK"))
				undo_redo.add_undo_method(self, &"_set_frames_changed", frames)
				undo_redo.add_do_method(self, &"_set_frames_changed", _frames_edit)
				undo_redo.commit_action()

				_state = State.NONE
				_index = -1

	elif event is InputEventMouseMotion:
		var mouse_event: InputEventMouseMotion = event

		match _state:
			State.DRAW_FRAMES:
				_update_frame_at_position(mouse_event.position, true)


func _draw() -> void:
	var transform := _get_frames_to_local_transform()
	var rect_inner := Rect2()
	rect_inner.position = transform * Vector2(0.0, -1.0)
	rect_inner.end = transform * Vector2(1.0, 1.0)

	_draw_grid(rect_inner)
	_draw_text(rect_inner)
	_draw_frames()


func set_frames(value: PackedFloat32Array) -> void:
	frames = value
	_frames_edit = frames

	_make_transform_dirty()
	size = _get_size()
	queue_redraw()


func _set_frames_changed(value: PackedFloat32Array) -> void:
	frames = value
	frames_changed.emit(frames)


func _update_theme_cache() -> void:
	var font := get_theme_font(&"font", &"Label")
	var font_size := get_theme_font_size(&"font_size", &"Label")
	var mono_color: Color = get_theme_color(&"mono_color", &"Editor")
	var mono_color_transparent := mono_color
	mono_color_transparent.a = 0.0

	_theme_cache.font = font
	_theme_cache.font_size = font_size
	_theme_cache.font_color = mono_color
	_theme_cache.margin = font.get_height(font_size)
	_theme_cache.margin_left = font_size * 2.5
	_theme_cache.bar_color = mono_color.lerp(mono_color_transparent, 0.85)
	_theme_cache.bar_tip_color = mono_color.lerp(mono_color_transparent, 0.1)
	_theme_cache.grid_color = mono_color.lerp(mono_color_transparent, 0.7)
	_theme_cache.grid_color_light = mono_color.lerp(mono_color_transparent, 0.95)


func _update_frame_at_position(point: Vector2, allow_out_of_bounds := false) -> bool:
	if not _frames_edit:
		return false

	var transform := _get_local_to_frames_transform()
	var value := transform * point

	var x := floori(value.x * len(frames))
	var y := clampf(value.y, -1.0, +1.0)

	if not allow_out_of_bounds:
		if x < 0 or x >= len(frames):
			return false

	x = clampi(x, 0, len(frames) - 1)

	if snap_steps > 0:
		y = snappedf(y, 1.0 / float(snap_steps))

	_frames_edit[x] = y

	queue_redraw()

	return true


func _make_transform_dirty() -> void:
	_is_transform_dirty = true


func _update_transform() -> void:
	var rect_size := _get_size()
	var margin: float = _theme_cache.margin
	var rect := Rect2(Vector2.ZERO, rect_size).grow(-margin)
	var margin_left: float = _theme_cache.margin_left
	rect = rect.grow_individual(-margin_left, 0.0, 0.0, 0.0)
	var transform := Transform2D()

	transform = transform.scaled(rect.size * Vector2(1.0, -0.5))
	transform = transform.translated_local(Vector2(0.0, -1.0))
	transform = transform.translated(rect.position)

	_frames_to_local_transform = transform
	_local_to_frames_transform = transform.affine_inverse()


func _get_frames_to_local_transform() -> Transform2D:
	if _is_transform_dirty:
		_update_transform()
		_is_transform_dirty = false

	return _frames_to_local_transform


func _get_local_to_frames_transform() -> Transform2D:
	if _is_transform_dirty:
		_update_transform()
		_is_transform_dirty = false

	return _local_to_frames_transform


func _get_size() -> Vector2i:
	const bar_width_default := 20.0
	var margin_right: float = _theme_cache.margin
	var margin_left: float = _theme_cache.margin_left
	var field_min_width := size.x - margin_left - margin_right
	var field_width := minf(bar_width_default * len(frames), field_min_width)
	var bar_width := floorf(field_width / len(frames))
	var width := len(_frames_edit) * bar_width + margin_left + margin_right

	return Vector2i(int(width), int(size.y))


func _draw_grid(rect: Rect2i) -> void:
	const line_width := 1.0
	var grid_color: Color = _theme_cache.grid_color
	var grid_color_light: Color = _theme_cache.grid_color_light

	draw_rect(rect, grid_color, false, line_width)

	if not frames:
		return

	var transform := _get_frames_to_local_transform()
	var frame_width := 1.0 / len(frames)
	const grid_steps := 8.0
	var grid_height := 1.0 / grid_steps

	for i in len(frames) - 1:
		var from_line := transform * Vector2(frame_width * float(i + 1), +1.0)
		var to_line := transform * Vector2(frame_width * float(i + 1), -1.0)
		draw_line(from_line, to_line, grid_color_light, line_width)

	for i in grid_steps - 1:
		var from_line := transform * Vector2(0.0, grid_height * float(i + 1))
		var to_line := transform * Vector2(1.0, grid_height * float(i + 1))
		draw_line(from_line, to_line, grid_color_light, line_width)

	var from_line2 := transform * Vector2(0.0, 0.0)
	var to_line2 := transform * Vector2(1.0, 0.0)
	draw_line(from_line2, to_line2, grid_color, line_width)

	for i in grid_steps - 1:
		var from_line := transform * Vector2(0.0, -grid_height * float(i + 1))
		var to_line := transform * Vector2(1.0, -grid_height * float(i + 1))
		draw_line(from_line, to_line, grid_color_light, line_width)


func _draw_text(rect: Rect2i) -> void:
	rect.position.x -= _theme_cache.font_size * 0.5
	_draw_text_right_aligned("%+d" % -display_range, rect.position)
	_draw_text_right_aligned("0", Vector2i(rect.position.x, rect.get_center().y))
	_draw_text_right_aligned("%+d" % display_range, Vector2i(rect.position.x, rect.end.y))


func _draw_text_right_aligned(text: String, text_position: Vector2) -> void:
	var font: Font = _theme_cache.font
	var font_size: int = _theme_cache.font_size
	var font_color: Color = _theme_cache.font_color
	var text_size := font.get_string_size(text, HORIZONTAL_ALIGNMENT_RIGHT, -1, font_size)
	var ascent := font.get_ascent(font_size)

	text_position += Vector2(-text_size.x, ascent * 0.5)
	draw_string(font, text_position, text, HORIZONTAL_ALIGNMENT_RIGHT, -1, font_size, font_color)


func _draw_frames() -> void:
	if not _frames_edit:
		return

	var frame_width := 1.0 / float(len(_frames_edit))
	var bar_color: Color = _theme_cache.bar_color
	var bar_tip_color: Color = _theme_cache.bar_tip_color
	var frame_margin: int = _theme_cache.frame_margin
	var transform := _get_frames_to_local_transform()

	for i in len(_frames_edit):
		var value := _frames_edit[i]
		var x := frame_width * float(i)
		var rect := transform * Rect2(Vector2(x, 0.0), Vector2(frame_width, value))
		rect = rect.grow_individual(-frame_margin, 0.0, -frame_margin, 0.0)

		rect.position.x = ceili(rect.position.x)
		rect.position.y = ceili(rect.position.y)
		rect.end.x = floori(rect.end.x)
		rect.end.y = floori(rect.end.y)

		draw_rect(rect, bar_color)

		var tip_rect := Rect2(rect.position, Vector2(rect.size.x, 3.0))
		if value < 0:
			tip_rect.position.y = rect.end.y
		tip_rect.position.y -= 2.0

		draw_rect(tip_rect, bar_tip_color)
