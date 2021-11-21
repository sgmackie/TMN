#include "../Platform.h"

namespace Core {
namespace Platform {
    File FileOpen(const char path)
    {
        File Result = {};
        Result.Handle = 0;
        return Result;
    }
}
}