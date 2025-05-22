#include "blipkit_bytecode.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_interpreter.hpp"
#include "blipkit_sample.hpp"
#include "blipkit_waveform.hpp"
#include "strings.hpp"
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_uid.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace BlipKit;
using namespace godot;

const BlipKitBytecode::Header BlipKitBytecode::binary_header{
	.version = VERSION,
};

int BlipKitBytecode::fail_with_error(State p_state, const String &p_error_message) {
	state = p_state;
	error_message = p_error_message;

	ERR_FAIL_V_MSG(-1, error_message);
}

bool BlipKitBytecode::read_header() {
	const uint32_t headers_size = byte_code.get_bytes(reinterpret_cast<uint8_t *>(&header), sizeof(header));

	if (headers_size != sizeof(header)) {
		fail_with_error(ERR_INVALID_BINARY, "Truncated header.");
		return false;
	}

	if (memcmp(header.magic, binary_header.magic, sizeof(binary_header.magic)) != 0) {
		fail_with_error(ERR_INVALID_BINARY, "Invalid header.");
		return false;
	}

	if (memcmp(header.code_magic, "code", 4) != 0) {
		fail_with_error(ERR_INVALID_BINARY, "Invalid code section.");
		return false;
	}

	// Check version.
	switch (header.version) {
		case 0: {
			// OK.
		} break;
		default: {
			fail_with_error(ERR_UNSUPPORTED_VERSION, vformat("Unsuported binary version %d.", header.version));
			return false;
		} break;
	}

	byte_code.seek(offsetof(Header, bytecode_size));
	header.bytecode_size = byte_code.get_u32();

	return true;
}

bool BlipKitBytecode::read_sections() {
	const uint32_t position = byte_code.get_position();

	byte_code.seek(position + header.bytecode_size);

	while (byte_code.get_available_bytes() > 0) {
		uint8_t magic[4] = { 0 };
		const uint32_t section_position = byte_code.get_position();
		const uint32_t read_size = byte_code.get_bytes(magic, 4);

		ERR_FAIL_COND_V_MSG(read_size < 4, false, vformat("Truncated section header at offset %d.", section_position));

		if (memcmp(magic, "labl", 4) == 0) {
			if (not read_labels()) {
				return false;
			}
		} else {
			fail_with_error(ERR_INVALID_BINARY, vformat("Unknown section'%x%x%x%x' at offset %d.", magic[0], magic[1], magic[2], magic[3], section_position));
			return false;
		}
	}

	// Reset to byte code.
	byte_code.seek(position);

	return true;
}

bool BlipKitBytecode::read_labels() {
	const uint32_t section_size = byte_code.get_u32();
	const uint32_t position = byte_code.get_position();
	const uint32_t label_count = byte_code.get_u32();

	labels.clear();
	labels.reserve(label_count);
	label_indices.clear();
	label_indices.reserve(label_count);

	PackedByteArray label_bytes;

	for (uint32_t i = 0; i < label_count; i++) {
		const uint32_t label_address = byte_code.get_u32();
		const uint32_t label_size = byte_code.get_u8();

		if (label_size > label_bytes.size()) {
			label_bytes.resize(label_size);
		}

		const uint32_t read_size = byte_code.get_bytes(label_bytes.ptrw(), label_size);

		if (read_size < label_size) {
			fail_with_error(ERR_INVALID_BINARY, "Truncated label.");
			return false;
		}

		const String &label_name = label_bytes.get_string_from_utf8();
		uint32_t &label_index = label_indices[label_name]; // Find or insert.

		// Insert label.
		if (label_indices.size() > labels.size()) {
			label_index = labels.size();
			labels.push_back({ .name = label_name, .byte_offset = label_address });
		}
	}

	byte_code.seek(position + section_size);

	return true;
}

bool BlipKitBytecode::is_valid() const {
	return state == OK;
}

void BlipKitBytecode::set_bytes(const Vector<uint8_t> &p_bytes) {
	byte_code.set_bytes(p_bytes);

	if (not read_header()) {
		return;
	}

	if (not read_sections()) {
		return;
	}
}

BlipKitBytecode::State BlipKitBytecode::get_state() const {
	return state;
}

