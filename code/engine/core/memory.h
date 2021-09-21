#pragma once

#include "allocators.h"
#include "types.h"

Allocator MemoryVirtualAllocatorCreate();

#define CORE_MEMORY_ALIGNMENT (2 * sizeof(void *)) // Likely 4 or 8 bytes

typedef struct MemoryArena {
    Allocator *backing;
} MemoryArena;

Allocator MemoryArenaCreate(Allocator *backingAllocator);