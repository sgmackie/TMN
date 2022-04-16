#include "Memory.h"
#include "Platform.h"
#include "Math.h"

namespace Core {
namespace Memory {
	void *AllocatorMalloc::Allocate(const usize size)
	{
		return mi_malloc(size);
	}

	void *AllocatorMalloc::Reallocate(void *oldPointer, const usize oldSize, const usize newSize)
	{
		return mi_realloc(oldPointer, newSize);
	}

	void AllocatorMalloc::Free(void *pointer, const usize size)
	{
        mi_free(pointer);
	}

	AllocatorArena::AllocatorArena(Allocator *backingAllocator, usize backingSize)
	{
		BackingAllocator = backingAllocator;
		Buffer = (u8 *) BackingAllocator->Allocate(backingSize);
		BufferSize = backingSize;
		PreviousOffset = 0;
		CurrentOffset = 0;
	}

	AllocatorArena::~AllocatorArena()
	{
		BackingAllocator->Free(Buffer);
		Buffer = nullptr;
		PreviousOffset = 0;
		CurrentOffset = 0;
	}

	void *AllocatorArena::Allocate(const usize size)
	{
		uptr currentPosition = (uptr) Buffer + (uptr) CurrentOffset;
		usize defaultAlignment = (2 * sizeof(void *));
		uptr currentPositionAligned = Memory::AlignPointer(currentPosition, defaultAlignment);
	
		// Make position relative
		currentPositionAligned -= (uptr) Buffer;

		if (currentPositionAligned + size > BufferSize)
		{
			return nullptr;
		}

		void *result = &Buffer[currentPositionAligned];
		PreviousOffset = currentPositionAligned;
		CurrentOffset = currentPositionAligned + size;
		memset(result, 0, size);
		return result;
	}

	void *AllocatorArena::Reallocate(void *oldPointer, const usize oldSize, const usize newSize)
	{
		if (oldPointer == nullptr || oldSize == 0) {
			return Allocate(newSize);
		}
	
		if (Buffer > oldPointer || oldPointer > Buffer + BufferSize) {
			return nullptr;
		}

		if (Buffer + PreviousOffset == oldPointer) {
			CurrentOffset = PreviousOffset + newSize;
			memset(&Buffer[CurrentOffset], 0, (newSize - oldSize));
			return oldPointer;
		}

		void *newPointer = Allocate(newSize);
		usize copySize = Max(oldSize, newSize);
		memmove(newPointer, oldPointer, copySize);
		return newPointer;
	}

	void AllocatorArena::Free(void *pointer, const usize size)
	{
		PreviousOffset = 0;
		CurrentOffset = 0;
	}
}
}

