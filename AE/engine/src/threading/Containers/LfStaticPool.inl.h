// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Threading
{
/*
=================================================
	constructor
=================================================
*/
	template <typename V, usize C, typename A>
	LfStaticPool<V,C,A>::LfStaticPool (const Allocator_t &alloc) __NE___ :
		_allocator{ alloc }
	{
		DRC_EXLOCK( _drCheck );

		_arr = Cast<ChunkArray_t>( _allocator.Allocate( SizeAndAlignOf<ChunkArray_t> ));
		CHECK( _arr != null );

		if ( _arr != null )
			PlacementNew<ChunkArray_t>( OUT _arr );

		// flush changes in '_arr'
		MemoryBarrier( EMemoryOrder::Release );
	}

/*
=================================================
	Release
----
	Must be synchronized with Put() and 'Extract()
	Can not use after release!
=================================================
*/
	template <typename V, usize C, typename A>
	template <typename FN>
	void  LfStaticPool<V,C,A>::Release (FN &&fn) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if_unlikely( _arr == null )
			return;

		for (usize i = 0; i < ChunksCount; ++i)
		{
			auto&		chunk		= (*_arr)[i];
			Bitfield_t	available	= chunk.available.exchange( 0 );
			Bitfield_t	assigned	= chunk.assigned.exchange( 0 );

			Unused( assigned );
			ASSERT( assigned == available );

			if ( available == 0 )
				continue;

			int		idx = BitScanForward( available );	// first 1 bit

			for (; idx >= 0;)
			{
				Bitfield_t	mask = Bitfield_t{1} << idx;

				CheckNothrow( IsNoExcept( fn( chunk.values[idx] )));
				fn( INOUT chunk.values[idx] );

				available	&= ~mask;						// 1 -> 0
				idx			= BitScanForward( available );	// first 1 bit
			}
		}

		PlacementDelete( INOUT *_arr );

		_allocator.Deallocate( _arr, SizeAndAlign{ SizeOf<ChunkArray_t>, AlignOf<ChunkArray_t> });
		_arr = null;
	}

/*
=================================================
	Put
=================================================
*/
	template <typename V, usize C, typename A>
	template <typename T>
	bool  LfStaticPool<V,C,A>::Put (T &&value) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		CHECK_ERR( _arr != null );

		const usize		initial_idx = ThreadUtils::GetIntID() & ThreadToChunkMask;

		for (uint a = 0; a < NumAttempts; ++a)
		{
			for (usize i = 0; i < ChunksCount; ++i)
			{
				usize		chunk_idx	= (initial_idx + i * ChunkIdxStep) % ChunksCount;
				auto&		chunk		= (*_arr)[ chunk_idx ];
				Bitfield_t	assigned	= chunk.assigned.load();		// 0 - unassigned
				int			idx			= BitScanForward( ~assigned );	// first 0 bit

				for (; idx >= 0;)
				{
					Bitfield_t	new_bit = Bitfield_t{1} << idx;

					// try to acquire index
					if_likely( chunk.assigned.CAS( INOUT assigned, assigned | new_bit ))	// 0 -> 1
					{
						// invalidate cache to load changes in 'chunk.values[idx]'
						MemoryBarrier( EMemoryOrder::Acquire );

						PlacementNew<Value_t>( OUT &chunk.values[idx], FwdArg<T>(value) );

						// flush cache to make visible changes in 'chunk.values[idx]' for all threads
						// and set availability bit
						Bitfield_t	old_available = chunk.available.fetch_or( new_bit, EMemoryOrder::Release );	// 0 -> 1
						Unused( old_available );
						ASSERT( not (old_available & new_bit) );	// previous bit must be 0

						return true;
					}

					idx = BitScanForward( ~assigned );	// first 0 bit
					ThreadUtils::Pause();
				}
			}

			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	Extract
=================================================
*/
	template <typename V, usize C, typename A>
	bool  LfStaticPool<V,C,A>::Extract (OUT Value_t &outValue) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		CHECK_ERR( _arr != null );

		const usize		initial_idx = ThreadUtils::GetIntID() & ThreadToChunkMask;

		for (uint a = 0; a < NumAttempts; ++a)
		{
			for (usize i = 0; i < ChunksCount; ++i)
			{
				usize		chunk_idx	= (initial_idx + i * ChunkIdxStep) % ChunksCount;
				auto&		chunk		= (*_arr)[ chunk_idx ];
				Bitfield_t	available	= chunk.available.load();		// 1 - assigned
				int			idx			= BitScanForward( available );	// first 1 bit

				for (; idx >= 0;)
				{
					Bitfield_t	mask = ~(Bitfield_t{1} << idx);

					// try to acquire index
					if_likely( chunk.available.CAS( INOUT available, available & mask ))	// 1 -> 0
					{
						// invalidate cache to load changes in 'chunk.values[idx]'
						MemoryBarrier( EMemoryOrder::Acquire );

						outValue = RVRef( chunk.values[idx] );

						PlacementDelete( INOUT chunk.values[idx] );

						// flush cache to make visible changes in 'chunk.values[idx]' for all threads
						// and unset assigned bit
						Bitfield_t	old_assign	= chunk.assigned.fetch_and( mask, EMemoryOrder::Release );	// 1 -> 0
						Unused( old_assign );
						ASSERT( old_assign & ~mask );		// previous bit must be 1

						return true;
					}

					idx = BitScanForward( available );	// first 1 bit
					ThreadUtils::Pause();
				}
			}

			ThreadUtils::Pause();
		}
		return false;
	}


} // AE::Threading
