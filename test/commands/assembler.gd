class_name BlipKitAssemblerTest
extends RefCounted

enum Instruction {
	NOP,
	NOTE,
	WAVEFORM,
	CUSTOM_WAVEFORM,
	DUTY_CYCLE,
	EFFECT_DIVIDER,
	VOLUME,
	VOLUME_SLIDE,
	MASTER_VOLUME,
	PANNING,
	PANNING_SLIDE,
	PITCH,
	PHASE_WRAP,
	PORTAMENTO,
	VIBRATO,
	TREMOLO,
	ARPEGGIO,
	ARPEGGIO_DIVIDER,
	INSTRUMENT,
	INSTRUMENT_DIVIDER,
	WAIT,
	CALL,
	RETURN,
	JUMP,
	RESET,
	MAX,
}

var _bytes := StreamPeerBuffer.new()
var _label_names := PackedStringArray()
var _label_index := {} # Dictionary[String, int]
var _label_position := PackedInt32Array()
var _addresses := [] # [(offset: int, label_index: int)]
var _compile_error := ""


func put(instruction: Instruction, args: Array) -> bool:
	if instruction < 0 or instruction >= Instruction.MAX:
		printerr(&"Instruction %d is not valid." % instruction)
		return false

	var expect_args := func (types: PackedInt32Array) -> bool:
		if len(types) != len(args):
			printerr(&"Expected %d arguments." % len(types))
			return false

		for i in len(types):
			if typeof(args[i]) != types[i]:
				printerr(&"Expected argument %d to be type %s." % [i, type_string(types[i])])
				return false

		return true

	_bytes.put_8(instruction)

	match instruction:
		Instruction.NOP:
			# Do nothing.
			pass

		Instruction.NOTE, \
		Instruction.VOLUME, \
		Instruction.MASTER_VOLUME, \
		Instruction.PANNING, \
		Instruction.PITCH:
			if not expect_args.call([TYPE_FLOAT]):
				return false

			_bytes.put_float(float(args[0]))

		Instruction.WAVEFORM, \
		Instruction.CUSTOM_WAVEFORM, \
		Instruction.DUTY_CYCLE, \
		Instruction.PHASE_WRAP, \
		Instruction.INSTRUMENT:
			if not expect_args.call([TYPE_INT]):
				return false

			_bytes.put_u8(int(args[0]))

		Instruction.EFFECT_DIVIDER, \
		Instruction.VOLUME_SLIDE, \
		Instruction.PANNING_SLIDE, \
		Instruction.PORTAMENTO, \
		Instruction.ARPEGGIO_DIVIDER, \
		Instruction.INSTRUMENT_DIVIDER, \
		Instruction.WAIT:
			if not expect_args.call([TYPE_INT]):
				return false

			_bytes.put_u32(int(args[0]))

		Instruction.TREMOLO, \
		Instruction.VIBRATO:
			if not expect_args.call([TYPE_INT, TYPE_FLOAT, TYPE_INT]):
				return false

			_bytes.put_u32(int(args[0]))
			_bytes.put_float(args[1])
			_bytes.put_u32(int(args[2]))

		Instruction.ARPEGGIO:
			if not expect_args.call([TYPE_PACKED_FLOAT32_ARRAY]):
				return false

			var deltas: PackedFloat32Array = args[0]
			var count := mini(len(deltas), 8)

			_bytes.put_u8(count)
			for i in count:
				var delta := deltas[i]
				_bytes.put_float(delta)

		Instruction.CALL, \
		Instruction.JUMP:
			if not expect_args.call([TYPE_STRING]):
				return false

			var label: String = args[0]
			var label_index := _add_label(label)
			_addresses.append({
				byte_offset = _bytes.get_position(),
				label_index = label_index,
			})
			_bytes.put_u32(0)

		Instruction.RETURN, \
		Instruction.RESET:
			# No arguments.
			pass

	return true

func put_0(instruction: Instruction) -> bool:
	return put(instruction, [])


func put_1(instruction: Instruction, arg: Variant) -> bool:
	return put(instruction, [arg])


func put_2(instruction: Instruction, arg1: Variant, arg2: Variant) -> bool:
	return put(instruction, [arg1, arg2])


func put_3(instruction: Instruction, arg1: Variant, arg2: Variant, arg3: Variant) -> bool:
	return put(instruction, [arg1, arg2, arg3])


func put_label(label: String) -> bool:
	_add_label(label)

	var label_index: int = _label_index[label]
	if _label_position[label_index] >= 0:
		printerr(&"Label '%s' already added." % label)
		return false

	_label_position[label_index] = _bytes.get_position()

	return true


func get_offset() -> int:
	return _bytes.get_position()


func compile() -> PackedByteArray:
	# Resolve label addresses.
	for address: Dictionary in _addresses:
		var label_index: int = address.label_index
		var label_position := _label_position[label_index]

		if label_position < 0:
			var label := _label_names[label_index]
			_compile_error = &"Label '%s' not defined." % label
			return []

		var byte_offset: int = address.byte_offset
		_bytes.seek(byte_offset)
		_bytes.put_32(label_position)

	var data := _bytes.data_array

	_bytes.clear()
	_label_names.clear()
	_label_index.clear()
	_label_position.clear()
	_addresses.clear()
	_compile_error = ""

	return data


func get_compile_error() -> String:
	return _compile_error


func _add_label(label: String) -> int:
	if label in _label_index:
		return _label_index[label]

	var index := len(_label_index)
	_label_index[label] = index
	_label_names.append(label)
	_label_position.append(-1)

	return index
