@tool
extends Control

var panel_button: Button


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
