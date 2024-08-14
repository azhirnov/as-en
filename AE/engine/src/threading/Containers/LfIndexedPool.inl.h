// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Threading
{
/*
=================================================
	constructor
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	LfIndexedPool<V,I,CS,MC,A>::LfIndexedPool (const Allocator_t &alloc) __NE___ :
		_allocator{ alloc }
	{
		EXLOCK( _allocGuard );

		_highLvl = Cast<HighLvlArray_t>( _allocator.Allocate( SizeAndAlignOf<HighLvlArray_t> ));
		CHECK( _highLvl != null );

		if ( _highLvl != null )
			PlacementNew< HighLvlArray_t >( OUT _highLvl );

		// flush cache when unlock mutex
	}

/*
=================================================
	Release
----
	Must be externally synchronized.
	Can not use after release!
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	void  LfIndexedPool<V,I,CS,MC,A>::Release (Bool checkForAssigned) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if_unlikely( _highLvl == null )
			return;

		EXLOCK( _allocGuard );

		for (auto& hi_chunk : *_highLvl)
		{
			LowLvlChunkArray_t*	low_chunks = hi_chunk.chunksPtr.exchange( null );

			if ( low_chunks == null )
				continue;

			if_unlikely( checkForAssigned )
			{
				HighLvlBits_t	hi_bits = hi_chunk.available.exchange( InitialHighLevel );

				CHECK( hi_bits == InitialHighLevel );

				for (auto& low_chunk : *low_chunks)
				{
					LowLvlBits_t	low_bits = low_chunk.assigned.exchange( 0 );
					CHECK( low_bits == 0 );
				}
			}

			PlacementDelete( *low_chunks );

			_allocator.Deallocate( low_chunks, SizeAndAlign{ SizeOf<LowLvlChunkArray_t>, AlignOf<LowLvlChunkArray_t> });
		}

		PlacementDelete( *_highLvl );

		_allocator.Deallocate( _highLvl, SizeAndAlign{ SizeOf<HighLvlArray_t>, AlignOf<HighLvlArray_t> });
		_highLvl = null;

		_highChunkCount.store( 0 );
	}

/*
=================================================
	UnassignAll
----
	Must be externally synchronized.
	User must invalidate cache before accessing the elements.
	Visit each assigned element and then unassign.
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	template <typename FN>
	void  LfIndexedPool<V,I,CS,MC,A>::UnassignAll (FN &&visitor) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if_unlikely( _highLvl == null )
			return;

		for (auto& hi_chunk : *_highLvl)
		{
			LowLvlChunkArray_t*	low_chunks = hi_chunk.chunksPtr.load();

			if ( low_chunks == null )
				continue;

			for (auto& low_chunk : *low_chunks)
			{
				LowLvlBits_t	low_bits	= low_chunk.assigned.exchange( 0 );
				int				idx			= BitScanForward( low_bits );		// first 1 bit

				for (; idx >= 0;)
				{
					LowLvlBits_t	bit = LowLvlBits_t{1} << idx;

					CheckNothrow( IsNoExcept( visitor( low_chunk.values[idx] )));
					visitor( INOUT low_chunk.values[idx] );

					low_bits &= ~bit;						// 1 -> 0
					idx		 = BitScanForward( low_bits );	// first 1 bit
				}
			}

			hi_chunk.available.store( InitialHighLevel );	// set 0 bit for working range, 1 bit for unused bits
		}
	}

/*
=================================================
	ForEachAssigned
----
	Must be externally synchronized.
	User must invalidate cache before accessing the elements.
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	template <typename FN>
	void  LfIndexedPool<V,I,CS,MC,A>::ForEachAssigned (FN &&fn) C_NE___
	{
		DRC_EXLOCK( _drCheck );

		if_unlikely( _highLvl == null )
			return;

		for (auto& hi_chunk : *_highLvl)
		{
			LowLvlChunkArray_t*	low_chunks = hi_chunk.chunksPtr.load();

			if ( low_chunks == null )
				continue;

			for (auto& low_chunk : *low_chunks)
			{
				LowLvlBits_t	low_bits	= low_chunk.assigned.load();
				int				idx			= BitScanForward( low_bits );		// first 1 bit

				for (; idx >= 0;)
				{
					LowLvlBits_t	bit = LowLvlBits_t{1} << idx;

					CheckNothrow( IsNoExcept( fn( low_chunk.values[idx] )));
					fn( low_chunk.values[idx] );

					low_bits &= ~bit;						// 1 -> 0
					idx		 = BitScanForward( low_bits );	// first 1 bit
				}
			}
		}
	}

/*
=================================================
	Assign
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	bool  LfIndexedPool<V,I,CS,MC,A>::Assign (OUT Index_t &outIndex) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		outIndex = UMax;

		if_unlikely( _highLvl == null )
			return false;

		const usize	hi_lvl_count = _highChunkCount.load();

		for (usize i = 0; i < hi_lvl_count; ++i)
		{
			HighLevelChunk&		high_chunk = (*_highLvl)[i];

			if ( _AssignInChunk( high_chunk, OUT outIndex ))
			{
				outIndex = CheckCast<Index_t>( outIndex + i * ChunkSize );
				return true;
			}
		}

		// allocate new chunk
		for (usize i = hi_lvl_count; i < MaxChunks; ++i)
		{
			HighLevelChunk&		high_chunk	= (*_highLvl)[i];
			LowLvlChunkArray_t*	low_chunks	= high_chunk.chunksPtr.load();

			if_likely( low_chunks == null )
			{
				// only one thread will allocate new chunk
				EXLOCK( _allocGuard );

				low_chunks = high_chunk.chunksPtr.load();

				// another thread may allocate this chunk
				if ( low_chunks == null )
				{
					low_chunks = Cast<LowLvlChunkArray_t>( _allocator.Allocate( SizeAndAlignOf<LowLvlChunkArray_t> ));
					CHECK_ERR( low_chunks != null );

					PlacementNew< LowLvlChunkArray_t >( OUT low_chunks );

					CHECK( high_chunk.chunksPtr.exchange( low_chunks ) == null );

					ASSERT( _highChunkCount.load() <= i );
					_highChunkCount.store( uint(i + 1) );
				}
			}

			if_likely( _AssignInChunk( high_chunk, OUT outIndex ))
			{
				outIndex = CheckCast<Index_t>( outIndex + i * ChunkSize );
				return true;
			}
		}

		// pool overflow
		return false;
	}

/*
=================================================
	_AssignInChunk
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	bool  LfIndexedPool<V,I,CS,MC,A>::_AssignInChunk (HighLevelChunk& highChunk, OUT Index_t &outIndex) __NE___
	{
		LowLvlChunkArray_t*	low_chunks		= highChunk.chunksPtr.load();
		HighLvlBits_t		hi_available	= ~highChunk.available.load();		// 1 - unassigned
		int					chunk_idx		= BitScanForward( hi_available );	// first 1 bit

		// TODO: offset 'chunk_idx' by thread id

		NonNull( low_chunks );

		for (; chunk_idx >= 0;)
		{
			LowLevelChunk&	low_chunk		= (*low_chunks)[ chunk_idx ];
			LowLvlBits_t	low_available	= low_chunk.assigned.load();		// 0 - unassigned
			int				idx				= BitScanForward( ~low_available );	// first 0 bit

			for (; idx >= 0;)
			{
				LowLvlBits_t	new_bit	= LowLvlBits_t{1} << idx;

				// try to acquire index
				if_likely( low_chunk.assigned.CAS( INOUT low_available, low_available | new_bit ))	// 0 -> 1
				{
					// update high level bits
					if_unlikely( low_available == ~new_bit )
					{
						EXLOCK( highChunk.availableGuard );

						// low level value may be changed at any time so check it inside spinlock
						if ( low_chunk.assigned.load() == UMax )
							highChunk.available.fetch_or( HighLvlBits_t{1} << chunk_idx );	// 0 -> 1
					}

					outIndex = CheckCast<Index_t>( chunk_idx * LowLvlCount + idx );
					return true;
				}

				idx = BitScanForward( ~low_available );	// first 0 bit
				ThreadUtils::Pause();
			}

			hi_available &= ~(HighLvlBits_t{1} << chunk_idx);	// 1 -> 0
			chunk_idx	 = BitScanForward( hi_available );		// first 1 bit
		}

		return false;
	}

/*
=================================================
	Unassign
----
	Must be externally synchronized with all threads that using 'index'
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	bool  LfIndexedPool<V,I,CS,MC,A>::Unassign (Index_t index) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		if_unlikely( _highLvl == null or index >= capacity() )
			return false;

		HighLevelChunk&		high_chunk	= (*_highLvl)[ index / ChunkSize ];
		LowLvlChunkArray_t*	low_chunks	= high_chunk.chunksPtr.load();

		if_unlikely( low_chunks == null )
			return false;

		const auto		index1		= index % ChunkSize;
		const auto		chunk_idx	= index1 / LowLvlCount;
		const auto		low_idx		= index1 % LowLvlCount;
		LowLevelChunk&	low_chunk	= (*low_chunks)[ chunk_idx ];
		LowLvlBits_t	mask		= LowLvlBits_t{1} << low_idx;
		LowLvlBits_t	old_bits	= low_chunk.assigned.fetch_and( ~mask );	// 1 -> 0

		if_unlikely( not (old_bits & mask) )
			return false;	// was not assigned

		// update high level bits
		if_unlikely( old_bits == UMax )
		{
			EXLOCK( high_chunk.availableGuard );

			// low level may be changed at any time so check it inside spinlock
			if_unlikely( low_chunk.assigned.load() != UMax )
				high_chunk.available.fetch_and( ~(HighLvlBits_t{1} << chunk_idx) );	// 1 -> 0
		}

		return true;
	}

/*
=================================================
	IsAssigned
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	bool  LfIndexedPool<V,I,CS,MC,A>::IsAssigned (Index_t index) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		if_unlikely( _highLvl == null or index >= capacity() )
			return false;

		HighLevelChunk&		high_chunk	= (*_highLvl)[ index / ChunkSize ];
		LowLvlChunkArray_t*	low_chunks	= high_chunk.chunksPtr.load();

		if_unlikely( low_chunks == null )
			return false;

		const auto		index1		= index % ChunkSize;
		const auto		chunk_idx	= index1 / LowLvlCount;
		const auto		low_idx		= index1 % LowLvlCount;
		LowLevelChunk&	low_chunk	= (*low_chunks)[ chunk_idx ];
		LowLvlBits_t	mask		= LowLvlBits_t{1} << low_idx;
		LowLvlBits_t	low_bits	= low_chunk.assigned.load();

		return (low_bits & mask);
	}

/*
=================================================
	operator []
----
	Read access by the same index is safe,
	but write access must be externally synchronized.
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	typename LfIndexedPool<V,I,CS,MC,A>::Value_t&
		LfIndexedPool<V,I,CS,MC,A>::operator [] (Index_t index) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		ASSERT( index < capacity() );
		NonNull( _highLvl );

		HighLevelChunk&		high_chunk	= (*_highLvl)[ index / ChunkSize ];
		LowLvlChunkArray_t*	low_chunks	= high_chunk.chunksPtr.load();

		NonNull( low_chunks );

		const auto		index1		= index % ChunkSize;
		const auto		chunk_idx	= index1 / LowLvlCount;
		const auto		low_idx		= index1 % LowLvlCount;
		LowLevelChunk&	low_chunk	= (*low_chunks)[ chunk_idx ];

		DBG_CHECK_MSG( HasBit( low_chunk.assigned.load(), low_idx ), "not assigned" );

		return low_chunk.values[ low_idx ];
	}

/*
=================================================
	At
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	typename LfIndexedPool<V,I,CS,MC,A>::Value_t*
		LfIndexedPool<V,I,CS,MC,A>::At (Index_t index) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		if_likely( (index < capacity()) and (_highLvl != null) )
		{
			HighLevelChunk&		high_chunk	= (*_highLvl)[ index / ChunkSize ];
			LowLvlChunkArray_t*	low_chunks	= high_chunk.chunksPtr.load();

			if_likely( low_chunks != null )
			{
				const auto		index1		= index % ChunkSize;
				const auto		chunk_idx	= index1 / LowLvlCount;
				const auto		low_idx		= index1 % LowLvlCount;
				LowLevelChunk&	low_chunk	= (*low_chunks)[ chunk_idx ];

				DBG_CHECK_MSG( HasBit( low_chunk.assigned.load(), low_idx ), "not assigned" );		// TODO: return null ?

				return &low_chunk.values[ low_idx ];
			}
		}
		return null;
	}

/*
=================================================
	DynamicSize
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	Bytes  LfIndexedPool<V,I,CS,MC,A>::DynamicSize () C_NE___
	{
		Bytes	result;
		if ( _highLvl != null )
		{
			result += SizeOf<HighLvlArray_t>;
			result += _highChunkCount.load() * SizeOf<LowLvlChunkArray_t>;
		}
		return result;
	}

/*
=================================================
	IndexOf
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	typename LfIndexedPool<V,I,CS,MC,A>::Index_t  LfIndexedPool<V,I,CS,MC,A>::IndexOf (const void* ptr) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		if_unlikely( _highLvl == null )
			return UMax;

		const uint	count = _highChunkCount.load();

		for (uint i = 0; i < count; ++i)
		{
			LowLvlChunkArray_t*		chunk_arr = (*_highLvl)[i].chunksPtr.load();

			if_unlikely( chunk_arr != null and
						 IsIntersects<const void*>( ptr, ptr, chunk_arr, chunk_arr+1 ))
			{
				usize	a			= (usize(ptr) - usize(chunk_arr->data())) / sizeof(LowLevelChunk);
				auto&	ll_chunk	= (*chunk_arr)[a];
				usize	b			= (usize(ptr) - usize(ll_chunk.values.data())) / sizeof(Value_t);

				return CheckCast<Index_t>( b + (a * LowLvlCount) + (i * ChunkSize) );
			}
		}

		// out of bounds
		return UMax;
	}

/*
=================================================
	Unassign
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	bool  LfIndexedPool<V,I,CS,MC,A>::Unassign (const void* ptr) __NE___
	{
		if_likely( Index_t  idx = IndexOf( ptr );  idx != UMax )
			return Unassign( idx );

		return false;
	}


} // AE::Threading
