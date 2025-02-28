#include "blipkit_bytecode.hpp"
#include "blipkit_interpreter.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_waveform.hpp"
#include "godot_cpp/core/error_macros.hpp"
#include "godot_cpp/templates/pair.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include "godot_cpp/variant/string.hpp"

using namespace BlipKit;
using namespace godot;

const BlipKitInterpreter::Header BlipKitBytecode::binary_header = {
	.version = VERSION,
};

void BlipKitBytecode::initialize(const PackedByteArray &p_byte_code) {
	byte_code.set_byte_array(p_byte_code);

	const uint32_t headers_size = byte_code.get_bytes(reinterpret_cast<uint8_t *>(&header), sizeof(header));

	if (headers_size != sizeof(header)) {
		// fail_with_error(ERR_INVALID_OPCODE, "Truncated binary header.");
		valid = false;
		return;
	}

	if (memcmp(header.name, binary_header.name, sizeof(binary_header.name)) != 0) {
		// fail_with_error(ERR_INVALID_OPCODE, "Invalid binary header.");
		valid = false;
		return;
	}

	// Check version.
	switch (header.version) {
		case 1: {
			// OK.
		} break;
		default: {
			// fail_with_error(ERR_UNSUPPORTED_VERSION, vformat("Unsuported binary version %d.", version));
			valid = false;
			return;
		} break;
	}

	read_labels();

	// return true;
}

void BlipKitBytecode::read_labels() {
	const uint32_t max_size = byte_code.size();
	uint32_t position = byte_code.get_position();

	byte_code.seek(header.footer_offset);

	uint32_t label_count = MIN(byte_code.get_u32(), max_size);
	labels.reserve(label_count);
	PackedByteArray label_bytes;

	for (uint32_t i = 0; i < label_count; i++) {
		uint32_t label_address = byte_code.get_u32();
		uint32_t label_size = byte_code.get_u8();

		if (label_size > label_bytes.size()) {
			label_bytes.resize(label_size);
		}

		const uint32_t read_size = byte_code.get_bytes(label_bytes.ptrw(), label_size);

		ERR_FAIL_COND(read_size < label_size);

		const String label = label_bytes.get_string_from_utf8();
		labels[label] = label_address;
	}

	byte_code.seek(position);
}

Ref<BlipKitBytecode> BlipKitBytecode::create_with_byte_code(const PackedByteArray &p_byte_code) {
	Ref<BlipKitBytecode> instance;

	instance.instantiate();
	instance->initialize(p_byte_code);

	return instance;
}

bool BlipKitBytecode::is_valid() const {
	return valid;
}

PackedByteArray BlipKitBytecode::get_byte_array() const {
	return byte_code.get_byte_array();
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
