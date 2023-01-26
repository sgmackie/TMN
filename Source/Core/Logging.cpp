#include "Logging.h"
#include "Platform.h"

#include "../3rdParty/stb/stb_sprintf.h"

namespace Core {
char *BuildMessage(Core::Allocator *Allocator, const char *Format, ...)
{
    // Find char buffer length
    va_list VArgs;
    va_start(VArgs, Format);
    i32 BufferLength = stbsp_vsnprintf(nullptr, 0, Format, VArgs);

    // Allocate buffer
    char *Buffer = reinterpret_cast<char *>(Allocator->Allocate(sizeof(char) * BufferLength + 1));
    stbsp_vsnprintf(Buffer, BufferLength + 1, Format, VArgs);
    va_end(VArgs);
    return Buffer;
}

void LogMessage(Core::Allocator *Allocator, const char *File, const u32 Line, const char *Message)
{
    TempAllocatorLinear TA = TempAllocatorLinear(Allocator);

    const char *Location = "";
    Location = BuildMessage(Allocator, "%s(%d)", File, Line);

    char *Result = BuildMessage(Allocator, "%s: %s\n", Location, Message);
    Platform::Process::PrintToConsole(Allocator, Result);
}
}