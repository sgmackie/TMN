#if defined(CORE_PLATFORM_MACOS)
#include "MacOSPlatform.h"

#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/mman.h>

namespace Core {
namespace Platform {
    MemoryStats GetMemoryStats()
    {
        MemoryStats result = {};

		xsw_usage swapUsage;
        size_t swapUsageSize = sizeof(swapUsage);
		sysctlbyname("vm.swapusage", &swapUsage, &swapUsageSize, nullptr, 0);
		
        i64 physicalCount = 0;
        size_t physicalCountSize = sizeof(i64);
        Array<i32, 2> parameters = {CTL_HW, HW_MEMSIZE};
		sysctl(parameters.ToCArray(), 2, &physicalCount, &physicalCountSize, nullptr, 0);
		
		result.TotalPhysical = physicalCount;
		result.TotalVirtual = physicalCount + swapUsage.xsu_total;
		result.PageSize = (size_t) vm_page_size;
    
        return result;
    }

    void *VirtualReserve(const usize size)
	{
		return mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
	}

    void *VirtualReserveAndCommit(const usize size)
    {
        return mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    }

	void VirtualCommit(void *pointer, const usize size)
	{
        pointer = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
	}

	void VirtualDecommit(void *pointer, const usize size)
	{
		munmap(pointer, size);
	}

    void VirtualProtect(void *pointer, const usize size)
    {
        mprotect(pointer, size, PROT_NONE);
    }
}
}

#endif
