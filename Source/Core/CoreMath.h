#pragma once

#include "CoreTypes.h"
#include "Logging.h"

#define Min(A, B) ((A < B) ? (A) : (B))
#define Max(A, B) ((A > B) ? (A) : (B))
#define CORE_SMALL_NUMBER (1.e-8f)

namespace Core {
template <typename T>
constexpr bool IsPowerOfTwo(const T Value)
{
    return ((Value & (Value - 1)) == (T) 0);
}

template <typename T>
constexpr T RoundUpToPower(const T Value, const T Factor)
{
    T Remainder = Value % Factor;
    if (Value == 0 || Remainder == 0) {
        return Value;
    }

    return (Value + Factor) - Remainder;
}

template <typename T>
constexpr bool IsAligned(const T Value, const u64 Alignment)
{
    return !((u64) Value & (Alignment - 1));
}

template <typename T>
constexpr T Align(const T Value, const u64 Alignment)
{
    return (T) (((u64) Value + Alignment - 1) & ~(Alignment - 1));
}

static bool IsNearlyZero(const f32 Value, f32 ErrorTolerance = CORE_SMALL_NUMBER)
{
	return fabs(Value) <= ErrorTolerance;
}

template<typename T>
constexpr T GetRangePercentage(const T Min, const T Max, const T Value)
{
	const T Divisor = Min - Max;
	return (Value - Min) / Divisor;
}

template <typename T>
constexpr T Lerp(const T A, const T B, const T Alpha)
{
	return (T) (A + Alpha * (B-A));
}

template <typename T>
static constexpr T Clamp(const T Value, const T Min, const T Max)
{
	return (Value < Min) ? Min : (Value < Max) ? Value : Max;
}

template <typename T1, typename T2>
static T2 MapRange(const T1 InMin, const T1 InMax, const T2 OutMin, const T2 OutMax, const T1 Value)
{
	const T1 Percentage = GetRangePercentage<T1>(InMin, InMax, Value);
	const T1 ClampedPercentage = Clamp<T1>(Percentage, 0, 1);
	return Lerp<T2>(OutMin, OutMax, ClampedPercentage);
}

union Vector3
{
	Vector3(const f32 InX = 0.0f, const f32 InY = 0.0f, const f32 InZ = 0.0f)
    {
        X = InX;
        Y = InY;
        Z = InZ;
    }

    Vector3(std::initializer_list<f32> List)
    {
        CORE_ASSERT(List.size() == 3);
        memcpy(Elements.Buffer, List.begin(), (sizeof(f32) * List.size()));
    }

	// TODO: Write unit test suite
    static Vector3 Add(const Vector3 &A, const Vector3 &B);
    static Vector3 Subtract(const Vector3 &A, const Vector3 &B);
    static Vector3 Multiply(const Vector3 &A, const Vector3 &B);
    static Vector3 MultiplyByScalar(const Vector3 &A, const f32 Scale);
	static Vector3 Divide(const Vector3 &A, const Vector3 &B);
    static Vector3 DivideByScalar(const Vector3 &A, const f32 Scale);
    static f32 DotProduct(const Vector3 &A, const Vector3 &B);
    static Vector3 CrossProduct(const Vector3 &A, const Vector3 &B);
    static f32 Length(const Vector3 &A);
    static Vector3 Normalize(const Vector3 &A);
	static Vector3 UnitVector(const Vector3 &A);

	// Operators
	inline Vector3 operator+(const Vector3 &A)
	{
		return Vector3::Add(*this, A);
	}

	inline Vector3 operator+=(const Vector3 &A)
	{
		return Vector3::Add(*this, A);
	}

	inline Vector3 operator+(const Vector3 &A) const
	{
		return Vector3::Add(*this, A);
	}

	inline Vector3 operator-(const Vector3 &A)
	{
		return Vector3::Subtract(*this, A);
	}

	inline Vector3 operator-(const Vector3 &A) const
	{
		return Vector3::Subtract(*this, A);
	}

	inline Vector3 operator*(const Vector3 &A)
	{
		return Vector3::Multiply(*this, A);
	}

	inline Vector3 operator*(const Vector3 &A) const
	{
		return Vector3::Multiply(*this, A);
	}

	inline Vector3 operator*(const f32 Scalar)
	{
		return Vector3::MultiplyByScalar(*this, Scalar);
	}

	inline Vector3 operator*(const f32 Scalar) const
	{
		return Vector3::MultiplyByScalar(*this, Scalar);
	}

	inline Vector3 operator/(const Vector3 &A)
	{
		return Vector3::Divide(*this, A);
	}

	inline Vector3 operator/(const Vector3 &A) const
	{
		return Vector3::Divide(*this, A);
	}

	inline Vector3 operator/(const f32 Scalar)
	{
		return Vector3::DivideByScalar(*this, Scalar);
	}

	inline Vector3 operator/(const f32 Scalar) const
	{
		return Vector3::DivideByScalar(*this, Scalar);
	}

	struct
	{
		f32 X;
		f32 Y;
		f32 Z;
	};
	Container::Array<f32, 3> Elements;
};

Vector3 RayCast(const Vector3 &Origin, const Vector3 &Direction, const f32 Position);
}
