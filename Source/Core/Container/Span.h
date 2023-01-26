#pragma once

#include "../CoreTypes.h"
#include "../Logging.h"

namespace Core {
namespace Container {
    template <typename T>
    struct Span {
        Span()
        {
            Clear();
        }

        ~Span()
        {
            Clear();
        }

        void Clear()
        {
            Buffer = nullptr;
            Count = 0;
        }

        Span(T *StartPointer, const usize InCount)
        {
            Buffer = StartPointer;
            Count = InCount;
        }

        usize SizeInBytes() const
        {
            return sizeof(T) * Count;
        }

        T &operator[](const usize Index)
        {
            CORE_ASSERT(Buffer + Index < end());
            return Buffer[Index];
        }

        T *begin()
        {
            return Buffer[0];
        }

        T *end()
        {
            return Buffer + SizeInBytes();
        }

        T *Buffer = nullptr;
        usize Count = 0;
    };
}
}
