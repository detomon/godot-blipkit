#pragma once

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/memory.hpp>
#include <type_traits>

namespace BlipKit {

template <class T, uint32_t CAPACITY>
class FixedVector {
private:
	uint32_t _size = 0;
	alignas(T) uint8_t _data[CAPACITY * sizeof(T)];

public:
	_FORCE_INLINE_ constexpr T *ptr() { return (T *)(_data); }
	_FORCE_INLINE_ constexpr const T *ptr() const { return (const T *)(_data); }

	_FORCE_INLINE_ constexpr uint32_t size() const { return _size; }
	_FORCE_INLINE_ constexpr bool is_empty() const { return !_size; }
	_FORCE_INLINE_ constexpr uint32_t capacity() const { return CAPACITY; }

	constexpr const T &operator[](uint32_t p_index) const {
		CRASH_COND(p_index >= _size);
		return ptr()[p_index];
	}

	constexpr T &operator[](uint32_t p_index) {
		CRASH_COND(p_index >= _size);
		return ptr()[p_index];
	}

	~FixedVector() {
		if constexpr (!std::is_trivially_destructible_v<T>) {
			for (uint32_t i = 0; i < _size; i++) {
				ptr()[i].~T();
			}
		}
	}

	constexpr void resize(uint32_t p_size) {
		if (p_size > _size) {
			CRASH_COND(p_size >= CAPACITY);
			for (size_t i = _size; i < p_size; i++) {
				memnew_placement(&ptr()[i], T);
			}
		} else if (p_size < _size) {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				for (uint32_t i = p_size; i < _size; i++) {
					ptr()[i].~T();
				}
			}
		}

		_size = p_size;
	}
};

} //namespace BlipKit
