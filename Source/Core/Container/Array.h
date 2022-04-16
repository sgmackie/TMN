#pragma once

#include "Types.h"
#include "Log.h"

namespace Core {
namespace Container {
    template<typename T, usize N> 
    class Array
    {
    public:
        Array() 
        {
            Clear();
        }

        usize Count() const 
        {                 
            return N;
        }

        void Clear()
        {
            memset(Buffer, 0, sizeof(Buffer));
        }

        T& operator[] (const usize index) 
        {
            assert(index <= N);
            return Buffer[index];
        }

        T Buffer[N];
    };
}
}
