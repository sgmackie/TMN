#if defined(CORE_PLATFORM_MAC)

#pragma once

#include "../../CoreTypes.h"
#include "../../Platform.h"

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
inline NSString *CoreStringToNSString(const Core::Container::String &string)
{
    return [NSString stringWithUTF8String:string.ToUTF8()];
}
#endif

#endif
