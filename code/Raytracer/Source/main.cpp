#include "AllocatorVirtualMemory.hpp"
#include "Platform.hpp"
#include "Types.hpp"

using namespace Core;

// TODO: Setup the Gitlab CI pipeline
// TODO: Get clang-tidy working again

int main()
{
    // Get the raytracer up and running

    AllocatorVirtualMemory allocatorVM;

    Platform::FileOpen(&allocatorVM, "Test", Platform::FileAccessMode::FILE_READ);

    return 0;
}
