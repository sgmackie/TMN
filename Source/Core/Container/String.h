#pragma once

#include "Types.h"
#include "DynamicArray.h"
#include "Memory/Allocator.h"

namespace Core {
namespace Container {
    class String
    {
    public:
        String(Memory::Allocator *allocator) : Buffer(allocator)
        {
        }

        String(Memory::Allocator *allocator, const char* stringLiteral) : Buffer(allocator)
        {
            memcpy(Buffer.Buffer, stringLiteral, sizeof(char) * strlen(stringLiteral));
        }

        char* ToCString()
        {
            return Buffer.Buffer;
        }

        char* ToCString() const
        {
            return Buffer.Buffer;
        }

        DynamicArray<char> Buffer;
    };
}
}

