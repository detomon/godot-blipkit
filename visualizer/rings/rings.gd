extends Node2D

const PARTICLES := preload("particles.tscn")

@export var notes := PackedInt32Array(): set = set_notes

var _particles: Array[Node] = []
var _active_particles := {}


func set_notes(value: PackedInt32Array) -> void:
	notes = value

	for note in notes:
		var particles: GPUParticles2D

		if note not in _active_particles:
			if _particles:
				particles = _particles.back()

			if not particles:
				particles = PARTICLES.instantiate()
				particles.finished.connect(_on_finished)
				particles.position = Vector2(remap(note, 12, 86, -500.0, +500.0), 0.0)
				add_child(particles)

			_active_particles[note] = particles

		particles = _active_particles[note]
		particles.emitting = true

	for note: int in _active_particles:
		if note not in notes:
			_active_particles[note].emitting = false


func _on_finished(particles: Node) -> void:
	print("finished")
	_active_particles.erase(particles)
	_particles.append(particles)
