#include "blipkit_bytecode.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_interpreter.hpp"
#include "blipkit_waveform.hpp"
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

int BlipKitBytecode::fail_with_error(Status p_status, const String &p_error_message) {
	status = p_status;
	error_message = p_error_message;

	// Seek to end.
	const int size = byte_code.size();
	byte_code.seek(size);

	ERR_FAIL_V_MSG(-1, error_message);
}

bool BlipKitBytecode::read_header() {
	const uint32_t headers_size = byte_code.get_bytes(reinterpret_cast<uint8_t *>(&header), sizeof(header));

	if (headers_size != sizeof(header)) {
		fail_with_error(ERR_INVALID_BINARY, "Truncated binary header.");
		return false;
	}

	if (memcmp(header.name, binary_header.name, sizeof(binary_header.name)) != 0) {
		fail_with_error(ERR_INVALID_BINARY, "Invalid binary header.");
		return false;
	}

	// Check version.
	switch (header.version) {
		case 1: {
			// OK.
		} break;
		default: {
			fail_with_error(ERR_UNSUPPORTED_VERSION, vformat("Unsuported binary version %d.", header.version));
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
	instance->set_bytes(p_bytes.get_bytes());

	if (!instance->is_valid()) {
		return nullptr;
	}

	return instance;
}

bool BlipKitBytecode::is_valid() const {
	return status == OK;
}

void BlipKitBytecode::set_bytes(const Vector<uint8_t> &p_bytes) {
	byte_code.set_bytes(p_bytes);

	if (read_header()) {
		read_footer();
	}
}

BlipKitBytecode::Status BlipKitBytecode::get_status() const {
	return status;
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

bool BlipKitBytecode::has_label(const String &p_label) const {
	return labels.has(p_label);
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

int BlipKitBytecode::get_start_position() const {
	return sizeof(Header);
}

int BlipKitBytecode::get_label_position(const String &p_label) const {
	const HashMap<String, uint32_t>::ConstIterator label_itor = labels.find(p_label);

	ERR_FAIL_COND_V_MSG(label_itor == labels.end(), get_start_position(), vformat("Label '%s' not found.", p_label));

	return label_itor->value;
}

void BlipKitBytecode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("is_valid"), &BlipKitBytecode::is_valid);
	ClassDB::bind_method(D_METHOD("get_status"), &BlipKitBytecode::get_status);
	ClassDB::bind_method(D_METHOD("get_error_message"), &BlipKitBytecode::get_error_message);
	ClassDB::bind_method(D_METHOD("get_byte_array"), &BlipKitBytecode::get_byte_array);
	ClassDB::bind_method(D_METHOD("has_label", "label"), &BlipKitBytecode::has_label);
	ClassDB::bind_method(D_METHOD("get_labels"), &BlipKitBytecode::get_labels);
	ClassDB::bind_method(D_METHOD("get_start_position"), &BlipKitBytecode::get_start_position);
	ClassDB::bind_method(D_METHOD("get_label_position", "label"), &BlipKitBytecode::get_label_position);

	BIND_ENUM_CONSTANT(OK);
	BIND_ENUM_CONSTANT(ERR_INVALID_BINARY);
	BIND_ENUM_CONSTANT(ERR_UNSUPPORTED_VERSION);

	BIND_CONSTANT(VERSION);
}

String BlipKitBytecode::_to_string() const {
	return vformat("<BlipKitBytecode:#%d>", int64_t(this));
}

void BlipKitBytecode::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "byte_code", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_STORAGE));
}

bool BlipKitBytecode::_set(const StringName &p_name, const Variant &p_value) {
	const String &name = p_name;

	if (name == "byte_code") {
		const PackedByteArray &byte_array = p_value;
		const uint32_t bytes_size = byte_array.size();
		Vector<uint8_t> bytes;
		bytes.resize(bytes_size);
		const uint8_t *ptr = byte_array.ptr();

		memcpy(bytes.ptrw(), ptr, bytes_size);
		set_bytes(bytes);
	} else {
		return false;
	}

	return true;
}

bool BlipKitBytecode::_get(const StringName &p_name, Variant &r_ret) const {
	const String &name = p_name;

	if (name == "byte_code") {
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

	return type == "BlipKitBytecode";
}

String BlipKitBytecodeLoader::_get_resource_type(const String &p_path) const {
	return "BlipKitBytecode";
}

Variant BlipKitBytecodeLoader::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const {
	if (!p_path.ends_with(".blipc")) {
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
	return vformat("<BlipKitBytecodeLoader:#%d>", int64_t(this));
}

Error BlipKitBytecodeSaver::_save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
	BlipKitBytecode *byte_code = Object::cast_to<BlipKitBytecode>(p_resource.ptr());

	ERR_FAIL_NULL_V(byte_code, godot::ERR_INVALID_PARAMETER);

	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE);

	if (!file.is_valid()) {
		return FileAccess::get_open_error();
	}

	file->store_buffer(byte_code->get_byte_array());
	file->close();

	return OK;
}

Error BlipKitBytecodeSaver::_set_uid(const String &p_path, int64_t p_uid) {
	Ref<FileAccess> file = FileAccess::open(vformat("%s.uid", p_path), FileAccess::WRITE);

	printf("_set_uid: %s\n", vformat("path: %s, p_uid: %d", p_path, p_uid).utf8().ptr());

	if (!file.is_valid()) {
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
	return vformat("<BlipKitBytecodeSaver:#%d>", int64_t(this));
}
