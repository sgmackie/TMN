#include "Memory.h"
#include "Math.h"

namespace Core {
namespace Memory {
    uptr AlignPointer(uptr pointer, usize alignment)
    {
        if (!Math::IsPow2(alignment)) {
			return 0;
		}

		uptr modulo = pointer & ((uptr) alignment - 1);
		if (modulo == 0) {
			return pointer;
		}

		uptr alignedPointer = (pointer += alignment - modulo);
		return alignedPointer;
    }
}
}
