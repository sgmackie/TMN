#pragma once

#include "../Allocator.h"
#include "../CoreTypes.h"
#include "../Logging.h"
#include "Array.h"

namespace Core {
namespace Container {
    template <typename T>
    class DynamicArray
    {
    public:
#define DYNAMIC_ARRAY_MAX_STACK_ELEMENTS 8
#define DYNAMIC_ARRAY_GROWTH_RATE 1.2

        DynamicArray(Core::Allocator *InAllocator, const usize InReserveCount = 0)
        {
            Count = 0;
            Capacity = DYNAMIC_ARRAY_MAX_STACK_ELEMENTS;
            Buffer = StackBuffer;
            Allocator = InAllocator;

#if defined(CORE_MEMORY_ZERO_INITIALISE)
            memset(StackBuffer, 0, sizeof(T) * Capacity);
#endif
            if (InReserveCount > 0) {
                Reserve(InReserveCount);
            }
        }

		DynamicArray(const DynamicArray& Copy)
		{
			Allocator = Copy.Allocator;
			Buffer = StackBuffer;
			Set(Copy.Buffer, Copy.Count);
		}

        ~DynamicArray()
        {
            if (IsDynamicallyAllocated()) {
                Allocator->Free(Buffer, sizeof(T) * Capacity);
                Buffer = nullptr;
            }

            Count = 0;
            Capacity = 0;
            Allocator = nullptr;
        }

        void Reserve(const usize ReserveCount)
        {
            if (ReserveCount < Capacity) {
                return;
            }

            T *TempBuffer = reinterpret_cast<T *>(Allocator->Allocate(sizeof(T) * ReserveCount, Allocator->DefaultAlignment));
            memcpy(TempBuffer, Buffer, sizeof(T) * Count);
            if (!IsDynamicallyAllocated()) {
                Buffer = TempBuffer;
            } else {
                Allocator->Free(Buffer, (sizeof(T) * Capacity));
                Buffer = TempBuffer;
            }

            Capacity = ReserveCount;
        }

        // Returns the current Index
        usize Add(const T &Object)
        {
            if (Count == Capacity) {
                Reserve(Capacity * DYNAMIC_ARRAY_GROWTH_RATE);
            }

            Buffer[Count] = Object;
            ++Count;
            return Count - 1;
        }

        void Append(const T *InBuffer, const usize Size)
        {
            Reserve((Capacity + Size) * DYNAMIC_ARRAY_GROWTH_RATE);
            for (usize i = 0; i < Size; ++i) {
                Buffer[Count + i] = InBuffer[i];
            }
            Count += Size;
        }

        void RemoveAt(const usize Index)
        {
            CORE_ASSERT(Index >= 0 && Index <= Count);
            Buffer[Index].~T();
            Buffer[Index] = 0;
            --Count;
        }

        void Set(const T *InBuffer, const usize Size)
        {
            if (Size > Capacity) {
                Reserve(Size);
            }

            for (usize i = 0; i < Size; ++i) {
                Buffer[i] = InBuffer[i];
            }

            Count = Size;
        }

        void Clear()
        {
            Count = 0;

            for (usize i = 0; i < Count; ++i) {
                Buffer[i].~T();
                Buffer[i] = 0;
            }
        }

        T &operator[](usize Index)
        {
            CORE_ASSERT(Index <= (Count - 1));
            return Buffer[Index];
        }

        const T &operator[](const usize Index) const
        {
            CORE_ASSERT(Index <= (Count - 1));
            return Buffer[Index];
        }

        T *begin()
        {
            return &Buffer[0];
        }

        T *end()
        {
            return &Buffer[Count];
        }

        const T *begin() const
        {
            return &Buffer[0];
        }

        const T *end() const
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

        usize Count = 0;
        usize Capacity = 0;
        T StackBuffer[DYNAMIC_ARRAY_MAX_STACK_ELEMENTS];
        T *Buffer = nullptr;
		Core::Allocator *Allocator = nullptr;
    };
}
}