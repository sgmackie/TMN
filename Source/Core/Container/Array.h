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

        T& operator= (std::initializer_list<T> list);

        T* ToCArray()
        {
            return Buffer;
        }

		T* begin()
		{
			return Buffer;
		}

		T* end()
		{
			return Buffer + N;
		}

        T Buffer[N];
    };
}
}
