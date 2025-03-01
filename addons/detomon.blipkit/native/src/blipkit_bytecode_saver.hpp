#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/resource_format_saver.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

using namespace godot;

namespace BlipKit {

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
