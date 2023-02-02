#pragma once

#include "../Allocator.h"
#include "../CoreTypes.h"
#include "../Platform.h"
#include "../Unicode.h"
#include "DynamicArray.h"

#include "../../3rdParty/stb/stb_sprintf.h"

namespace Core {
namespace Container {
    class String
    {
    public:
#define STRING_FORMAT_BUFFER 512

        String(Core::Allocator *InAllocator, const usize InReserveSize = 0)
            : Buffer(InAllocator, InReserveSize)
        {
            Buffer.Add('\0');
        }

        String(Core::Allocator *InAllocator, const char *InStringLiteral, const usize InLength)
            : Buffer(InAllocator, InLength)
        {
            Buffer.Set(InStringLiteral, InLength);
            Buffer.Add('\0');
        }

        String(Core::Allocator *InAllocator, const char *InStringLiteral)
            : Buffer(InAllocator)
        {
            Buffer.Set(InStringLiteral, strlen(InStringLiteral));
            Buffer.Add('\0');
        }

        ~String()
        {
        }

        // TODO: Why doesn't this work?
        char &operator[](usize Index)
        {
            CORE_ASSERT(Index <= (Length() - 1));
            return Buffer[Index];
        }

        const char &operator[](const usize Index) const
        {
            CORE_ASSERT(Index <= (Length() - 1));
            return Buffer[Index];
        }

        char &GetCharacterAtIndex(usize Index)
        {
            CORE_ASSERT(Index <= (Length() - 1));
            return Buffer[Index];
        }

        void Append(const char character)
        {
            Append(&character, 1);
        }

        void Append(const char *StringLiteral)
        {
            Append(StringLiteral, strlen(StringLiteral));
        }

        void Append(const char *StringLiteral, const usize Length)
        {
            if (IsNullTerminated() && Buffer.Count > 0) {
                Buffer.RemoveAt(Buffer.Count - 1);
            }

            Buffer.Append(StringLiteral, Length);
            Buffer.Add('\0');
        }

        void AppendAsPath(const char *StringLiteral)
        {
            Append(Platform::FileIO::GetPathSeperator());
            Append(StringLiteral);
        }

        void AppendAsPath(const String *InBuffer)
        {
            AppendAsPath(InBuffer->ToUTF8());
        }

        void AppendFormat(const char *Format, ...)
        {
            va_list VArgs;
            va_start(VArgs, Format);
            char TempBuffer[STRING_FORMAT_BUFFER]; // TODO: Handle cases bigger than 512
            usize CharactersWritten = stbsp_vsnprintf(TempBuffer, STRING_FORMAT_BUFFER, Format, VArgs);
            va_end(VArgs);

            if ((Length() + CharactersWritten) > Buffer.Capacity) {
                Buffer.Reserve(Buffer.Capacity + CharactersWritten);
            }

            if (IsNullTerminated() && Length() > 0) {
                Buffer.RemoveAt(Buffer.Count - 1);
            }

            memmove(Buffer.Buffer + Length(), TempBuffer, sizeof(char) * CharactersWritten);
            Buffer.Count += CharactersWritten;
            Buffer.Add('\0');
        }

        void Trim(const usize Count)
        {
            if (IsNullTerminated() && Length() > 0) {
                Buffer.RemoveAt(Buffer.Count - 1);
            }

            const usize originalSize = Buffer.Count;
            for (usize i = 0; i < Count; ++i) {
                Buffer.RemoveAt(originalSize - i);
            }

            Buffer.Add('\0');
        }

        char *ToUTF8() const
        {
            if (!UTF8IsValid(Buffer.Buffer)) {
				 return nullptr;
            }

			return Buffer.Buffer;
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

        static u64 ToU64(char *String, const u8 Base = 10)
        {
			return strtoull(String, 0, Base);
        }

		static f32 ToF32(char *String)
		{
			char **EndPoint = nullptr;
			return strtof(String, EndPoint);
		}

		static String FromSystemTime(Core::Allocator *InAllocator, const Platform::Time::SystemTime& Time, const bool bWithUnderScores)
		{
			char TempBuffer[STRING_FORMAT_BUFFER];
			if (bWithUnderScores)
			{
				stbsp_sprintf(TempBuffer, "%lu_%lu_%lu_%lu_%lu_%lu_%lu", Time.Day, Time.Month, Time.Year, Time.Hour, Time.Minute, Time.Second, Time.Milliseconds);
			}
			else
			{
				stbsp_sprintf(TempBuffer, "%lu/%lu/%lu %lu:%lu:%lu:%lu", Time.Day, Time.Month, Time.Year, Time.Hour, Time.Minute, Time.Second, Time.Milliseconds);
			}
			
			String Result(InAllocator, TempBuffer, strlen(TempBuffer));
			return Result;
		}

        Container::DynamicArray<char> Buffer;
    };
}
}