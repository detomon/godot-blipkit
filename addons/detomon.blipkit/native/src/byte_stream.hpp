#pragma once

#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>

using namespace godot;

namespace BlipKit {

class ByteStream {
private:
	uint32_t count = 0;
	uint32_t pointer = 0;
	PackedByteArray bytes;

	template <typename T>
	_FORCE_INLINE_ T read() {
		constexpr uint32_t byte_count = sizeof(T);

		// Not enough bytes left to read.
		if (unlikely(pointer + byte_count > count)) {
			return T(0);
		}

		const uint8_t *ptr = &bytes.ptr()[pointer];
		T value = 0;
		pointer += byte_count;

		for (uint32_t i = 0; i < byte_count; i++) {
			value |= T(ptr[i]) << (i * 8);
		}

		return value;
	}

	template <typename T>
	_FORCE_INLINE_ void write(T value) {
		constexpr uint32_t byte_count = sizeof(T);
		const uint32_t capacity = bytes.size();

		// Not enough space left to write.
		if (unlikely(pointer + byte_count > capacity)) {
			reserve(pointer + byte_count);
		}

		uint8_t *ptrw = &bytes.ptrw()[pointer];
		pointer += byte_count;
		count = MAX(count, pointer);

		for (uint32_t i = 0; i < byte_count; i++) {
			ptrw[i] = (value >> (i * 8)) & 0xFF;
		}
	}

public:
	void put_u8(uint8_t p_value);
	void put_s8(int8_t p_value);
	void put_u16(uint16_t p_value);
	void put_s16(int16_t p_value);
	void put_f16(float p_value);
	void put_u32(uint32_t p_value);
	void put_s32(int32_t p_value);
	void put_f32(float p_value);
	void put_bytes(const uint8_t *p_bytes, uint32_t p_count);

	uint8_t get_u8();
	int8_t get_s8();
	uint16_t get_u16();
	int16_t get_s16();
	float get_f16();
	uint32_t get_u32();
	int32_t get_s32();
	float get_f32();
	uint32_t get_bytes(uint8_t *r_bytes, uint32_t p_count);

	_FORCE_INLINE_ uint32_t size() const { return count; };
	_FORCE_INLINE_ uint32_t get_position() const { return pointer; }
	_FORCE_INLINE_ uint32_t get_available_bytes() const { return count - pointer; }
	_FORCE_INLINE_ void seek(uint32_t p_offset) { pointer = MIN(p_offset, size()); }

	_FORCE_INLINE_ const uint8_t *ptr() const { return bytes.ptr(); }
	PackedByteArray get_byte_array() const;
	void set_byte_array(const PackedByteArray &p_bytes);

	void reserve(uint32_t p_size);
	void clear();
};

} //namespace BlipKit
