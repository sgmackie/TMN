#pragma once

#include "Types.h"

namespace Core {
namespace Unicode {

    // UTF8

    u8 UTF8GetCodePointLength(const u32 codePoint);

    // UTF16
    u32 UTF16DecodeToCodePoint(const u16 *utf16);
    char *UTF16ConvertToUTF8(const u16 *utf16);
}
}