#pragma once

#include "../CoreTypes.h"
#include "../Hashing.h"
#include "../Logging.h"
#include "DynamicArray.h"

namespace Core {
namespace Container {
    template <typename T>
    class HashTable
    {
    public:
        HashTable(Core::Allocator *InAllocator, const usize InExponent)
            : Keys(InAllocator)
            , Values(InAllocator)
        {
            Allocator = InAllocator;
            Exponent = InExponent;
            Count = 0;
        }

        usize Lookup(u64 Hash, usize Index)
        {
            u32 Mask = ((u32) 1 << Exponent) - 1;
            u32 Step = (Hash >> (64 - Exponent)) | 1;
            return (Index + Step) & Mask;
        }

        usize Count;
        usize Exponent;
        DynamicArray<T> Keys;
        DynamicArray<T> Values;
        Core::Allocator *Allocator;
    };
}
}
