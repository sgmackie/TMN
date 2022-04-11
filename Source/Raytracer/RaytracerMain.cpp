#include "Core.h"

using namespace Core;

// TODO: Fix clang-tidy auto save

int main()
{
	AllocatorHeap globalHeap;
	globalHeap.Create();


	globalHeap.Destroy();

	return 0;
}