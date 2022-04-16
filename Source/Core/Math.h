#pragma once

#include "Types.h"

#define Min(A, B) ((A < B) ? (A) : (B))
#define Max(A, B) ((A > B) ? (A) : (B))

namespace Core {
namespace Math {
    bool IsPow2(u32 value);
}
}

