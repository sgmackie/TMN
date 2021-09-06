#pragma once

#include "memory.h"
#include "math.h"
#include "platform.h"

usize AlignmentGetOffset(MemoryArena *arena, usize alignment) {
    usize result = 0;
    usize spacePointer = (usize)arena->currentBlock->base + arena->currentBlock->usedSpace;

    if (spacePointer & (alignment - 1))
        result = alignment - (spacePointer & (alignment - 1));

    return result;
}

void *PushMemoryArena(Allocator *allocator, void *block, usize size) {
    void *result = 0;
    MemoryArena *arena = (MemoryArena *)allocator->instanceData;

    usize newSize = 0;
    if (arena->currentBlock)
        newSize = size + AlignmentGetOffset(arena, CORE_MEMORY_ARENA_ALIGNMENT);

    // Grow the arena
    if (!arena->currentBlock || (arena->currentBlock->usedSpace + newSize) > arena->currentBlock->size) {
        newSize = size;
        arena->minimumBlockSize = CORE_MEMORY_ARENA_BLOCK_SIZE;
        usize blockSize = Maximum(newSize, arena->minimumBlockSize);

        // TODO: Add a flag to set how we allocate memory here, might not always be VM
        MemoryArenaBlock *newBlock = platform->virtualMemory->Allocate(blockSize);
        newBlock->previousBlock = arena->currentBlock;
        arena->currentBlock = newBlock;
    }

    usize alignmentOffset = AlignmentGetOffset(arena, CORE_MEMORY_ARENA_ALIGNMENT);
    uptr blockOffset = arena->currentBlock->usedSpace + alignmentOffset;
    result = arena->currentBlock->base + blockOffset;
    arena->currentBlock->usedSpace += newSize;
    memset(result, 0, newSize);

    return result;
}

MemoryArena MemoryArenaCreate() {
    MemoryArena result = { 0 };
    result.allocator.Reallocate = PushMemoryArena;
    return result;
}