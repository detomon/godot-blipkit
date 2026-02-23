#pragma once

#include "byte_stream.hpp"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/resource_format_saver.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitBytecode : public Resource {
	GDCLASS(BlipKitBytecode, Resource)

public:
	enum State {
		OK,
		ERR_INVALID_BINARY,
		ERR_UNSUPPORTED_VERSION,
	};

	struct Header {
		uint8_t magic[4] = { 'B', 'L', 'I', 'P' };
		uint8_t version = 0;
		uint8_t flags[3] = { 0 };
		uint8_t code_magic[4] = { 'c', 'o', 'd', 'e' };
		uint32_t bytecode_size = 0;
	};

	struct Label {
		String name;
		uint32_t byte_offset = 0;
	};

	static constexpr int VERSION = 0;

	static const Header binary_header;

private:
	Header header;
	ByteStreamReader bytecode;
	HashMap<String, uint32_t> label_indices;
	LocalVector<Label> labels;
	State state = OK;
	String error_message;

	bool read_header();
	bool read_sections();
	bool read_labels();

	int fail_with_error(State p_state, const String &p_error_message);

public:
	bool is_valid() const;
	State get_state() const;
	String get_error_message() const;

	void set_bytes(const Vector<uint8_t> &p_bytes);

	Vector<uint8_t> get_bytes() const;
	PackedByteArray get_byte_array() const;

	int get_code_section_offset() const;
	int get_code_section_size() const;
	bool has_label(const String &p_name) const;
	int find_label(const String &p_name) const;
	int get_label_count() const;
	String get_label_name(int p_label_index) const;
	int get_label_position(int p_label_index) const;

protected:
	static void _bind_methods();
	String _to_string() const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
};

class BlipKitBytecodeLoader : public ResourceFormatLoader {
	GDCLASS(BlipKitBytecodeLoader, ResourceFormatLoader)

public:
	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _handles_type(const StringName &p_type) const override;
	virtual String _get_resource_type(const String &p_path) const override;
	virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;

protected:
	static void _bind_methods();
	String _to_string() const;
};

class BlipKitBytecodeSaver : public ResourceFormatSaver {
	GDCLASS(BlipKitBytecodeSaver, ResourceFormatSaver)

public:
	virtual Error _save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) override;
	virtual Error _set_uid(const String &p_path, int64_t p_uid) override;
	virtual bool _recognize(const Ref<Resource> &p_resource) const override;
	virtual PackedStringArray _get_recognized_extensions(const Ref<Resource> &p_resource) const override;

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit

VARIANT_ENUM_CAST(BlipKit::BlipKitBytecode::State);
