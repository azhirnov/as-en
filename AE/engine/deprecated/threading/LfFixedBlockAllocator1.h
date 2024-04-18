// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Primitives/SpinLock.h"
# include "threading/Primitives/SyncEvent.h"
#endif

#ifdef AE_DEBUG
#	define AE_LFFIXEDBLOCKALLOC_DEBUG	1
#else
#	define AE_LFFIXEDBLOCKALLOC_DEBUG	0
#endif

namespace AE::Threading
{

	//
	// Lock-free Fixed block Allocator
	//

	template <usize ChunkSize_v = 64*64,
			  usize MaxChunks_v = 1024,
			  typename BlockAllocatorType	= UntypedAllocator,
			  typename GeneralAllocatorType	= UntypedAllocator
			 >
	class LfFixedBlockAllocator1 final : public IAllocatorTS
	{
		StaticAssert( ChunkSize_v > 0 );
		StaticAssert( IsMultipleOf( ChunkSize_v, 32 ) or IsMultipleOf( ChunkSize_v, 64 ));
		StaticAssert( ChunkSize_v <= 64*64 );
		StaticAssert( MaxChunks_v > 0 );
		StaticAssert( IsPowerOfTwo( ChunkSize_v ));	// must be power of 2 to increase performance
		StaticAssert( BlockAllocatorType::IsThreadSafe );
		StaticAssert( GeneralAllocatorType::IsThreadSafe );

	// types
	public:
		using Self				= LfFixedBlockAllocator1< ChunkSize_v, MaxChunks_v, BlockAllocatorType, GeneralAllocatorType >;
		using BlockAllocator_t	= BlockAllocatorType;
		using GenAllocator_t	= GeneralAllocatorType;
		using Ptr_t				= RstPtr<void>;

	private:
		static constexpr uint	ChunkSize		= ChunkSize_v;
		static constexpr uint	MaxChunks		= MaxChunks_v;
		static constexpr uint	LowLevel_Count	= (ChunkSize <= 32 ? 32 : 64);
		static constexpr uint	HiLevel_Count	= Max( 1u, (ChunkSize + LowLevel_Count - 1) / LowLevel_Count );

		using LowLevelBits_t	= Conditional< (LowLevel_Count <= 32), uint, ulong >;
		using LowLevels_t		= StaticArray< Atomic< LowLevelBits_t >, HiLevel_Count >;
		using HiLevelBits_t		= Conditional< (HiLevel_Count <= 32), uint, ulong >;

		StaticAssert( HiLevel_Count <= 64 );

		struct alignas(AE_CACHE_LINE) ChunkInfo
		{
			Atomic< HiLevelBits_t >	hiLevel;		// 0 - is unassigned bit, 1 - assigned bit
			SpinLockRelaxed			hiLevelGuard;	// only for 'hiLevel' modification

			LowLevels_t				lowLevel;		// 0 - is unassigned bit, 1 - assigned bit
			Atomic< void * >		memBlock;

			#if AE_LFFIXEDBLOCKALLOC_DEBUG
			Atomic< SourceLoc *>	dbgInfo;
			SyncEvent				dbgInfoEvent {SyncEvent::EFlags::ManualReset};
			#endif
		};

		using ChunkInfos_t	= StaticArray< ChunkInfo, MaxChunks >;

		static constexpr HiLevelBits_t	MaxHighLevel		= ToBitMask<HiLevelBits_t>( HiLevel_Count );
		static constexpr HiLevelBits_t	InitialHighLevel	= ~MaxHighLevel;
		static constexpr uint			HighWaitCount		= 2;
		static constexpr usize			ThreadToChunkMask	= MaxChunks < 5 ? 0 : 15;


	// variables
	private:
		ChunkInfos_t		_chunkInfo;
		const POTBytes		_blockSize;
		const POTBytes		_blockAlign;

		NO_UNIQUE_ADDRESS BlockAllocator_t	_blockAlloc;
		NO_UNIQUE_ADDRESS GenAllocator_t	_genAlloc;

		Atomic<ulong>		_dbgCounter		{0};
		Atomic<ulong>		_dbgLockCounter	{0};


	// methods
	public:
		LfFixedBlockAllocator1  (Bytes blockSize,
								Bytes blockAlign,
								const BlockAllocator_t&	blockAlloc	= Default,
								const GenAllocator_t&	genAlloc	= Default) __NE___;

		~LfFixedBlockAllocator1 ()								__NE___	{ Release( True{"checkMemLeak"} ); }

			void		Release (Bool checkMemLeak)				__NE___;



		ND_ Ptr_t		AllocBlock ()							__NE___	{ return AllocBlock( Default ); }
		ND_ Ptr_t		AllocBlock (const SourceLoc &loc)		__NE___;
			bool		DeallocBlock (void *)					__NE___;

		ND_ Bytes		BlockSize ()							C_NE___	{ return Bytes{ _blockSize }; }
		ND_ Bytes		BlockAlign ()							C_NE___	{ return Bytes{ _blockAlign }; }
		ND_ POTBytes	BlockSizePOT ()							C_NE___	{ return _blockSize; }
		ND_ POTBytes	BlockAlignPOT ()						C_NE___	{ return _blockAlign; }

		ND_ Bytes		LargeBlockSize ()						C_NE___	{ return BlockSize() * ChunkSize; }
		ND_ Bytes		MaxMemorySize ()						C_NE___	{ return LargeBlockSize() * MaxChunks; }
		ND_ usize		MaxBlockCount ()						C_NE___	{ return MaxChunks * ChunkSize; }
		ND_ Bytes		AllocatedSize ()						C_NE___;


		// IAllocator //
		ND_ void*		Allocate (const SizeAndAlign sa)		__NE_OV;
			using		IAllocator::Allocate;

			void		Deallocate (void*, const SizeAndAlign)	__NE_OV;
			void		Deallocate (void* ptr)					__NE_OV	{ CHECK( DeallocBlock( ptr )); }
			using		IAllocator::Deallocate;

	private:
		ND_ Ptr_t		_Alloc (uint chunkIndex, const SourceLoc &loc, INOUT ulong& dbgCounter, INOUT ulong& lockCounter) __NE___;

		ND_ static constexpr Bytes	_DbgInfoSize ()	__NE___	{ return SizeOf<SourceLoc> * ChunkSize; }
		ND_ static constexpr Bytes	_DbgInfoAlign ()__NE___	{ return AlignOf<SourceLoc>; }
	};


} // AE::Threading

#include "LfFixedBlockAllocator1.inl.h"
#undef AE_LFFIXEDBLOCKALLOC_DEBUG
