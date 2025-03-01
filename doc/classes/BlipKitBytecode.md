# Class: BlipKitBytecode

Inherits: *Resource*

**A [`Resource`](https://docs.godotengine.org/en/stable/classes/class_resource.html) used to save byte code gererated with [`BlipKitAssembler`](BlipKitAssembler.md).**

## Description


## Methods

- *PackedByteArray* [**`get_byte_array`**](#packedbytearray-get_byte_array-const)() const
- *String* [**`get_error_message`**](#string-get_error_message-const)() const
- *int* [**`get_label_position`**](#int-get_label_positionlabel-string-const)(label: String) const
- *PackedStringArray* [**`get_labels`**](#packedstringarray-get_labels-const)() const
- *int* [**`get_start_position`**](#int-get_start_position-const)() const
- *int* [**`get_status`**](#int-get_status-const)() const
- *bool* [**`has_label`**](#bool-has_labellabel-string-const)(label: String) const
- *bool* [**`is_valid`**](#bool-is_valid-const)() const

## Enumerations

### enum `Status`

- `OK` = `0`
	- The byte code is loaded successfully.
- `ERR_INVALID_BINARY` = `1`
	- The byte code is not valid.
- `ERR_UNSUPPORTED_VERSION` = `2`
	- The byte code version is not supported.

## Constants

- `VERSION` = `1`
	- The current supported byte code version.

## Method Descriptions

### `PackedByteArray get_byte_array() const`

Returns the byte code.

### `String get_error_message() const`

Returns the error message after loading the byte code.

### `int get_label_position(label: String) const`

Returns the byte position for `label`.

### `PackedStringArray get_labels() const`


### `int get_start_position() const`

Returns the default byte code start position.

### `int get_status() const`

Returns the status [`OK`](#enum-ok) on success.

### `bool has_label(label: String) const`

Returns `true` if the `label` exists.

### `bool is_valid() const`

Returns `true` if the byte code is valid.

Returns `false` if the byte code is not valid. In this case, [`get_status()`](#int-get_status-const) returns the status and [`get_error_message()`](#string-get_error_message-const) returns the error message.


