class_name BlipKitInterpreterTest
extends RefCounted

const STACK_MAX_SIZE := 64
const Instruction := BlipKitAssemblerTest.Instruction

var _bytes := StreamPeerBuffer.new()
var _stack := PackedInt32Array()
var _instruments: Array[BlipKitInstrument] = []
var _waveforms: Array[BlipKitWaveform] = []


func _init() -> void:
	_instruments.resize(256)
	_waveforms.resize(256)


func set_instrument_at(slot: int, instrument: BlipKitInstrument) -> void:
	if slot < 0 or slot >= len(_instruments):
		printerr(&"Instrument slot %s out of bounds." % slot)
		return

	_instruments[slot] = instrument


func get_instrument_at(slot: int) -> BlipKitInstrument:
	if slot < 0 or slot >= len(_instruments):
		printerr(&"Instrument slot %s out of bounds." % slot)
		return null

	return _instruments[slot]


func set_waveform_at(slot: int, waveform: BlipKitWaveform) -> void:
	if slot < 0 or slot >= len(_waveforms):
		printerr(&"Waveform slot %s out of bounds." % slot)
		return

	_waveforms[slot] = waveform


func get_waveform_at(slot: int) -> BlipKitWaveform:
	if slot < 0 or slot >= len(_waveforms):
		printerr(&"Waveform slot %s out of bounds." % slot)
		return null

	return _waveforms[slot]


func set_instructions(bytes: PackedByteArray) -> void:
	_bytes.data_array = bytes


func advance(track: BlipKitTrack) -> int:
	while _bytes.get_available_bytes() > 0:
		var instr_offset := _bytes.get_position()
		var instr := _bytes.get_u8()

		match instr:
			Instruction.NOTE:
				track.note = _bytes.get_float()

			Instruction.VOLUME:
				track.volume = _bytes.get_float()

			Instruction.MASTER_VOLUME:
				track.master_volume = _bytes.get_float()

			Instruction.PANNING:
				track.panning = _bytes.get_float()

			Instruction.PITCH:
				track.pitch = _bytes.get_float()

			Instruction.WAVEFORM:
				track.waveform = _bytes.get_u8() as BlipKitTrack.Waveform

			Instruction.CUSTOM_WAVEFORM:
				var index := _bytes.get_u8()
				var waveform := _waveforms[index]
				track.custom_waveform = waveform

			Instruction.DUTY_CYCLE:
				track.duty_cycle = _bytes.get_u8()

			Instruction.PHASE_WRAP:
				track.duty_cycle = _bytes.get_u8()

			Instruction.INSTRUMENT:
				var index := _bytes.get_u8()
				var instrument := _instruments[index]
				track.instrument = instrument

			Instruction.EFFECT_DIVIDER:
				track.effect_divider = _bytes.get_u32()

			Instruction.VOLUME_SLIDE:
				track.volume_slide = _bytes.get_u32()

			Instruction.PANNING_SLIDE:
				track.panning_slide = _bytes.get_u32()

			Instruction.PORTAMENTO:
				track.portamento = _bytes.get_u32()

			Instruction.ARPEGGIO_DIVIDER:
				track.arpeggio_divider = _bytes.get_u32()

			Instruction.INSTRUMENT_DIVIDER:
				track.instrument_divider = _bytes.get_u32()

			Instruction.WAIT:
				var wait := _bytes.get_u32()
				return wait

			Instruction.TREMOLO:
				var ticks := _bytes.get_u32()
				var delta := _bytes.get_float()
				var slide_ticks := _bytes.get_u32()
				track.set_tremolo(ticks, delta, slide_ticks)

			Instruction.VIBRATO:
				var ticks := _bytes.get_u32()
				var delta := _bytes.get_float()
				var slide_ticks := _bytes.get_u32()
				track.set_vibrato(ticks, delta, slide_ticks)

			Instruction.ARPEGGIO:
				var count := _bytes.get_u8()
				var delta := PackedFloat32Array()
				delta.resize(count)

				for i in count:
					delta[i] = _bytes.get_float()
				track.arpeggio = delta

			Instruction.CALL:
				var offset := _bytes.get_u32()
				var position := _bytes.get_position()

				if len(_stack) >= STACK_MAX_SIZE:
					printerr(&"Stack overflow.")
					return 0

				_stack.append(position)
				_bytes.seek(offset)

			Instruction.JUMP:
				var offset := _bytes.get_u32()
				_bytes.seek(offset)

			Instruction.RETURN:
				if not _stack:
					printerr(&"Stack underflow.")
					return 0

				var index := len(_stack) - 1
				var offset := _stack[index]
				_stack.remove_at(index)
				_bytes.seek(offset)

			Instruction.RESET:
				track.reset()

			_:
				var size := _bytes.get_size()
				_bytes.seek(size)
				printerr(&"Invalid instruction %d at offset %d." % [instr, instr_offset])
				return 0

	return 0
