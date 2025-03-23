// Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md).
// Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Partially copied from: https://github.com/godotengine/godot/blob/master/core/math/math_funcs.h

#pragma once

#define __STDC_WANT_IEC_60559_TYPES_EXT__

#include <float.h>
#include <godot_cpp/core/defs.hpp>

#if defined(FLT16_MIN) && !defined(__wasm__)
#define HAS_FLOAT16 1
#endif

namespace BlipKit {

#ifdef HAS_FLOAT16

typedef _Float16 float16;

#else

struct float16 {
private:
	union FInt32 {
		uint32_t u32;
		float f32;
	};

	uint16_t hf;

public:
	_ALWAYS_INLINE_ operator uint16_t() const {
		return hf;
	}

	_ALWAYS_INLINE_ operator float() const {
		FInt32 fi;
		uint16_t h_exp = (hf & 0x7c00u);
		uint32_t f_sgn = ((uint32_t)hf & 0x8000u) << 16;

		switch (h_exp) {
			case 0x0000u: { /* 0 or subnormal */
				uint16_t h_sig = (hf & 0x03ffu);
				/* Signed zero */
				if (h_sig == 0) {
					fi.u32 = f_sgn;
					break;
				}
				/* Subnormal */
				h_sig <<= 1;
				while ((h_sig & 0x0400u) == 0) {
					h_sig <<= 1;
					h_exp++;
				}
				uint32_t f_exp = ((uint32_t)(127 - 15 - h_exp)) << 23;
				uint32_t f_sig = ((uint32_t)(h_sig & 0x03ffu)) << 13;
				fi.u32 = f_sgn + f_exp + f_sig;
			} break;
			case 0x7c00u: { /* inf or NaN */
				/* All-ones exponent and a copy of the significand */
				fi.u32 = f_sgn + 0x7f800000u + (((uint32_t)(hf & 0x03ffu)) << 13);
			} break;
			default: { /* normalized */
				/* Just need to adjust the exponent and shift */
				fi.u32 = f_sgn + (((uint32_t)(hf & 0x7fffu) + 0x1c000u) << 13);
			} break;
		}

		return fi.f32;
	}

	_ALWAYS_INLINE_ float16 operator=(uint16_t u) {
		hf = u;
		return *this;
	}

	_ALWAYS_INLINE_ float16 operator=(float f) {
		FInt32 fi;
		fi.f32 = f;

		uint32_t x = fi.u32;
		uint32_t sign = (unsigned short)(x >> 31);
		uint32_t mantissa = x & ((1 << 23) - 1);
		uint32_t exponent = x & (0xFF << 23);

		if (exponent >= 0x47800000) {
			// check if the original single precision float number is a NaN
			if (mantissa && (exponent == (0xFF << 23))) {
				// we have a single precision NaN
				mantissa = (1 << 23) - 1;
			} else {
				// 16-bit half-float representation stores number as Inf
				mantissa = 0;
			}
			hf = (((uint16_t)sign) << 15) | (uint16_t)((0x1F << 10)) |
					(uint16_t)(mantissa >> 13);
		}
		// check if exponent is <= -15
		else if (exponent <= 0x38000000) {
			/*
			// store a denorm half-float value or zero
			exponent = (0x38000000 - exponent) >> 23;
			mantissa >>= (14 + exponent);

			hf = (((uint16_t)sign) << 15) | (uint16_t)(mantissa);
			*/
			hf = 0; //denormals do not work for 3D, convert to zero
		} else {
			hf = (((uint16_t)sign) << 15) |
					(uint16_t)((exponent - 0x38000000) >> 13) |
					(uint16_t)(mantissa >> 13);
		}

		return *this;
	}
};
#endif //!HAS_FLOAT16

} //namespace BlipKit
