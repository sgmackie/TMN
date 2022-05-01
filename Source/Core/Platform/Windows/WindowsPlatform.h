#if defined(CORE_PLATFORM_WINDOWS)

#pragma once

#include "Types.h"
#include "Platform.h"

// Includes
// TODO: Look at how to reduce windows header file include size
#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#include <windows.h>

// Libs
#pragma comment(lib, "Advapi32.lib")


#endif
