#pragma once

#include "Allocator.h"
#include "CoreTypes.h"

namespace Core {
// UTF8
usize GetUTF8CodpointLength(u32 CodePoint);
bool UTF8IsValid(const char *UTF8String);

// UTF8 Encoding
char *UTF8Encode(char *UTF8String, u32 CodePoint);
uint32_t UTF8Decode(const char **UTF8String);

// UTF16 Encoding
u16 *UTF16Encode(u16 *UTF16String, u32 CodePoint);
u32 UTF16Decode(const u16 **UTF16String);

// UTF8 - UTF16 Conversions
usize GetUTF8LengthFromUTF16(const u16 *UTF16String);
usize GetUTF16LengthFromUTF8(const char *UTF8String);
u16 *UTF8ToUTF16(Core::Allocator *Allocator, const char *UTF8String);
char *UTF16ToUTF8(Core::Allocator *Allocator, const u16 *UTF16String);
}