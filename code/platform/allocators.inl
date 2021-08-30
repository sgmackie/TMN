#pragma once

#include "types.inl"

// Prefer opaque structs to void pointers!
typedef struct AlloctorInstance AllocatorInstance;

typedef struct Allocator {
    usize initialSize;

    AllocatorInstance *instanceData;

    // Allocation function
    void *(*Reallocate)(struct Allocator *allocator, void *oldBlock, usize newSize);

    // Free
    void (*Free)(void *block, usize size);

    // Memset block values to 0 (do I need the size type?)
    void (*Clear)(void *block);

    // Frees all memory
    void (*Destroy)(void *block);
} Allocator;