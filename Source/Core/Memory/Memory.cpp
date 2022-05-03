#include "Memory.h"
#include "Maths.h"

namespace Core {
namespace Memory {
    uptr AlignPointer(uptr pointer, usize alignment)
    {
		if (!Math::IsPowerOfTwo<usize>(alignment)) {
			return 0;
		}

		uptr modulo = pointer & ((uptr) alignment - 1);
		if (modulo == 0) {
			return pointer;
		}

		uptr alignedPointer = (pointer += alignment - modulo);
		return alignedPointer;
    }

    usize GetPointerAlignmentOffset(uptr a, uptr b, const usize alignment)
    {
        usize offset = (usize) a + b;
        usize mask = alignment - 1;
        if(offset & mask)
        {
            return alignment - (offset & mask);
        }
    
        return 0;
    }
}
}
