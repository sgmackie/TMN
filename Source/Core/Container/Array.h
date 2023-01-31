#pragma once

#include "../CoreTypes.h"
#include "../Logging.h"

namespace Core {
namespace Container {
    template <typename T, const usize N>
    class Array
    {
    public:
        Array()
        {
            Clear();
        }

        Array(T *Buffer)
        {
            CORE_ASSERT(Buffer != nullptr);
            memcpy(Buffer, Buffer, (sizeof(T) * N));
        }

        Array(std::initializer_list<T> List)
        {
            CORE_ASSERT(List.size() == N);
            memcpy(Buffer, List.begin(), (sizeof(T) * List.size()));
        }

        usize Count() const
        {
            return N;
        }

        void Clear()
        {
            memset(Buffer, 0, (sizeof(T) * N));
        }

        T &operator[](const usize Index)
        {
            CORE_ASSERT(Index <= N);
            return Buffer[Index];
        }

        const T &operator[](const usize Index) const
        {
			CORE_ASSERT(Index <= N);
            return Buffer[Index];
        }

        T &operator=(std::initializer_list<T> List);

        T *begin()
        {
            return &Buffer[0];
        }

        T *end()
        {
            return &Buffer[N - 1];
        }

        const T *begin() const
        {
            return &Buffer[0];
        }

        const T *end() const
        {
            return &Buffer[N - 1];
        }

        T *ToCArray()
        {
            return Buffer;
        }

        T Buffer[N];
    };
}
}
