#pragma once

#include "Types.h"
#include "mimalloc.h"

namespace Core {

	class Allocator
	{
	public:
		virtual void *Allocate(const usize size) = 0;
		virtual void *Reallocate(void *oldPointer, const usize oldSize, const usize newSize) = 0;
		virtual void Free(void *pointer, const usize size) = 0;
	};

	class AllocatorHeap : public Allocator
	{
	public:
		void Create();
		void Destroy();
		void *Allocate(const usize size) override;
		void *Reallocate(void *oldPointer, const usize oldSize, const usize newSize) override;
		void Free(void *pointer, const usize size) override;

		mi_heap_t *InternalHeap = nullptr;
	};
}