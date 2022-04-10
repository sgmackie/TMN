#include "Platform.h"
#include "Unicode.h"

#if defined(CORE_PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <wchar.h>
#include <windows.h>

namespace Core {

	// Memory
	void *VirtualReserve(usize size)
	{
		return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
	}

	void VirtualCommit(void *pointer, usize size)
	{
		VirtualAlloc(pointer, size, MEM_COMMIT, PAGE_READWRITE);
	}

	void *VirtualMap(usize size)
	{
		return VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
	}

	void VirtualUnmap(void *pointer, usize size)
	{
		VirtualFree(pointer, 0, MEM_RELEASE);
	}

	// File IO	
	const wchar_t *Platform::CharToWideChar(Allocator *allocator, const char *string)
	{
		const u16 *utf16 = Unicode::UTF8ConvertToUTF16(allocator, string);
		return (const wchar_t *) utf16;
	}

	Platform::File Platform::FileOpen(Allocator *allocator, const char *path, const FileAccessMode mode)
	{
		File result = {};

		const wchar_t *widePath = CharToWideChar(allocator, path);
		result.Handle = 0;

		// EASTL and passing allocators around in template parameters
		BufferDynamic<char, 12> test;

		// Unicode::UTF16ConvertToUTF8();

		return result;
	}

}

#endif