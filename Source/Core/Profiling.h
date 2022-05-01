#pragma once

#include "Math.h"

#if defined(CORE_PROFILER_SUPERLUMINAL)
	#include "Superluminal/PerformanceAPI_capi.h"
	#define PROFILER_THREAD_ID(ID) PerformanceAPI_SetCurrentThreadName(ID);
#define PROFILER_EVENT_START(ID, Colour) PerformanceAPI_BeginEvent(ID, nullptr, Math::ConvertSRGBToU32(Colour));
	#define PROFILER_EVENT_STOP() PerformanceAPI_EndEvent();
#else
	#define PROFILER_THREAD_ID(ID)
	#define PROFILER_EVENT_START(ID, Colour)
	#define PROFILER_EVENT_STOP()
#endif