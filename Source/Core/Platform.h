#pragma once

#include "Allocator.h"
#include "CoreTypes.h"
#include "CoreMath.h"

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
            usize MinimumAllocationSize;
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

	namespace Time {
		struct SystemTime {
			u64 Year;
			u64 Month;
			u64 Day;
			u64 Hour;
			u64 Minute;
			u64 Second;
			u64 Milliseconds;
		};
		
		u64 GetCPUTime();
		SystemTime GetSystemTime();
	}

    namespace FileIO {
        struct File {
            u64 Handle;
        };

        // File system
		bool DirectoryExists(Core::Allocator *Allocator, const char *DirectoryPath);
		bool DirectoryCreate(Core::Allocator *Allocator, const char *DirectoryPath);
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
		typedef struct Window Window;
		typedef struct InputEvent InputEvent;

		struct CommandLineParameters
		{
			usize Size;
			i32 Count;
		};

		void PrintToConsole(Core::Allocator *Allocator, const char *Buffer);
		CommandLineParameters GetCommandLineParameters();
		void GetCommandLineArguments(Core::Allocator *Allocator, CommandLineParameters Parameters, char** ArgBuffer);
		Window* WindowCreate(Core::Allocator *Allocator, Rect* Rect, const char* Title);
		void WindowDestroy(Window* Window);
		void WindowUpdate(Window* Window);

		InputEvent* GetNextInputEvent();
		void ClearInputEvent(InputEvent* Event);
    }
}
}

#if defined(CORE_PLATFORM_WINDOWS)
#include "Platform/Windows/PlatformWindows.h"
#endif

#if defined(CORE_PLATFORM_MAC)
#include "Platform/Mac/PlatformMac.h"
#endif