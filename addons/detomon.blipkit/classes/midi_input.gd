@icon("../icons/midi_input.svg")
class_name BlipKitMidiInput
extends Node

signal notes_changes(notes: Dictionary)

@export var enabled := true: set = set_enabled

static var _is_midi_open := false
var _notes := {}


func _enter_tree() -> void:
	_open_midi_inputs()


func _ready() -> void:
	set_enabled(enabled)


func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventMIDI:
		var midi_event: InputEventMIDI = event
		_input_midi_event(midi_event)


func set_enabled(value: bool) -> void:
	enabled = value
	set_process_unhandled_input(enabled)


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

	print_debug("MIDI inputs: ", OS.get_connected_midi_inputs())


func _close_midi_inputs() -> void:
	if not _is_midi_open:
		return
	_is_midi_open = false

	OS.close_midi_inputs()
