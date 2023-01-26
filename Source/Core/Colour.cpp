#include "Colour.h"

namespace Core {
u32 ConvertSRGBAToU32(const ColourSRGBA &Colour)
{
    return (Colour.Red << 24) + (Colour.Green << 16) + (Colour.Blue << 8) + Colour.Alpha;
}
}
