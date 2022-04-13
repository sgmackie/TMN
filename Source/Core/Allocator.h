#pragma once

#include "Types.h"
#include "mimalloc.h"

namespace Core {

	class Allocator
	{
	public:
		Allocator() {};
		~Allocator() {};
		virtual void *Allocate(const usize size) = 0;
		virtual void *Reallocate(void *oldPointer, const usize oldSize, const usize newSize) = 0;
		virtual void Free(void *pointer, const usize size) = 0;
	};

	class AllocatorHeap : public Allocator
	{
	public:
		AllocatorHeap();
		~AllocatorHeap();
		void *Allocate(const usize size);
		void *Reallocate(void *oldPointer, const usize oldSize, const usize newSize);
		void Free(void *pointer, const usize size);

		mi_heap_t *InternalHeap = nullptr;
	};
}
