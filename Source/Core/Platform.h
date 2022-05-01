#pragma once

#include "Types.h"
#include "Platform/PlatformUnicode.h"
#include "Platform/PlatformFile.h"

namespace Core {
namespace Platform {
    struct MemoryStats
    {
        // Everything is in bytes
	    u64 TotalPhysical;
        u64 TotalVirtual;
        usize PageSize;
    };

	enum class VirtualMemoryFlags
	{
		ReadWrite,
		Protect
	};

    MemoryStats GetMemoryStats();

	void *VirtualMemoryReserve(const usize size, const VirtualMemoryFlags flags);
	void VirtualMemoryCommit(void *pointer, const usize size, const VirtualMemoryFlags flags);
    void VirtualMemoryDecommit(void *pointer, const usize size);
    void VirtualMemoryProtect(void *pointer, const usize size);
}
}

#if defined(CORE_PLATFORM_WINDOWS)
#include "Platform/Windows/WindowsPlatform.h"
#endif


#if defined(CORE_PLATFORM_MACOS)
#include "Platform/MacOS/MacOSPlatform.h"
#endif