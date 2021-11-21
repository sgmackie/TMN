#include "../Platform.h"
#include "../Unicode.h"

#if defined(CORE_PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <wchar.h>
#include <windows.h>

namespace Core {
namespace Platform {

    File FileOpen(const char *path, const FileAccessMode mode)
    {
        File result = {};

        // Unicode::UTF16ConvertToUTF8();

        return result;
    }
}
}

#endif