#pragma once

#include "Types.h"

namespace Core {
namespace Memory {
	class Allocator
	{
	public:
		#define ALLOCATOR_DEFAULT_ALIGNMENT 16

		virtual void *Allocate(const usize size, const usize alignment = 0) = 0;
		virtual void *Reallocate(void *oldPointer, const usize newSize, const usize alignment = 0) = 0;
		virtual void Free(void *pointer, const usize size = 0) = 0;

		template<typename T>
		T *AllocateElement(const usize count = 1) 
		{
			return reinterpret_cast<T*>(Allocate(sizeof(T) * count, 0)); // TODO: Sort out alignment
		}

		template<typename T>
		T *ReallocateElement(T *pointer, const usize oldCount, const usize newCount)
		{
			return reinterpret_cast<T*>(Reallocate(pointer, sizeof(T) * oldCount, sizeof(T) * newCount, 0));
		}
	};

	class AllocatorMalloc : public Allocator
	{
	public:
		void *Allocate(const usize size, const usize alignment = 0) override;
		void *Reallocate(void *oldPointer, const usize oldSize, const usize alignment = 0) override;
		void Free(void *pointer, const usize size = 0) override;
	};

	class AllocatorMiMalloc : public Allocator
	{
	public:
		void *Allocate(const usize size, const usize alignment = 0) override;
		void *Reallocate(void *oldPointer, const usize oldSize, const usize alignment = 0) override;
		void Free(void *pointer, const usize size = 0) override;
	};

	// https://pzurita.wordpress.com/2015/06/29/memory-stomp-allocator-for-unreal-engine-4/
	class AllocatorVirtualGuard : public Allocator
	{
	public:
		#define ALLOCATOR_GUARD_BLOCK_SIZE Gigabytes(1)

		struct GuardBlock
		{
			void *Allocation;
			usize Size;
			usize RequestedSize;
		};

		AllocatorVirtualGuard();
		void *Allocate(const usize size, const usize alignment = 0) override;
		void *Reallocate(void *oldPointer, const usize oldSize, const usize alignment = 0) override;
		void Free(void *pointer, const usize size = 0) override;

		usize PageSize;
		uptr VirtualOffset;
		usize CurrentVirtualBlockSize;
	};

	class AllocatorLinear : public Allocator
	{
	public:
		AllocatorLinear(Allocator *allocator, const usize capacity);
		~AllocatorLinear();
		void *Allocate(const usize size, const usize alignment = 0) override;
		void *Reallocate(void *oldPointer, const usize oldSize, const usize alignment = 0) override;
		void Free(void *pointer, const usize size = 0) override;

		u8* Block;
		usize Size;
		usize Capacity; // In bytes
		Allocator *Backing;
	};

	// http://dmitrysoshnikov.com/compilers/writing-a-pool-allocator/
	class AllocatorPool : public Allocator
	{
	public:
		struct PoolBlock
		{
			PoolBlock *NextBlock;
		};

		AllocatorPool(Allocator *allocator, const usize blockSize);
		void *Allocate(const usize size, const usize alignment = 0) override;
		void *Reallocate(void *oldPointer, const usize oldSize, const usize alignment = 0) override;
		void Free(void *pointer, const usize size = 0) override;

		usize BlockSize;
		PoolBlock *CurrentBlock;
		Allocator *Backing;
	};
}
}

