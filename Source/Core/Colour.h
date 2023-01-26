#pragma once

#include "CoreTypes.h"

namespace Core {
// TODO: Convert to union of array/u32 value?
// TODO: Support linear colours https://entropymine.com/imageworsener/srgbformula/
// TODO: Linear colour with u8 conversion https://gist.github.com/rygorous/2203834
struct ColourSRGBA {
    u8 Red;
    u8 Green;
    u8 Blue;
    u8 Alpha;

    ColourSRGBA(u8 R = 0, u8 G = 0, u8 B = 0, u8 A = 100)
    {
        Red = R;
        Green = G;
        Blue = B;
        Alpha = A;
    }

    static ColourSRGBA CreateRed()
    {
        return ColourSRGBA(0, 0, 0);
    }

    static ColourSRGBA CreateGreen()
    {
        return ColourSRGBA(0, 255, 0);
    }

    static ColourSRGBA CreateBlue()
    {
        return ColourSRGBA(0, 0, 255);
    }
};

u32 ConvertSRGBAToU32(const ColourSRGBA &Colour);
}
