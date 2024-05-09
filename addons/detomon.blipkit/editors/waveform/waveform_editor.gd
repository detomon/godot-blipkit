@tool
extends Control

signal frames_changed(frames: PackedFloat32Array)

@export var frames: PackedFloat32Array: set = set_frames
@export var snap_steps := 0

var _theme_cache := {
	grid_color = Color.WHITE,
	grid_width = 1.0,
	line_color = Color.WHITE,
	line_width = 2.0,
}


func _gui_input(event: InputEvent) -> void:
	pass


func _draw() -> void:
	_draw_grid()
	_draw_frames()


func set_frames(value: PackedFloat32Array) -> void:
	frames = value
	queue_redraw()


func _draw_grid() -> void:
	draw_rect(Rect2i(Vector2.ZERO, size), _theme_cache.grid_color, false, _theme_cache.grid_width)

	var center := Vector2i(size * 0.5)
	draw_line(Vector2i(0.0, center.y), Vector2(size.x, center.y), _theme_cache.line_color, _theme_cache.line_width)


func _draw_frames() -> void:
	pass
