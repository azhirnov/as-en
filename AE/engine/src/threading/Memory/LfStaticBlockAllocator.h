// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Primitives/SpinLock.h"
# include "threading/Primitives/SyncEvent.h"
# include "threading/Primitives/DataRaceCheck.h"
#endif

namespace AE::Threading
{

	//
	// Lock-free Static Block Allocator
	//

	template <usize ChunkSize_v = 64*64,
			  usize MaxChunks_v = 1024
			 >
	class LfStaticBlockAllocator final : public IAllocatorTS
	{
		StaticAssert( ChunkSize_v > 0 );
		StaticAssert( IsMultipleOf( ChunkSize_v, 32 ) or IsMultipleOf( ChunkSize_v, 64 ));
		StaticAssert( ChunkSize_v <= 64*64 );
		StaticAssert( MaxChunks_v > 0 );
		StaticAssert( IsPowerOfTwo( ChunkSize_v ));	// must be power of 2 to increase performance

	// types
	public:
		using Self		= LfStaticBlockAllocator< ChunkSize_v, MaxChunks_v >;
		using Ptr_t		= RstPtr<void>;

	private:
		static constexpr uint	ChunkSize		= ChunkSize_v;
		static constexpr uint	MaxChunks		= MaxChunks_v;
		static constexpr uint	LowLevel_Count	= (ChunkSize <= 32 ? 32 : 64);
		static constexpr uint	HiLevel_Count	= Max( 1u, (ChunkSize + LowLevel_Count - 1) / LowLevel_Count );

		using LowLevelBits_t	= Conditional< (LowLevel_Count <= 32), uint, ulong >;
		using LowLevels_t		= StaticArray< Atomic< LowLevelBits_t >, HiLevel_Count >;
		using HiLevelBits_t		= Conditional< (HiLevel_Count <= 32), uint, ulong >;

		StaticAssert( HiLevel_Count <= 64 );

		struct alignas(AE_CACHE_LINE) BottomChunk
		{
			SpinLockRelaxed				hiLevelGuard;	// only for 'hiLevel' modification
			Atomic< HiLevelBits_t >		hiLevel;		// 0 - is unassigned bit, 1 - assigned bit
			LowLevels_t					lowLevel;		// 0 - is unassigned bit, 1 - assigned bit
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
		Ptr_t				_storage;
		const POTBytes		_blockSize;
		const POTBytes		_blockAlign;

		TopChunks_t			_topChunks;
		BottomChunks_t		_bottomChunks;


	// methods
	public:
		LfStaticBlockAllocator (void*	storage,
								Bytes	storageSize,
								Bytes	blockSize,
								Bytes	blockAlign)				__NE___;
		~LfStaticBlockAllocator ()								__NE___	{ Release( True{"checkMemLeak"} ); }

			void	Release (Bool checkMemLeak)					__NE___;

		ND_ Ptr_t	AllocBlock ()								__NE___;
			bool	DeallocBlock (void *)						__NE___;

		ND_ Bytes	BlockSize ()								C_NE___	{ return Bytes{ _blockSize }; }
		ND_ Bytes	BlockAlign ()								C_NE___	{ return Bytes{ _blockAlign }; }
		ND_ POTBytes BlockSizePOT ()							C_NE___	{ return _blockSize; }
		ND_ POTBytes BlockAlignPOT ()							C_NE___	{ return _blockAlign; }

		ND_ Bytes	LargeBlockSize ()							C_NE___	{ return BlockSize() * ChunkSize; }
		ND_ Bytes	MaxMemorySize ()							C_NE___	{ return LargeBlockSize() * MaxChunks; }
		ND_ usize	MaxBlockCount ()							C_NE___	{ return MaxChunks * ChunkSize; }
		ND_ Bytes	AllocatedSize ()							C_NE___;

		// utils //
		ND_ static Bytes  CalcStorageSize (Bytes blockSize)		__NE___	{ return blockSize * MaxChunks * ChunkSize; }


		// IAllocator //
		ND_ void*	Allocate (const SizeAndAlign sa)			__NE_OV;
			using	IAllocator::Allocate;

			void	Deallocate (void*, const SizeAndAlign)		__NE_OV;
			void	Deallocate (void* ptr)						__NE_OV	{ CHECK( DeallocBlock( ptr )); }
			using	IAllocator::Deallocate;

	private:
		ND_ Ptr_t  _Alloc (uint chunkIndex)						__NE___;
		ND_ Bytes  _PtrOffset (uint chunkIndex, uint idxInChunk)C_NE___;
	};


} // AE::Threading

#include "threading/Memory/LfStaticBlockAllocator.inl.h"
