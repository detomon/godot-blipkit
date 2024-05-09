@tool
extends EditorPlugin

const WAVEFORM_EDITOR := preload("editors/waveform/waveform_panel.tscn")

const ResourceEditor := preload("editors/resource_editor.gd")

var _resource_editors: Array[ResourceEditor] = [
	WAVEFORM_EDITOR.instantiate(),
]


func _enter_tree() -> void:
	for editor in _resource_editors:
		var title := editor.get_panel_title()
		var button := add_control_to_bottom_panel(editor, title)
		editor.panel_button = button
		button.visible = false


func _exit_tree() -> void:
	for editor in _resource_editors:
		remove_control_from_bottom_panel(editor)


func _handles(object: Object) -> bool:
	for editor in _resource_editors:
		if editor._handles(object):
			return true

	return false


func _edit(object: Object) -> void:
	var handler: ResourceEditor

	for editor in _resource_editors:
		if editor._handles(object):
			handler = editor
			break

	for editor in _resource_editors:
		editor.panel_button.visible = editor == handler

	if handler:
		make_bottom_panel_item_visible(handler)
		handler._edit(object)
	else:
		hide_bottom_panel()
