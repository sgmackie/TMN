#include "Allocator.h"
#include "CoreMath.h"
#include "Logging.h"
#include "Platform.h"

namespace Core {
uptr Allocator::AlignPointer(uptr Pointer, const usize Alignment)
{
    if (!IsPowerOfTwo<usize>(Alignment)) {
        return 0;
    }

    uptr Modulo = Pointer & ((uptr) Alignment - 1);
    if (Modulo == 0) {
        return Pointer;
    }

    uptr AlignedPointer = (Pointer += Alignment - Modulo);
    return AlignedPointer;
}

usize Allocator::GetPointerAlignmentOffset(uptr A, uptr B, const usize Alignment)
{
    const usize Offset = (usize) A + B;
    const usize Mask = Alignment - 1;
    if (Offset & Mask) {
        return Alignment - (Offset & Mask);
    }

    return 0;
}

void *AllocatorCRT::Allocate(const usize Size, const usize Alignment)
{
    if (Alignment == 0) {
        return malloc(Size);
    }

#if defined(CORE_PLATFORM_WINDOWS)
    return _aligned_malloc(Size, Alignment);
#else
    return aligned_alloc(Size, Alignment);
#endif
}

void *AllocatorCRT::Reallocate(void *OldPointer, const usize NewSize, const usize Alignment)
{
    return realloc(OldPointer, NewSize);
}

void AllocatorCRT::Free(void *Pointer, const usize Size)
{
    free(Pointer);
}

void *AllocatorMiMalloc::Allocate(const usize Size, const usize Alignment)
{
	void *Result = mi_malloc_aligned(Size, Alignment);
	CORE_ASSERT(Result != nullptr);
	return Result;
}

void *AllocatorMiMalloc::Reallocate(void *OldPointer, const usize NewSize, const usize Alignment)
{
	CORE_ASSERT(OldPointer != nullptr);
	void *Result = mi_realloc_aligned(OldPointer, NewSize, Alignment);
	CORE_ASSERT(Result != nullptr);
	return Result;
}

void AllocatorMiMalloc::Free(void *Pointer, const usize Size)
{
	CORE_ASSERT(Pointer != nullptr);
	mi_free(Pointer);
}

AllocatorTLSF::AllocatorTLSF(const usize InPoolSize)
{
	Platform::VirtualMemory::MemoryStats SystemStats = Platform::VirtualMemory::GetMemoryStats();
	CORE_ASSERT(InPoolSize >= SystemStats.MinimumAllocationSize);
	PoolSize = InPoolSize;

	AllocatorBacking = malloc(tlsf_size()); // TODO: Move this allocation into the first VirtualAlloc pool call?
	TLSF = tlsf_create(AllocatorBacking);
	CurrentPool = CreatePool();
}

AllocatorTLSF::~AllocatorTLSF()
{
	while (CurrentPool != nullptr)
	{
		tlsf_remove_pool(TLSF, CurrentPool->Handle);
		Platform::VirtualMemory::Decommit(CurrentPool, PoolSize);
		CurrentPool = CurrentPool->Previous;
	}

    tlsf_destroy(TLSF);
	free(AllocatorBacking);
    AllocatorBacking = nullptr;
}

AllocatorTLSF::TLSFPool* AllocatorTLSF::CreatePool()
{
	TLSFPool *NewPool = (TLSFPool*) Platform::VirtualMemory::Reserve(PoolSize, Platform::VirtualMemory::Flags::ReadWrite);
	Platform::VirtualMemory::Commit(NewPool, PoolSize, Platform::VirtualMemory::Flags::ReadWrite);

	const usize BlockOffset = sizeof(TLSFPool);
	NewPool->Block = (u8 *) NewPool + BlockOffset;
	NewPool->UsedSpace = 0;
	NewPool->Handle = tlsf_add_pool(TLSF, NewPool->Block, PoolSize - BlockOffset);
	NewPool->Previous = CurrentPool;
	return NewPool;
}

void *AllocatorTLSF::Allocate(const usize Size, const usize Alignment)
{
	const usize BlockOffset = sizeof(TLSFPool);
	const usize ActualFreeSpace = PoolSize - (CurrentPool->UsedSpace + BlockOffset + tlsf_pool_overhead());

	CORE_ASSERT(Size <= ActualFreeSpace);

	void *Result = tlsf_memalign(TLSF, Alignment, Size);
	CORE_ASSERT(Result != nullptr);
	CurrentPool->UsedSpace += Size;
	return Result;
}

void *AllocatorTLSF::Reallocate(void *OldPointer, const usize NewSize, const usize Alignment)
{
    void *Result = tlsf_realloc(TLSF, OldPointer, NewSize);
    CORE_ASSERT(Result != nullptr);
	CurrentPool->UsedSpace += NewSize; // TODO: Double check the sizes here
    return Result;
}

void AllocatorTLSF::Free(void *Pointer, const usize Size)
{
	CORE_ASSERT(Pointer != nullptr);
    tlsf_free(TLSF, Pointer);
	CurrentPool->UsedSpace -= Size;
}

AllocatorLinear::AllocatorLinear(Allocator *InBackingAllocator, const usize BlockSizeInBytes)
{
    BackingAllocator = InBackingAllocator;
    CORE_ASSERT(BackingAllocator != nullptr);

    Block = reinterpret_cast<u8 *>(BackingAllocator->Allocate(BlockSizeInBytes, BackingAllocator->DefaultAlignment));
    CORE_ASSERT(Block != nullptr);

    BlockSize = BlockSizeInBytes;
    CurrentOffset = 0;
    PreviousOffset = 0;
}

AllocatorLinear::~AllocatorLinear()
{
    BackingAllocator->Free(Block, BlockSize);
    CurrentOffset = 0;
    PreviousOffset = 0;
}

void *AllocatorLinear::Allocate(const usize Size, const usize Alignment)
{
    uptr CurrentPointer = (uptr) Block + (uptr) CurrentOffset;
    uptr BlockOffset = AlignPointer(CurrentPointer, Alignment);
    BlockOffset -= (uptr) Block;

    if (BlockOffset + Size > BlockSize) {
        return nullptr;
    }

    void *Result = &Block[BlockOffset];
    PreviousOffset = BlockOffset;
    CurrentOffset = BlockOffset + Size;

    memset(Result, 0, Size);
    return Result;
}

void *AllocatorLinear::Reallocate(void *OldPointer, const usize NewSize, const usize Alignment)
{
    return nullptr;
}

void AllocatorLinear::Free(void *Pointer, const usize Size)
{
    return;
}

TempAllocatorLinear::TempAllocatorLinear(Allocator *InParent)
{
    AllocatorLinear *LinearAllocator = reinterpret_cast<AllocatorLinear *>(InParent);
    ParentAllocator = LinearAllocator;
    CurrentOffset = LinearAllocator->CurrentOffset;
    PreviousOffset = LinearAllocator->PreviousOffset;
}

TempAllocatorLinear::~TempAllocatorLinear()
{
    AllocatorLinear *LinearAllocator = reinterpret_cast<AllocatorLinear *>(ParentAllocator);
    LinearAllocator->CurrentOffset = CurrentOffset;
    LinearAllocator->PreviousOffset = PreviousOffset;
    ParentAllocator = nullptr;
}

void *TempAllocatorLinear::Allocate(const usize Size, const usize Alignment)
{
    return ParentAllocator->Allocate(Size, ParentAllocator->DefaultAlignment);
}

AllocatorArena::AllocatorArena(const usize InMinimumBlockSize, const usize InDefaultAlignment)
{
    MinimumBlockSize = InMinimumBlockSize;
    DefaultAlignment = InDefaultAlignment;
}

void *AllocatorArena::Allocate(const usize Size, const usize Alignment)
{
    CORE_ASSERT(Alignment <= 128);
    CORE_ASSERT(IsPowerOfTwo<usize>(Alignment));

    // Calculate total Allocation size
    usize AlignmentOffset = 0;
    bool bIsArenaOutOfMemory = false;
    if (CurrentBlock != nullptr) {
        AlignmentOffset = GetPointerAlignmentOffset((uptr) CurrentBlock->Buffer, CurrentBlock->UsedSpace, Alignment);
    }
    usize AllocationSize = AlignmentOffset + Size;

    if (CurrentBlock != nullptr) {
        bIsArenaOutOfMemory = ((CurrentBlock->UsedSpace + AllocationSize) > CurrentBlock->Size);
    }

    if (CurrentBlock == nullptr || bIsArenaOutOfMemory) {
        AllocationSize = Size;
        const usize NewBlockSize = Max(AllocationSize, MinimumBlockSize);

        const usize TotalAllocationSize = NewBlockSize + sizeof(ArenaBlock);

        ArenaBlock *NewBlock = reinterpret_cast<ArenaBlock *>(Platform::VirtualMemory::Reserve(TotalAllocationSize, Platform::VirtualMemory::Flags::ReadWrite));
        Platform::VirtualMemory::Commit(NewBlock, TotalAllocationSize, Platform::VirtualMemory::Flags::ReadWrite);
        NewBlock->Size = NewBlockSize;
        NewBlock->PreviousBlock = nullptr;
        NewBlock->UsedSpace = 0;

        const usize BaseOffset = sizeof(ArenaBlock);
        NewBlock->Buffer = (u8 *) NewBlock + BaseOffset;

        NewBlock->PreviousBlock = CurrentBlock;
        CurrentBlock = NewBlock;
    }

    CORE_ASSERT((CurrentBlock->UsedSpace + AllocationSize) <= CurrentBlock->Size);

    // Recalculate alignment
    AlignmentOffset = GetPointerAlignmentOffset((uptr) CurrentBlock->Buffer, CurrentBlock->UsedSpace, Alignment);
    uptr OffsetInBlock = CurrentBlock->UsedSpace + AlignmentOffset;
    void *Result = CurrentBlock->Buffer + OffsetInBlock;
    CurrentBlock->UsedSpace += AllocationSize;

    CORE_ASSERT(AllocationSize >= Size);

    return Result;
}

void *AllocatorArena::Reallocate(void *OldPointer, const usize NewSize, const usize Alignment)
{
    return nullptr;
}

void AllocatorArena::Free(void *Pointer, const usize Size)
{
    return;
}

void AllocatorArena::Destroy()
{
    while (CurrentBlock != nullptr) {
        const bool bIsLastBlock = (CurrentBlock->PreviousBlock == nullptr); // TODO: Why is this crashing

        ArenaBlock *LastBlock = CurrentBlock;
        CurrentBlock = LastBlock->PreviousBlock;
        Platform::VirtualMemory::Decommit(LastBlock, LastBlock->Size);
        LastBlock = nullptr;

        if (bIsLastBlock) {
            return;
        }
    }
}

AllocatorArenaTemp *AllocatorArena::CreateTempArena()
{
    AllocatorArenaTemp *Result = (AllocatorArenaTemp *) Allocate(sizeof(AllocatorArenaTemp *), DefaultAlignment);
    Result->ParentArena = this;
    Result->CurrentBlock = CurrentBlock;
    Result->UsedSpace = CurrentBlock != nullptr ? CurrentBlock->UsedSpace : 0;
    TempArenaCount++;
    return Result;
}

void AllocatorArena::DestroyTempArena(AllocatorArenaTemp *TempArena)
{
    AllocatorArena *ParentArena = TempArena->ParentArena;
    while (ParentArena->CurrentBlock != TempArena->CurrentBlock) {
        Free(nullptr, 0);
    }

    if (ParentArena->CurrentBlock != nullptr) {
        CORE_ASSERT(ParentArena->CurrentBlock->UsedSpace >= TempArena->UsedSpace);
        ParentArena->CurrentBlock->UsedSpace = TempArena->UsedSpace;
    }

    CORE_ASSERT(ParentArena->TempArenaCount > 0);
    ParentArena->TempArenaCount--;
}

void *AllocatorArenaTemp::Allocate(const usize Size, const usize Alignment)
{
    return ParentArena->Allocate(Size, Alignment);
}

AllocatorVirtualGuard::AllocatorVirtualGuard()
{
    VirtualOffset = 0;
    CurrentVirtualBlockSize = 0;
    PageSize = Platform::VirtualMemory::GetMemoryStats().PageSize;
}

void *AllocatorVirtualGuard::Allocate(const usize Size, const usize Alignment)
{
    const usize ActualAlignment = Max(Alignment, ALLOCATOR_DEFAULT_ALIGNMENT);
    const usize AlignedSize = Align(Size, ActualAlignment);
    const usize AllocationSize = (AlignedSize + sizeof(GuardBlock)) + (PageSize - 1) & ~(PageSize - 1U);
    const usize AlignedAllocationSize = AllocationSize + PageSize;

    void *Allocation = nullptr;
    if ((VirtualOffset + AlignedAllocationSize) <= CurrentVirtualBlockSize) {
        Allocation = (void *) VirtualOffset;
    } else {
        const usize ReserveSize = Max(AlignedAllocationSize, ALLOCATOR_GUARD_BLOCK_SIZE);
        Allocation = Platform::VirtualMemory::Reserve(ReserveSize, Platform::VirtualMemory::Flags::Protect); // TODO: Fix this on Mac
        VirtualOffset = reinterpret_cast<uptr>(Allocation);
        CurrentVirtualBlockSize = (VirtualOffset + ReserveSize);
    }

    CORE_ASSERT(Allocation != nullptr);
    VirtualOffset += AlignedAllocationSize;

    void *Result = reinterpret_cast<void *>(reinterpret_cast<u8 *>(Allocation) + AllocationSize - AlignedSize);
    const GuardBlock GuardData = { Allocation, AlignedAllocationSize, AlignedSize };

    Platform::VirtualMemory::Commit(Allocation, AllocationSize, Platform::VirtualMemory::Flags::ReadWrite);

    GuardBlock *GuardDataAllocation = reinterpret_cast<GuardBlock *>(reinterpret_cast<u8 *>(Result) - sizeof(GuardBlock));
    *GuardDataAllocation = GuardData;

    return Result;
}

void *AllocatorVirtualGuard::Reallocate(void *OldPointer, const usize NewSize, const usize Alignment)
{
    CORE_ASSERT(OldPointer != nullptr);
    void *Result = Allocate(NewSize, Alignment);
    GuardBlock *GuardData = reinterpret_cast<GuardBlock *>(reinterpret_cast<u8 *>(OldPointer) - sizeof(GuardBlock));
    memcpy(Result, OldPointer, Min(GuardData->RequestedSize, NewSize));
    Free(OldPointer);
    return Result;
}

void AllocatorVirtualGuard::Free(void *Pointer, const usize Size)
{
    CORE_ASSERT(Pointer != nullptr);

    GuardBlock *GuardData = reinterpret_cast<GuardBlock *>(Pointer);
    GuardData--;

    Platform::VirtualMemory::Decommit(GuardData->Allocation, GuardData->Size);
}

}