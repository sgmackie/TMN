#if defined(CORE_PLATFORM_WINDOWS)

#include "../../Allocator.h"
#include "../../Unicode.h"
#include "Platform.h"

namespace Core {
namespace Platform {
    namespace FileIO {
		bool DirectoryExists(Core::Allocator *Allocator, const char *DirectoryPath)
		{
			WIN32_FILE_ATTRIBUTE_DATA FileData;
			wchar_t *WidePath = reinterpret_cast<wchar_t *>(UTF8ToUTF16(Allocator, DirectoryPath));
			if (!GetFileAttributesExW(WidePath, GetFileExInfoStandard, &FileData)) {
				return false;
			}

			if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				return true;
			}

			return false;
		}

		bool DirectoryCreate(Core::Allocator *Allocator, const char *DirectoryPath)
		{
			wchar_t *WidePath = reinterpret_cast<wchar_t *>(UTF8ToUTF16(Allocator, DirectoryPath));
			return CreateDirectoryW(WidePath, 0);
		}

        const char *GetPathSeperator()
        {
            return "\\";
        }

        bool Exists(Core::Allocator *Allocator, const char *Path)
        {
            WIN32_FILE_ATTRIBUTE_DATA FileData;
            wchar_t *WidePath = reinterpret_cast<wchar_t *>(UTF8ToUTF16(Allocator, Path));
            if (!GetFileAttributesExW(WidePath, GetFileExInfoStandard, &FileData)) {
                return false;
            }

            return true;
        }

        bool CreateToWrite(Core::Allocator *Allocator, const char *Path, File *File)
        {
            wchar_t *WidePath = reinterpret_cast<wchar_t *>(UTF8ToUTF16(Allocator, Path));
            const HANDLE Handle = CreateFileW(WidePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
            if (Handle == INVALID_HANDLE_VALUE) {
				CORE_LOG(Allocator, "%s\n", Platform::GetLastSystemError(Allocator));
                return false;
            }
            File->Handle = reinterpret_cast<u64>(Handle);
            return true;
        }

        bool OpenToWrite(Core::Allocator *Allocator, const char *Path, File *File)
        {
            wchar_t *WidePath = reinterpret_cast<wchar_t *>(UTF8ToUTF16(Allocator, Path));
            const HANDLE Handle = CreateFileW(WidePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
            if (Handle == INVALID_HANDLE_VALUE) {
				CORE_LOG(Allocator, "%s\n", Platform::GetLastSystemError(Allocator));
                return false;
            }
            File->Handle = reinterpret_cast<u64>(Handle);
            return true;
        }

        bool Write(Core::Allocator *Allocator, File *File, u8 *Buffer, const usize SizeInBytes)
        {
            if (INVALID_HANDLE_VALUE == reinterpret_cast<HANDLE>(File->Handle)) {
                return false;
            }

            usize BytesToWrite = SizeInBytes;
            do {
                DWORD BytesWritten = 0;
                if (!WriteFile(reinterpret_cast<HANDLE>(File->Handle), Buffer, BytesToWrite, &BytesWritten, 0)) {
					CORE_LOG(Allocator, "%s\n", Platform::GetLastSystemError(Allocator));
                    return false;
                }

				BytesToWrite -= BytesWritten;
				Buffer += BytesWritten;
            } while (BytesToWrite > 0);

            return true;
        }

        bool Remove(Core::Allocator *Allocator, const char *Path)
        {
            wchar_t *WidePath = reinterpret_cast<wchar_t *>(UTF8ToUTF16(Allocator, Path));
            if (!DeleteFileW(WidePath)) {
				CORE_LOG(Allocator, "%s\n", Platform::GetLastSystemError(Allocator));
                return false;
            }

            return true;
        }

        void Close(File *File)
        {
            CloseHandle(reinterpret_cast<HANDLE>(File->Handle));
            File->Handle = reinterpret_cast<u64> INVALID_HANDLE_VALUE;
        }
    }
}
}
#endif