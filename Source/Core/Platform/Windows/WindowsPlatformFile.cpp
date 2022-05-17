#if defined(CORE_PLATFORM_WINDOWS)

#include "Platform.h"

namespace Core {
namespace Platform {
namespace FileIO {
	const char *GetPathSeperator()
	{
		return "\\";
	}

	bool Exists(Memory::Allocator *allocator, const char* path)
	{
		WIN32_FILE_ATTRIBUTE_DATA fileData;
		wchar_t* widePath = (wchar_t* ) Unicode::UTF8ToUTF16(allocator, path);
		if (!GetFileAttributesExW(widePath, GetFileExInfoStandard, &fileData)) {
			return false;
		}

		return true;
	}

	bool CreateToWrite(Memory::Allocator *allocator, const char* path, File* file)
	{
		// TODO: Use temp allocations
		wchar_t* widePath = (wchar_t* ) Unicode::UTF8ToUTF16(allocator, path);
		const HANDLE handle = CreateFileW(widePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
		if (handle == INVALID_HANDLE_VALUE) {
			printf("%s\n", Platform::GetLastSystemError(allocator));
			return false;
		}
		file->Handle = (u64) handle;
		return true;
	}

	bool OpenToWrite(Memory::Allocator *allocator, const char* path, File* file)
	{
		// TODO: Use temp allocations
		wchar_t* widePath = (wchar_t* ) Unicode::UTF8ToUTF16(allocator, path);
		const HANDLE handle = CreateFileW(widePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (handle == INVALID_HANDLE_VALUE) {
			printf("%s\n", Platform::GetLastSystemError(allocator));
			return false;
		}
		file->Handle = (u64) handle;
		return true;
	}

	bool Write(Memory::Allocator *allocator, File* file, u8* buffer, const usize sizeInBytes)
	{
		if (INVALID_HANDLE_VALUE == (HANDLE) file->Handle) {
			return false;
		}

		usize bytesToWrite = sizeInBytes;
		do {
			DWORD bytesWritten = 0;
			if (!WriteFile((HANDLE) file->Handle, buffer, bytesToWrite, &bytesWritten, 0)) {
				printf("%s\n", Platform::GetLastSystemError(allocator));
				return false;
			}

			bytesToWrite -= sizeInBytes;
			buffer += sizeInBytes;

			if (bytesToWrite != bytesWritten) {
				printf("%s\n", Platform::GetLastSystemError(allocator));
				return false;
			}

		} while (bytesToWrite > 0);

		return true;
	}

	bool Remove(Memory::Allocator *allocator, const char* path)
	{
		wchar_t* widePath = (wchar_t* ) Unicode::UTF8ToUTF16(allocator, path);
		if (!DeleteFileW(widePath)) {
			printf("%s\n", Platform::GetLastSystemError(allocator));
			return false;
		}

		return true;
	}

	void Close(File* file)
	{
		CloseHandle((HANDLE) file->Handle);
		file->Handle = (u64) INVALID_HANDLE_VALUE;
	}
}
}
}
#endif