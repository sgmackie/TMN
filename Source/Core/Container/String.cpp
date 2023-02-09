#include "String.h"

namespace Core {
namespace Container {
	String String::FromSystemTime(Core::Allocator *InAllocator, const Platform::Time::SystemTime& Time, const bool bWithUnderScores)
	{
		char TempBuffer[STRING_FORMAT_BUFFER];
		usize StringSize;
		if (bWithUnderScores)
		{
			StringSize = stbsp_sprintf(TempBuffer, "%lu_%lu_%lu_%lu_%lu_%lu_%lu", Time.Day, Time.Month, Time.Year, Time.Hour, Time.Minute, Time.Second, Time.Milliseconds);
		}
		else
		{
			StringSize = stbsp_sprintf(TempBuffer, "%lu/%lu/%lu %lu:%lu:%lu:%lu", Time.Day, Time.Month, Time.Year, Time.Hour, Time.Minute, Time.Second, Time.Milliseconds);
		}

		String Result(InAllocator, TempBuffer, StringSize);
		return Result;
	}
}
}