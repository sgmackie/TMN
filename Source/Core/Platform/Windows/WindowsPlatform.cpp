#if defined(CORE_PLATFORM_WINDOWS)

#include "WindowsPlatform.h"

namespace Core {
namespace Platform {
		MemoryStats GetMemoryStats()
		{
			MemoryStats result = {};
			
			MEMORYSTATUSEX memoryStatus = {};
			memoryStatus.dwLength = sizeof(memoryStatus);
			GlobalMemoryStatusEx(&memoryStatus);
			result.TotalPhysical = memoryStatus.ullTotalPhys;
			result.TotalVirtual = memoryStatus.ullTotalPageFile;

			SYSTEM_INFO systemInfo = {};
			GetSystemInfo(&systemInfo);
			result.PageSize = systemInfo.dwPageSize;

			return result;
		}

		DWORD GetAllocFlags(const VirtualMemoryFlags flags)
		{
			DWORD result = 0;
			if (flags == VirtualMemoryFlags::ReadWrite) {
				result = PAGE_READWRITE;
			}
			if (flags == VirtualMemoryFlags::Protect) {
				result = PAGE_NOACCESS;
			}
			return result;
		}

		void *VirtualMemoryReserve(const usize size, const VirtualMemoryFlags flags)
		{
			return VirtualAlloc(0, size, MEM_RESERVE, GetAllocFlags(flags));
		}

		void VirtualMemoryCommit(void *pointer, const usize size, const VirtualMemoryFlags flags)
		{
			VirtualAlloc(pointer, size, MEM_COMMIT, GetAllocFlags(flags));
		}

		void VirtualMemoryDecommit(void *pointer, const usize size)
		{
			VirtualFree(pointer, size, MEM_DECOMMIT);
		}

		void VirtualMemoryProtect(void *pointer, const usize size)
		{
			BOOL result = VirtualProtect(pointer, size, PAGE_NOACCESS, 0);
			assert(result);
		}
}
}


#endif
