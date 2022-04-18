#include "PlatformUnicode.h"
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
        const u8 *utf8 = (const u8*) utf8String;
        for (usize i = 0; i < strlen(utf8String); i++) {
            const u32 codePoint = utf8[i];

            if ((codePoint & 0x80) == 0x0) {
                continue;
            }
            else if ((codePoint & 0xe0) == 0xc0) {
                const u32 codePoint0 = utf8[i + 1];
                if (!((codePoint0 & 0xc0) == 0x80)) {
                    return false;
                }
            } 
            else if ((codePoint & 0xf0) == 0xe0) {
                const u32 codePoint0 = utf8[i + 1];
                const u32 codePoint1 = utf8[i + 2];
                if (!(((codePoint0 & 0xc0) == 0x80) && ((codePoint1 & 0xc0) == 0x80))) {
                    return false;
                }
            }
            else if ((codePoint & 0xf8) == 0xf0) {
                const u32 codePoint0 = utf8[i + 1];
                const u32 codePoint1 = utf8[i + 2];
                const u32 codePoint2 = utf8[i + 3];
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
}
}
}


