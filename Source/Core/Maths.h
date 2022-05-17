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

	union Vector3
	{
		struct
		{
			f32 X;
			f32 Y;
			f32 Z;
		};
		f32 Elements[3];

		Vector3(const f32 x = 0, const f32 y = 0, const f32 z = 0) 
		{
			X = x;
			Y = y;
			Z = z;
		}

		static Vector3 Add(const Vector3& a, const Vector3& b);
		static Vector3 Sub(const Vector3& a, const Vector3& b);
		static Vector3 Multiply(const Vector3& a, const Vector3& b);
		static Vector3 MultiplyByScalar(const Vector3& a, const f32 scale);
		static f32 DotProduct(const Vector3& a, const Vector3& b);
		static Vector3 CrossProduct(const Vector3& a, const Vector3& b);
		static f32 Length(const Vector3& a);
		static Vector3 Normalize(const Vector3& a);
	};


	Vector3 RayCast(const Vector3& origin, const Vector3& direction, const f32 position);
}
}

