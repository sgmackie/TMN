#pragma once

#include "types.h"

// Prefer opaque structs to void pointers!
typedef struct AlloctorInstance AllocatorInstance;

typedef struct Allocator {
    // Data to be used by different allocator types - needs to be allocated!
    AllocatorInstance *instanceData;
    usize currentSize;

    // Allocation function
    void *(*Reallocate)(struct Allocator *allocator, void *currentAllocation, usize oldSize, usize newSize);

    // Free
    void (*Free)(struct Allocator *allocator, void *currentAllocation, usize size);
} Allocator;