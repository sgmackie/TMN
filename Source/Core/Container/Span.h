#pragma once

#include "Types.h"
#include "Log.h"

namespace Core {
namespace Container {
    template <typename T>
    struct Span
    {
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
            Start = nullptr;
            End = nullptr;
        }

	    Span(T* startPointer, const usize count)
        {
            Start = startPointer;
            End = startPointer + count;
        }

        usize Count() const
        {
            return (usize) (End - Start);
        }

        T& operator[] (const usize index) 
        {
            assert(Start + index < End);
            return Start[index];
        }

        T* begin()
        {
            return Start;
        }

        T* end()
        {
            return End;
        }

		// TODO: Use a single pointer with an offset instead
		// TODO: Support indexing offsets
	    T* Start;
	    T* End;
    };
}
}

