#include "CoreMath.h"
#include "math.h"

namespace Core {
Vector3 Vector3::Add(const Vector3 &A, const Vector3 &B)
{
    const f32 X = A.X + B.X;
    const f32 Y = A.Y + B.Y;
    const f32 Z = A.Z + B.Z;
    return Vector3(X, Y, Z);
}

Vector3 Vector3::Sub(const Vector3 &A, const Vector3 &B)
{
    const f32 X = A.X - B.X;
    const f32 Y = A.Y - B.Y;
    const f32 Z = A.Z - B.Z;
    return Vector3(X, Y, Z);
}

Vector3 Vector3::Multiply(const Vector3 &A, const Vector3 &B)
{
    const f32 X = A.X * B.X;
    const f32 Y = A.Y * B.Y;
    const f32 Z = A.Z * B.Z;
    return Vector3(X, Y, Z);
}

Vector3 Vector3::MultiplyByScalar(const Vector3 &A, const f32 Scalar)
{
    const f32 X = A.X * Scalar;
    const f32 Y = A.Y * Scalar;
    const f32 Z = A.Z * Scalar;
    return Vector3(X, Y, Z);
}

f32 Vector3::DotProduct(const Vector3 &A, const Vector3 &B)
{
    return (A.X * B.X) + (A.Y * B.Y) + (A.Z * B.Z);
}

Vector3 Vector3::CrossProduct(const Vector3 &A, const Vector3 &B)
{
    const f32 X = (A.Y * B.Z) - (A.Z * B.Y);
    const f32 Y = (A.Z * B.X) - (A.X * B.Z);
    const f32 Z = (A.X * B.Y) - (A.Y * B.X);
    return Vector3(X, Y, Z);
}

f32 Vector3::Length(const Vector3 &A)
{
    return sqrtf(Vector3::DotProduct(A, A));
}

Vector3 Vector3::Normalize(const Vector3 &A)
{
    const f32 Length = Vector3::Length(A);
    if (Length < 0.00001f) {
        return Vector3(0, 0, 0);
    }
    const f32 InverseLength = 1.0f / Length;
    return Vector3::MultiplyByScalar(A, InverseLength);
}

Vector3 RayCast(const Vector3 &Origin, const Vector3 &Direction, const f32 Position)
{
    const Vector3 ScaledDirection = Vector3::MultiplyByScalar(Direction, Position);
    return Vector3::Add(Origin, ScaledDirection);
}
}
