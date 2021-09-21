#pragma once

#include "math.h"

bool U64IsPowerOf2(u64 x) {
    return (x & (x - 1)) == 0;
}

bool USizeIsPowerOf2(usize x) {
    return (x & (x - 1)) == 0;
}

u64 U32CombineHighLow(u32 highPart, u32 lowPart) {
    return (u64)highPart << 32 | lowPart;
}