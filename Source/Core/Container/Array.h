#pragma once

#include "Types.h"
#include "Log.h"

namespace Core {
namespace Container {
    template<typename T, const usize N>
    class Array
    {
    public:
        Array()
        {
            Clear();
        }

        Array(T* buffer) 
        {
            assert(buffer != nullptr);
            memcpy(Buffer, buffer, (sizeof(T) * N));
        }

        Array(std::initializer_list<T> list) 
        {
            assert(list.size() == N);
            memcpy(Buffer, list.begin(), (sizeof(T) * list.size()));
        }

        usize Count() const 
        {                 
            return N;
        }

        void Clear()
        {
			memset(Buffer, 0, (sizeof(T) * N));
        }

        T& operator[] (const usize index) 
        {
            assert(index <= N);
            return Buffer[index];
        }

		const T& operator[] (const usize index) const
		{
			assert(index <= N);
			return Buffer[index];
		}

        T& operator= (std::initializer_list<T> list);

		T* begin()
		{
			return &Buffer[0];
		}

		T* end()
		{
			return &Buffer[N-1];
		}

		const T* begin() const
		{
			return &Buffer[0];
		}

		const T* end() const
		{
			return &Buffer[N-1];
		}

        T Buffer[N];
    };
}
}
