#include "Core.h"

using namespace Core;
using namespace Core::Container;

// TODO: Fix clang-tidy auto save
// TODO: assert with message and call stack

struct PoolData
{
	Array<f32, 2> points;
	i32 handle;
	u64 flag;
};


void Run(Memory::Allocator *allocator)
{
	PROFILER_EVENT_START("Run", Math::ColourSRGB(255, 0, 0))

	f64 *stompBlock = allocator->AllocateElement<f64>(512);
	stompBlock[341] = 3141242135;
	allocator->Free(stompBlock);

	Memory::AllocatorLinear programAllocator(allocator, Megabytes(4));
	i32 *blocktrt = (i32 *) programAllocator.Allocate(sizeof(i32) * 9);
	blocktrt[4] = 3124;
	blocktrt[13] = 214;
	printf("%d\n", blocktrt[4]);
	programAllocator.Free(blocktrt);

	DynamicArray<float> testArray(&programAllocator);
	for (usize i = 0; i < 8; ++i) {
		testArray.Add(i);
	}

	testArray.RemoveAt(2);
	testArray.Clear();

	i32 *spanTest = programAllocator.AllocateElement<i32>(8);
	Span<i32> spanA(spanTest, 4);
	spanTest[7] = 55;
	spanA[3] = 44;

	for (const i32 n : spanA) {
		printf("%d\n", n);
	}

	Memory::AllocatorPool poolAllocator(&programAllocator, sizeof(PoolData));
	PoolData* dataAllocation = (PoolData*) poolAllocator.Allocate(64);
	Span<PoolData> poolBlock(dataAllocation, 64);

	for (PoolData &pd : poolBlock) {
		pd.points[0] = 124;
		pd.points[1] = 3124;
		f32 result = pd.points[0] + pd.points[1];
		printf("%f\n", result);
	}

	poolAllocator.Free(dataAllocation);

	// GApplicationPath.Append(allocator, argv[0]);
	// String testFile = String(allocator, argv[1]);
	// String testFilePath = String::JoinAsPath(allocator, &GApplicationPath, &testFile);
	// printf("%s", testFilePath.ToUTF8());

	PROFILER_EVENT_STOP()
}

int main(int argc, char *argv[])
{
	assert(argc == 2);

#if defined(CORE_ALLOCATOR_USE_VIRTUALGUARD)
	Memory::AllocatorVirtualGuard *gAllocator = new Memory::AllocatorVirtualGuard();
#else
	Memory::AllocatorMalloc gAllocator;
#endif

	PROFILER_THREAD_ID("Main")

	Run(gAllocator);
		
	delete gAllocator;

	return 0;
}

