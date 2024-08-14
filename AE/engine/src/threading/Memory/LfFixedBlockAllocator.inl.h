// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Threading
{

/*
=================================================
	constructor
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	LfFixedBlockAllocator<CS,MC,BA,GA>::LfFixedBlockAllocator (Bytes					blockSize,
															   Bytes					blockAlign,
															   const BlockAllocator_t&	blockAlloc,
															   const GenAllocator_t&	genAlloc) __NE___ :
		_blockSize{ blockSize },
		_blockAlign{ blockAlign },
		_blockAlloc{ blockAlloc },
		_genAlloc{ genAlloc }
	{
		DRC_EXLOCK( _drCheck );

		ASSERT( blockSize > 0 );
		ASSERT_Eq( BlockSize(), blockSize );
		ASSERT_Eq( BlockAlign(), blockAlign );

		StaticArray< Bitfield<TopLevelBits_t>, TopLevel_Count >		top_bits = {};

		for (usize i = 0; i < _bottomChunks.size(); ++i)
		{
			auto&	chunk	= _bottomChunks[i];
			auto&	top		= top_bits[ i / CT_SizeOfInBits<TopLevelBits_t> ];

			for (usize j = 0; j < HiLevel_Count; ++j)
			{
				chunk.lowLevel[j].Store( Default );
			}

			chunk.hiLevel.Store( InitialHighLevel );	// set 0 bit for working range, 1 bit for unused bits
			chunk.memBlock.store( null );

			top.Set( i % CT_SizeOfInBits<TopLevelBits_t> );

		  #if AE_LFFIXEDBLOCKALLOC_DEBUG
			chunk.dbgInfo.store( null );
		  #endif
		}

		for (usize i = 0; i < _topChunks.size(); ++i)
		{
			_topChunks[i].assigned.Store( ~top_bits[i] );	// set 0 bit for working range, 1 bit for unused bits
		}
	}

/*
=================================================
	Release
----
	Must be externally synchronized
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	void  LfFixedBlockAllocator<CS,MC,BA,GA>::Release (Bool checkMemLeak) __NE___
	{
		DRC_EXLOCK( _drCheck );

		for (usize i = 0; i < MaxChunks; ++i)
		{
			auto&	chunk		= _bottomChunks[i];
			void*	ptr			= chunk.memBlock.exchange( null );
			auto&	top_chunk	= _topChunks[ i / CT_SizeOfInBits<TopLevelBits_t> ];

			if ( ptr != null )
				_blockAlloc.Deallocate( ptr, SizeAndAlign{ LargeBlockSize(), BlockAlign() });

			EXLOCK( chunk.hiLevelGuard );

			Bitfield<HiLevelBits_t>		old_hi_level = chunk.hiLevel.Exchange( InitialHighLevel );

			if ( checkMemLeak )
			{
				CHECK( old_hi_level == InitialHighLevel );	// some blocks is still allocated

				auto&	top_bits = top_chunk.assigned;
				CHECK( not top_bits.Has( i % CT_SizeOfInBits<TopLevelBits_t> ));
			}

		  #if AE_LFFIXEDBLOCKALLOC_DEBUG
			SourceLoc*	dbg_info = chunk.dbgInfo.exchange( null );
		  #endif

			for (usize j = 0; j < HiLevel_Count; ++j)
			{
				Bitfield<LowLevelBits_t>	old_low_level = chunk.lowLevel[j].Exchange( Default );

				if ( checkMemLeak )
				{
					CHECK( old_low_level.None() );	// some blocks is still allocated

					// dump allocated memory blocks
				  #if AE_LFFIXEDBLOCKALLOC_DEBUG
					if ( dbg_info != null )
					{
						int		low_idx = old_low_level.ExtractBitIndex();

						for (; low_idx >= 0;)
						{
							auto&	dbg = dbg_info[ low_idx + j * HiLevel_Count ];

							AE_LOGI( "Leaked memory block: "s << ToString( BlockSize() ), dbg.file, dbg.line );

							low_idx = old_low_level.ExtractBitIndex();
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

	  #if AE_LFFIXEDBLOCKALLOC_DEBUG
		ulong	atom_cnt	= _dbgCounter.exchange( 0 );
		ulong	lock_cnt	= _dbgLockCounter.exchange( 0 );

		if ( atom_cnt and lock_cnt )
			AE_LOGI( "atomic iteration count: "s << ToString( atom_cnt ) << ",  lock count: " << ToString( lock_cnt ));
	  #endif
	}

/*
=================================================
	AllocBlock
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	typename LfFixedBlockAllocator<CS,MC,BA,GA>::Ptr_t
		LfFixedBlockAllocator<CS,MC,BA,GA>::AllocBlock (const SourceLoc &loc) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		struct Dbg
		{
			ulong	counter	= 0;
			ulong	locks	= 0;
			Self &	ref;

			Dbg (Self& r) : ref{r}
			{}

			~Dbg ()
			{
			  #if AE_LFFIXEDBLOCKALLOC_DEBUG
				ref._dbgCounter.fetch_add( counter );
				ref._dbgLockCounter.fetch_add( locks );
			  #endif
			}
		} dbg{ *this };

		for (usize i = 0; i < _topChunks.size(); ++i, ++dbg.counter)
		{
			auto&	top_chunk	= _topChunks[i];
			auto	available	= ~top_chunk.assigned.Load();	// 1 - unassigned
			int		idx			= available.ExtractBitIndex();

			for (; idx >= 0; ++dbg.counter)
			{
				Ptr_t	ptr = _Alloc( uint(idx + i * CT_SizeOfInBits<TopLevelBits_t>), loc, INOUT dbg.counter, INOUT dbg.locks );

				if_likely( ptr != null )
					return ptr;

				idx = available.ExtractBitIndex();
			}
		}

		return null;
	}

/*
=================================================
	_Alloc
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	typename LfFixedBlockAllocator<CS,MC,BA,GA>::Ptr_t
		LfFixedBlockAllocator<CS,MC,BA,GA>::_Alloc (const uint chunkIndex, const SourceLoc &loc, INOUT ulong& dbgCounter, INOUT ulong& lockCounter) __NE___
	{
		BottomChunk&	chunk	= _bottomChunks[ chunkIndex ];
		auto*			ptr		= chunk.memBlock.load( EMemoryOrder::Acquire );

	  #if AE_LFFIXEDBLOCKALLOC_DEBUG
		SourceLoc*	dbg_info = chunk.dbgInfo.load( EMemoryOrder::Acquire );
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
			if ( chunk.memBlock.CAS_Loop( INOUT ptr, new_block ))
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
						ZeroMem( OUT new_dbg_info, _DbgInfoSize() );

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
				// TODO: memleak in linear allocator
				NonNull( ptr );
				_blockAlloc.Deallocate( new_block, SizeAndAlign{ LargeBlockSize(), BlockAlign() });

				// wait for debug info
				#if AE_LFFIXEDBLOCKALLOC_DEBUG
				{
					chunk.dbgInfoEvent.Wait();

					dbg_info = chunk.dbgInfo.load();
					NonNull( dbg_info );
				}
				#endif
			}
		}

		// find available index in high level
		for (uint j = 0; j < HighWaitCount; ++j, ++dbgCounter)
		{
			auto	hi_available	= ~chunk.hiLevel.Load();			// 1 - unassigned
			int		hi_lvl_idx		= hi_available.ExtractBitIndex();

			for (; hi_lvl_idx >= 0; ++dbgCounter)
			{
				ASSERT( usize(hi_lvl_idx) < chunk.lowLevel.size() );

				// find available index in low level
				auto&	level			= chunk.lowLevel[ hi_lvl_idx ];
				auto	low_available	= level.Load();							// 0 - unassigned
				int		low_lvl_idx		= low_available.GetFirstZeroBitIndex();	// first 0 bit

				for (; low_lvl_idx >= 0; ++dbgCounter)
				{
					const auto	low_lvl_bit = Bitfield<LowLevelBits_t>{}.Set( low_lvl_idx );

					if_likely( level.CAS( INOUT low_available, low_available | low_lvl_bit ))	// 0 -> 1
					{
						// update high level
						if_unlikely( low_available == ~low_lvl_bit )
						{
							bool	update_top	= false;
							{
								EXLOCK( chunk.hiLevelGuard );
								++lockCounter;

								// low level value may be changed at any time so check it inside spinlock
								if ( level.All() )
								{
									update_top = (chunk.hiLevel.Set( hi_lvl_idx ) == MaxHighLevel);
									++dbgCounter;
								}
							}

							if_unlikely( update_top )
							{
								auto&	top_chunk = _topChunks[ chunkIndex / CT_SizeOfInBits<TopLevelBits_t> ];

								EXLOCK( top_chunk.assignedGuard );
								++lockCounter;

								if ( chunk.hiLevel.Load() == MaxHighLevel )
								{
									top_chunk.assigned.Set( chunkIndex % CT_SizeOfInBits<TopLevelBits_t> );	// 0 -> 1
									++dbgCounter;
								}
							}
						}

						const uint	idx_in_chunk	= hi_lvl_idx * LowLevel_Count + low_lvl_idx;
						ASSERT( idx_in_chunk < ChunkSize );

					  #if AE_LFFIXEDBLOCKALLOC_DEBUG
						if ( dbg_info != null )
						{
							dbg_info[idx_in_chunk] = loc;
							MemoryBarrier( EMemoryOrder::Release );
						}
					  #endif
						Unused( loc );

						void*	result = ptr + BlockSize() * idx_in_chunk;

						ASSERT( CheckPointerAlignment( result, usize(_blockAlign) ));
						DEBUG_ONLY( DbgInitMem( result, BlockSize() ));
						return Ptr_t{result};
					}

					low_lvl_idx = low_available.GetFirstZeroBitIndex();	// first 0 bit
					ThreadUtils::Pause();
				}

				hi_lvl_idx = hi_available.ExtractBitIndex();
			}

			ThreadUtils::Pause();
		}

		return null;
	}

/*
=================================================
	DeallocBlock
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	bool  LfFixedBlockAllocator<CS,MC,BA,GA>::DeallocBlock (void* ptr) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		const Bytes	block_size	= LargeBlockSize();

		for (uint i = 0; i < MaxChunks; ++i)
		{
			auto&	chunk	= _bottomChunks[i];
			void*	mem		= chunk.memBlock.load();

			if ( ptr < mem or ptr >= mem + block_size )
				continue;

			const uint	idx = uint((Bytes{ptr} - Bytes{mem}) / _blockSize);
			ASSERT( idx < ChunkSize );

			const uint		hi_lvl_idx	= idx / LowLevel_Count;
			const uint		low_lvl_idx	= idx % LowLevel_Count;
			auto&			level		= chunk.lowLevel[ hi_lvl_idx ];
			const uint		idx_in_chunk= hi_lvl_idx * LowLevel_Count + low_lvl_idx;

			ASSERT( ptr == mem + BlockSize() * idx_in_chunk );	Unused( idx_in_chunk );
			DEBUG_ONLY( DbgFreeMem( ptr, BlockSize() ));

			const auto		old_bits	= level.Erase( low_lvl_idx );	// 1 -> 0

			if_unlikely( not old_bits.Has( low_lvl_idx ))
			{
				AE_LOGE( "failed to deallocate memory: block is already deallocated" );
				return false;
			}

			// update high level bits
			if_unlikely( old_bits.All() )
			{
				bool	update_top	= false;
				{
					EXLOCK( chunk.hiLevelGuard );

					// low level value may be changed at any time so check it inside spinlock
					if ( not level.All() )
					{
						update_top = (chunk.hiLevel.Erase( hi_lvl_idx ) == MaxHighLevel);
					}
				}

				if_unlikely( update_top )
				{
					auto&	top_chunk = _topChunks[ i / CT_SizeOfInBits<TopLevelBits_t> ];

					EXLOCK( top_chunk.assignedGuard );

					if ( chunk.hiLevel.Load() != MaxHighLevel )
					{
						top_chunk.assigned.Erase( i % CT_SizeOfInBits<TopLevelBits_t> );	// 1 -> 0
					}
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
		DRC_SHAREDLOCK( _drCheck );

		const Bytes	block_size	= LargeBlockSize();
		Bytes		result;

		for (uint i = 0; i < MaxChunks; ++i)
		{
			auto&	chunk	= _bottomChunks[i];

			if ( chunk.memBlock.load() != null )
				result += block_size;
		}
		return result;
	}

/*
=================================================
	Allocate
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	void*  LfFixedBlockAllocator<CS,MC,BA,GA>::Allocate (const SizeAndAlign sizeAndAlign) __NE___
	{
		CHECK_ERR(	sizeAndAlign.size  <= BlockSize() and
					sizeAndAlign.align <= BlockAlign() );
		return AllocBlock();
	}

/*
=================================================
	Deallocate
=================================================
*/
	template <usize CS, usize MC, typename BA, typename GA>
	void  LfFixedBlockAllocator<CS,MC,BA,GA>::Deallocate (void* ptr, const SizeAndAlign sizeAndAlign) __NE___
	{
		ASSERT(	sizeAndAlign.size  <= BlockSize() and
				sizeAndAlign.align <= BlockAlign() );
		Unused( sizeAndAlign );
		CHECK( DeallocBlock( ptr ));
	}


} // AE::Threading
