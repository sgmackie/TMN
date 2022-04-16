#include "Core.h"

using namespace Core;

// TODO: Fix clang-tidy auto save

int main()
{
	Memory::AllocatorArena memoryArena = Memory::AllocatorArena(&GAllocator, Megabytes(4));

	i32 *test = memoryArena.AllocateElement<i32>(8);
	test[3] = 235236;
	test = memoryArena.ReallocateElement<i32>(test, 8, 32);
	printf("%d \n", test[3]);
	memoryArena.Free(test);

	Container::Array<f32, 50> safeArray;
	safeArray[3] = 6.623;

	Container::DynamicArray<u64> dynamicArray(&memoryArena, 256);

	for (usize i = 0; i < 128; ++i)
	{
		dynamicArray.Add(i);
	}

	printf("%llu \n", dynamicArray[89]);
	printf("%llu \n", dynamicArray[127]);
	printf("%llu \n", dynamicArray[1]);

	return 0;
}

