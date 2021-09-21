#include "../math.h"
#include "../platform.h"

#pragma region Windows

#if defined(CORE_PLATFORM_WINDOWS)

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
    char *output = allocator->Reallocate(allocator, 0, 0, sizeof(char) * bufferSize + 1);
    WideCharToMultiByte(CP_UTF8, 0, input, -1, output, bufferSize, NULL, NULL);
    return output;
}

#pragma endregion

#pragma region Virtual Memory

static void *windowsVirtualAllocate(const usize size) {
    void *block = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    return block;
}

static void windowsVirtualFree(void *block, const usize size) {
    // VirtualFree(block, size, MEM_RELEASE);
}

static struct VirtualMemoryAPI windowsVirtualMemory = {
    .Allocate = windowsVirtualAllocate,
    .Free = windowsVirtualFree
};

#pragma endregion

#pragma region Time

static u64 windowsTimeGetCPU() {
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
}

static SystemTime windowsTimeGetSystem() {
    SystemTime result = { 0 };
    SYSTEMTIME counter;
    GetSystemTime(&counter);

    result.hours = counter.wHour;
    result.minutes = counter.wMinute;
    result.seconds = counter.wSecond;
    result.milliseconds = counter.wMilliseconds;

    return result;
}

static struct TimeAPI windowsTime = {
    .GetCPU = windowsTimeGetCPU,
    .GetSystem = windowsTimeGetSystem
};

#pragma endregion

#pragma region File IO

static File windowsFileOpenToRead(const char *path) {
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

static void windowsFileClose(File *file) {
    if (file->isValid)
        CloseHandle((HANDLE)file->handle);
}

static struct FileAPI windowsFileIO = {
    .OpenToRead = windowsFileOpenToRead,
    .Close = windowsFileClose
};

#pragma endregion

#pragma region File System

static FileInfo windowsFileSystemGetInfo(const char *path) {
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

    result.size = U32CombineHighLow(attributeData.nFileSizeHigh, attributeData.nFileSizeLow);
    return result;
}

static FileInfo *windowsFindFiles(const char *path, const char *filter, Allocator *allocator) {
    FileInfo *test = (FileInfo *)allocator->Reallocate(allocator, 0, 0, sizeof(FileInfo));
    return test;
}

static FileInfo windowsGetWorkingDirectory(Allocator *allocator) {
    FileInfo result = { 0 };

    wchar_t wideString[MAX_PATH];
    if (!GetCurrentDirectoryW(MAX_PATH, wideString))
        return result;

    result.flags |= FILE_IS_VALID;
    result.flags |= FILE_IS_DIRECTORY;
    result.path = windowsConvertUTF16ToUTF8(wideString, allocator);

    return result;
}

static FileInfo windowsGetProcessName(Allocator *allocator) {
    FileInfo result = { 0 };

    wchar_t wideString[MAX_PATH];
    if (!GetModuleFileNameW(0, wideString, MAX_PATH))
        return result;

    result.flags |= FILE_IS_VALID;
    result.flags |= FILE_IS_PROCESS;
    result.path = windowsConvertUTF16ToUTF8(wideString, allocator);

    return result;
}

static bool windowsFileSystemRemoveFile(const char *path) {
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

static File windowsDLLOpen(const char *path) {
    File result = { 0 };

    wchar_t wideString[MAX_PATH];
    WINDOWS_UTF8_TO_UTF16(path, wideString);
    HMODULE handle = LoadLibraryExW(wideString, 0, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
    result.handle = (u64)handle;
    result.isValid = handle != 0 ? true : false;

    return result;
}

static void windowsDLLClose(File *dll) {
    if (!dll->isValid)
        return;

    FreeLibrary((HMODULE)dll->handle);
}

static struct DLLAPI windowsDLLHandling = {
    .Open = windowsDLLOpen,
    .Close = windowsDLLClose
};

#pragma endregion

#pragma region General System

static void windowsSystemLogDebug(const char *location, const u32 line, const char *text) {
    //TODO: Create our own sprintf (or at least look at the STB one)
    //TODO: Handle dynamic memory allocations here
    char time[16];
    SystemTime systemTime = platform->time->GetSystem();
    sprintf(time, "%u:%u:%u:%u", systemTime.hours, systemTime.minutes, systemTime.seconds, systemTime.milliseconds);

    char message[CORE_PLATFORM_MAX_LOG_SIZE];
    if (sprintf(message, "[%s] %s \t %s(%u)\n", time, text, location, line)) { //TODO: Is the time accurate here? Should it be captured when the log function is called rather than in this function?
        //TODO: Use Wide char version
        OutputDebugStringA(message);
        return;
    }

    OutputDebugStringA("Message too long!\n");
}

static void windowsSystemLogConsole(const char *location, const u32 line, const char *text) {
    char time[16];
    SystemTime systemTime = platform->time->GetSystem();
    sprintf(time, "%u:%u:%u:%u", systemTime.hours, systemTime.minutes, systemTime.seconds, systemTime.milliseconds);

    char message[CORE_PLATFORM_MAX_LOG_SIZE];
    if (sprintf(message, "[%s] %s \t %s(%u)\n", time, text, location, line)) {
        printf(message);
        return;
    }

    // //TODO: Get the WideChar converter working here
    // wchar_t time[16];
    // SystemTime systemTime = platform->time->GetSystem();
    // swprintf_s(time, 16, "%u:%u:%u:%u", systemTime.hours, systemTime.minutes, systemTime.seconds, systemTime.milliseconds);

    // wchar_t message[CORE_PLATFORM_MAX_LOG_SIZE];
    // if (swprintf_s(message, "[%s] %s \t %s(%u)\n", time, text, location, line)) {
    //     HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    //     DWORD charsWritten = 0;
    //     WriteConsoleW(outputHandle, message, _tcslen(message), &charsWritten, NULL);
    //     CloseHandle(outputHandle);
    // }
}

static void windowsDebugBreak() {
    DebugBreak();
}

static struct SystemAPI windowsSystem = {
    .LogDebug = windowsSystemLogDebug,
    .LogConsole = windowsSystemLogConsole,
    .DebugBreak = windowsDebugBreak
};

#pragma endregion

#pragma region Global API

static struct PlatformAPI windowsPlatform = {
    .virtualMemory = &windowsVirtualMemory,
    .time = &windowsTime,
    .file = &windowsFileIO,
    .fileSystem = &windowsFileSystem,
    .dll = &windowsDLLHandling,
    .system = &windowsSystem
};

struct PlatformAPI *platform = &windowsPlatform;

#pragma endregion

#endif