#pragma once

#include "memory.h"
#include "math.h"
#include "platform.h"
#include "profiling.h"

void *MemoryVirtualReallocate(Allocator *allocator, void *currentAllocation, usize oldSize, usize newSize) {
    void *pointer = platform->virtualMemory->Allocate(newSize);
    ASSERT(pointer != NULL, "Memory: Virtual Allocator failed to commit pages!");
    allocator->currentSize += newSize;

    return pointer;
}

void MemoryVirtualFree(Allocator *allocator, void *currentAllocation, usize size) {
    ASSERT(size >= allocator->currentSize, "Memory: Virtual Allocator trying to free size larger than committed");
    platform->virtualMemory->Free(currentAllocation, size);
    allocator->currentSize -= size;
}

Allocator MemoryVirtualAllocatorCreate() {
    Allocator result = { 0 };
    result.currentSize = 0;
    result.Reallocate = MemoryVirtualReallocate;
    result.Free = MemoryVirtualFree;
    return result;
}
