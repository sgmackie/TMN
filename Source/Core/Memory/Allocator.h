#pragma once

#include "Types.h"
#include "mimalloc.h"

namespace Core {
namespace Memory {
	class Allocator
	{
	public:
		virtual void *Allocate(const usize size) = 0;
		virtual void *Reallocate(void *oldPointer, const usize oldSize, const usize newSize) = 0;
		virtual void Free(void *pointer, const usize size = 0) = 0;

		template<typename T>
		T *AllocateElement(const usize count = 1) 
		{
			return reinterpret_cast<T*>(Allocate(sizeof(T) * count));
		}

		template<typename T>
		T *ReallocateElement(T *pointer, const usize oldCount, const usize newCount)
		{
			return reinterpret_cast<T*>(Reallocate(pointer, sizeof(T) * oldCount, sizeof(T) * newCount));
		}
	};

	class AllocatorMalloc : public Allocator
	{
	public:
		void *Allocate(const usize size) override;
		void *Reallocate(void *oldPointer, const usize oldSize, const usize newSize) override;
		void Free(void *pointer, const usize size = 0) override;
	};

	class AllocatorArena : public Allocator
	{
	public:
		AllocatorArena(Allocator *backingAllocator, usize backingSize);
		~AllocatorArena();
		void *Allocate(const usize size) override;
		void *Reallocate(void *oldPointer, const usize oldSize, const usize newSize) override;
		void Free(void *pointer, const usize size = 0) override;

		Allocator *BackingAllocator;
		u8 *Buffer;
		usize BufferSize;
		usize PreviousOffset;
		usize CurrentOffset;
	};
}
}

