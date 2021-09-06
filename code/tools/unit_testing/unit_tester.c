#include <core.h>

int main(int argc, char **argv) {
    LOG("Start tests");
    ASSERT(argc == 1, "Invalid arg count");

    // Load modules
    File coreDLL = platform->dll->Open(argv[1]);

    ASSERT(coreDLL.isValid, "Test");

    MemoryArena arena = MemoryArenaCreate();
    f32 *array = arena.allocator.Reallocate(&arena.allocator, 0, sizeof(f32) * 64);

    platform->dll->Close(&coreDLL);

    return 0;
}