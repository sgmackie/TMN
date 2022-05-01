#include "Math.h"

namespace Core {
namespace Math {
	u32 ConvertSRGBToU32(const ColourSRGB colour)
	{
		return (colour.Red << 24) + (colour.Green << 16) + (colour.Blue << 8) + colour.Alpha;
	}
}
}

