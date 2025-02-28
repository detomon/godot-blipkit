#include "blipkit_bytecode.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_interpreter.hpp"
#include "blipkit_waveform.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/templates/pair.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"

using namespace BlipKit;
using namespace godot;

const BlipKitBytecode::Header BlipKitBytecode::binary_header;

void BlipKitBytecode::initialize(const ByteStream &p_bytes) {
	byte_code = p_bytes;

	valid = read_header();

	if (valid) {
		valid = read_footer();
	}
}

bool BlipKitBytecode::read_header() {
	const uint32_t headers_size = byte_code.get_bytes(reinterpret_cast<uint8_t *>(&header), sizeof(header));

	if (headers_size != sizeof(header)) {
		// fail_with_error(ERR_INVALID_OPCODE, "Truncated binary header.");
		return false;
	}

	if (memcmp(header.name, binary_header.name, sizeof(binary_header.name)) != 0) {
		// fail_with_error(ERR_INVALID_OPCODE, "Invalid binary header.");
		return false;
	}

	// Check version.
	switch (header.version) {
		case 1: {
			// OK.
		} break;
		default: {
			// fail_with_error(ERR_UNSUPPORTED_VERSION, vformat("Unsuported binary version %d.", version));
			return false;
		} break;
	}

	return true;
}

bool BlipKitBytecode::read_footer() {
	const uint32_t max_size = byte_code.size();
	const uint32_t position = byte_code.get_position();

	byte_code.seek(header.footer_offset);

	const uint32_t label_count = MIN(byte_code.get_u32(), max_size);
	labels.reserve(label_count);
	PackedByteArray label_bytes;

	for (uint32_t i = 0; i < label_count; i++) {
		const uint32_t label_address = byte_code.get_u32();
		const uint32_t label_size = byte_code.get_u8();

		if (label_size > label_bytes.size()) {
			label_bytes.resize(label_size);
		}

		const uint32_t read_size = byte_code.get_bytes(label_bytes.ptrw(), label_size);

		ERR_FAIL_COND_V(read_size < label_size, false);

		const String label = label_bytes.get_string_from_utf8();
		labels[label] = label_address;
	}

	byte_code.seek(position);

	return true;
}

Ref<BlipKitBytecode> BlipKitBytecode::create_with_byte_stream(const ByteStream &p_bytes) {
	Ref<BlipKitBytecode> instance;

	instance.instantiate();
	instance->initialize(p_bytes);

	return instance;
}

bool BlipKitBytecode::is_valid() const {
	return valid;
}

void BlipKitBytecode::set_bytes(const Vector<uint8_t> &p_bytes) {
	byte_code.set_bytes(p_bytes);
}

Vector<uint8_t> BlipKitBytecode::get_bytes() const {
	return byte_code.get_bytes();
}

PackedByteArray BlipKitBytecode::get_byte_array() const {
	const uint32_t bytes_size = byte_code.size();
	PackedByteArray bytes;
	bytes.resize(bytes_size);
	uint8_t *ptrw = bytes.ptrw();

	memcpy(ptrw, byte_code.ptr(), bytes_size);

	return bytes;
}

PackedStringArray BlipKitBytecode::get_labels() const {
	const uint32_t label_count = labels.size();
	PackedStringArray label_names;

	label_names.resize(label_count);
	String *ptrw = label_names.ptrw();

	uint32_t i = 0;
	for (const KeyValue<String, uint32_t> &label : labels) {
		ptrw[i++] = label.key;
	}

	return label_names;
}

void BlipKitBytecode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("is_valid"), &BlipKitBytecode::is_valid);
	ClassDB::bind_method(D_METHOD("get_byte_array"), &BlipKitBytecode::get_byte_array);
	ClassDB::bind_method(D_METHOD("get_labels"), &BlipKitBytecode::get_labels);
}

String BlipKitBytecode::_to_string() const {
	return vformat("<BlipKitBytecode:#%d>", int64_t(this));
}
