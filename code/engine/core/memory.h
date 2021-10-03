#pragma once

#include "allocators.h"
#include "types.h"

#define CORE_MEMORY_DEFAULT_ALIGNMENT (2 * sizeof(void *)) // Likely 4 or 8 bytes
#define CORE_MEMORY_VIRTUAL_RESERVE Megabytes(64)

void AllocatorClear(Allocator *allocator);

#pragma region Virtual Memory

typedef struct MemoryVirtual {
    void *reserveBlock;
} MemoryVirtual;

void *MemoryVirtualReallocate(Allocator *allocator, void *currentAllocation, usize oldSize, usize newSize);
void MemoryVirtualFree(Allocator *allocator, void *currentAllocation, usize size);
Allocator MemoryVirtualAllocatorCreate();
void MemoryVirtualAllocatorDestroy(Allocator *allocator);

#pragma endregion

#pragma region Memory Arena

typedef struct MemoryArena {
    Allocator *backing;
} MemoryArena;

void *MemoryArenaReallocate(Allocator *allocator, void *currentAllocation, usize oldSize, usize newSize);
void MemoryArenaFree(Allocator *allocator, void *currentAllocation, usize size);
Allocator MemoryArenaCreate(Allocator *backingAllocator);
void MemoryArenaDestroy(Allocator *allocator);

#pragma endregion