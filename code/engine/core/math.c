#pragma once

#include "math.h"

u64 U32CombineHighLow(u32 highPart, u32 lowPart) {
    return (u64)highPart << 32 | lowPart;
}