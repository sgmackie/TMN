#pragma once

#include "allocators.h"
#include "types.h"


#define MEMORY_ARENA_ALIGNMENT 4
#define MEMORY_ARENA_BLOCK_SIZE 1024

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

usize GetAlignmentOffset(MemoryArena *arena, usize alignment);
void *MemoryArenaPush(Allocator *allocator, void *block, usize size);
MemoryArena CreateMemoryArena();