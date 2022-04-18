#if defined(CORE_PLATFORM_MACOS)

#pragma once

#include "Types.h"
#include "Container/String.h"

// Mac includes
typedef f64 double_t;
#if defined(__OBJC__)
#import <Cocoa/Cocoa.h>
#endif
#include <Carbon/Carbon.h>
#include <CoreServices/CoreServices.h>

#include <CoreFoundation/CoreFoundation.h>
#undef TYPE_BOOL

// String handling
#if defined(__OBJC__)
inline NSString* CoreStringToNSString (const Core::Container::String& string)
{
    return [NSString stringWithUTF8String: string.ToUTF8()];
}
#endif


#endif

