#pragma once

#include "Allocator.h"


namespace Core {
namespace Unicode {
    // UTF8
    u32 UTF8DecodeToCodePoint(const char *utf8, const usize index);
    u16 *UTF8ConvertToUTF16(Allocator *allocator, const char *utf8);
    u8 UTF8GetCodePointLength(const u32 codePoint);

    // UTF16

    u32 UTF16DecodeToCodePoint(const u16 *utf16, const usize index);
    char *UTF16ConvertToUTF8(const u16 *utf16);
}
}