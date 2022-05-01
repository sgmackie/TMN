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
        #define DYNAMIC_ARRAY_MAX_STACK_ELEMENTS 16 // TODO: Double check this size
        #define DYNAMIC_ARRAY_GROWTH_RATE 1.5

        DynamicArray(Memory::Allocator *allocator, const usize reserveCount = 0)
        {
            Count = 0;
            Capacity = DYNAMIC_ARRAY_MAX_STACK_ELEMENTS;
            Buffer = StackBuffer.Buffer;
            Allocator = allocator;

            if (reserveCount > 0) {
                Reserve(reserveCount);
            }
        }

        ~DynamicArray()
        {
            Clear();
            if (Buffer != StackBuffer.Buffer) {
                Allocator->Free(Buffer);
                Buffer = nullptr;
            }

            Count = 0;
            Capacity = 0;
            Allocator = nullptr;
        }

        void Reserve(const usize reserveCount)
        {
            if (reserveCount < Capacity) {
                return;
            }

            if (Buffer == StackBuffer.Buffer && Count < DYNAMIC_ARRAY_MAX_STACK_ELEMENTS) {
                return;
            }
            
            T* TempBuffer = Allocator->AllocateElement<T>(reserveCount);
			for (usize i = 0; i < Count; ++i)
			{
				TempBuffer[i] = Buffer[i];
                Buffer[i].~T();
            }

            // Don't free if previously using the stack buffer
            if (Buffer != StackBuffer.Buffer) {
                StackBuffer.Clear();
                Allocator->Free(Buffer);
            }

            Buffer = TempBuffer;
            Capacity = reserveCount;
        }

        // Returns the current index
        usize Add(const T& object)
        {
            if (Count == Capacity) {
                Reserve(Capacity * DYNAMIC_ARRAY_GROWTH_RATE);
            }

            Buffer[Count] = object;
            ++Count;
            return Count - 1;
        }

		void RemoveAt(const usize index)
		{
			assert(index >= 0 && index < Count);
            Buffer[index].~T();
            Buffer[index] = 0;
            --Count;
		}

        void Set(const T* buffer, const usize size)
        {
            if (size > Capacity) {
                Reserve(size * DYNAMIC_ARRAY_GROWTH_RATE);
            }

			for (usize i = 0; i < size; ++i)
			{
				Buffer[i] = buffer[i];
            }
            
            Count = size;
        }

		void Clear()
		{
			Count = 0;

            if (Buffer == StackBuffer.Buffer) {
                StackBuffer.Clear();
                return;
            }

			for (usize i = 0; i < Count; ++i) {
                Buffer[i].~T();
                Buffer[i] = 0;
            }
		}

        T& operator[] (usize index) 
        {
            assert(index <= (Count - 1));
            return Buffer[index];
        }

        usize Count;
        usize Capacity;
        T *Buffer;
        Array<T, DYNAMIC_ARRAY_MAX_STACK_ELEMENTS> StackBuffer;
        Memory::Allocator *Allocator;
    };
}
}
