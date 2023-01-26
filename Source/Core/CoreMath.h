#pragma once

#include "CoreTypes.h"
#include "Logging.h"

#define Min(A, B) ((A < B) ? (A) : (B))
#define Max(A, B) ((A > B) ? (A) : (B))

namespace Core {
template <typename T>
constexpr bool IsPowerOfTwo(T Value)
{
    return ((Value & (Value - 1)) == (T) 0);
}

template <typename T>
constexpr T RoundUpToPower(T Value, T Factor)
{
    T Remainder = Value % Factor;
    if (Value == 0 || Remainder == 0) {
        return Value;
    }

    return (Value + Factor) - Remainder;
}

template <typename T>
constexpr bool IsAligned(T Value, u64 Alignment)
{
    return !((u64) Value & (Alignment - 1));
}

template <typename T>
constexpr T Align(T Value, u64 Alignment)
{
    return (T) (((u64) Value + Alignment - 1) & ~(Alignment - 1));
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

    Vector3(const f32 InX = 0, const f32 InY = 0, const f32 InZ = 0)
    {
        X = InX;
        Y = InY;
        Z = InZ;
    }

    Vector3(std::initializer_list<f32> List)
    {
        CORE_ASSERT(List.size() == 3);
        memcpy(Elements, List.begin(), (sizeof(f32) * List.size()));
    }

    // TODO: Write unit test suite
    static Vector3 Add(const Vector3 &A, const Vector3 &B);
    static Vector3 Sub(const Vector3 &A, const Vector3 &B);
    static Vector3 Multiply(const Vector3 &A, const Vector3 &B);
    static Vector3 MultiplyByScalar(const Vector3 &A, const f32 Scale);
    static f32 DotProduct(const Vector3 &A, const Vector3 &B);
    static Vector3 CrossProduct(const Vector3 &A, const Vector3 &B);
    static f32 Length(const Vector3 &A);
    static Vector3 Normalize(const Vector3 &A);
};

Vector3 RayCast(const Vector3 &Origin, const Vector3 &Direction, const f32 Position);
}
