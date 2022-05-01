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

        String(Memory::Allocator *allocator, const String* string) : Buffer(allocator)
        {
            Append(allocator, string->ToUTF8());
        }

        void Append(Memory::Allocator *allocator, const char* stringToAppend)
        {
            usize appendLength = strlen(stringToAppend);
            usize totalStringLength = Buffer.Count + appendLength + 1;

            // TODO: Provide temporary allocator interface to free after exiting the function scope
            char* tempBuffer = allocator->AllocateElement<char>(totalStringLength);
            memcpy(tempBuffer, Buffer.Buffer, Buffer.Count);
            memcpy(tempBuffer + Buffer.Count, stringToAppend, appendLength);
            tempBuffer[totalStringLength - 1] = 0;
            Buffer.Set(tempBuffer, totalStringLength);
            allocator->Free(tempBuffer, totalStringLength);
        }

        void Append(Memory::Allocator *allocator, const String* stringToAppend)
        {
            Append(allocator, stringToAppend->ToUTF8());
        }

        char* ToUTF8() const
        {
            if (Platform::Unicode::UTF8IsValid(Buffer.Buffer)) {
                return Buffer.Buffer;
            }

            return nullptr;
        }

        static String JoinAsPath(Memory::Allocator *allocator, const String *a, const String *b)
        {
            String result(allocator, a);
            result.Append(allocator, "/");
            result.Append(allocator, b);
            return result;
        }

        Container::DynamicArray<char> Buffer;
    };
}
}
