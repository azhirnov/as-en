// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "base/Math/BitMath.h"
# include "base/Math/Bytes.h"
# include "base/Math/Math.h"
# include "base/Math/POTValue.h"
# include "base/Memory/UntypedAllocator.h"
# include "base/Utils/SourceLoc.h"
# include "base/Algorithms/StringUtils.h"
# include "threading/Primitives/Atomic.h"
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
	class LfFixedBlockAllocator
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
		using Self				= LfFixedBlockAllocator< ChunkSize_v, MaxChunks_v, BlockAllocatorType, GeneralAllocatorType >;
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
		const POTValue		_blockSize;
		const POTValue		_blockAlign;

		NO_UNIQUE_ADDRESS BlockAllocator_t	_blockAlloc;
		NO_UNIQUE_ADDRESS GenAllocator_t	_genAlloc;

		Atomic<ulong>		_dbgCounter		{0};
		Atomic<ulong>		_dbgLockCounter	{0};


	// methods
	public:
		LfFixedBlockAllocator  (Bytes blockSize,
								Bytes blockAlign,
								const BlockAllocator_t&	blockAlloc	= Default,
								const GenAllocator_t&	genAlloc	= Default) __NE___;

		~LfFixedBlockAllocator ()					__NE___	{ Release( true ); }
		
		LfFixedBlockAllocator (const Self &) = delete;
		LfFixedBlockAllocator (Self &&) = delete;

		Self& operator = (const Self &) = delete;
		Self& operator = (Self &&) = delete;
		
		NDRST( void *)	Alloc ()					__NE___	{ return Alloc( Default ); }
		NDRST( void *)	Alloc (const SourceLoc &loc)__NE___;
				bool	Dealloc (void *)			__NE___;
	
				void	Release (bool checkMemLeak)	__NE___;

		ND_ Bytes		BlockSize ()				C_NE___	{ return Bytes{ ulong( _blockSize )}; }
		ND_ Bytes		BlockAlign ()				C_NE___	{ return Bytes{ ulong( _blockAlign )}; }
		ND_ POTValue	BlockSizePOT ()				C_NE___	{ return _blockSize; }
		ND_ POTValue	BlockAlignPOT ()			C_NE___	{ return _blockAlign; }

		ND_ Bytes		LargeBlockSize ()			C_NE___	{ return BlockSize() * ChunkSize; }
		ND_ Bytes		MaxMemorySize ()			C_NE___	{ return LargeBlockSize() * MaxChunks; }
		ND_ usize		MaxBlockCount ()			C_NE___	{ return MaxChunks * ChunkSize; }
		ND_ Bytes		AllocatedSize ()			C_NE___;

	private:
		NDRST( void *)	_Alloc (uint chunkIndex, const SourceLoc &loc, INOUT ulong& dbgCounter, INOUT ulong& lockCounter) __NE___;

		ND_ static constexpr Bytes	_DbgInfoSize ()	__NE___	{ return SizeOf<SourceLoc> * ChunkSize; }
		ND_ static constexpr Bytes	_DbgInfoAlign ()__NE___	{ return AlignOf<SourceLoc>; }
	};


	
