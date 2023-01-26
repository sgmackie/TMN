#include "Hashing.h"

namespace Core {
u64 HashStringFNV(Container::String *String)
{
    u64 Hash = 0x100;
    for (i32 i = 0; i < String->Length(); i++) {
        const char Character = String->GetCharacterAtIndex(i);
        Hash ^= Character & 255;
        Hash *= 1111111111111111111;
    }
    return Hash ^ Hash >> 32;
}
}