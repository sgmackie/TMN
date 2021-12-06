#pragma once

#include "Allocator.hpp"

namespace Core {
namespace Platform {
    // General
    const wchar_t *CharToWideChar(Allocator *Allocator, const char *string);

    // File IO
    typedef struct File {
        u64 Handle;
    } File;

    enum class FileAccessMode {
        FILE_READ,
        FILE_WRITE
    };

    File FileOpen(Allocator *allocator, const char *path, const FileAccessMode mode);
}
}