/*
=================================================
	constructor
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	LfFixedBlockAllocator<CS,MC,BA,GA>::LfFixedBlockAllocator  (Bytes blockSize,
																Bytes blockAlign,
																const BlockAllocator_t&	blockAlloc,
																const GenAllocator_t&	genAlloc) __NE___ :
		_blockSize{ POTValue::From( usize(blockSize) )},
		_blockAlign{ POTValue::From( usize(blockAlign) )},
		_blockAlloc{ blockAlloc },
		_genAlloc{ genAlloc }
	{
		ASSERT( BlockSize() == blockSize );
		ASSERT( BlockAlign() == blockAlign );

		for (usize i = 0; i < MaxChunks; ++i)
		{
			auto&	chunk = _chunkInfo[i];

			for (usize j = 0; j < HiLevel_Count; ++j)
			{
				chunk.lowLevel[j].store( 0 );
			}

			chunk.hiLevel.store( InitialHighLevel );	// set 0 bits for working range, 1 bit for unused bits
			chunk.memBlock.store( null );
			
			#if AE_LFFIXEDBLOCKALLOC_DEBUG
			chunk.dbgInfo.store( null );
			#endif
		}

		ThreadFence( EMemoryOrder::Release );
	}
	
/*
=================================================
	Release
----
	Must be externally synchronized
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	void  LfFixedBlockAllocator<CS,MC,BA,GA>::Release (bool checkMemLeak) __NE___
	{
		ThreadFence( EMemoryOrder::Acquire );
		
		for (usize i = 0; i < MaxChunks; ++i)
		{
			auto&	chunk	= _chunkInfo[i];
			void*	ptr		= chunk.memBlock.exchange( null );

			if ( ptr != null )
				_blockAlloc.Deallocate( ptr, SizeAndAlign{ LargeBlockSize(), BlockAlign() });
			
			EXLOCK( chunk.hiLevelGuard );
			
			HiLevelBits_t	old_hi_level = chunk.hiLevel.exchange( InitialHighLevel );
			
			if ( checkMemLeak )
				CHECK( old_hi_level == InitialHighLevel );	// some blocks is still allocated

			#if AE_LFFIXEDBLOCKALLOC_DEBUG
			SourceLoc*	dbg_info = chunk.dbgInfo.exchange( null );
			#endif
			
			for (usize j = 0; j < HiLevel_Count; ++j)
			{
				LowLevelBits_t	old_low_level = chunk.lowLevel[j].exchange( 0 );
				
				if ( checkMemLeak )
				{
					CHECK( old_low_level == 0 );	// some blocks is still allocated
				
					// dump allocated memory blocks
					#if AE_LFFIXEDBLOCKALLOC_DEBUG
					if ( dbg_info != null )
					{
						int		low_idx = BitScanForward( old_low_level );	// first 1 bit

						for (; low_idx >= 0;)
						{
							auto&	dbg = dbg_info[ low_idx + j * HiLevel_Count ];

							AE_LOGI( "Leaked memory block: "s << ToString( BlockSize() ), dbg.file, dbg.line );

							old_low_level &= (LowLevelBits_t{1} << low_idx);	// 1 -> 0

							low_idx = BitScanForward( old_low_level );	// first 1 bit
						}
					}
					#endif
				}
			}

			#if AE_LFFIXEDBLOCKALLOC_DEBUG
			if ( dbg_info != null )
				_genAlloc.Deallocate( dbg_info, SizeAndAlign{ _DbgInfoSize(), _DbgInfoAlign() });
			#endif
		}

		ulong	atom_cnt	= _dbgCounter.exchange( 0 );
		ulong	lock_cnt	= _dbgLockCounter.exchange( 0 );

		if ( atom_cnt and lock_cnt )
		{
			AE_LOGI( "atomic iteration count: "s << ToString( atom_cnt ) << ",  lock count: " << ToString( lock_cnt ));
		}
	}

/*
=================================================
	Alloc
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	NDRST( void *)  LfFixedBlockAllocator<CS,MC,BA,GA>::Alloc (const SourceLoc &loc) __NE___
	{
		struct Dbg
		{
			ulong	counter	= 0;
			ulong	locks	= 0;
			Self &	ref;

			Dbg (Self& r) : ref{r}
			{}

			~Dbg ()
			{
				ref._dbgCounter.fetch_add( counter );
				ref._dbgLockCounter.fetch_add( locks );
			}
		} dbg{ *this };

		for (uint i = ThreadUtils::GetIntID() & ThreadToChunkMask;
			i < MaxChunks; i += (ThreadToChunkMask >> 1))
		{
			auto&	chunk = _chunkInfo[i];
			
			if ( chunk.hiLevel.load() == UMax )
				continue;

			void*	ptr = _Alloc( i, loc, INOUT dbg.counter, INOUT dbg.locks );

			if_likely( ptr != null )
				return ptr;
		}

		// TODO
		for (uint i = 0; i < MaxChunks; ++i)
		{
			auto&	chunk = _chunkInfo[i];
			
			if ( chunk.hiLevel.load() == UMax )
				continue;

			void*	ptr = _Alloc( i, loc, INOUT dbg.counter, INOUT dbg.locks );

			if_likely( ptr != null )
				return ptr;
		}

		return null;
	}

/*
=================================================
	_Alloc
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	NDRST( void *)  LfFixedBlockAllocator<CS,MC,BA,GA>::_Alloc (const uint chunkIndex, const SourceLoc &loc, INOUT ulong& dbgCounter, INOUT ulong& lockCounter) __NE___
	{
		Unused( loc );

		ChunkInfo&	chunk	= _chunkInfo[ chunkIndex ];
		void *		ptr		= chunk.memBlock.load( EMemoryOrder::Acquire );
		++dbgCounter;
		
		#if AE_LFFIXEDBLOCKALLOC_DEBUG
		SourceLoc*	dbg_info = chunk.dbgInfo.load( EMemoryOrder::Acquire );
		++dbgCounter;
		#endif
		
		// allocate new block
		if_unlikely( ptr == null )
		{
			// memory must be already visible for all threads
			void*	new_block = _blockAlloc.Allocate( SizeAndAlign{ LargeBlockSize(), BlockAlign() });

			if_unlikely( new_block == null )
				RETURN_ERR( "failed to allocate mem block" );
			
			ASSERT( CheckPointerAlignment( new_block, usize(_blockAlign) ));

			++dbgCounter;

			// memory may be allocated in another thread.
			if ( chunk.memBlock.compare_exchange_strong( INOUT ptr, new_block ))
			{
				ASSERT( ptr == null );
				ptr = new_block;
				
				// init allocation debug info
				#if AE_LFFIXEDBLOCKALLOC_DEBUG
				{
					ASSERT( dbg_info == null );

					// memory must be already visible for all threads
					SourceLoc*	new_dbg_info = Cast<SourceLoc>( _genAlloc.Allocate( SizeAndAlign{ _DbgInfoSize(), _DbgInfoAlign() }));

					if ( new_dbg_info != null )
					{
						ZeroMem( new_dbg_info, _DbgInfoSize() );

						SourceLoc*	old_dbg_info = chunk.dbgInfo.exchange( new_dbg_info );
						ASSERT( old_dbg_info == null );

						chunk.dbgInfoEvent.Signal();

						dbg_info = new_dbg_info;
					}
					else
						AE_LOGE( "failed to allocate mem block debug info" );
				}
				#endif
			}
			else
			{
				ASSERT( ptr != null );
				_blockAlloc.Deallocate( new_block, SizeAndAlign{ LargeBlockSize(), BlockAlign() });
				
				// wait for debug info
				#if AE_LFFIXEDBLOCKALLOC_DEBUG
				{
					chunk.dbgInfoEvent.Wait();
					
					dbg_info = chunk.dbgInfo.load();
					ASSERT( dbg_info != null );
				}
				#endif
			}
		}
		
		// find available index in high level
		for (uint j = 0; j < HighWaitCount; ++j, ++dbgCounter)
		{
			HiLevelBits_t	hi_available	= ~chunk.hiLevel.load();			// 1 - unassigned bit
			int				hi_lvl_idx		= BitScanForward( hi_available );	// first 1 bit

			for (; hi_lvl_idx >= 0; ++dbgCounter)
			{
				ASSERT( usize(hi_lvl_idx) < chunk.lowLevel.size() );
				
				// find available index in low level
				auto&			level			= chunk.lowLevel[ hi_lvl_idx ];
				LowLevelBits_t	low_available	= level.load();						// 0 - unassigned bit
				int				low_lvl_idx		= BitScanForward( ~low_available );	// first 0 bit

				for (; low_lvl_idx >= 0; ++dbgCounter)
				{
					const LowLevelBits_t	low_lvl_bit = (LowLevelBits_t{1} << low_lvl_idx);

					if ( level.CAS( INOUT low_available, low_available | low_lvl_bit ))		// 0 -> 1
					{
						// update high level
						if_unlikely( low_available == ~low_lvl_bit )
						{
							EXLOCK( chunk.hiLevelGuard );
							++lockCounter;

							// low level value may be changed at any time so check it inside spinlock
							if ( level.load() == UMax )
							{
								const auto	hi_lvl_bit = (HiLevelBits_t{1} << hi_lvl_idx);
							
								chunk.hiLevel.fetch_or( hi_lvl_bit );	// 0 -> 1
								++dbgCounter;
							}
						}
						
						const uint	idx_in_chunk	= hi_lvl_idx * LowLevel_Count + low_lvl_idx;
						ASSERT( idx_in_chunk < ChunkSize );

						#if AE_LFFIXEDBLOCKALLOC_DEBUG
						if ( dbg_info != null )
						{
							dbg_info[idx_in_chunk] = loc;
							ThreadFence( EMemoryOrder::Release );
						}
						#endif

						void* RST	result = ptr + BlockSize() * idx_in_chunk;

						ASSERT( CheckPointerAlignment( result, usize(_blockAlign) ));
						DEBUG_ONLY( DbgInitMem( result, BlockSize() ));
						return result;
					}

					low_lvl_idx = BitScanForward( ~low_available );	// first 0 bit
					ThreadUtils::Pause();
				}

				hi_available &= ~(HiLevelBits_t{1} << hi_lvl_idx);	// 1 -> 0
				hi_lvl_idx = BitScanForward( hi_available );		// first 1 bit
			}

			ThreadUtils::Pause();
		}

		return null;
	}

/*
=================================================
	Dealloc
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	bool  LfFixedBlockAllocator<CS,MC,BA,GA>::Dealloc (void* ptr) __NE___
	{
		const Bytes	block_size	= LargeBlockSize();
		
		for (uint i = 0; i < MaxChunks; ++i)
		{
			auto&	chunk	= _chunkInfo[i];
			void*	mem		= chunk.memBlock.load();

			if ( ptr < mem or ptr >= mem + block_size )
				continue;
			
			const uint	idx = uint(Bytes{ptr} - Bytes{mem}) / _blockSize;
			ASSERT( idx < ChunkSize );

			const uint		hi_lvl_idx	= idx / LowLevel_Count;
			const uint		low_lvl_idx	= idx % LowLevel_Count;
			LowLevelBits_t	low_bit		= LowLevelBits_t{1} << low_lvl_idx;
			auto&			level		= chunk.lowLevel[ hi_lvl_idx ];
			const uint		idx_in_chunk= hi_lvl_idx * LowLevel_Count + low_lvl_idx;

			ASSERT( ptr == mem + BlockSize() * idx_in_chunk );
			DEBUG_ONLY( DbgFreeMem( ptr, BlockSize() ));

			LowLevelBits_t	old_bits = level.fetch_and( ~low_bit );	// 1 -> 0

			if_unlikely( not (old_bits & low_bit) )
			{
				AE_LOGE( "failed to deallocate memory: block is already deallocated" );
				return false;
			}
				
			// update high level bits
			if_unlikely( old_bits == UMax )
			{
				EXLOCK( chunk.hiLevelGuard );

				// low level value may be changed at any time so check it inside spinlock
				if ( level.load() != UMax )
				{
					const auto	hi_bit	= (HiLevelBits_t{1} << hi_lvl_idx);
				
					chunk.hiLevel.fetch_and( ~hi_bit );	// 1 -> 0
				}
			}
			return true;
		}

		// pointer is not allocated by this allocator
		return false;
	}
	
/*
=================================================
	AllocatedSize
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	Bytes  LfFixedBlockAllocator<CS,MC,BA,GA>::AllocatedSize () C_NE___
	{
		const Bytes	block_size	= LargeBlockSize();
		Bytes		result;

		for (uint i = 0; i < MaxChunks; ++i)
		{
			auto&	chunk	= _chunkInfo[i];

			if ( chunk.memBlock.load() != null )
				result += block_size;
		}
		return result;
	}

} // AE::Threading
