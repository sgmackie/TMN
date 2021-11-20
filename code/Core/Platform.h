#pragma once

#include "Types.h"

namespace Core
{
namespace Platform
{
    // File IO
    typedef struct File {
        u64 Handle;
    } File;

    File FileOpen(const char path);
}
}