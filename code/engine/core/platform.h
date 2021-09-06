#pragma once

#include "allocators.h"
#include "types.h"

// Platform layer
#pragma region Virtual Memory

typedef struct VirtualMemoryAPI {
    void *(*Allocate)(const usize size);
    void (*Free)(void *block, const usize size);
} VirtualMemoryAPI;

#pragma endregion

#pragma region File IO

typedef struct File {
    u64 handle;
    bool isValid; //TODO: Remove this and use the FileInfoFlags instead?
} File;

typedef struct FileAPI {
    File (*OpenToRead)(const char *path);
    File (*OpenToWrite)(const char *path);
    void (*Close)(File *file);
} FileAPI;

#pragma endregion

#pragma region File System

typedef enum FileInfoFlags {
    FILE_IS_VALID = 10,
    FILE_IS_DIRECTORY = 20,
    FILE_IS_HIDDEN = 30,
    FILE_IS_READONLY = 40,
    FILE_IS_PROCESS = 50
} FileInfoFlags;

typedef struct FileInfo {
    u8 flags;
    u64 size;
    const char *path;
} FileInfo;

typedef struct FileSystemAPI {
    FileInfo (*GetFileInfo)(const char *path);
    FileInfo *(*FindFiles)(const char *path, const char *filter, Allocator *allocator);
    FileInfo (*GetWorkingDirectory)(Allocator *allocator);
    FileInfo (*GetProcessName)(Allocator *allocator);
    bool (*RemoveFile)(const char *path);
} FileSystemAPI;

#pragma endregion

#pragma region DLL Handling

typedef struct DLLAPI {
    File (*Open)(const char *path);
    void (*Close)(File *dll);
} DLLAPI;

#pragma endregion

#pragma region General System

typedef struct SystemAPI {
    void (*Log)(const char *text);
    void (*DebugBreak)();
} SystemAPI;

#pragma endregion

#pragma region Global API

typedef struct PlatformAPI {
    VirtualMemoryAPI *virtualMemory;
    FileAPI *file;
    FileSystemAPI *fileSystem;
    DLLAPI *dll;
    SystemAPI *system;
} PlatformAPI;

static struct PlatformAPI *platform;

#pragma endregion
