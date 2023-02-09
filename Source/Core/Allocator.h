#pragma once

#include "../3rdParty/tlsf/tlsf.h"
#include "mimalloc.h"
#include "CoreTypes.h"

#define ARENA_DEFAULT_ALIGNMENT 4

namespace Core {
class Allocator
{
public:
#define ALLOCATOR_DEFAULT_ALIGNMENT 16
    virtual void *Allocate(const usize Size, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT) = 0;
    virtual void *Reallocate(void *OldPointer, const usize NewSize, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT) = 0;
    virtual void Free(void *Pointer, const usize Size) = 0;

    template <typename T>
    T *AllocateElement(const usize Count, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT)
    {
        return reinterpret_cast<T *>(Allocate(sizeof(T) * Count, Alignment));
    }

    template <typename T>
    T *ReallocateElement(T *Pointer, const usize OldCount, const usize NewCount, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT)
    {
        return reinterpret_cast<T *>(Reallocate(Pointer, sizeof(T) * OldCount, sizeof(T) * NewCount, Alignment));
    }

    static uptr AlignPointer(uptr Pointer, const usize Alignment);
    static usize GetPointerAlignmentOffset(uptr A, uptr B, const usize Alignment);

    usize DefaultAlignment = ALLOCATOR_DEFAULT_ALIGNMENT;
};

class TempAllocator : public Allocator
{
public:
    TempAllocator(Allocator *InParent) {};
    ~TempAllocator() {};

    void *Allocate(const usize Size, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT) { return nullptr; }
    void *Reallocate(void *OldPointer, const usize NewSize, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT) { return nullptr; }
    void Free(void *Pointer, const usize Size) { }
};

class AllocatorCRT : public Allocator
{
public:
    void *Allocate(const usize Size, const usize Alignment = 0) override;
    void *Reallocate(void *OldPointer, const usize NewSize, const usize Alignment = 0) override;
    void Free(void *Pointer, const usize Size = 0) override;
};

class AllocatorMiMalloc : public Allocator
{
public:
	void *Allocate(const usize Size, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT) override;
	void *Reallocate(void *OldPointer, const usize NewSize, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT) override;
	void Free(void *Pointer, const usize Size = 0) override;
};


class AllocatorTLSF : public Allocator
{
public:
	struct TLSFPool
	{
		pool_t Handle = nullptr;
		usize UsedSpace = 0;
		void *Block = nullptr;
		TLSFPool *Previous = nullptr;
	};

    AllocatorTLSF(const usize InPoolSize);
    ~AllocatorTLSF();
    void *Allocate(const usize Size, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT) override;
    void *Reallocate(void *OldPointer, const usize NewSize, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT) override;
    void Free(void *Pointer, const usize Size = 0) override;
	TLSFPool* CreatePool();

    void *AllocatorBacking = nullptr;
    usize PoolSize = 0;
    tlsf_t TLSF;
	TLSFPool *CurrentPool = nullptr;
};

class AllocatorLinear : public Allocator
{
public:
    AllocatorLinear(Allocator *InBackingAllocator, const usize BlockSizeInBytes);
    ~AllocatorLinear();
    void *Allocate(const usize Size, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT) override;
    void *Reallocate(void *OldPointer, const usize NewSize, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT) override;
    void Free(void *Pointer, const usize Size = 0) override;

    usize PreviousOffset = 0;
    usize CurrentOffset = 0;
    usize BlockSize = 0;
    Allocator *BackingAllocator = nullptr;
    u8 *Block = nullptr;
};

class TempAllocatorLinear : public Allocator
{
public:
    TempAllocatorLinear(Allocator *InParent);
    ~TempAllocatorLinear();

    void *Allocate(const usize Size, const usize Alignment) override;
    void *Reallocate(void *OldPointer, const usize NewSize, const usize Alignment = ALLOCATOR_DEFAULT_ALIGNMENT) { return nullptr; }
    void Free(void *Pointer, const usize Size) { }

    usize PreviousOffset = 0;
    usize CurrentOffset = 0;
    Allocator *ParentAllocator = nullptr;
};

class AllocatorArenaTemp; // TODO: Remove forward declare?
class AllocatorArena : public Allocator
{
public:
    typedef struct ArenaBlock {
        usize Size;
        uptr UsedSpace;
        u8 *Buffer;
        ArenaBlock *PreviousBlock;
    } ArenaBlock;

    AllocatorArena(const usize InMinimumBlockSize, const usize InDefaultAlignment);
    void *Allocate(const usize Size, const usize Alignment) override;
    void *Reallocate(void *OldPointer, const usize NewSize, const usize Alignment) override;
    void Free(void *Pointer, const usize Size = 0) override;
    void Destroy();

    // Temp allocations
    AllocatorArenaTemp *CreateTempArena();
    void DestroyTempArena(AllocatorArenaTemp *TempArena);

    uptr MinimumBlockSize = Megabytes(1); // TODO: Find suitable size
    usize TempArenaCount = 0;
    ArenaBlock *CurrentBlock;
};

class AllocatorArenaTemp : public TempAllocator
{
public:
    void *Allocate(const usize Size, const usize Alignment = ARENA_DEFAULT_ALIGNMENT) override;

    uptr UsedSpace;
    AllocatorArena *ParentArena;
    AllocatorArena::ArenaBlock *CurrentBlock;
};

// https://pzurita.wordpress.com/2015/06/29/memory-stomp-allocator-for-unreal-engine-4/
class AllocatorVirtualGuard : public Allocator
{
public:
#define ALLOCATOR_GUARD_BLOCK_SIZE Gigabytes(1)
    typedef struct GuardBlock {
        void *Allocation;
        usize Size;
        usize RequestedSize;
    } GuardBlock;

    AllocatorVirtualGuard();
    void *Allocate(const usize Size, const usize Alignment = 0) override;
    void *Reallocate(void *OldPointer, const usize NewSize, const usize Alignment = 0) override;
    void Free(void *Pointer, const usize Size = 0) override;

    usize PageSize;
    uptr VirtualOffset;
    usize CurrentVirtualBlockSize;
};
}