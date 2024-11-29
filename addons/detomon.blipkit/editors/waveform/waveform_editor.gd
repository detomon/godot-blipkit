@tool
extends Control

signal frames_changed(frames: PackedFloat32Array)

enum State {
	NONE,
	DRAW_FRAMES,
}

@export var frames: PackedFloat32Array: set = set_frames
@export var snap_steps := 0
@export var editor_lock := false

var _is_transform_dirty := false
var _frames_to_local_transform := Transform2D()
var _local_to_frames_transform := Transform2D()
var _editor_scale := 1.0
var _state := State.NONE
var _index := -1
var _theme_cache := {
	font_size = 0,
	margin = 0,
	grid_color = Color.WHITE,
	grid_color_light = Color.WHITE,
	grid_width = 1.0,
	line_color = Color.WHITE,
	line_width = 2.0,
	frame_color = Color.WHITE,
	frame_margin = 2,
}


func _init() -> void:
	_editor_scale = EditorInterface.get_editor_scale()
	focus_mode = FOCUS_ALL

	resized.connect(_make_transform_dirty)
	theme_changed.connect(_update_theme_cache)


func _gui_input(event: InputEvent) -> void:
	if editor_lock:
		return

	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_LEFT:
			if event.pressed:
				if _update_frame_at_position(event.position, false):
					frames_changed.emit(frames)
					_state = State.DRAW_FRAMES

			else:
				_state = State.NONE
				_index = -1

			queue_redraw()

	elif event is InputEventMouseMotion:
		match _state:
			State.DRAW_FRAMES:
				if _update_frame_at_position(event.position, true):
					frames_changed.emit(frames)


func _draw() -> void:
	var rect_size := _get_size()
	var rect_outer := Rect2i(Vector2.ZERO, rect_size)
	var rect_inner := rect_outer.grow(-_theme_cache.margin * _editor_scale)

	_draw_grid(rect_inner)
	_draw_frames()


#func _get_minimum_size() -> Vector2:
	#var width: float = _theme_cache.margin * 2.0
#
	#if frames:
		#width += len(frames) * 16.0
#
	#return Vector2(width, 200.0)


func set_frames(value: PackedFloat32Array) -> void:
	frames = value
	_make_transform_dirty()
	queue_redraw()


func _update_theme_cache() -> void:
	var mono_color: Color = get_theme_color(&"mono_color", &"Editor")
	var mono_color_transparent := mono_color
	mono_color_transparent.a = 0.0

	_theme_cache.font = get_theme_font(&"font", &"Label")
	_theme_cache.font_size = get_theme_font_size(&"font_size", &"Label")
	_theme_cache.margin = _theme_cache.font.get_height(_theme_cache.font_size)
	_theme_cache.frame_color = mono_color.lerp(mono_color_transparent, 0.5)
	_theme_cache.grid_color = mono_color.lerp(mono_color_transparent, 0.7)
	_theme_cache.grid_color_light = mono_color.lerp(mono_color_transparent, 0.9)


func _update_frame_at_position(point: Vector2, allow_out_of_bounds := false) -> bool:
	if not frames:
		return false

	var transform := _get_local_to_frames_transform()
	var frame_value := transform * point

	if not allow_out_of_bounds:
		if frame_value.x < 0.0 or frame_value.x > 1.0 \
			or frame_value.y < -1.0 or frame_value.y > +1.0:
				return false

	var x := clampi(floori(frame_value.x * len(frames)), 0, len(frames) - 1)
	if snap_steps > 0:
		frame_value.y = snappedf(frame_value.y, 1.0 / float(snap_steps))
	frames[x] = clampf(frame_value.y, -1.0, +1.0)

	return true


func _make_transform_dirty() -> void:
	_is_transform_dirty = true


func _update_transform() -> void:
	var rect_size := _get_size()
	var rect := Rect2(Vector2.ZERO, rect_size).grow(-_theme_cache.margin * _editor_scale)
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
	var width_min := len(frames) * 24.0
	var width := minf(width_min, size.x)

	return Vector2i(width, size.y)


func _draw_grid(rect: Rect2i) -> void:
	var line_width := 1.0 * _editor_scale

	draw_rect(rect, _theme_cache.grid_color, false, line_width)

	if frames:
		var transform := _get_frames_to_local_transform()
		var frame_width := 1.0 / len(frames)
		const grid_steps := 8.0
		var grid_height := 1.0 / grid_steps

		for i in len(frames) - 1:
			var from_line := transform * Vector2(frame_width * float(i + 1), +1.0)
			var to_line := transform * Vector2(frame_width * float(i + 1), -1.0)
			draw_line(from_line, to_line, _theme_cache.grid_color_light, line_width)

		for i in grid_steps - 1:
			var from_line := transform * Vector2(0.0, grid_height * float(i + 1))
			var to_line := transform * Vector2(1.0, grid_height * float(i + 1))
			draw_line(from_line, to_line, _theme_cache.grid_color_light, line_width)

		var from_line2 := transform * Vector2(0.0, 0.0)
		var to_line2 := transform * Vector2(1.0, 0.0)
		draw_line(from_line2, to_line2, _theme_cache.grid_color, line_width)

		for i in grid_steps - 1:
			var from_line := transform * Vector2(0.0, -grid_height * float(i + 1))
			var to_line := transform * Vector2(1.0, -grid_height * float(i + 1))
			draw_line(from_line, to_line, _theme_cache.grid_color_light, line_width)


func _draw_frames() -> void:
	if not frames:
		return

	var frame_width := 1.0 / float(len(frames))
	var frame_color: Color = _theme_cache.frame_color
	var frame_margin: int = _theme_cache.frame_margin
	var transform := _get_frames_to_local_transform()

	for i in len(frames):
		var value := frames[i]
		var x := frame_width * float(i)
		var rect := transform * Rect2(Vector2(x, 0.0), Vector2(frame_width, value))
		rect.position.x += frame_margin
		rect.size.x -= frame_margin * 2.0

		if is_zero_approx(rect.size.y):
			rect.position.y += 3.0
			rect.size.y -= 6.0

		draw_rect(Rect2i(rect), frame_color)
