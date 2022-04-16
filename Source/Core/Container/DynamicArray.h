#pragma once

#include "Types.h"
#include "Log.h"
#include "Array.h"
#include "Memory/Allocator.h"

namespace Core {
namespace Container {
    template<typename T>
    class DynamicArray
    {
    public:
        #define DYNAMIC_ARRAY_MAX_STACK_ELEMENTS 16
        #define DYNAMIC_ARRAY_GROWTH_RATE 2

        DynamicArray(Memory::Allocator *allocator, const usize reserveCount = 0)
        {
            Count = 0;
            MaxCount = DYNAMIC_ARRAY_MAX_STACK_ELEMENTS;
            Buffer = StackBuffer.Buffer;
            Allocator = allocator;
            IsHeapAllocated = false;

            if (reserveCount > 0) {
                Reserve(reserveCount);
            }
        }

        ~DynamicArray()
        {
            if (IsHeapAllocated) {
                Allocator->Free(Buffer);
                Buffer = nullptr;
            }

            Count = 0;
            MaxCount = 0;
            Allocator = nullptr;
        }

        void Reserve(const usize reserveCount)
        {
            if (reserveCount < MaxCount) {
                return;
            }

            if (IsHeapAllocated) {
                Buffer = Allocator->ReallocateElement<T>(Buffer, MaxCount, reserveCount);
                MaxCount = reserveCount;
                return;
            }

            Buffer = Allocator->AllocateElement<T>(reserveCount);
        }

        // Returns the current index
        usize Add(const T& object)
        {
            if (Count == MaxCount) {
                MaxCount = Count * DYNAMIC_ARRAY_GROWTH_RATE;
                if (!IsHeapAllocated) {
                    Buffer = Allocator->AllocateElement<T>(MaxCount);
                    memmove(Buffer, StackBuffer.Buffer, sizeof(T) * StackBuffer.Count());
                    StackBuffer.Clear();
                    IsHeapAllocated = true;
                }
                else
                {
                    Buffer = Allocator->ReallocateElement<T>(Buffer, Count, MaxCount);
                }
            }

            if (IsHeapAllocated) {
                Buffer[Count] = object;
                ++Count;
                return Count - 1;
            }

            StackBuffer[Count] = object;
            ++Count;
            return Count - 1;
        }

        T& operator[] (usize index) 
        {
            assert(index <= (Count - 1));
            if (IsHeapAllocated) {
                return Buffer[index];
            }

            return StackBuffer[index];
        }

        // TODO: Find way to remove bool
        bool IsHeapAllocated;
        usize Count;
        usize MaxCount;
        T *Buffer;
        Array<T, DYNAMIC_ARRAY_MAX_STACK_ELEMENTS> StackBuffer;
        Memory::Allocator *Allocator;
    };
}
}
