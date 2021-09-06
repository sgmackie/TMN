#pragma once

#include "profiling.h"

#define LOG(text) platform->system->Log(__FILE__, __LINE__, text)

#if defined(GLOBAL_USE_ASSERTS)
#define ASSERT(test, text, ...) ((test) || (platform->system->Log(__FILE__, __LINE__, text), platform->system->DebugBreak(), false))
#define COMPILE_ASSERT(test) static_assert(test, #test)
#else
#define ASSERT(test, text, ...)
#define COMPILE_ASSERT(test)
#endif