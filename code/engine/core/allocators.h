#pragma once

#include "types.h"

// Prefer opaque structs to void pointers!
typedef struct AlloctorInstance AllocatorInstance;

typedef struct Allocator {
    // Data to be used by different allocator types
    AllocatorInstance *instanceData;

    // Allocation function
    void *(*Reallocate)(struct Allocator *allocator, void *currentAllocation, usize newSize);

    // Free
    void (*Free)(void *currentAllocation, usize size);
} Allocator;