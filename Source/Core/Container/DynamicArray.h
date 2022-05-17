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
        #define DYNAMIC_ARRAY_MAX_STACK_ELEMENTS 8
        #define DYNAMIC_ARRAY_GROWTH_RATE 1.2

        DynamicArray(Memory::Allocator *allocator, const usize reserveCount = 0)
        {
            Count = 0;
            Capacity = DYNAMIC_ARRAY_MAX_STACK_ELEMENTS;
            Buffer = StackBuffer;
            Allocator = allocator;

#if defined(CORE_MEMORY_ZERO_INITIALISE)
			memset(StackBuffer, 0, sizeof(T) * Capacity);
#endif
            if (reserveCount > 0) {
				Reserve(reserveCount);
            }
        }

        ~DynamicArray()
        {
			if (IsDynamicallyAllocated()) {
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

			T* TempBuffer = reinterpret_cast<T*>(Allocator->Allocate(sizeof(T) * reserveCount));
			memcpy(TempBuffer, Buffer, sizeof(T) * Count);
			if (!IsDynamicallyAllocated()) {
				Buffer = TempBuffer;
            }
			else {
				Allocator->Free(Buffer);
				Buffer = TempBuffer;
			}

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

		void Append(const T* buffer, const usize size)
		{
			Reserve((Capacity + size) * DYNAMIC_ARRAY_GROWTH_RATE);
			for (usize i = 0; i < size; ++i) {
				Buffer[Count + i] = buffer[i];
			}
			Count += size;
		}

		void RemoveAt(const usize index)
		{
			assert(index >= 0 && index <= Count);
            Buffer[index].~T();
            Buffer[index] = 0;
            --Count;
		}

        void Set(const T* buffer, const usize size)
        {
            if (size > Capacity) {
                Reserve(size);
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

		const T& operator[] (const usize index) const
		{
			assert(index <= (Count - 1));
			return Buffer[index];
		}

		T* begin()
		{
			return &Buffer[0];
		}

		T* end()
		{
			return &Buffer[Count];
		}

		const T* begin() const
		{
			return &Buffer[0];
		}

		const T* end() const
		{
			return &Buffer[Count];
		}

		bool IsDynamicallyAllocated() const
		{
			return Buffer != StackBuffer;
		}

		usize SizeInBytes() const
		{
			return sizeof(T) * Count;
		}

        usize Count;
        usize Capacity;
		T StackBuffer[DYNAMIC_ARRAY_MAX_STACK_ELEMENTS];
		T* Buffer;
        Memory::Allocator *Allocator;
    };
}
}