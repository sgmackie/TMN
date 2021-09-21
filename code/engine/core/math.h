#pragma once

#include "types.h"

#define Minimum(A, B) ((A < B) ? (A) : (B))
#define Maximum(A, B) ((A > B) ? (A) : (B))

bool U64IsPowerOf2(u64 x);

// Combine two u32s together by shifting the high part to the left:
// (0x0000000000000001 << 32) = 0x0000000100000000
u64 U32CombineHighLow(u32 highPart, u32 lowPart);