#include "Allocator.h"
#include "Platform.h"

namespace Core {
	void Allocator::Free(void *pointer, const usize size)
	{
        return;
	}

	AllocatorHeap::AllocatorHeap()
	{
		InternalHeap = mi_heap_new();
	}

	AllocatorHeap::~AllocatorHeap()
	{
		mi_heap_destroy(InternalHeap);
	}

	void *AllocatorHeap::Allocate(const usize size)
	{
		return nullptr;
	}

	void *AllocatorHeap::Reallocate(void *oldPointer, const usize oldSize, const usize newSize)
	{
		return nullptr;
	}

	void AllocatorHeap::Free(void *pointer, const usize size)
	{
        return;
	}
}
