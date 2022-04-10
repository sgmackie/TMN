#pragma once

#include "Types.h"

namespace Core {

	struct MemoryBlock
	{
		usize size; // Value greater than 0 means the block is being used
		MemoryBlock *nextBlock;
		iptr data[1];
	};

	class Allocator
	{
	public:
		virtual void *Allocate(const usize size) = 0;
		virtual void *Reallocate(void *oldPointer, const usize oldSize, const usize newSize) = 0;
		virtual void Free(void *pointer, const usize size) = 0;
	};

	class AllocatorGeneral : public Allocator
	{
	public:
		void *Allocate(const usize size) override;
		void *Reallocate(void *oldPointer, const usize oldSize, const usize newSize) override;
		void Free(void *pointer, const usize size) override;
	};

	static inline usize AlignByTypeSize(const usize size, const usize typeSize)
	{
		return (size + typeSize - 1) & ~(typeSize - 1);
	}

	// Look at UE5's FMalloc interface and the BinnedArena Allocator - how do the free lists work?

	// Follow this list: https://www.reddit.com/r/gamedev/comments/f4nh49/c_memory_allocator_for_games/
	// Want:
	// Arena
	// Pool
	// Tagged arena of pools
	// General free list allocator for things like string handling http://dmitrysoshnikov.com/compilers/writing-a-memory-allocator/#segregated-list-search
	// Can create a temporary child allocator from these
	// Stack allocator

	class AllocatorCRT : public Allocator
	{
	public:
	};
}