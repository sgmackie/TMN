#include "Unicode.h"

// References:
// https://www.unicode.org/faq/utf_bom.html
// https://www.unicode.org/Public/UNIDATA/

namespace Core {
// UTF8
usize GetUTF8CodpointLength(u32 CodePoint)
{
    if (CodePoint < 0x80U) {
        return 1;
    } else if (CodePoint < 0x800U) {
        return 2;
    } else if (CodePoint < 0x10000U) {
        return 3;
    } else if (CodePoint < 0x110000U) {
        return 4;
    } else {
        return 0;
    }
}

bool UTF8IsValid(const char *UTF8String)
{
    const u8 *UTF8 = (const u8 *) UTF8String;
    while (*UTF8) {
        const u32 CodePoint = *UTF8++;
        if ((CodePoint & 0x80) == 0x0) {
            continue;
        } else if ((CodePoint & 0xe0) == 0xc0) {
            const u32 CodePoint0 = *UTF8++;
            if (!((CodePoint0 & 0xc0) == 0x80)) {
                return false;
            }
        } else if ((CodePoint & 0xf0) == 0xe0) {
            const u32 CodePoint0 = *UTF8++;
            const u32 CodePoint1 = *UTF8++;
            if (!(((CodePoint0 & 0xc0) == 0x80) && ((CodePoint1 & 0xc0) == 0x80))) {
                return false;
            }
        } else if ((CodePoint & 0xf8) == 0xf0) {
            const u32 CodePoint0 = *UTF8++;
            const u32 CodePoint1 = *UTF8++;
            const u32 CodePoint2 = *UTF8++;
            if (!(((CodePoint0 & 0xc0) == 0x80) && ((CodePoint1 & 0xc0) == 0x80) && ((CodePoint2 & 0xc0) == 0x80))) {
                return false;
            }
        } else {
            return false;
        }
    }

    return true;
}

// UTF8 Encoding
char *UTF8Encode(char *UTF8String, u32 CodePoint)
{
    if (CodePoint < 0x80U) {
        *UTF8String = (u8) CodePoint;
        return UTF8String + 1;
    } else if (CodePoint < 0x800U) {
        UTF8String[0] = (u8) ((CodePoint >> 6) | 0xc0);
        UTF8String[1] = (u8) ((CodePoint & 0x3f) | 0x80);
        return UTF8String + 2;
    } else if (CodePoint < 0x10000U) {
        UTF8String[0] = (u8) ((CodePoint >> 12) | 0xe0);
        UTF8String[1] = (u8) (((CodePoint >> 6) & 0x3f) | 0x80);
        UTF8String[2] = (u8) ((CodePoint & 0x3f) | 0x80);
        return UTF8String + 3;
    } else if (CodePoint < 0x110000U) {
        UTF8String[0] = (u8) ((CodePoint >> 18) | 0xf0);
        UTF8String[1] = (u8) (((CodePoint >> 12) & 0x3F) | 0x80);
        UTF8String[2] = (u8) (((CodePoint >> 6) & 0x3F) | 0x80);
        UTF8String[3] = (u8) ((CodePoint & 0x3F) | 0x80);
        return UTF8String + 4;
    } else {
        return UTF8String;
    }
}

u32 UTF8Decode(const char **UTF8String)
{
    const u8 **UTF8 = (const u8 **) UTF8String;
    const u32 CodePoint = *(*UTF8)++;
    if ((CodePoint & 0x80) == 0x0) {
        return CodePoint;
    } else if ((CodePoint & 0xe0) == 0xc0) {
        const u32 CodePoint0 = *(*UTF8)++;
        return ((CodePoint & 0x1f) << 6) | (CodePoint0 & 0x3f);
    } else if ((CodePoint & 0xf0) == 0xe0) {
        const u32 CodePoint0 = *(*UTF8)++;
        const u32 CodePoint1 = *(*UTF8)++;
        return ((CodePoint & 0x0f) << 12) | ((CodePoint0 & 0x3f) << 6) | (CodePoint1 & 0x3f);
    } else if ((CodePoint & 0xf8) == 0xf0) {
        const u32 CodePoint0 = *(*UTF8)++;
        const u32 CodePoint1 = *(*UTF8)++;
        const u32 CodePoint2 = *(*UTF8)++;
        return ((CodePoint & 0x07) << 18) | ((CodePoint0 & 0x3f) << 12) | ((CodePoint1 & 0x3f) << 6) | (CodePoint2 & 0x3f);
    } else {
        return 0;
    }
}

// UTF16 Encoding
u16 *UTF16Encode(u16 *UTF16String, u32 CodePoint)
{
    if (CodePoint >= 0xd800U && CodePoint <= 0xdfff) {
        return nullptr;
    } else if (CodePoint >= 0x10000U) {
        const u32 Offset = CodePoint - 0x10000U;
        const u32 HighSurrogate = 0xd800U + (Offset >> 10);
        const u32 LowSurrogate = 0xdc00U + (Offset & 0x3ffU);
        *UTF16String++ = (u16) HighSurrogate;
        *UTF16String++ = (u16) LowSurrogate;
    } else {
        *UTF16String++ = (u16) CodePoint;
    }
    return UTF16String;
}

u32 UTF16Decode(const u16 **UTF16String)
{
    const u32 CodePoint = *(*UTF16String)++;
    if (CodePoint < 0xd800U || CodePoint > 0xdfffU) {
        return CodePoint;
    } else {
        uint32_t HighSurrogate = CodePoint - 0xd800U;
        uint32_t LowSurrogate = *(*UTF16String)++ - 0xdc00U;
        uint32_t Offset = (HighSurrogate << 10) + LowSurrogate;
        return Offset + 0x10000U;
    }
}

// UTF8 - UTF16 Conversions
usize GetUTF8LengthFromUTF16(const u16 *UTF16String)
{
    usize Length = 0;
    while (*UTF16String) {
        Length += GetUTF8CodpointLength(UTF16Decode(&UTF16String));
    }
    return Length;
}

usize GetUTF16LengthFromUTF8(const char *UTF8String)
{
    usize Length = 0;
    while (*UTF8String) {
        const u32 CodePoint = UTF8Decode(&UTF8String);
        ++Length;
        if (CodePoint >= 0x10000U) {
            ++Length;
        }
    }
    return Length;
}

u16 *UTF8ToUTF16(Core::Allocator *Allocator, const char *UTF8String)
{
    const uint32_t Length = GetUTF16LengthFromUTF8(UTF8String);
    u16 *UTF16 = reinterpret_cast<u16 *>(Allocator->Allocate(sizeof(u16) * (Length + 1)));
    u16 *UTF16Start = UTF16;

    while (*UTF8String) {
        UTF16 = UTF16Encode(UTF16, UTF8Decode(&UTF8String));
    }

    *UTF16++ = 0;
    return reinterpret_cast<u16 *>(UTF16Start);
}

char *UTF16ToUTF8(Core::Allocator *Allocator, const u16 *UTF16String)
{
    const uint32_t Length = GetUTF8LengthFromUTF16(UTF16String);

    char *UTF8 = reinterpret_cast<char *>(Allocator->Allocate(sizeof(char) * (Length + 1)));
    char *UTF8Start = UTF8;

    while (*UTF16String) {
        UTF8 = UTF8Encode(UTF8, UTF16Decode(&UTF16String));
    }

    *UTF8++ = 0;
    return UTF8Start;
}
}
