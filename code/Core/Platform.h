#pragma once

#include "Types.h"

namespace Core {
namespace Platform {
    // File IO
    typedef struct File {
        u64 Handle;
    } File;

    typedef enum FileAccessMode {
        FILE_READ,
        FILE_WRITE
    } FileAccessMode;

    File FileOpen(const char *path, const FileAccessMode mode);
}
}