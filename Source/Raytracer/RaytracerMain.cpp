#include "Core.h"

using namespace Core;

// TODO: Setup the Gitlab CI pipeline
// TODO: Get clang-tidy working again

int main()
{
	// Get the raytracer up and running

	// AllocatorVM allocatorVM;

	float test = 12;
	test++;
	float a = test + 2;
	a += test;

	// Platform::FileOpen(&allocatorVM, "Test", Platform::FileAccessMode::FILE_READ);

	// Create a macro for allocations that wraps the allocation with a log to a file, like:
	// void* Block = nullptr;
	// ALLOCATE(Alloctor, Block, "Asset Pool", FILE, LINE)
	// Block = Alloctor->Alloc(...);
	// GlobalLogger->Log(...);


	AllocatorGeneral GA;
	i32 *block = (i32 *) GA.Allocate(sizeof(i32) * 4);
	block[2] = 23;
	printf("%d", block[2]);
	GA.Free(block, 0);

	return 0;
}