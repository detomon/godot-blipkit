#include "blipkit_bytecode_saver.hpp"
#include "blipkit_bytecode.hpp"
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/templates/hashfuncs.hpp>

using namespace BlipKit;
using namespace godot;

Error BlipKitBytecodeSaver::_save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
	BlipKitBytecode *byte_code = Object::cast_to<BlipKitBytecode>(p_resource.ptr());

	ERR_FAIL_NULL_V(byte_code, godot::ERR_INVALID_PARAMETER);

	Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::WRITE);

	ERR_FAIL_COND_V(file.is_null(), godot::ERR_FILE_CANT_WRITE);

	file->store_buffer(byte_code->get_byte_array());
	file->close();

	return OK;
}

Error BlipKitBytecodeSaver::_set_uid(const String &p_path, int64_t p_uid) {
	// int64_t save_id = hash_murmur3_one_64(p_path.hash()) & 0x7FFFFFFFFFFFFFFF;

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
	// ClassDB::bind_method(D_METHOD("_save"), &BlipKitBytecodeSaver::_save);
	// ClassDB::bind_method(D_METHOD("_set_uid"), &BlipKitBytecodeSaver::_set_uid);
	// ClassDB::bind_method(D_METHOD("_recognize"), &BlipKitBytecodeSaver::_recognize);
	// ClassDB::bind_method(D_METHOD("_get_recognized_extensions"), &BlipKitBytecodeSaver::_get_recognized_extensions);
}

String BlipKitBytecodeSaver::_to_string() const {
	return vformat("<BlipKitBytecodeSaver:#%d>", int64_t(this));
}
