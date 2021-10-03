#pragma once

#include "memory.h"
#include "math.h"
#include "platform.h"
#include "profiling.h"

void AllocatorClear(Allocator *allocator) {
    allocator->instanceData = NULL;
    allocator->currentSize = 0;
    allocator->Reallocate = NULL;
    allocator->Free = NULL;
}

#pragma region Virtual Allocator

void *MemoryVirtualReallocate(Allocator *allocator, void *currentAllocation, usize oldSize, usize newSize) {
    void *reserveBlock = allocator->instanceData;
    ASSERT(reserveBlock != NULL, "Memory: Virtual Allocator reserve block is invalid!");

    void *pointer = platform->virtualMemory->Commit(reserveBlock, newSize);
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
    result.Reallocate = &MemoryVirtualReallocate;
    result.Free = &MemoryVirtualFree;

    // Reserve a large block ahead of time to commit from
    result.instanceData = platform->virtualMemory->Reserve(CORE_MEMORY_VIRTUAL_RESERVE);

    return result;
}

void MemoryVirtualAllocatorDestroy(Allocator *allocator) {
    platform->virtualMemory->Free((void *)allocator->instanceData, CORE_MEMORY_VIRTUAL_RESERVE);
    AllocatorClear(allocator);
}

#pragma endregion

#pragma region Memory Arena

void *MemoryArenaAlignedAllocation(MemoryArena *arena, usize size, usize alignment) {
    const usize allocationSize = size + alignment;
    return NULL;
}

void *MemoryArenaReallocate(Allocator *allocator, void *currentAllocation, usize oldSize, usize newSize) {
    MemoryArena *arena = (MemoryArena *)allocator->instanceData;
    return MemoryArenaAlignedAllocation(arena, newSize, CORE_MEMORY_DEFAULT_ALIGNMENT);
}

void MemoryArenaFree(Allocator *allocator, void *currentAllocation, usize size) {
}

Allocator MemoryArenaCreate(Allocator *backingAllocator) {
    Allocator result = { 0 };

    MemoryArena *arena = (MemoryArena *)backingAllocator->Reallocate(backingAllocator, NULL, 0, sizeof(MemoryArena));
    arena->backing = backingAllocator;

    result.instanceData = (AllocatorInstance *)arena;
    result.currentSize = 0;
    result.Reallocate = &MemoryArenaReallocate;
    result.Free = &MemoryArenaFree;
    return result;
}

void MemoryArenaDestroy(Allocator *allocator) {
    MemoryArena *arena = (MemoryArena *)allocator->instanceData;
    arena->backing->Free(arena->backing, allocator->instanceData, sizeof(MemoryArena));
    AllocatorClear(allocator);
}

#pragma endregion