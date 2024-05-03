class_name MidiInput
extends Node

signal notes_changes(notes: Dictionary)

static var _is_midi_open := false
var _notes := {}


func _enter_tree() -> void:
	_open_midi_inputs()


func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventMIDI:
		_input_midi_event(event)


func _input_midi_event(event: InputEventMIDI) -> void:
	if event.velocity:
		_notes[event.pitch] = event.velocity

	else:
		_notes.erase(event.pitch)

	notes_changes.emit(_notes)


func _open_midi_inputs() -> void:
	if _is_midi_open:
		return
	_is_midi_open = true

	OS.open_midi_inputs()


func _close_midi_inputs() -> void:
	if not _is_midi_open:
		return
	_is_midi_open = false

	OS.close_midi_inputs()
