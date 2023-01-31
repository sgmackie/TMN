#include "Colour.h"
#include "CoreMath.h"

namespace Core {
ColourLinear ColourLinear::Add(const ColourLinear &A, const ColourLinear &B)
{
	const f32 Red = A.Red + B.Red;
	const f32 Green = A.Green + B.Green;
	const f32 Blue = A.Blue + B.Blue;
	const f32 Alpha = A.Alpha + B.Alpha;
	return ColourLinear(Red, Green, Blue, Alpha);
}

ColourLinear ColourLinear::Subtract(const ColourLinear &A, const ColourLinear &B)
{
	const f32 Red = A.Red - B.Red;
	const f32 Green = A.Green - B.Green;
	const f32 Blue = A.Blue - B.Blue;
	const f32 Alpha = A.Alpha - B.Alpha;
	return ColourLinear(Red, Green, Blue, Alpha);
}

ColourLinear ColourLinear::Multiply(const ColourLinear &A, const ColourLinear &B)
{
	const f32 Red = A.Red * B.Red;
	const f32 Green = A.Green * B.Green;
	const f32 Blue = A.Blue * B.Blue;
	const f32 Alpha = A.Alpha * B.Alpha;
	return ColourLinear(Red, Green, Blue, Alpha);
}

ColourLinear ColourLinear::MultiplyByScalar(const ColourLinear &A, const f32 Scale)
{
	const f32 Red = A.Red * Scale;
	const f32 Green = A.Green * Scale;
	const f32 Blue = A.Blue * Scale;
	const f32 Alpha = A.Alpha * Scale;
	return ColourLinear(Red, Green, Blue, Alpha);
}

ColourLinear ColourLinear::Divide(const ColourLinear &A, const ColourLinear &B)
{
	const f32 Red = A.Red / B.Red;
	const f32 Green = A.Green / B.Green;
	const f32 Blue = A.Blue / B.Blue;
	const f32 Alpha = A.Alpha / B.Alpha;
	return ColourLinear(Red, Green, Blue, Alpha);
}

ColourLinear ColourLinear::DivideByScalar(const ColourLinear &A, const f32 Scale)
{
	const f32 Red = A.Red / Scale;
	const f32 Green = A.Green / Scale;
	const f32 Blue = A.Blue / Scale;
	const f32 Alpha = A.Alpha / Scale;
	return ColourLinear(Red, Green, Blue, Alpha);
}

ColourSRGBA ColourLinear::ConvertToU8() const
{
	Container::Array<u8, 3> ConvertedElements;
	for (usize i = 0; i < Elements.Count(); ++i)
	{
		f32 Conversion = 0.0f;
		f32 Element = Elements[i];

		if (!(Element > 0.0f))
		{
			Conversion = 0.0f;
		}
		else if (Element <= 0.0031308f)
		{
			Conversion = 12.92f * Element;
		}
		else if (Element < 1.0f)
		{
			Conversion = 1.055f * pow(Element, 1.0f / 2.4f) - 0.055f;
		}
		else
		{
			Conversion = 1.0f;
		}

		ConvertedElements[i] = Conversion * 255.0f;
	}

	const u8 ConvertedAlpha = MapRange<f32, u8>(0.0f, 1.0f, 0, 255, Alpha);
	return ColourSRGBA(ConvertedElements[0], ConvertedElements[1], ConvertedElements[2], ConvertedAlpha);
}

u32 ColourSRGBA::ConvertToU32(const ColourSRGBA &Colour)
{
    return (Colour.Red << 24) + (Colour.Green << 16) + (Colour.Blue << 8) + Colour.Alpha;
}

ColourLinear ColourSRGBA::ConvertToF32(const ColourSRGBA &Colour)
{
	// F32 conversion
	static Container::Array<f32, 256> SRGBTable;
	static bool IsTableInit;

	if (!IsTableInit)
	{
		for (usize i = 0; i < SRGBTable.Count(); ++i)
		{
			f32 Conversion = (f32) i * (1.0f / 255.0f);
			if (Conversion <= 0.04045f)
			{
				SRGBTable[i] = Conversion / 12.92f;
			}
			else
			{
				SRGBTable[i] = pow((Conversion + 0.055f) / 1.055f, 2.4f);
			}
		}
		IsTableInit = true;
	}

	const f32 Red = SRGBTable[Colour.Red];
	const f32 Green = SRGBTable[Colour.Green];
	const f32 Blue = SRGBTable[Colour.Blue];
	const f32 Alpha = MapRange<u8, f32>(0, 255, 0.0f, 1.0f, Colour.Alpha);
	return ColourLinear(Red, Green, Blue, Alpha);
}
}