String BlipKitBytecode::get_error_message() const {
	return error_message;
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

int BlipKitBytecode::get_code_section_offset() const {
	return sizeof(Header);
}

int BlipKitBytecode::get_code_section_size() const {
	return header.bytecode_size;
}

bool BlipKitBytecode::has_label(const String &p_name) const {
	return label_indices.has(p_name);
}

int BlipKitBytecode::find_label(const String &p_name) const {
	const HashMap<String, uint32_t>::ConstIterator label_itor = label_indices.find(p_name);

	if (label_itor == label_indices.end()) {
		return -1;
	}

	return label_itor->value;
}

int BlipKitBytecode::get_label_count() const {
	return label_indices.size();
}

String BlipKitBytecode::get_label_name(int p_label_index) const {
	ERR_FAIL_INDEX_V(p_label_index, labels.size(), "");

	return labels[p_label_index].name;
}

int BlipKitBytecode::get_label_position(int p_label_index) const {
	ERR_FAIL_INDEX_V(p_label_index, labels.size(), 0);

	return labels[p_label_index].byte_offset;
}

void BlipKitBytecode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("is_valid"), &BlipKitBytecode::is_valid);
	ClassDB::bind_method(D_METHOD("get_state"), &BlipKitBytecode::get_state);
	ClassDB::bind_method(D_METHOD("get_error_message"), &BlipKitBytecode::get_error_message);
	ClassDB::bind_method(D_METHOD("get_byte_array"), &BlipKitBytecode::get_byte_array);
	ClassDB::bind_method(D_METHOD("get_code_section_offset"), &BlipKitBytecode::get_code_section_offset);
	ClassDB::bind_method(D_METHOD("get_code_section_size"), &BlipKitBytecode::get_code_section_size);
	ClassDB::bind_method(D_METHOD("has_label", "name"), &BlipKitBytecode::has_label);
	ClassDB::bind_method(D_METHOD("find_label", "name"), &BlipKitBytecode::find_label);
	ClassDB::bind_method(D_METHOD("get_label_count"), &BlipKitBytecode::get_label_count);
	ClassDB::bind_method(D_METHOD("get_label_name", "label_index"), &BlipKitBytecode::get_label_name);
	ClassDB::bind_method(D_METHOD("get_label_position", "label_index"), &BlipKitBytecode::get_label_position);

	BIND_ENUM_CONSTANT(OK);
	BIND_ENUM_CONSTANT(ERR_INVALID_BINARY);
	BIND_ENUM_CONSTANT(ERR_UNSUPPORTED_VERSION);

	BIND_CONSTANT(VERSION);
}

String BlipKitBytecode::_to_string() const {
	return vformat("<BlipKitBytecode#%d>", get_instance_id());
}

void BlipKitBytecode::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "_bytes", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
}

bool BlipKitBytecode::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == BKStringName(_bytes)) {
		const PackedByteArray &byte_array = p_value;
		const uint32_t bytes_size = byte_array.size();
		const uint8_t *ptr = byte_array.ptr();
		Vector<uint8_t> bytes;

		bytes.resize(bytes_size);
		memcpy(bytes.ptrw(), ptr, bytes_size);

		set_bytes(bytes);
	} else {
		return false;
	}

	return true;
}

bool BlipKitBytecode::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == BKStringName(_bytes)) {
		r_ret = get_byte_array();
	} else {
		return false;
	}

	return true;
}

PackedStringArray BlipKitBytecodeLoader::_get_recognized_extensions() const {
	return { "blipc" };
}

bool BlipKitBytecodeLoader::_handles_type(const StringName &p_type) const {
	const String &type = p_type;

	return type == BKStringName(BlipKitBytecode);
}

String BlipKitBytecodeLoader::_get_resource_type(const String &p_path) const {
	if (not p_path.ends_with(".blipc")) {
		return "";
	}

	return BKStringName(BlipKitBytecode);
}

Variant BlipKitBytecodeLoader::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const {
	if (not p_path.ends_with(".blipc")) {
		return nullptr;
	}

	Ref<BlipKitBytecode> byte_code;
	const PackedByteArray byte_array = FileAccess::get_file_as_bytes(p_path);

	const uint32_t bytes_size = byte_array.size();
	Vector<uint8_t> bytes;
	bytes.resize(bytes_size);
	uint8_t *ptrw = bytes.ptrw();

	memcpy(ptrw, byte_array.ptr(), bytes_size);

	byte_code.instantiate();
	byte_code->set_bytes(bytes);

	return byte_code;
}

void BlipKitBytecodeLoader::_bind_methods() {
}

String BlipKitBytecodeLoader::_to_string() const {
	return vformat("<BlipKitBytecodeLoader#%d>", get_instance_id());
}

Error BlipKitBytecodeSaver::_save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
	if (not p_path.ends_with(".blipc")) {
		return ERR_FILE_UNRECOGNIZED;
	}

	BlipKitBytecode *byte_code = Object::cast_to<BlipKitBytecode>(p_resource.ptr());

	ERR_FAIL_NULL_V(byte_code, ERR_INVALID_PARAMETER);

	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE);

	if (not file.is_valid()) {
		return FileAccess::get_open_error();
	}

	file->store_buffer(byte_code->get_byte_array());
	file->close();

	return OK;
}

Error BlipKitBytecodeSaver::_set_uid(const String &p_path, int64_t p_uid) {
	if (not p_path.ends_with(".blipc")) {
		return ERR_FILE_UNRECOGNIZED;
	}

	Ref<FileAccess> file = FileAccess::open(vformat("%s.uid", p_path), FileAccess::WRITE);

	if (not file.is_valid()) {
		return FileAccess::get_open_error();
	}

	ResourceUID *resid = ResourceUID::get_singleton();
	const int64_t id = resid->create_id();

	resid->add_id(id, p_path);
	file->store_line(resid->id_to_text(id));

	return OK;
}

bool BlipKitBytecodeSaver::_recognize(const Ref<Resource> &p_resource) const {
	BlipKitBytecode *byte_code = Object::cast_to<BlipKitBytecode>(p_resource.ptr());

	return byte_code != nullptr;
}

PackedStringArray BlipKitBytecodeSaver::_get_recognized_extensions(const Ref<Resource> &p_resource) const {
	return { "blipc" };
}

void BlipKitBytecodeSaver::_bind_methods() {
}

String BlipKitBytecodeSaver::_to_string() const {
	return vformat("<BlipKitBytecodeSaver#%d>", get_instance_id());
}
