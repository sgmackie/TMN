#include "Platform.h"
#include "Container/Array.h"

// References:
// https://www.unicode.org/faq/utf_bom.html
// https://www.unicode.org/Public/UNIDATA/

namespace Core {
namespace Platform {
namespace Unicode {
    // UTF8
    bool UTF8IsValid(const char* utf8String)
    {
        const u8 *utf8 = (const u8* )utf8String;
        while (*utf8) {
            const u32 codePoint = *utf8++;
            if ((codePoint & 0x80) == 0x0) {
                continue;
            }
            else if ((codePoint & 0xe0) == 0xc0) {
                const u32 codePoint0 = *utf8++;
                if (!((codePoint0 & 0xc0) == 0x80)) {
                    return false;
                }
            } 
            else if ((codePoint & 0xf0) == 0xe0) {
                const u32 codePoint0 = *utf8++;
                const u32 codePoint1 = *utf8++;
                if (!(((codePoint0 & 0xc0) == 0x80) && ((codePoint1 & 0xc0) == 0x80))) {
                    return false;
                }
            }
            else if ((codePoint & 0xf8) == 0xf0) {
                const u32 codePoint0 = *utf8++;
                const u32 codePoint1 = *utf8++;
                const u32 codePoint2 = *utf8++;
                if (!(((codePoint0 & 0xc0) == 0x80) && ((codePoint1 & 0xc0) == 0x80) && ((codePoint2 & 0xc0) == 0x80))) {
                    return false;
                }
            } 
            else {
                return false;
            }
        }

        return true;
    }

	char *UTF8Encode(char *utf8String, u32 codepoint)
	{
		if (codepoint < 0x80U) {
			*utf8String = (u8) codepoint;
			return utf8String + 1;
		} 
		else if (codepoint < 0x800U) {
			utf8String[0] = (u8)((codepoint >> 6) | 0xc0);
			utf8String[1] = (u8)((codepoint & 0x3f) | 0x80);
			return utf8String + 2;
		} 
		else if (codepoint < 0x10000U) {
			utf8String[0] = (u8)((codepoint >> 12) | 0xe0);
			utf8String[1] = (u8)(((codepoint >> 6) & 0x3f) | 0x80);
			utf8String[2] = (u8)((codepoint & 0x3f) | 0x80);
			return utf8String + 3;
		} 
		else if (codepoint < 0x110000U) {
			utf8String[0] = (u8)((codepoint >> 18) | 0xf0);
			utf8String[1] = (u8)(((codepoint >> 12) & 0x3F) | 0x80);
			utf8String[2] = (u8)(((codepoint >> 6) & 0x3F) | 0x80);
			utf8String[3] = (u8)((codepoint & 0x3F) | 0x80);
			return utf8String + 4;
		} 
		else {
			return utf8String;
		}
	}

	u32 UTF8Decode(const char **utf8String)
	{
		const u8 **utf8 = (const u8 **) utf8String;
		const u32 codepoint = *(*utf8)++;
		if ((codepoint & 0x80) == 0x0) {
			return codepoint;
		} 
		else if ((codepoint & 0xe0) == 0xc0) {
			const u32 codePoint0 = *(*utf8)++;
			return ((codepoint & 0x1f) << 6) | (codePoint0 & 0x3f);
		} 
		else if ((codepoint & 0xf0) == 0xe0) {
			const u32 codePoint0 = *(*utf8)++;
			const u32 codePoint1 = *(*utf8)++;
			return ((codepoint & 0x0f) << 12) | ((codePoint0 & 0x3f) << 6) | (codePoint1 & 0x3f);
		} 
		else if ((codepoint & 0xf8) == 0xf0) {
			const u32 codePoint0 = *(*utf8)++;
			const u32 codePoint1 = *(*utf8)++;
			const u32 codePoint2 = *(*utf8)++;
			return ((codepoint & 0x07) << 18) | ((codePoint0 & 0x3f) << 12) | ((codePoint1 & 0x3f) << 6) | (codePoint2 & 0x3f);
		} 
		else {
			return 0;
		}
	}

	u16 *UTF16Encode(u16 *utf16String, u32 codepoint)
	{
		if (codepoint >= 0xd800U && codepoint <= 0xdfff) {
			return nullptr;
		}
		else if (codepoint >= 0x10000U) {
			const u32 offset = codepoint - 0x10000U;
			const u32 highSurrogate = 0xd800U + (offset >> 10);
			const u32 lowSurrogate = 0xdc00U + (offset & 0x3ffU);
			*utf16String++ = (u16) highSurrogate;
			*utf16String++ = (u16) lowSurrogate;
		} 
		else {
			*utf16String++ = (u16) codepoint;
		}
		return utf16String;
	}

	u32 UTF16Decode(const u16 **utf16String)
	{
		const u32 codepoint = *(*utf16String)++;
		if (codepoint < 0xd800U || codepoint > 0xdfffU) {
			return codepoint;
		}
		else {
			uint32_t highSurrogate = codepoint - 0xd800U;
			uint32_t lowSurrogate = *(*utf16String)++ - 0xdc00U;
			uint32_t offset = (highSurrogate << 10) + lowSurrogate;
			return offset + 0x10000U;
		}
	}

	usize GetUTF16LengthFromUTF8(const char* utf8String)
	{
		usize length = 0;
		while (*utf8String) {
			const u32 codepoint = UTF8Decode(&utf8String);
			++length;
			if (codepoint >= 0x10000U) {
				++length;
			}
		}
		return length;
	}

	u16* UTF8ToUTF16(Memory::Allocator *allocator, const char* utf8String)
	{
		const uint32_t length = GetUTF16LengthFromUTF8(utf8String);
		u16 *uft16 = (u16 *) allocator->Allocate(sizeof(u16) * (length + 1));
		u16 *uft16Start = uft16;
		
		while (*utf8String) {
			uft16 = UTF16Encode(uft16, UTF8Decode(&utf8String));
		}
		
		*uft16++ = 0;
		return (u16 *) uft16Start;
	}

	usize GetUTF8CodpointLength(u32 codepoint)
	{
		if (codepoint < 0x80U) {
			return 1;
		}
		else if (codepoint < 0x800U) {
			return 2;
		}
		else if (codepoint < 0x10000U) {
			return 3;
		}
		else if (codepoint < 0x110000U) {
			return 4;
		}
		else {
			return 0;
		}
	}

	usize GetUTF8LengthFromUTF16(const u16* utf16String)
	{
		usize length = 0;
		while (*utf16String) {
			length += GetUTF8CodpointLength(UTF16Decode(&utf16String));
		}
		return length;
	}

	char* UTF16ToUTF8(Memory::Allocator *allocator, const u16* utf16String)
	{
		const uint32_t length = GetUTF8LengthFromUTF16(utf16String);

		char *utf8 = (char *) allocator->Allocate(sizeof(char) * (length + 1));
		char *utf8Start = utf8;

		while (*utf16String) {
			utf8 = UTF8Encode(utf8, UTF16Decode(&utf16String));
		}

		*utf8++ = 0;
		return utf8Start;
	}
}
}
}

