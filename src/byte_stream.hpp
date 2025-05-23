#pragma once

#include "decls.hpp"
#include <godot_cpp/templates/local_vector.hpp>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

namespace BlipKit {

class ByteStreamReader {
private:
	Vector<uint8_t> bytes;
	uint32_t count = 0;
	uint32_t pointer = 0;

	template <typename T>
	_ALWAYS_INLINE_ T read() {
		constexpr uint32_t byte_count = sizeof(T);

		// Not enough bytes left to read.
		if (pointer + byte_count > count) [[unlikely]] {
			return T(0);
		}

		T value = 0;
		const uint8_t *ptr = &bytes.ptr()[pointer];
		pointer += byte_count;

		for (uint32_t i = 0; i < byte_count; i++) {
			value |= T(ptr[i]) << (i * 8);
		}

		return value;
	}

public:
	uint8_t get_u8();
	int8_t get_s8();
	uint16_t get_u16();
	int16_t get_s16();
	float get_f16();
	uint32_t get_u32();
	int32_t get_s32();
	float get_f32();
	uint32_t get_bytes(uint8_t *r_bytes, uint32_t p_count);

	_ALWAYS_INLINE_ uint32_t size() const { return count; };
	_ALWAYS_INLINE_ uint32_t get_position() const { return pointer; }
	_ALWAYS_INLINE_ uint32_t get_available_bytes() const { return count - pointer; }
	_ALWAYS_INLINE_ void seek(uint32_t p_offset) { pointer = MIN(p_offset, size()); }

	void set_bytes(const Vector<uint8_t> &p_bytes);

	_ALWAYS_INLINE_ const uint8_t *ptr() const { return bytes.ptr(); }
	Vector<uint8_t> get_bytes() const;
};

class ByteStreamWriter {
private:
	LocalVector<uint8_t> bytes;
	uint32_t count = 0;
	uint32_t pointer = 0;

	template <typename T>
	_ALWAYS_INLINE_ T read() {
		constexpr uint32_t byte_count = sizeof(T);

		// Not enough bytes left to read.
		if (pointer + byte_count > count) [[unlikely]] {
			return T(0);
		}

		T value = 0;
		const uint8_t *ptr = &bytes.ptr()[pointer];
		pointer += byte_count;

		for (uint32_t i = 0; i < byte_count; i++) {
			value |= T(ptr[i]) << (i * 8);
		}

		return value;
	}

	template <typename T>
	_ALWAYS_INLINE_ void write(T value) {
		constexpr uint32_t byte_count = sizeof(T);
		const uint32_t capacity = bytes.size();

		// Not enough space left to write.
		if (pointer + byte_count > capacity) [[unlikely]] {
			reserve(pointer + byte_count);
		}

		uint8_t *ptrw = &bytes.ptr()[pointer];
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
	uint32_t put_bytes(const Vector<uint8_t> &p_bytes, uint32_t p_from = 0, uint32_t p_size = INT_MAX);
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

	_ALWAYS_INLINE_ uint32_t size() const { return count; };
	_ALWAYS_INLINE_ uint32_t get_position() const { return pointer; }
	_ALWAYS_INLINE_ uint32_t get_available_bytes() const { return count - pointer; }
	_ALWAYS_INLINE_ void seek(uint32_t p_offset) { pointer = MIN(p_offset, size()); }

	_ALWAYS_INLINE_ const uint8_t *ptr() const { return bytes.ptr(); }
	Vector<uint8_t> get_bytes() const;

	_NO_INLINE_ void reserve(uint32_t p_size);
	void clear();
};

} //namespace BlipKit
