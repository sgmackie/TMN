#pragma once

#include "Allocator.hpp"

namespace Core {
class AllocatorVirtualMemory : public Allocator
{
public:
    void *Allocate(const usize size) override;
    void *Reallocate(void *oldPointer, const usize oldSize, const usize newSize) override;
    void Free(void *pointer, const usize size) override;
};
}