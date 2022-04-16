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
            memset(Buffer, 0, sizeof(Buffer));
        }

        usize Count() const 
        {                 
            return N;
        }

        T& operator[] (usize index) 
        {
            assert(index <= N);
            return Buffer[index];
        }

        T Buffer[N];
    };
}
}
