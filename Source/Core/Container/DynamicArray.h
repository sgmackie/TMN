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

        DynamicArray(Memory::Allocator *allocator)
        {
            Count = 0;
            MaxCount = DYNAMIC_ARRAY_MAX_STACK_ELEMENTS;
            Buffer = nullptr;
            Allocator = allocator;
        }

        ~DynamicArray()
        {
            if (Buffer != nullptr) {
                Allocator->Free(Buffer);
                Buffer = nullptr;
            }

            Count = 0;
            MaxCount = 0;
            Allocator = nullptr;
        }

        // Returns the current index
        usize Add(const T& object)
        {
            if (Count == MaxCount) {
                MaxCount = Count * DYNAMIC_ARRAY_GROWTH_RATE;
                if (Buffer == nullptr) {
                    Buffer = Allocator->AllocateElement<T>(MaxCount);
                    memmove(Buffer, StackBuffer.Buffer, sizeof(T) * StackBuffer.Count());
                    StackBuffer.Clear();
                }
                else
                {
                    Buffer = Allocator->ReallocateElement<T>(Buffer, Count, MaxCount);
                }
            }

            if (Buffer) {
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
            if (Buffer) {
                return Buffer[index];
            }

            return StackBuffer[index];
        }

        usize Count;
        usize MaxCount;
        Array<T, DYNAMIC_ARRAY_MAX_STACK_ELEMENTS> StackBuffer;
        T *Buffer;
        Memory::Allocator *Allocator;
    };
}
}
