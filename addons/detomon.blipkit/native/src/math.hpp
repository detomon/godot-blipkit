#pragma once

#include <godot_cpp/core/defs.hpp>

namespace detomon::BlipKit {

// Copied from: https://github.com/godotengine/godot/blob/master/core/math/math_funcs.h
static _ALWAYS_INLINE_ float half_to_float(uint16_t h) {
	union {
		uint32_t u32;
		float f32;
	} u;

	uint16_t h_exp, h_sig;
	uint32_t f_sgn, f_exp, f_sig;

	h_exp = (h & 0x7c00u);
	f_sgn = ((uint32_t)h & 0x8000u) << 16;
	switch (h_exp) {
		case 0x0000u: { /* 0 or subnormal */
			h_sig = (h & 0x03ffu);
			/* Signed zero */
			if (h_sig == 0) {
				u.u32 = f_sgn;
				break;
			}
			/* Subnormal */
			h_sig <<= 1;
			while ((h_sig & 0x0400u) == 0) {
				h_sig <<= 1;
				h_exp++;
			}
			f_exp = ((uint32_t)(127 - 15 - h_exp)) << 23;
			f_sig = ((uint32_t)(h_sig & 0x03ffu)) << 13;
			u.u32 = f_sgn + f_exp + f_sig;
		} break;
		case 0x7c00u: { /* inf or NaN */
			/* All-ones exponent and a copy of the significand */
			u.u32 = f_sgn + 0x7f800000u + (((uint32_t)(h & 0x03ffu)) << 13);
		} break;
		default: { /* normalized */
			/* Just need to adjust the exponent and shift */
			u.u32 = f_sgn + (((uint32_t)(h & 0x7fffu) + 0x1c000u) << 13);
		} break;
	}

	return u.f32;
}

// Copied from: https://github.com/godotengine/godot/blob/master/core/math/math_funcs.h
static _ALWAYS_INLINE_ uint16_t float_to_half(float f) {
	union {
		float fv;
		uint32_t ui;
	} ci;
	ci.fv = f;

	uint32_t x = ci.ui;
	uint32_t sign = (unsigned short)(x >> 31);
	uint32_t mantissa;
	uint32_t exponent;
	uint16_t hf;

	// get mantissa
	mantissa = x & ((1 << 23) - 1);
	// get exponent bits
	exponent = x & (0xFF << 23);
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

	return hf;
}

} //namespace detomon::BlipKit
