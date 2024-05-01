@tool
extends Control

const POINT_COUNT := 512

@export var bus_name := &"Master": set = set_bus_name
@export var effect_index := 0: set = set_effect_index

var _effect_capture: AudioEffectCapture
var _buffer_index := 0

@onready var _line: Line2D = %Line


func _ready() -> void:
	var points := PackedVector2Array()
	points.resize(POINT_COUNT)
	_line.points = points

	_update_effect_capture()


func _process(_delta: float) -> void:
	if not _effect_capture:
		return

	var frame_count := mini(_effect_capture.get_frames_available(), POINT_COUNT)
	var frames := _effect_capture.get_buffer(frame_count)
	var x := size.x / float(POINT_COUNT - 1)
	var sy := size.y * 0.25

	for i in frame_count:
		var f := frames[_buffer_index]
		var y := (f.x + f.y) * sy
		var point := Vector2(x * float(i), -y)
		_line.set_point_position(_buffer_index, point)
		_buffer_index = wrapi(_buffer_index + 1, 0, POINT_COUNT)

	_line.position = Vector2(0.0, floorf(size.y * 0.5))


func set_bus_name(value: StringName) -> void:
	bus_name = value
	_update_effect_capture()


func set_effect_index(value: int) -> void:
	effect_index = value
	_update_effect_capture()


func _update_effect_capture() -> void:
	var bus_index := AudioServer.get_bus_index(bus_name)
	_effect_capture = AudioServer.get_bus_effect(bus_index, effect_index)
