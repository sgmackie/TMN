#include "Math.h"

namespace Core {
namespace Math {
    bool IsPow2(u32 value)
    {
        return ((value & ~(value - 1)) == value);
    }

}
}

