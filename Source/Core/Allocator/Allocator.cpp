#include "Allocator.h"
#include "Platform.h"

namespace Core {

	// Allocator General
	void *AllocatorGeneral::Allocate(const usize size)
	{
		// MemoryBlock *currentBlock = (MemoryBlock*) Platform::VirtualMap(1);

		usize alignedSize = AlignByTypeSize(size, sizeof(iptr));
		return nullptr;
	}

	void *AllocatorGeneral::Reallocate(void *oldPointer, const usize oldSize, const usize newSize)
	{
		return nullptr;
	}

	void AllocatorGeneral::Free(void *pointer, const usize size)
	{
		return;
	}
}