#if defined(CORE_PLATFORM_WINDOWS)

#pragma once

#include "CoreTypes.h"
#include "Platform.h"

// Includes
// TODO: Look at how to reduce windows header file include size
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#include <windows.h>

// Libs
#pragma comment(lib, "Kernel32.lib")
#pragma comment(lib, "Advapi32.lib")

namespace Core {
namespace Platform {
    namespace VirtualMemory {
        DWORD GetAllocFlags(const Flags Flags);
    }
}
}

#endif
