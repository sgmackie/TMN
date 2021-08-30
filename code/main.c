#include "core/platform.h"
#include "core/types.h"

#if defined(TESTS_RUN)
#include "../tests/test_runner.h"
#endif

int main() {

#if defined(TESTS_RUN)
    if (!TestRunAll())
        return -1;
#endif

    platform->fileIO->OpenToRead("C:/BuildScratch/rtw/data/test.txt");

    return 0;
}

