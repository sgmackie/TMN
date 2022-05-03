#include "Memory.h"
#include "Platform.h"
#include "Maths.h"
#include "mimalloc.h"

namespace Core {
namespace Memory {
	void *AllocatorMalloc::Allocate(const usize size, const usize alignment)
	{
		if (alignment == 0) {
			return malloc(size);
		}

		return _aligned_malloc(size, alignment);
	}
	
	void *AllocatorMalloc::Reallocate(void *oldPointer, const usize newSize, const usize alignment)
	{
		if (alignment == 0) {
			return realloc(oldPointer, newSize);
		}

		return _aligned_realloc(oldPointer, newSize, alignment);
	}
	
	void AllocatorMalloc::Free(void *pointer, const usize size)
	{
		free(pointer);
	}

	void *AllocatorMiMalloc::Allocate(const usize size, const usize alignment)
	{
		const usize actualAlignment = Max(alignment, ALLOCATOR_DEFAULT_ALIGNMENT);
		void *result = mi_malloc_aligned(size, actualAlignment);
#if defined(CORE_MEMORY_ZERO_INITIALISE)
		memset(result, 0, size);
#endif
		return result;
	}
	
	void *AllocatorMiMalloc::Reallocate(void *oldPointer, const usize newSize, const usize alignment)
	{
		const usize actualAlignment = Max(alignment, ALLOCATOR_DEFAULT_ALIGNMENT);
		void *result = mi_realloc_aligned(oldPointer, newSize, actualAlignment);
#if defined(CORE_MEMORY_ZERO_INITIALISE)
		memset(result, 0, newSize);
#endif
		return result;
	}
	
	void AllocatorMiMalloc::Free(void *pointer, const usize size)
	{
		mi_free(pointer);
	}

	AllocatorVirtualGuard::AllocatorVirtualGuard()
	{
		VirtualOffset = 0;
		CurrentVirtualBlockSize = 0;
		PageSize = Platform::GetMemoryStats().PageSize;
	}

	void *AllocatorVirtualGuard::Allocate(const usize size, const usize alignment)
	{
		const usize actualAlignment = Max(alignment, ALLOCATOR_DEFAULT_ALIGNMENT);
		const usize alignedSize = Math::Align(size, actualAlignment);
		const usize allocationSize = (alignedSize + sizeof(GuardBlock)) + (PageSize - 1) & ~(PageSize - 1U);
		const usize alignedAllocationSize = allocationSize + PageSize;

		void* allocation = nullptr;
		if ((VirtualOffset + alignedAllocationSize) <= CurrentVirtualBlockSize)
		{
			allocation = (void*) VirtualOffset;
		}
		else
		{
			const usize reserveSize = Max(alignedAllocationSize, ALLOCATOR_GUARD_BLOCK_SIZE);
			allocation = Platform::VirtualMemoryReserve(reserveSize, Platform::VirtualMemoryFlags::Protect);
			VirtualOffset = reinterpret_cast<uptr>(allocation);
			CurrentVirtualBlockSize = (VirtualOffset + reserveSize);
		}

		assert(allocation != nullptr);
		VirtualOffset += alignedAllocationSize;

		void *result = reinterpret_cast<void*>(reinterpret_cast<u8*>(allocation) + allocationSize - alignedSize);
		const GuardBlock guardData = { allocation, alignedAllocationSize, alignedSize };
		
		Platform::VirtualMemoryCommit(allocation, allocationSize, Platform::VirtualMemoryFlags::ReadWrite);

		GuardBlock* guardDataAllocation = reinterpret_cast<GuardBlock*>(reinterpret_cast<u8*>(result) - sizeof(GuardBlock));
		*guardDataAllocation = guardData;

		return result;
	}
	
