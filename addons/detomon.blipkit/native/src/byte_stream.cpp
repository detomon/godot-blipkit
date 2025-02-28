#include "byte_stream.hpp"
#include "float16.hpp"

using namespace BlipKit;
using namespace godot;

union FInt16 {
	uint16_t u;
	float16 f;
};

union FInt32 {
	uint32_t u;
	float f;
};

void ByteStream::put_u8(uint8_t p_value) {
	write(p_value);
}

void ByteStream::put_s8(int8_t p_value) {
	write(p_value);
}

void ByteStream::put_u16(uint16_t p_value) {
	write(p_value);
}

void ByteStream::put_s16(int16_t p_value) {
	write(p_value);
}

void ByteStream::put_f16(float p_value) {
	FInt16 d;
	d.f = p_value;
	write(d.u);
}

void ByteStream::put_u32(uint32_t p_value) {
	write(p_value);
}

void ByteStream::put_s32(int32_t p_value) {
	write(p_value);
}

void ByteStream::put_f32(float p_value) {
	FInt32 d;
	d.f = p_value;
	write(d.u);
}

void ByteStream::put_bytes(const int8_t *p_bytes, uint32_t p_count) {
	const uint32_t capacity = bytes.size();

	if (unlikely(pointer + p_count > capacity)) {
		reserve(pointer + p_count);
	}

	uint8_t *ptr = &bytes.ptrw()[pointer];

	memcpy(ptr, p_bytes, p_count);
	pointer += p_count;
	count = MAX(count, pointer);
}

uint8_t ByteStream::get_u8() {
	return read<uint8_t>();
}

int8_t ByteStream::get_s8() {
	return read<int8_t>();
}

uint16_t ByteStream::get_u16() {
	return read<uint16_t>();
}

int16_t ByteStream::get_s16() {
	return read<int16_t>();
}

float ByteStream::get_f16() {
	FInt16 d;
	d.u = read<uint16_t>();
	return d.f;
}

uint32_t ByteStream::get_u32() {
	return read<uint32_t>();
}

int32_t ByteStream::get_s32() {
	return read<int32_t>();
}

float ByteStream::get_f32() {
	FInt32 d;
	d.u = read<uint32_t>();
	return d.f;
}

uint32_t ByteStream::get_bytes(int8_t *r_bytes, uint32_t p_count) {
	p_count = MIN(p_count, get_available_bytes());

	const uint8_t *ptr = &bytes.ptr()[pointer];
	memcpy(r_bytes, ptr, p_count);
	pointer += p_count;

	return p_count;
}

PackedByteArray ByteStream::get_byte_array() const {
	PackedByteArray ret = bytes;
	ret.resize(count);

	return ret;
}

void ByteStream::set_byte_array(const PackedByteArray &p_bytes) {
	bytes = p_bytes;
	count = bytes.size();
	pointer = 0;
}

void ByteStream::reserve(uint32_t p_size) {
	p_size = MAX(128, next_power_of_2(p_size));

	if (p_size > bytes.size()) {
		bytes.resize(p_size);
	}
}

void ByteStream::clear() {
	count = 0;
	pointer = 0;
}
