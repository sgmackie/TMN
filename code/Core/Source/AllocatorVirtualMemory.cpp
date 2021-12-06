#include "AllocatorVirtualMemory.hpp"

namespace Core {
void *AllocatorVirtualMemory::Allocate(const usize size)
{
    return nullptr;
}

void *AllocatorVirtualMemory::Reallocate(void *oldPointer, const usize oldSize, const usize newSize)
{
    return nullptr;
}

void AllocatorVirtualMemory::Free(void *pointer, const usize size)
{
}
}