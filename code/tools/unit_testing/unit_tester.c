#include <core.h>

#define MAX_TEST_CASES 1
#define MAX_TEST_REPEAT_COUNT 4

#define TEST_CONDITION(test, text, result)                          \
    if (test) {                                                     \
        result = TEST_PASS;                                         \
    } else {                                                        \
        result = TEST_FAIL;                                         \
        platform->system->LogConsole(__FUNCTION__, __LINE__, text); \
    }

typedef enum TestResult {
    TEST_FAIL,
    TEST_PASS
} TestResult;

typedef struct TestUnit {
    TestResult (*TestCase)();
} TestUnit;

static void I32ArrayLoop(i32 *array, const usize count) {
    for (usize i = 0; i < count; ++i) {
        array[i] = (i + 1) + count;
    }
}

static TestResult TestAllocators() {
    TestResult result = TEST_FAIL;

    // Virtual Allocator
    Allocator virtualAlloctor = MemoryVirtualAllocatorCreate();
    const usize allocationSize = (sizeof(i32) * 16);
    i32 *array = virtualAlloctor.Reallocate(&virtualAlloctor, NULL, 0, allocationSize);
    TEST_CONDITION(array != NULL, "Virtually allocated address is invalid!", result);

    I32ArrayLoop(array, 16);
    TEST_CONDITION(array[15] == (16 * 2), "Allocated array is invalid!", result);

    virtualAlloctor.Free(&virtualAlloctor, array, allocationSize);
    TEST_CONDITION(virtualAlloctor.currentSize == 0, "Virtual allocation size is not 0 after free!", result);

    // Memory Arena
    Allocator memoryArena = MemoryArenaCreate(&virtualAlloctor);
    TEST_CONDITION(memoryArena.instanceData != NULL, "Memory Arena has no backing allocator!", result);
    MemoryArenaDestroy(&memoryArena);
    TEST_CONDITION(memoryArena.instanceData == NULL, "Memory Arena was not destroyed correctly!", result);

    // Cleanup
    MemoryVirtualAllocatorDestroy(&virtualAlloctor);

    return result;
}

int main(int argc, char **argv) {
    TestUnit testCases[MAX_TEST_CASES];

    // Create tests
    testCases[0].TestCase = TestAllocators;

    // Run tests
    TestResult mainResult = TEST_FAIL;
    for (usize i = 0; i < MAX_TEST_CASES; ++i) {
        for (usize j = 0; j < MAX_TEST_REPEAT_COUNT; ++j) {
            mainResult = testCases[i].TestCase();

            if (mainResult == TEST_FAIL) {
                return 0;
            }
        }
    }

    return 1;
}