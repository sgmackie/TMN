#pragma once

#include "Allocator.h"
#include "CoreTypes.h"

namespace Core {
namespace Platform {
    // Misc
    const char *GetLastSystemError(Core::Allocator *Allocator);

    namespace VirtualMemory {
        struct MemoryStats {
            // Everything is in bytes
            u64 TotalPhysical;
            u64 TotalVirtual;
            usize PageSize;
        };

        enum class Flags {
            ReadWrite,
            Protect
        };

        MemoryStats GetMemoryStats();

        void *Reserve(const usize Size, const Flags Flags);
        void Commit(void *Pointer, const usize Size, const Flags Flags);
        void Decommit(void *Pointer, const usize Size);
        void Protect(void *Pointer, const usize Size);
    }

    namespace FileIO {
        struct File {
            u64 Handle;
        };

        // File system
        bool DirectoryExists(const char *DirectoryPath);
        const char *GetPathSeperator();

        // File IO
        bool Exists(Core::Allocator *Allocator, const char *Path);
        bool CreateToWrite(Core::Allocator *Allocator, const char *Path, File *File);
        bool OpenToWrite(Core::Allocator *Allocator, const char *Path, File *File);
        bool Write(Core::Allocator *Allocator, File *File, u8 *Buffer, const usize SizeInBytes);
        bool Remove(Core::Allocator *Allocator, const char *Path);
        void Close(File *File);
    }

    namespace Process {
        void PrintToConsole(Core::Allocator *Allocator, const char *Buffer);
    }
}
}

#if defined(CORE_PLATFORM_WINDOWS)
#include "Platform/Windows/PlatformWindows.h"
#endif

#if defined(CORE_PLATFORM_MAC)
#include "Platform/Mac/PlatformMac.h"
#endif