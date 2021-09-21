#pragma once

#include "platform.h"
#include "types.h"

#define LOG(text) platform->system->Log(__FILE__, __LINE__, text)

#if defined(GLOBAL_USE_ASSERTS)
#define ASSERT(test, text, ...) ((test) || (platform->system->LogDebug(__FILE__, __LINE__, text), platform->system->DebugBreak(), false))
#define COMPILE_ASSERT(test) static_assert(test, #test) //TODO: This isn't working on MSVC
#else
#define ASSERT(test, text, ...)
#define COMPILE_ASSERT(test)
#endif