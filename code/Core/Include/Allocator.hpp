#pragma once

#include "Types.hpp"

namespace Core {
class Allocator
{
public:
    virtual void *Allocate(const usize size) = 0;
    virtual void *Reallocate(void *oldPointer, const usize oldSize, const usize newSize) = 0;
    virtual void Free(void *pointer, const usize size) = 0;
};
}