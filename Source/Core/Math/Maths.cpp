#include "Maths.h"
#include "math.h"

namespace Core {
namespace Math {
	Vector3 Vector3::Add(const Vector3& a, const Vector3& b)
	{
		const f32 x = a.X + b.X;
		const f32 y = a.Y + b.Y;
		const f32 z = a.Z + b.Z;
		return Vector3(x, y, z);
	}

	Vector3 Vector3::Sub(const Vector3& a, const Vector3& b)
	{
		const f32 x = a.X - b.X;
		const f32 y = a.Y - b.Y;
		const f32 z = a.Z - b.Z;
		return Vector3(x, y, z);
	}

	Vector3 Vector3::Multiply(const Vector3& a, const Vector3& b)
	{
		const f32 x = a.X * b.X;
		const f32 y = a.Y * b.Y;
		const f32 z = a.Z * b.Z;
		return Vector3(x, y, z);
	}

	Vector3 Vector3::MultiplyByScalar(const Vector3& a, const f32 scalar)
	{
		const f32 x = a.X * scalar;
		const f32 y = a.Y * scalar;
		const f32 z = a.Z * scalar;
		return Vector3(x, y, z);
	}

	f32 Vector3::DotProduct(const Vector3& a, const Vector3& b)
	{
		return (a.X * b.X) + (a.Y * b.Y) + (a.Z * b.Z);
	}

	Vector3 Vector3::CrossProduct(const Vector3& a, const Vector3& b)
	{
		const f32 x = (a.Y * b.Z) - (a.Z * b.Y);
		const f32 y = (a.Z * b.X) - (a.X * b.Z);
		const f32 z = (a.X * b.Y) - (a.Y * b.X);
		return Vector3(x, y, z);
	}

	f32 Vector3::Length(const Vector3& a)
	{
		return sqrtf(Vector3::DotProduct(a, a));
	}

	Vector3 Vector3::Normalize(const Vector3& a)
	{
		const f32 length = Length(a);
		if (length < 0.00001f) {
			return Vector3(0, 0, 0);
		}
		const f32 inverseLength = 1.0f / length;
		return Vector3::MultiplyByScalar(a, inverseLength);
	}

	Vector3 RayCast(const Vector3& origin, const Vector3& direction, const f32 position)
	{
		const Vector3 scaledDirection = Vector3::MultiplyByScalar(direction, position);
		return Vector3::Add(origin, scaledDirection);
	}
}
}

