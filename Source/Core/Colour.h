#pragma once

#include "Types.h"

namespace Core {
	namespace Colour {
		// TODO: Convert to union of array/u32 value?
		// TODO: Support linear colours https://entropymine.com/imageworsener/srgbformula/
		// TODO: Linear colour with u8 conversion https://gist.github.com/rygorous/2203834
		struct SRGBA
		{
			u8 Red;
			u8 Green;
			u8 Blue;
			u8 Alpha;

			SRGBA::SRGBA(u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 100)
			{
				Red = r;
				Green = g;
				Blue = b;
				Alpha = a;
			}
		};

		u32 ConvertSRGBAToU32(const SRGBA& colour);
	}
}

