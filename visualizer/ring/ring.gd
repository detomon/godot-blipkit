extends Node2D

@export_range(0.0, 1.0) var strength := 0.0: set = set_strength

@onready var _particles: GPUParticles2D = %Particles


func _ready() -> void:
	set_strength(strength)


func set_strength(value: float) -> void:
	strength = clampf(value, 0.0, 1.0)

	if _particles:
		_particles.emitting = strength > 0.0
		_particles.amount_ratio = strength

		#if pitch >= 0:
			#var particle_material: ParticleProcessMaterial = _particles.process_material
			#var offset := remap(pitch, 24, 72, 0.0, 1.0)
			#var color := color_gradient.sample(offset)
			#color.r *= 2.5
			#color.g *= 2.5
			#color.b *= 2.5
			#particle_material.color = color
