@tool
extends Control

var panel_button: Button
var undo_redo: EditorUndoRedoManager

@warning_ignore("unused_private_class_variable")
var _theme_cache := {}


func _init() -> void:
	theme_changed.connect(_update_theme)


func _ready() -> void:
	_update_theme_elements()


func get_panel_title() -> String:
	return _get_panel_title()


@warning_ignore("unused_parameter")
func _handles(object: Object) -> bool:
	printerr("Not implemented.")
	return false


@warning_ignore("unused_parameter")
func _edit(object: Object) -> void:
	printerr("Not implemented.")


func _get_panel_title() -> String:
	printerr("Not implemented.")
	return ""


func _update_theme() -> void:
	_update_theme_cache()
	_update_theme_elements()


func _update_theme_cache() -> void:
	pass


func _update_theme_elements() -> void:
	pass
