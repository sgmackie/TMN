#if defined(CORE_PLATFORM_MAC)
#include "PlatformMac.h"

#include "../../Container/Array.h"
#include "../../Logging.h"
using namespace Core::Container;

#include <sys/mman.h>
#include <sys/sysctl.h>
#include <sys/types.h>

namespace Core {
namespace Platform {
    VirtualMemory::MemoryStats VirtualMemory::GetMemoryStats()
    {
        VirtualMemory::MemoryStats result = {};

        xsw_usage swapUsage;
        size_t swapUsageSize = sizeof(swapUsage);
        sysctlbyname("vm.swapusage", &swapUsage, &swapUsageSize, nullptr, 0);

        i64 physicalCount = 0;
        size_t physicalCountSize = sizeof(i64);
        Array<i32, 2> parameters = { CTL_HW, HW_MEMSIZE };
        sysctl(parameters.ToCArray(), 2, &physicalCount, &physicalCountSize, nullptr, 0);

        result.TotalPhysical = physicalCount;
        result.TotalVirtual = physicalCount + swapUsage.xsu_total;
        result.PageSize = (size_t) vm_page_size;

        return result;
    }

    void *VirtualMemory::Reserve(const usize Size, const MemoryFlags Flags)
    {
        void *Result = mmap(0, Size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
        CORE_ASSERT(Result != nullptr);
        return Result;
    }

    void VirtualMemory::Commit(void *Pointer, const usize Size, const MemoryFlags Flags)
    {
        // Pointer = mmap(Pointer, Size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
        mprotect(Pointer, Size, PROT_READ | PROT_WRITE);
        CORE_ASSERT(Pointer != nullptr);
    }

    void VirtualMemory::Decommit(void *Pointer, const usize Size)
    {
        CORE_ASSERT(Pointer != nullptr);
        munmap(Pointer, Size);
    }

    void VirtualMemory::Protect(void *Pointer, const usize Size)
    {
        CORE_ASSERT(Pointer != nullptr);
        mprotect(Pointer, Size, PROT_NONE);
    }
}
}

#endif
