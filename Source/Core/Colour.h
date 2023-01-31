#pragma once

#include "CoreTypes.h"
#include "Container/Array.h"

namespace Core {
union ColourSRGBA;
union ColourLinear
{
	ColourLinear(f32 R = 0, f32 G = 0, f32 B = 0, f32 A = 1.0f)
	{
		Red = R;
		Green = G;
		Blue = B;
		Alpha = A;
	}

	static ColourLinear CreateRed()
	{
		return ColourLinear(1.0f, 0, 0);
	}

	static ColourLinear CreateGreen()
	{
		return ColourLinear(0, 1.0f, 0);
	}

	static ColourLinear CreateBlue()
	{
		return ColourLinear(0, 0, 1.0f);
	}

	static ColourLinear Add(const ColourLinear &A, const ColourLinear &B);
	static ColourLinear Subtract(const ColourLinear &A, const ColourLinear &B);
	static ColourLinear Multiply(const ColourLinear &A, const ColourLinear &B);
	static ColourLinear MultiplyByScalar(const ColourLinear &A, const f32 Scale);
	static ColourLinear Divide(const ColourLinear &A, const ColourLinear &B);
	static ColourLinear DivideByScalar(const ColourLinear &A, const f32 Scale);

	// Operators
	inline ColourLinear operator+(const ColourLinear &A)
	{
		return ColourLinear::Add(*this, A);
	}

	inline ColourLinear operator+(const ColourLinear &A) const
	{
		return ColourLinear::Add(*this, A);
	}

	inline ColourLinear operator-(const ColourLinear &A)
	{
		return ColourLinear::Subtract(*this, A);
	}

	inline ColourLinear operator-(const ColourLinear &A) const
	{
		return ColourLinear::Subtract(*this, A);
	}

	inline ColourLinear operator*(const ColourLinear &A)
	{
		return ColourLinear::Multiply(*this, A);
	}

	inline ColourLinear operator*=(const ColourLinear &A)
	{
		return ColourLinear::Multiply(*this, A);
	}

	inline ColourLinear operator*(const ColourLinear &A) const
	{
		return ColourLinear::Multiply(*this, A);
	}

	inline ColourLinear operator*(const f32 Scalar)
	{
		return ColourLinear::MultiplyByScalar(*this, Scalar);
	}

	inline ColourLinear operator*(const f32 Scalar) const
	{
		return ColourLinear::MultiplyByScalar(*this, Scalar);
	}

	inline ColourLinear operator/(const ColourLinear &A)
	{
		return ColourLinear::Divide(*this, A);
	}

	inline ColourLinear operator/(const ColourLinear &A) const
	{
		return ColourLinear::Divide(*this, A);
	}

	inline ColourLinear operator/(const f32 Scalar)
	{
		return ColourLinear::DivideByScalar(*this, Scalar);
	}

	inline ColourLinear operator/(const f32 Scalar) const
	{
		return ColourLinear::DivideByScalar(*this, Scalar);
	}

	ColourSRGBA ConvertToU8() const;

	struct
	{
		f32 Red;
		f32 Green;
		f32 Blue;
		f32 Alpha;
	};
	Container::Array<f32, 4> Elements;
};

// Float to U8 conversions https://gist.github.com/rygorous/2203834
union ColourSRGBA {
    ColourSRGBA(u8 R = 0, u8 G = 0, u8 B = 0, u8 A = 100)
    {
        Red = R;
        Green = G;
        Blue = B;
        Alpha = A;
    }

    static ColourSRGBA CreateRed()
    {
        return ColourSRGBA(255, 0, 0);
    }

    static ColourSRGBA CreateGreen()
    {
        return ColourSRGBA(0, 255, 0);
    }

    static ColourSRGBA CreateBlue()
    {
        return ColourSRGBA(0, 0, 255);
    }

	static u32 ConvertToU32(const ColourSRGBA &Colour);
	ColourLinear ConvertToF32(const ColourSRGBA &Colour);
	
	struct
	{
		u8 Red;
		u8 Green;
		u8 Blue;
		u8 Alpha;
	};
	Container::Array<u8, 4> Elements;
};
}
