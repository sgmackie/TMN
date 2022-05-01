#include "Memory.h"
#include "Platform.h"
#include "Math.h"

// References:
// https://hero.handmade.network/episode/code/day345/#809

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

	AllocatorVirtualGuard::AllocatorVirtualGuard()
	{
		VirtualOffset = 0;
		CurrentVirtualBlockSize = 0;
		PageSize = Platform::GetMemoryStats().PageSize;
	}

	void *AllocatorVirtualGuard::Allocate(const usize size, const usize alignment)
	{


		const usize actualAlignment = Max(alignment, 16);
		const usize alignedSize = Math::Align(size, actualAlignment);
		const usize allocationSize = (alignedSize + sizeof(StompAllocationData)) + (PageSize - 1) & ~(PageSize - 1U);
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
		const StompAllocationData stompData = { allocation, alignedAllocationSize, alignedSize };
		
		Platform::VirtualMemoryCommit(allocation, allocationSize, Platform::VirtualMemoryFlags::ReadWrite);

		StompAllocationData* stompDataPointer = reinterpret_cast<StompAllocationData*>(reinterpret_cast<u8*>(result) - sizeof(StompAllocationData));
		*stompDataPointer = stompData;

		return result;
	}
	
	void *AllocatorVirtualGuard::Reallocate(void *oldPointer, const usize newSize, const usize alignment)
	{
		assert(oldPointer != nullptr);
		void *result = Allocate(newSize, alignment);
		StompAllocationData* stompDataPointer = reinterpret_cast<StompAllocationData*>(reinterpret_cast<u8*>(oldPointer) - sizeof(StompAllocationData));
		memcpy(result, oldPointer, Min(stompDataPointer->RequestedSize, newSize));
		Free(oldPointer);
		return result;
	}
	
	void AllocatorVirtualGuard::Free(void *pointer, const usize size)
	{
		assert(pointer != nullptr);

		StompAllocationData *stompDataPointer = reinterpret_cast<StompAllocationData*>(pointer);
		stompDataPointer--;

		Platform::VirtualMemoryDecommit(stompDataPointer->Allocation, stompDataPointer->Size);
	}

	AllocatorLinear::AllocatorLinear(Allocator *allocator, const usize capacity)
	{
		assert(allocator != nullptr);
		Backing = allocator;

		Block = reinterpret_cast<u8*>(Backing->Allocate(sizeof(u8) * capacity, 16)); // TODO: Safe alignment?
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

	void *AllocatorLinear::Allocate(const usize size, const usize alignment)
	{
		usize allocationSize = Math::RoundUpToPower<usize>(size, 16);
		assert((allocationSize + Size) <= Capacity);
		
		// TODO: Remove the capacity and split each allocation into a seperate block
		void* allocation = Block + Size;
		Size += allocationSize;
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
		memset(Block, 0, sizeof(u8) * Capacity);
	}
}
}

