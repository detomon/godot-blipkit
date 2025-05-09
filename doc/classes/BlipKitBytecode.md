# Class: BlipKitBytecode

Inherits: *Resource*

**A [`Resource`](https://docs.godotengine.org/en/stable/classes/class_resource.html) used to save byte code gererated with [`BlipKitAssembler`](BlipKitAssembler.md).**

## Description

Can be saved as `.blipc` files.

**Example:** Save byte code:

```gdscript
# Get byte code from a BlipKitAssembler.
var bytecode := assem.get_byte_code()

# Save byte code to a file.
ResourceSaver.save(bytecode, "res://bytecode.blipc")
```
## Online Tutorials

- [Format description of .blipc files](https://github.com/detomon/godot-blipkit/blob/master/doc/blipc_file.md)

## Methods

- *int* [**`find_label`**](#int-find_labelname-string-const)(name: String) const
- *PackedByteArray* [**`get_byte_array`**](#packedbytearray-get_byte_array-const)() const
- *int* [**`get_code_section_offset`**](#int-get_code_section_offset-const)() const
- *int* [**`get_code_section_size`**](#int-get_code_section_size-const)() const
- *String* [**`get_error_message`**](#string-get_error_message-const)() const
- *int* [**`get_label_count`**](#int-get_label_count-const)() const
- *String* [**`get_label_name`**](#string-get_label_namelabel_index-int-const)(label_index: int) const
- *int* [**`get_label_position`**](#int-get_label_positionlabel_index-int-const)(label_index: int) const
- *int* [**`get_state`**](#int-get_state-const)() const
- *bool* [**`has_label`**](#bool-has_labelname-string-const)(name: String) const
- *bool* [**`is_valid`**](#bool-is_valid-const)() const

## Enumerations

### enum `State`

- `OK` = `0`
	- The byte code is loaded successfully.
- `ERR_INVALID_BINARY` = `1`
	- The byte code is not valid.
- `ERR_UNSUPPORTED_VERSION` = `2`
	- The byte code version is not supported.

## Constants

- `VERSION` = `0`
	- The current supported byte code version.

## Method Descriptions

### `int find_label(name: String) const`

Returns the index of a label with `name`.

Returns `-1` if no label with [name] exists.

### `PackedByteArray get_byte_array() const`

Returns the byte code.

### `int get_code_section_offset() const`

Returns the byte offset of the code section.

### `int get_code_section_size() const`

Returns the size in bytes the code section.

### `String get_error_message() const`

Returns the error message when loading the byte code fails.

### `int get_label_count() const`

Returns the number of labels.

### `String get_label_name(label_index: int) const`

Returns the label name for the label with index `label_index`.

### `int get_label_position(label_index: int) const`

Returns the byte position for the label with index `label_index` relative to the byte code section offset.

### `int get_state() const`

Returns [`OK`](#ok) on success.

### `bool has_label(name: String) const`

Returns `true` if the `label` exists.

### `bool is_valid() const`

Returns `true` if the byte code is valid.

Returns `false` if the byte code is not valid. In this case, [`get_state()`](#int-get_state-const) returns the state and [`get_error_message()`](#string-get_error_message-const) returns the error message.


