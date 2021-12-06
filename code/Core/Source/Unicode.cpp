#include "Unicode.hpp"

// References:
// https://www.unicode.org/faq/utf_bom.html
// https://www.unicode.org/Public/UNIDATA/

namespace Core {
u8 Unicode::UTF8GetCodePointLength(const u32 codePoint)
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

u32 Unicode::UTF8DecodeToCodePoint(const char *utf8, const usize index)
{
    // If the codepoint is chained across 1-4 units then we need to check for that
    const u32 codePoint = utf8[index];
    if ((codePoint & 0x80) == 0x0) {
        return codePoint;
    } else if ((codePoint & 0xe0) == 0xc0) {
        const u32 codeUnit0 = utf8[index + 1];
        return ((codePoint & 0x1f) << 6) | (codeUnit0 & 0x3f);
    } else if ((codePoint & 0xf0) == 0xe0) {
        const u32 codeUnit0 = utf8[index + 1];
        const u32 codeUnit1 = utf8[index + 2];
        return ((codePoint & 0x0f) << 12) | ((codeUnit0 & 0x3f) << 6) | (codeUnit1 & 0x3f);
    } else if ((codePoint & 0xf8) == 0xf0) {
        const u32 codeUnit0 = utf8[index + 1];
        const u32 codeUnit1 = utf8[index + 2];
        const u32 codeUnit2 = utf8[index + 3];
        return ((codePoint & 0x07) << 18) | ((codeUnit0 & 0x3f) << 12) | ((codeUnit1 & 0x3f) << 6) | (codeUnit2 & 0x3f);
    }

    return 0;
}

u16 *Unicode::UTF8ConvertToUTF16(Allocator *allocator, const char *utf8)
{
    // Get the lenght of the UTF8 string - static function for length of any arbitrary array?
    // Create an Allocator base class with alloc and free virtual functions
    // Alloctator can spawn a temp allocator that exists within a function scope?
    // Temp allocate the code point array, dynamic allocate the UTF16 result

    u32 codePoints[16];

    for (usize i = 0; i < 16; ++i) {
        codePoints[i] = UTF8DecodeToCodePoint(utf8, i);
    }

    return nullptr;
}

u32 Unicode::UTF16DecodeToCodePoint(const u16 *utf16, const usize index)
{
    const u32 codePoint = utf16[index];
    if (codePoint < 0xd800U || codePoint > 0xdfffU) {
        return codePoint;
    }

    const u32 highSurrogate = codePoint - 0xd800U;
    const u32 lowSurrogate = (utf16[index] + 1) - 0xdc00U; // Get the next point to complete the pair
    const u32 offset = (highSurrogate << 10) + lowSurrogate;
    return offset + 0x10000U;
}

char *Unicode::UTF16ConvertToUTF8(const u16 *utf16)
{
    u32 codePoints[16];

    for (usize i = 0; i < 16; ++i) {
        codePoints[i] = UTF16DecodeToCodePoint(utf16, i);
    }

    return nullptr;
}
}
