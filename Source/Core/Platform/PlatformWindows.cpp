#if defined(TMNE_PLATFORM_WINDOWS)

#include "Platform.h"
#include "Unicode.h"

// Includes
#define WIN32_LEAN_AND_MEAN
#include <wchar.h>
#include <windows.h>

// Libs
#pragma comment(lib, "Advapi32.lib")

namespace Core {

	// Memory
	void *VirtualReserve(usize size)
	{
		return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
	}

	void VirtualCommit(void *pointer, usize size)
	{
		VirtualAlloc(pointer, size, MEM_COMMIT, PAGE_READWRITE);
	}

	void *VirtualMap(usize size)
	{
		return VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
	}

	void VirtualUnmap(void *pointer, usize size)
	{
		VirtualFree(pointer, 0, MEM_RELEASE);
	}

}

#endif
