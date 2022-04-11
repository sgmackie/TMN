#include "Allocator.h"
#include "Platform.h"

namespace Core {
	void AllocatorHeap::Create()
	{
		InternalHeap = mi_heap_new();
	}

	void AllocatorHeap::Destroy()
	{
		mi_heap_destroy(InternalHeap);
	}
}