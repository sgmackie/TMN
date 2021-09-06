#pragma once

#include "profiling.h"

#if defined(GLOBAL_USE_ASSERTS)
#define ASSERT(test, text, ...) ((test) || (platform->system->Log(text), platform->system->DebugBreak(), false))
#define COMPILE_ASSERT(test) static_assert(test, #test)
#else
#define ASSERT(test, text, ...)
#define COMPILE_ASSERT(test)
#endif