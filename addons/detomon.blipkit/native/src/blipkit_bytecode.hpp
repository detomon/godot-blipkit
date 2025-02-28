#pragma once

#include "byte_stream.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitBytecode : public Resource {
	GDCLASS(BlipKitBytecode, Resource)

public:
	static constexpr int VERSION = 1;

	struct Header {
		char name[4] = { 'B', 'L', 'I', 'P' };
		uint8_t version = VERSION;
		uint8_t flags[3] = { 0 };
		uint32_t footer_offset = 0;
	};

	static const Header binary_header;

private:
	Header header;
	ByteStream byte_code;
	HashMap<String, uint32_t> labels;
	bool valid = false;

	void initialize(const ByteStream &p_bytes);
	bool read_header();
	bool read_footer();

public:
	static Ref<BlipKitBytecode> create_with_byte_stream(const ByteStream &p_bytes);

	bool is_valid() const;
	void set_bytes(const Vector<uint8_t> &p_bytes);
	Vector<uint8_t> get_bytes() const;
	PackedByteArray get_byte_array() const;
	PackedStringArray get_labels() const;

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit
