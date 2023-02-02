#if defined(CORE_PLATFORM_WINDOWS)

#include "PlatformWindows.h"

#include "../../Allocator.h"
#include "../../Logging.h"
#include "../../Unicode.h"

namespace Core {
namespace Platform {
    const char *GetLastSystemError(Core::Allocator *Allocator)
    {
        wchar_t *UTF16 = NULL;
        const DWORD ErrorCode = GetLastError();
        const DWORD StringLength = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &UTF16, 0, NULL);

        // TODO: Use temp memory
        char *UFT8 = UTF16ToUTF8(Allocator, (const u16 *) UTF16);
        LocalFree(UTF16);

        return UFT8;
    }

    VirtualMemory::MemoryStats VirtualMemory::GetMemoryStats()
    {
        MemoryStats Result = {};

        MEMORYSTATUSEX MemoryStatus = {};
        MemoryStatus.dwLength = sizeof(MemoryStatus);
        GlobalMemoryStatusEx(&MemoryStatus);
        Result.TotalPhysical = MemoryStatus.ullTotalPhys;
        Result.TotalVirtual = MemoryStatus.ullTotalPageFile;

        SYSTEM_INFO SystemInfo = {};
        GetSystemInfo(&SystemInfo);
        Result.PageSize = SystemInfo.dwPageSize;

        return Result;
    }

    DWORD VirtualMemory::GetAllocFlags(const Flags Flags)
    {
        DWORD Result = 0;
        if (Flags == Flags::ReadWrite) {
            Result = PAGE_READWRITE;
        }
        if (Flags == Flags::Protect) {
            Result = PAGE_NOACCESS;
        }
        return Result;
    }

    void *VirtualMemory::Reserve(const usize Size, const Flags Flags)
    {
        return VirtualAlloc(0, Size, MEM_RESERVE, GetAllocFlags(Flags));
    }

    void VirtualMemory::Commit(void *Pointer, const usize Size, const Flags Flags)
    {
        VirtualAlloc(Pointer, Size, MEM_COMMIT, GetAllocFlags(Flags));
    }

    void VirtualMemory::Decommit(void *Pointer, const usize Size)
    {
        VirtualFree(Pointer, Size, MEM_DECOMMIT);
    }

    void VirtualMemory::Protect(void *Pointer, const usize Size)
    {
        BOOL Result = VirtualProtect(Pointer, Size, PAGE_NOACCESS, 0);
        CORE_ASSERT(Result);
    }

	u64 Time::GetCPUTime()
	{
		LARGE_INTEGER Counter;
		QueryPerformanceCounter(&Counter);
		return Counter.QuadPart;
	}

	Time::SystemTime Time::GetSystemTime()
	{
		SYSTEMTIME ClockTime;
		GetSystemTime(&ClockTime);

		Time::SystemTime Result;
		Result.Year = ClockTime.wYear;
		Result.Month = ClockTime.wMonth;
		Result.Day = ClockTime.wDay;
		Result.Hour = ClockTime.wHour;
		Result.Minute = ClockTime.wMinute;
		Result.Second = ClockTime.wSecond;
		Result.Milliseconds = ClockTime.wMilliseconds;
		return Result;
	}

    void Process::PrintToConsole(Core::Allocator *Allocator, const char *Buffer)
    {
        wchar_t *WideBuffer = reinterpret_cast<wchar_t *>(UTF8ToUTF16(Allocator, Buffer));
        OutputDebugStringW(WideBuffer);
    }
}
}

#endif
