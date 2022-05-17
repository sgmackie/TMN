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

		const char* GetLastSystemError(Memory::Allocator *allocator)
		{
			wchar_t *utf16 = NULL;
			const DWORD errorCode = GetLastError();
			const DWORD stringLength = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &utf16, 0, NULL);
			
			// TODO: Use temp memory
			char *utf8 = Unicode::UTF16ToUTF8(allocator, (const u16*) utf16);
			LocalFree(utf16);

			return utf8;
		}
}
}


#endif
