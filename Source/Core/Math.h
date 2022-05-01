#pragma once

#include "Types.h"

#define Min(A, B) ((A < B) ? (A) : (B))
#define Max(A, B) ((A > B) ? (A) : (B))

namespace Core {
namespace Math {
	template <typename T>
	constexpr bool IsPowerOfTwo(T value)
	{
		return ((value & (value - 1)) == (T) 0);
	}

	template <typename T>
	constexpr T RoundUpToPower(T value, T factor)
	{
		T remainder = value % factor;
		if (value == 0 || remainder == 0) {
			return value;
		}

		return (value + factor) - remainder;
	}

	template <typename T>
	constexpr bool IsAligned(T value, u64 alignment)
	{
		return !((u64) value & (alignment - 1));
	}

	template <typename T>
	constexpr T Align(T value, u64 alignment)
	{
		return (T)(((u64) value + alignment - 1) & ~(alignment - 1));
	}

	// TODO: Convert to union of array/u32 value?
	struct ColourSRGB
	{
		u8 Red;
		u8 Green;
		u8 Blue;
		u8 Alpha;

		ColourSRGB::ColourSRGB(u8 r, u8 g, u8 b, u8 a = 100)
		{
			Red = r;
			Green = g;
			Blue = b;
			Alpha = a;
		}
	};

	u32 ConvertSRGBToU32(const ColourSRGB colour);
}
}

