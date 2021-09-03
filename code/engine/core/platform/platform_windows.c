#include "../platform.h"

#pragma region Windows

#if defined(PLATFORM_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#include <wchar.h>
#include <windows.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")

#pragma region Utility Functions

// Uses the "MultiByteToWideChar" function to convert a utf8 char to utf16 wide char
#define WINDOWS_UTF8_TO_UTF16(__input, __output)                                \
    usize __bufferSize = MultiByteToWideChar(CP_UTF8, 0, __input, -1, NULL, 0); \
    MultiByteToWideChar(CP_UTF8, 0, __input, -1, __output, __bufferSize);

#define WINDOWS_UTF16_TO_UTF8(__input, __output)                                            \
    usize __bufferSize = WideCharToMultiByte(CP_UTF8, 0, __input, -1, NULL, 0, NULL, NULL); \
    WideCharToMultiByte(CP_UTF8, 0, __input, -1, __output, __bufferSize, NULL, NULL);

char *windowsConvertUTF16ToUTF8(wchar_t *input, Allocator *allocator) {
    usize bufferSize = WideCharToMultiByte(CP_UTF8, 0, input, -1, NULL, 0, NULL, NULL);
    char *output = allocator->Reallocate(allocator, 0, sizeof(char) * bufferSize + 1);
    WideCharToMultiByte(CP_UTF8, 0, input, -1, output, bufferSize, NULL, NULL);
    return output;
}

// Combine two u32s together by shifting the high part to the left:
// (0x0000000000000001 << 32) = 0x0000000100000000
u64 windowsU32Combine(u32 highPart, u32 lowPart) {
    return (u64)highPart << 32 | lowPart;
}

#pragma endregion

#pragma region Virtual Memory

void *windowsVirtualAllocate(const usize size) {
    void *block = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    return block;
}

void windowsVirtualFree(void *block, const usize size) {
    VirtualFree(block, size, MEM_RELEASE);
}

static struct VirtualMemoryAPI windowsVirtualMemory = {
    .Allocate = windowsVirtualAllocate,
    .Free = windowsVirtualFree
};

#pragma endregion

#pragma region File IO

File windowsFileOpenToRead(const char *path) {
    File result = { 0 };

    usize wideCharSize = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
    usize bufferSize = wideCharSize > MAX_PATH ? MAX_PATH : wideCharSize;
    // wchar_t wideString[bufferSize];
    // MultiByteToWideChar(CP_UTF8, 0, path, -1, wideString, bufferSize);

    // const HANDLE handle = CreateFileW(wideString, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    // result.handle = (u64)handle;
    // result.isValid = handle != INVALID_HANDLE_VALUE ? true : false;
    return result;
}

void windowsFileClose(File *file) {
    if (file->isValid)
        CloseHandle((HANDLE)file->handle);
}

static struct FileAPI windowsFileIO = {
    .OpenToRead = windowsFileOpenToRead,
    .Close = windowsFileClose
};

#pragma endregion

#pragma region File System

FileInfo windowsFileSystemGetInfo(const char *path) {
    FileInfo result = { 0 };

    wchar_t wideString[MAX_PATH];
    WINDOWS_UTF8_TO_UTF16(path, wideString);

    WIN32_FILE_ATTRIBUTE_DATA attributeData = { 0 };
    GetFileAttributesExW(wideString, GetFileExInfoStandard, &attributeData);
    if (attributeData.dwFileAttributes == 0)
        return result;

    result.flags |= FILE_IS_VALID;

    if (attributeData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        result.flags |= FILE_IS_DIRECTORY;
    if (attributeData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
        result.flags |= FILE_IS_HIDDEN;
    if (attributeData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
        result.flags |= FILE_IS_READONLY;

    result.size = windowsU32Combine(attributeData.nFileSizeHigh, attributeData.nFileSizeLow);
    return result;
}

FileInfo *windowsFindFiles(const char *path, const char *filter, Allocator *allocator) {
    FileInfo *test = (FileInfo *)allocator->Reallocate(allocator, 0, sizeof(FileInfo));
    return test;
}

FileInfo windowsGetWorkingDirectory(Allocator *allocator) {
    FileInfo result = { 0 };

    wchar_t wideString[MAX_PATH];
    if (!GetCurrentDirectoryW(MAX_PATH, wideString))
        return result;

    result.flags |= FILE_IS_VALID;
    result.flags |= FILE_IS_DIRECTORY;
    result.path = windowsConvertUTF16ToUTF8(wideString, allocator);

    return result;
}

FileInfo windowsGetProcessName(Allocator *allocator) {
    FileInfo result = { 0 };

    wchar_t wideString[MAX_PATH];
    if (!GetModuleFileNameW(0, wideString, MAX_PATH))
        return result;

    result.flags |= FILE_IS_VALID;
    result.flags |= FILE_IS_PROCESS;
    result.path = windowsConvertUTF16ToUTF8(wideString, allocator);

    return result;
}

bool windowsFileSystemRemoveFile(const char *path) {
    // return DeleteFileW(path);
    return false;
}

static struct FileSystemAPI windowsFileSystem = {
    .GetFileInfo = windowsFileSystemGetInfo,
    .FindFiles = windowsFindFiles,
    .RemoveFile = windowsFileSystemRemoveFile,
    .GetWorkingDirectory = windowsGetWorkingDirectory,
    .GetProcessName = windowsGetProcessName
};

#pragma endregion

#pragma region DLL Handling

File windowsDLLOpen(const char *path) {
    File result = { 0 };

    wchar_t wideString[MAX_PATH];
    WINDOWS_UTF8_TO_UTF16(path, wideString);
    HMODULE handle = LoadLibraryExW(wideString, 0, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
    result.handle = (u64)handle;
    result.isValid = handle != 0 ? true : false;

    return result;
}

void windowsDLLClose(File *dll) {
    if (!dll->isValid)
        return;

    FreeLibrary((HMODULE)dll->handle);
}

static struct DLLAPI windowsDLLHandling = {
    .Open = windowsDLLOpen,
    .Close = windowsDLLClose
};

#pragma endregion

#pragma region Global API

static struct PlatformAPI windowsPlatform = {
    .virtualMemory = &windowsVirtualMemory,
    .file = &windowsFileIO,
    .fileSystem = &windowsFileSystem,
    .dll = &windowsDLLHandling
};

platform = &windowsPlatform;

#pragma endregion

#endif