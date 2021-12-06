#include "Platform.hpp"
#include "Unicode.hpp"

#if defined(CORE_PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <wchar.h>
#include <windows.h>

namespace Core {
const wchar_t *Platform::CharToWideChar(Allocator *allocator, const char *string)
{
    const u16 *utf16 = Unicode::UTF8ConvertToUTF16(allocator, string);
    return (const wchar_t *) utf16;
}

Platform::File Platform::FileOpen(Allocator *allocator, const char *path, const FileAccessMode mode)
{
    File result = {};

    const wchar_t *widePath = CharToWideChar(allocator, path);
    result.Handle = 0;

    // Unicode::UTF16ConvertToUTF8();

    return result;
}
}

#endif