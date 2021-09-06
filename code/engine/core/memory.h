#pragma once

#include "allocators.h"
#include "types.h"

#define CORE_MEMORY_ARENA_ALIGNMENT 4
#define CORE_MEMORY_ARENA_BLOCK_SIZE 1024

typedef struct MemoryArenaBlock {
    u8 *base;
    usize size;
    uptr usedSpace;
    struct MemoryArenaBlock *previousBlock;
} MemoryArenaBlock;

typedef struct MemoryArena {
    usize minimumBlockSize;
    MemoryArenaBlock *currentBlock;
    Allocator allocator;
} MemoryArena;

usize AlignmentGetOffset(MemoryArena *arena, usize alignment);
void *MemoryArenaPush(Allocator *allocator, void *block, usize size);
MemoryArena MemoryArenaCreate();