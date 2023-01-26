#pragma once

#include "Allocator.h"
#include "Colour.h"
#include "CoreTypes.h"

// Profiling
#if defined(CORE_PROFILER_SUPERLUMINAL)
#include "Superluminal/PerformanceAPI_capi.h"
#define PROFILER_THREAD_ID(ID) PerformanceAPI_SetCurrentThreadName(ID)
#define PROFILER_EVENT_START(ID, SRGB) PerformanceAPI_BeginEvent(ID, nullptr, Core::ConvertSRGBAToU32(SRGB))
#define PROFILER_EVENT_STOP() PerformanceAPI_EndEvent()
#else
#define PROFILER_THREAD_ID(ID)
#define PROFILER_EVENT_START(ID, Colour)
#define PROFILER_EVENT_STOP()
#endif

// Asserts
#define CORE_ASSERT(Condition) assert(Condition)

// Logging
#define CORE_LOG(TempAllocator, Format, ...) \
    LogMessage(TempAllocator, __FILE__, __LINE__, BuildMessage(TempAllocator, "" Format "", ##__VA_ARGS__))

namespace Core {
char *BuildMessage(Core::Allocator *Allocator, const char *Format, ...);
void LogMessage(Core::Allocator *Allocator, const char *File, const u32 Line, const char *Message);
}