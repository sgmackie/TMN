#include "../Unicode.h"

// References:
// https://www.unicode.org/faq/utf_bom.html

namespace Core {
namespace Unicode {
    u8 UTF8GetCodePointLength(const u32 codePoint)
    {
        if (codePoint < 0x80U) {
            return 1;
        } else if (codePoint < 0x800U) {
            return 2;
        } else if (codePoint < 0x10000U) {
            return 3;
        } else if (codePoint < 0x110000U) {
            return 4;
        }

        return 0;
    }

    u32 UTF16DecodeToCodePoint(const u16 *utf16, const usize index)
    {
        u32 codePoint = static_cast<u16>(utf16[index]);
        if (codePoint < 0xd800U || codePoint > 0xdfffU) {
            return codePoint;
        }

        u32 highSurrogate = codePoint - 0xd800U;
        u32 lowSurrogate = static_cast<u16>(utf16[index] + 1) - 0xdc00U; // Get the next point to complete the pair
        u32 offset = (highSurrogate << 10) + lowSurrogate;
        return offset + 0x10000U;
    }

    char *UTF16ConvertToUTF8(const u16 *utf16)
    {
        u32 codePoints[16];

        for (usize i = 0; i < 16; ++i) {
            codePoints[i] = UTF16DecodeToCodePoint(utf16, i);
        }

        return nullptr;
    }
}
}
