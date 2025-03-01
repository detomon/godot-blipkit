#pragma once

#include "byte_stream.hpp"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitBytecode : public Resource {
	GDCLASS(BlipKitBytecode, Resource)

public:
	enum Status {
		OK,
		ERR_INVALID_BINARY,
		ERR_UNSUPPORTED_VERSION,
	};

	struct Header {
		char name[4] = { 'B', 'L', 'I', 'P' };
		uint8_t version = 0;
		uint8_t flags[3] = { 0 };
		uint32_t footer_offset = 0;
	};

	static constexpr int VERSION = 1;

	static const Header binary_header;

private:
	Header header;
	ByteStream byte_code;
	HashMap<String, uint32_t> labels;
	Status status = OK;
	String error_message;

	bool read_header();
	bool read_footer();

	int fail_with_error(Status p_status, const String &p_error_message);

public:
	static Ref<BlipKitBytecode> create_with_byte_stream(const ByteStream &p_bytes);

	bool is_valid() const;
	Status get_status() const;
	String get_error_message() const;

	void set_bytes(const Vector<uint8_t> &p_bytes);

	Vector<uint8_t> get_bytes() const;
	PackedByteArray get_byte_array() const;

	bool has_label(const String &p_label) const;
	PackedStringArray get_labels() const;
	int get_start_position() const;
	int get_label_position(const String &p_label) const;

protected:
	static void _bind_methods();
	String _to_string() const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
};

} // namespace BlipKit

VARIANT_ENUM_CAST(BlipKit::BlipKitBytecode::Status);
