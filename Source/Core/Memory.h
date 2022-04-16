#pragma once

#include "Types.h"
#include "Memory/Allocator.h"

namespace Core {
namespace Memory {
    uptr AlignPointer(uptr pointer, usize alignment);
}
}

