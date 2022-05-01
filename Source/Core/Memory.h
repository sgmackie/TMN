#pragma once

#include "Types.h"
#include "Memory/Allocator.h"

namespace Core {
namespace Memory {
    uptr AlignPointer(uptr pointer, const usize alignment);
    usize GetPointerAlignmentOffset(uptr a, uptr b, const usize alignment);
}
}

