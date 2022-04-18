#include "Core.h"

using namespace Core;
using namespace Core::Container;

// TODO: Fix clang-tidy auto save

// TODO: Move this to a global string hash table
static String GApplicationPath(&GAllocator);

int main(int argc, char *argv[])
{
	assert(argc == 2);
	GApplicationPath.Append(&GAllocator, argv[0]);

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

	String utfTest(&memoryArena, argv[1]);
	char* buffer = utfTest.ToUTF8();
	printf("%s\n", buffer);

	return 0;
}

