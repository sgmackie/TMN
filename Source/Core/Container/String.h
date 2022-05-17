#pragma once

#include "Types.h"
#include "DynamicArray.h"
#include "Memory/Allocator.h"
#include "Platform.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

namespace Core {
namespace Container {
    class String
    {
    public:
		#define STRING_FORMAT_BUFFER 512

		String(Memory::Allocator *allocator, const usize reserveSize = 0) : Buffer(allocator, reserveSize)
        {
			Buffer.Add('\0');
        }

		String(Memory::Allocator *allocator, const char* stringLiteral, const usize length) : Buffer(allocator, length)
		{
			Buffer.Set(stringLiteral, length);
			Buffer.Add('\0');
		}

		String(Memory::Allocator *allocator, const char* stringLiteral) : Buffer(allocator)
		{
			Buffer.Set(stringLiteral, strlen(stringLiteral));
			Buffer.Add('\0');
		}

		~String()
		{
		}

		void Append(const char character)
		{
			Append(&character, 1);
		}

		void Append(const char* stringLiteral)
		{
			Append(stringLiteral, strlen(stringLiteral));
		}

		void Append(const char* stringLiteral, const usize length)
		{
			if (IsNullTerminated() && Length() > 0) {
				Buffer.RemoveAt(Buffer.Count - 1);
			}

			Buffer.Append(stringLiteral, length);
			Buffer.Add('\0');
		}

		void AppendAsPath(const char* stringLiteral)
		{
			Append(Platform::FileIO::GetPathSeperator());
			Append(stringLiteral);
		}
	
		void AppendAsPath(const String* input)
		{
			AppendAsPath(input->ToUTF8());
		}

		void AppendFormat(const char *format, ...)
		{
			va_list args;
			va_start(args, format);
			char tempBuffer[STRING_FORMAT_BUFFER]; // TODO: Handle cases bigger than 512
			usize charactersWritten = stbsp_vsnprintf(tempBuffer, STRING_FORMAT_BUFFER, format, args);
			va_end(args);
			
			if ((Length() + charactersWritten) > Buffer.Capacity) {
				Buffer.Reserve(Buffer.Capacity + charactersWritten);
			}

			if (IsNullTerminated() && Length() > 0) {
				Buffer.RemoveAt(Buffer.Count - 1);
			}

			memmove(Buffer.Buffer + Length(), tempBuffer, sizeof(char) * charactersWritten);
			Buffer.Count += charactersWritten;
			Buffer.Add('\0');
		}

		void Trim(const usize count)
		{
			if (IsNullTerminated() && Length() > 0) {
				Buffer.RemoveAt(Buffer.Count - 1);
			}

			const usize originalSize = Buffer.Count;
			for (usize i = 0; i < count; ++i) {
				Buffer.RemoveAt(originalSize - i);
			}

			Buffer.Add('\0');
		}

        char* ToUTF8() const
        {
            if (Platform::Unicode::UTF8IsValid(Buffer.Buffer)) {
                return Buffer.Buffer;
            }

            return nullptr;
        }

		void Clear()
		{
			Buffer.Clear();
		}

		bool IsNullTerminated() const
		{
			return Buffer.Buffer[Length()] == 0 ? true : false;
		}

		usize Length() const
		{
			return Buffer.Count;
		}

		usize SizeInBytes() const
		{
			return Buffer.SizeInBytes();
		}

		static u64 ToU64(char *string, const u8 base = 10)
		{
			return strtoull(string, 0, base);
		}

        Container::DynamicArray<char> Buffer;
    };
}
}