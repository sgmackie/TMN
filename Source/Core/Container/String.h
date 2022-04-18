#pragma once

#include "Types.h"
#include "DynamicArray.h"
#include "Memory/Allocator.h"
#include "Platform/PlatformUnicode.h"

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
            Append(allocator, stringLiteral);
        }

        void Append(Memory::Allocator *allocator, const char* stringToAppend)
        {
            usize appendLength = strlen(stringToAppend);
            usize totalStringLength = Buffer.Count + appendLength;

            // TODO: Provide temporary allocator interface to free after exiting the function scope
            char* tempBuffer = allocator->AllocateElement<char>(totalStringLength + 1);
            memcpy(tempBuffer, Buffer.Buffer, sizeof(char) * Buffer.Count);
            memcpy(tempBuffer + sizeof(char) * Buffer.Count, stringToAppend, sizeof(char) * appendLength);
            tempBuffer[totalStringLength] = 0;
            Buffer.Set(tempBuffer, totalStringLength + 1);
            allocator->Free(tempBuffer, totalStringLength + 1);
        }

        char* ToUTF8() const
        {
            if (Platform::Unicode::UTF8IsValid(Buffer.Buffer)) {
                return Buffer.Buffer;
            }

            return nullptr;
        }

        Container::DynamicArray<char> Buffer;
    };
}
}