	void *AllocatorVirtualGuard::Reallocate(void *oldPointer, const usize newSize, const usize alignment)
	{
		assert(oldPointer != nullptr);
		void *result = Allocate(newSize, alignment);
		GuardBlock* guardData = reinterpret_cast<GuardBlock*>(reinterpret_cast<u8*>(oldPointer) - sizeof(GuardBlock));
		memcpy(result, oldPointer, Min(guardData->RequestedSize, newSize));
		Free(oldPointer);
		return result;
	}
	
	void AllocatorVirtualGuard::Free(void *pointer, const usize size)
	{
		assert(pointer != nullptr);

		GuardBlock *guardData = reinterpret_cast<GuardBlock*>(pointer);
		guardData--;

		Platform::VirtualMemoryDecommit(guardData->Allocation, guardData->Size);
	}

	AllocatorLinear::AllocatorLinear(Allocator *allocator, const usize capacity)
	{
		assert(allocator != nullptr);
		Backing = allocator;

		Block = reinterpret_cast<u8*>(Backing->Allocate(sizeof(u8) * capacity, ALLOCATOR_DEFAULT_ALIGNMENT));
		assert(Block != nullptr);
		Size = 0;
		Capacity = Math::Align<usize>(capacity, 16);
	}

	AllocatorLinear::~AllocatorLinear()
	{
		Backing->Free(Block, sizeof(u8) * Capacity);
		Block = nullptr;
		Size = 0;
		Capacity = 0;
	}

	// TODO: Worth splitting the block in two and having two offsets that meet in the middle?
	// "Frame-Based Memory Allocation in Game Programming Gems" as example
	void *AllocatorLinear::Allocate(const usize size, const usize alignment)
	{
		const usize actualAlignment = Max(alignment, ALLOCATOR_DEFAULT_ALIGNMENT);
		usize allocationSize = Math::RoundUpToPower<usize>(size, actualAlignment);
		assert((allocationSize + Size) <= Capacity);
		
		void* allocation = Block + Size;
		Size += allocationSize;

#if defined(CORE_MEMORY_ZERO_INITIALISE)
		memset(allocation, 0, Size);
#endif
		return allocation;
	}
	
	void *AllocatorLinear::Reallocate(void *oldPointer, const usize newSize, const usize alignment)
	{
		assert(false);
		return nullptr;
	}
	
	void AllocatorLinear::Free(void *pointer, const usize size)
	{
		Size = 0;

#if defined(CORE_MEMORY_ZERO_INITIALISE)
		memset(Block, 0, sizeof(u8) * Capacity);
#endif
	}

	AllocatorPool::AllocatorPool(Allocator *allocator, const usize blockSize)
	{
		BlockSize = blockSize;
		CurrentBlock = nullptr;
		Backing = allocator;
	}

	void *AllocatorPool::Allocate(const usize size, const usize alignment)
	{
		if (CurrentBlock == nullptr) {
			const usize allocationSize = BlockSize * size;
			const usize alignedSize= Math::RoundUpToPower<usize>(allocationSize, ALLOCATOR_DEFAULT_ALIGNMENT);

			PoolBlock *startingBlock = reinterpret_cast<PoolBlock*>(Backing->Allocate(alignedSize)); 
			PoolBlock *block = startingBlock;
 
			for (usize i = 0; i < BlockSize - 1; ++i) {
				block->NextBlock = reinterpret_cast<PoolBlock*>(reinterpret_cast<PoolBlock*>(block) + size);
				block = block->NextBlock;
			}
 
			block->NextBlock = nullptr;
			CurrentBlock = startingBlock;
		}

		PoolBlock *result = CurrentBlock;
		CurrentBlock = CurrentBlock->NextBlock;
		return result;
	}
	
	void *AllocatorPool::Reallocate(void *oldPointer, const usize newSize, const usize alignment)
	{
		assert(false);
		return nullptr;
	}
	
	void AllocatorPool::Free(void *pointer, const usize size)
	{
		reinterpret_cast<PoolBlock*>(pointer)->NextBlock = CurrentBlock;
		CurrentBlock = reinterpret_cast<PoolBlock*>(pointer);
		return;
	}
}
}

