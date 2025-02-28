#pragma once

#include "blipkit_interpreter.hpp"
#include "byte_stream.hpp"
#include "godot_cpp/variant/packed_string_array.hpp"
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

namespace BlipKit {

class BlipKitBytecode : public Resource {
	GDCLASS(BlipKitBytecode, Resource)

private:
	static const BlipKitInterpreter::Header binary_header;

	ByteStream byte_code;
	HashMap<String, uint32_t> labels;
	BlipKitInterpreter::Header header;
	bool valid = false;

	void initialize(const PackedByteArray &p_byte_code);
	void read_labels();

public:
	static constexpr int VERSION = 1;

	static Ref<BlipKitBytecode> create_with_byte_code(const PackedByteArray &p_byte_code);

	bool is_valid() const;
	PackedByteArray get_byte_array() const;
	PackedStringArray get_labels() const;

protected:
	static void _bind_methods();
	String _to_string() const;
};

} // namespace BlipKit
