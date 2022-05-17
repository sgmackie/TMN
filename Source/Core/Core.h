#pragma once

#include "Types.h"
#include "Platform.h"
#include "Maths.h"
#include "Colour.h"
#include "Memory.h"
#include "Container.h"
#include "Log.h"
#include "Profiling.h"

namespace Core {

	enum class GlobalAllocatorType
	{
		MiMalloc,
		Malloc,
		VirtualGuard
	};

	static Memory::Allocator* Startup(GlobalAllocatorType allocatorType)
	{
		if (allocatorType == GlobalAllocatorType::VirtualGuard) {
			return new Memory::AllocatorVirtualGuard();
		}
		else if (allocatorType == GlobalAllocatorType::Malloc) {
			return new Memory::AllocatorMalloc();
		}

		return new Memory::AllocatorMiMalloc();
	}

	static void Shutdown(Memory::Allocator* globalAllocator)
	{
		delete globalAllocator;
	}
}

void *operator new(const size_t size, Core::Memory::Allocator* allocator)
{
	return allocator->Allocate(size);
}
 
void operator delete(void *pointer, Core::Memory::Allocator* allocator)
{
	allocator->Free(pointer);
}
