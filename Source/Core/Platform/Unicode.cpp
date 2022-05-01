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
}
}
}


