#pragma once

#include "Types.h"
#include "Memory/Allocator.h"

namespace Core {
namespace Platform {
    struct MemoryStats
    {
        // Everything is in bytes
	    u64 TotalPhysical;
        u64 TotalVirtual;
        usize PageSize;
    };

	enum class VirtualMemoryFlags
	{
		ReadWrite,
		Protect
	};

    MemoryStats GetMemoryStats();

	void *VirtualMemoryReserve(const usize size, const VirtualMemoryFlags flags);
	void VirtualMemoryCommit(void *pointer, const usize size, const VirtualMemoryFlags flags);
    void VirtualMemoryDecommit(void *pointer, const usize size);
    void VirtualMemoryProtect(void *pointer, const usize size);

	struct File
	{
		u64 Handle;
	};

	const char* GetLastSystemError(Memory::Allocator *allocator);

	namespace Unicode {
		bool UTF8IsValid(const char* utf8String);
		char *UTF8Encode(char *utf8String, u32 codepoint);
		uint32_t UTF8Decode(const char **utf8String);
		u16 *UTF16Encode(u16 *utf16String, u32 codepoint);
		u32 UTF16Decode(const u16 **utf16String);
		usize GetUTF8CodpointLength(u32 codepoint);
		usize GetUTF16LengthFromUTF8(const char* utf8String);
		usize GetUTF8LengthFromUTF16(const u16* utf16String);
		u16* UTF8ToUTF16(Memory::Allocator *allocator, const char* utf8String);
		char* UTF16ToUTF8(Memory::Allocator *allocator, const u16* utf16String);
	}

	namespace FileIO {
		// File system
		bool DirectoryExists(const char* directoryPath);
		const char* GetPathSeperator();

		// File IO
		bool Exists(Memory::Allocator *allocator, const char* path);
		bool CreateToWrite(Memory::Allocator *allocator, const char* path, File* file);
		bool OpenToWrite(Memory::Allocator *allocator, const char* path, File* file);
		bool Write(Memory::Allocator *allocator, File* file, u8* buffer, const usize sizeInBytes);
		bool Remove(Memory::Allocator *allocator, const char* path);
		void Close(File* file);
	}
}
}

#if defined(CORE_PLATFORM_WINDOWS)
#include "Platform/Windows/WindowsPlatform.h"
#endif


#if defined(CORE_PLATFORM_MACOS)
#include "Platform/MacOS/MacOSPlatform.h"
#endif