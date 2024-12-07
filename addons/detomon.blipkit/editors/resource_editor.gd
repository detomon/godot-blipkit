@tool
extends Control

var panel_button: Button
var undo_redo: EditorUndoRedoManager

var _theme_cache := {}


func _init() -> void:
	theme_changed.connect(_update_theme)


func _ready() -> void:
	_update_theme_icons()


func get_panel_title() -> String:
	return _get_panel_title()


func _handles(object: Object) -> bool:
	printerr("Not implemented.")
	return false


func _edit(object: Object) -> void:
	printerr("Not implemented.")


func _get_panel_title() -> String:
	printerr("Not implemented.")
	return ""


func _update_theme() -> void:
	_update_theme_cache()
	_update_theme_icons()


func _update_theme_cache() -> void:
	pass


func _update_theme_icons() -> void:
	pass
