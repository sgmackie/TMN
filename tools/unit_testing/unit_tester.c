#include <core/memory.h>
#include <platform.h>

int main() {
    // Load modules
    platform->dll->Open("");

    MemoryArena arena = CreateMemoryArena();
    f32 *array = arena.allocator.Reallocate(&arena.allocator, 0, sizeof(f32) * 64);
    return 0;
}