#include "Core.h"

using namespace Core;

// TODO: Fix clang-tidy auto save

int main()
{
	Memory::AllocatorArena memoryArena = Memory::AllocatorArena(&GAllocator, Megabytes(4));

	i32 *test = memoryArena.AllocateElement<i32>(8);
	test[3] = 235236;
	test = memoryArena.ReallocateElement<i32>(test, 8, 32);
	printf("%d", test[3]);
	memoryArena.Free(test);



	return 0;
}

