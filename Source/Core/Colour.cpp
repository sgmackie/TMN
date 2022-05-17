#include "Colour.h"

namespace Core {
namespace Colour {
	u32 ConvertSRGBAToU32(const SRGBA& colour)
	{
		return (colour.Red << 24) + (colour.Green << 16) + (colour.Blue << 8) + colour.Alpha;
	}
}
}

