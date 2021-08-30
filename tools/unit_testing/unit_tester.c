#include <core/memory.h>
#include <platform.h>

int main(int argc, char **argv) {
    if (argc != 2)
        return;

    // Load modules
    platform->dll->Open(argv[1]);

    // MemoryArena arena = CreateMemoryArena();
    // f32 *array = arena.allocator.Reallocate(&arena.allocator, 0, sizeof(f32) * 64);
    return 0;
}