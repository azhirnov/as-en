// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Primitives/Atomic.h"
# include "base/Math/BitMath.h"
# include "base/Math/Bytes.h"
# include "base/Math/Math.h"
# include "base/Math/POTValue.h"
# include "base/Memory/UntypedAllocator.h"
# include "base/Utils/SourceLoc.h"
# include "threading/Primitives/SpinLock.h"
# include "threading/Primitives/SyncEvent.h"
# include "base/Algorithms/StringUtils.h"
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
			  typename BlockAllocatorType	= UntypedAlignedAllocator,
			  typename GeneralAllocatorType	= UntypedAlignedAllocator
			 >
	class LfFixedBlockAllocator3
	{
		STATIC_ASSERT( ChunkSize_v > 0 );
		STATIC_ASSERT( ((ChunkSize_v % 32) == 0) or ((ChunkSize_v % 64) == 0) );
		STATIC_ASSERT( ChunkSize_v <= 64*64 );
		STATIC_ASSERT( MaxChunks_v > 0 );
		STATIC_ASSERT( IsPowerOfTwo( ChunkSize_v ));	// must be power of 2 to increase performance
		STATIC_ASSERT( BlockAllocatorType::IsThreadSafe );
		STATIC_ASSERT( GeneralAllocatorType::IsThreadSafe );

	// types
	public:
		using Self				= LfFixedBlockAllocator3< ChunkSize_v, MaxChunks_v, BlockAllocatorType, GeneralAllocatorType >;
		using BlockAllocator_t	= BlockAllocatorType;
		using GenAllocator_t	= GeneralAllocatorType;

		static constexpr bool	IsThreadSafe = true;

	private:
		static constexpr uint	ChunkSize		= ChunkSize_v;
		static constexpr uint	MaxChunks		= MaxChunks_v;
		static constexpr uint	LowLevel_Count	= (ChunkSize <= 32 ? 32 : 64);
		static constexpr uint	HiLevel_Count	= Max( 1u, (ChunkSize + LowLevel_Count - 1) / LowLevel_Count );
		
		using LowLevelBits_t	= Conditional< (LowLevel_Count <= 32), uint, ulong >;
		using LowLevels_t		= StaticArray< Atomic< LowLevelBits_t >, HiLevel_Count >;
		using HiLevelBits_t		= Conditional< (HiLevel_Count <= 32), uint, ulong >;

		STATIC_ASSERT( HiLevel_Count <= 64 );

		struct alignas(AE_CACHE_LINE) BottomChunk
		{
			SpinLockRelaxed				hiLevelGuard;	// only for 'hiLevel' modification
			Atomic< HiLevelBits_t >		hiLevel;		// 0 - is unassigned bit, 1 - assigned bit
			LowLevels_t					lowLevel;		// 0 - is unassigned bit, 1 - assigned bit
			Atomic< void * >			memBlock;

			#if AE_LFFIXEDBLOCKALLOC_DEBUG
			Atomic< SourceLoc *>		dbgInfo;
			SyncEvent					dbgInfoEvent {SyncEvent::EFlags::ManualReset};
			#endif
		};
		
		static constexpr HiLevelBits_t	MaxHighLevel		= ToBitMask<HiLevelBits_t>( HiLevel_Count );
		static constexpr HiLevelBits_t	InitialHighLevel	= ~MaxHighLevel;
		static constexpr uint			HighWaitCount		= 2;
		static constexpr usize			ThreadToChunkMask	= MaxChunks < 5 ? 0 : 3;
		
		using TopLevelBits_t	= Conditional< (MaxChunks <= 32), uint, ulong >;
		static constexpr uint	TopLevel_Count	= (MaxChunks + CT_SizeOfInBits<TopLevelBits_t> - 1) / CT_SizeOfInBits<TopLevelBits_t>;

		struct TopChunk
		{
			SpinLockRelaxed				assignedGuard;	// only for 'assigned' modification
			Atomic< TopLevelBits_t >	assigned;		// 0 - is unassigned bit, 1 - assigned bit
		};
		
		using BottomChunks_t	= StaticArray< BottomChunk, MaxChunks >;
		using TopChunks_t		= StaticArray< TopChunk, TopLevel_Count >;


	// variables
	private:
		TopChunks_t			_topChunks;
		const POTValue		_blockSize;
		const POTValue		_blockAlign;
		BlockAllocator_t	_blockAlloc;

		NO_UNIQUE_ADDRESS GenAllocator_t	_genAlloc;
		NO_UNIQUE_ADDRESS BottomChunks_t	_bottomChunks;
		
		Atomic<ulong>		_dbgCounter		{0};
		Atomic<ulong>		_dbgLockCounter	{0};


	// methods
	public:
		LfFixedBlockAllocator3  (Bytes blockSize,
								Bytes blockAlign,
								const BlockAllocator_t&	blockAlloc	= Default,
								const GenAllocator_t&	genAlloc	= Default);

		~LfFixedBlockAllocator3 ()	{ Release( true ); }
		
		LfFixedBlockAllocator3 (const Self &) = delete;
		LfFixedBlockAllocator3 (Self &&) = delete;

		Self& operator = (const Self &) = delete;
		Self& operator = (Self &&) = delete;
		
		NDRST( void *)	Alloc ()								{ return Alloc( Default ); }
		NDRST( void *)	Alloc (const SourceLoc &loc);
				bool	Dealloc (void *);

		ND_ Bytes	BlockSize ()		const	{ return Bytes{ usize( _blockSize )}; }
		ND_ Bytes	BlockAlign ()		const	{ return Bytes{ usize( _blockAlign )}; }
		ND_ Bytes	LargeBlockSize ()	const	{ return BlockSize() * ChunkSize; }
		ND_ Bytes	MaxMemorySize ()	const	{ return LargeBlockSize() * MaxChunks; }
		ND_ usize	MaxBlockCount ()	const	{ return MaxChunks * ChunkSize; }
		ND_ Bytes	AllocatedSize ()	const;

			void	Release (bool checkMemLeak);

	private:
		NDRST( void *)	_Alloc (uint chunkIndex, const SourceLoc &loc, INOUT ulong& dbgCounter, INOUT ulong& lockCounter);

		ND_ static constexpr Bytes	_DbgInfoSize ()		{ return SizeOf<SourceLoc> * ChunkSize; }
		ND_ static constexpr Bytes	_DbgInfoAlign ()	{ return AlignOf<SourceLoc>; }
	};


} // AE::Threading
