#pragma once

#include "Allocator.h"

namespace Core {
namespace Platform {
	// General
    const wchar_t *CharToWideChar(Allocator *Allocator, const char *string);

	// Memory
	void *VirtualReserve(usize size);
	void VirtualCommit(void *pointer, usize size);
	void *VirtualMap(usize size);
	void VirtualUnmap(void *pointer, usize size);

    // File IO
    typedef struct File {
        u64 Handle;
    } File;

    enum class FileAccessMode {
        FILE_READ,
        FILE_WRITE
    };

    static File FileOpen(Allocator *allocator, const char *path, const FileAccessMode mode);
}
}