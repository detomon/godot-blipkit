extends Node2D

@export var notes := PackedInt32Array(): set = set_notes

@onready var _particles: GPUParticles2D = %Particles


func set_notes(value: PackedInt32Array) -> void:
	notes = value

	if _particles:
		_particles.emitting = len(notes) > 0
		_particles.amount_ratio = clampf(remap(len(notes), 0, 3, 0.0, 1.0), 0.0, 1.0)
