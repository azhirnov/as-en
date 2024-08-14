// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Threading
{
/*
=================================================
	constructor
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	inline LfIndexedPool2<V,I,CS,MC,A>::LfIndexedPool2 (const Allocator_t &alloc) __NE___ :
		_allocator{ alloc }
	{
		_topLevel.store( InitialTopLevel );

		for (usize i = 0; i < MaxChunks; ++i)
		{
			auto&	info = _chunkInfo[i];

			for (usize j = 0; j < HiLevel_Count; ++j)
			{
				info.lowLevel[j].store( 0 );
				info.created[j].store( 0 );
			}

			info.hiLevel.store( InitialHighLevel );

			_chunkData[i].store( null );
		}

		MemoryBarrier( EMemoryOrder::Release );
	}

/*
=================================================
	Release
----
	Must be externally synchronized
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	template <typename FN>
	inline void  LfIndexedPool2<V,I,CS,MC,A>::Release (FN &&dtor, Bool checkForAssigned) __NE___
	{
		EXLOCK( _topLevelGuard );
		MemoryBarrier( EMemoryOrder::Acquire );

		TopLevelBits_t	old_top_level = _topLevel.exchange( InitialTopLevel );

		if_unlikely( checkForAssigned )
			CHECK( old_top_level == InitialTopLevel );	// some items is still assigned

		for (usize i = 0; i < MaxChunks; ++i)
		{
			ValueChunk_t*	data = _chunkData[i].exchange( null );
			auto&			info = _chunkInfo[i];

			if ( not data )
				continue;

			EXLOCK( info.hiLevelGuard );	// TODO: may be deadlock

			HiLevelBits_t	old_hi_level = info.hiLevel.exchange( InitialHighLevel );

			if_unlikely( checkForAssigned )
				CHECK( old_hi_level == InitialHighLevel );	// some items is still assigned

			for (usize j = 0; j < HiLevel_Count; ++j)
			{
				LowLevelBits_t	old_low_level = info.lowLevel[j].exchange( 0 );

				if_unlikely( checkForAssigned )
					CHECK( old_low_level == 0 );	// some items is still assigned

				LowLevelBits_t	ctor_bits = info.created[j].exchange( 0 );

				// call destructor
				for (usize c = 0; c < LowLevel_Count; ++c)
				{
					if ( ctor_bits & (LowLevelBits_t{1} << c) )
						dtor( (*data)[ c + j*LowLevel_Count ] );
				}
			}

			_allocator.Deallocate( data, SizeAndAlign{ SizeOf<ValueChunk_t>, AlignOf<ValueChunk_t> });
		}

		MemoryBarrier( EMemoryOrder::Release );
	}

/*
=================================================
	Assign
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	template <typename FN>
	inline bool  LfIndexedPool2<V,I,CS,MC,A>::Assign (OUT Index_t &outIndex, FN &&ctor) __NE___
	{
		for (uint j = 0; j < TopWaitCount; ++j)
		{
			TopLevelBits_t	available	= ~_topLevel.load();			// 1 - unassigned
			int				idx			= BitScanForward( available );	// first 1 bit

			for (uint i = 0; idx >= 0 and i < TopWaitCount; ++i)
			{
				ASSERT( usize(idx) < _chunkInfo.size() );

				if ( _AssignInChunk( OUT outIndex, idx, ctor ))
					return true;

				available	&= ~(TopLevelBits_t{1} << idx);		// 1 -> 0
				idx			= BitScanForward( available );		// first 1 bit
			}

			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	_AssignInChunk
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	template <typename FN>
	inline bool  LfIndexedPool2<V,I,CS,MC,A>::_AssignInChunk (OUT Index_t &outIndex, int chunkIndex, const FN &ctor) __NE___
	{
		auto&			info = _chunkInfo[ chunkIndex ];
		ValueChunk_t*	data = _chunkData[ chunkIndex ].load();

		// allocate
		if_unlikely( data == null )
		{
			// TODO: use PtrWithSpinLock to avoid deallocation?
			data = Cast<ValueChunk_t>(_allocator.Allocate( SizeAndAlignOf<ValueChunk_t> ));
			CHECK_ERR( data != null );

			// set new pointer and invalidate cache
			for (ValueChunk_t* expected = null;
				 not _chunkData[ chunkIndex ].CAS( INOUT expected, data, EMemoryOrder::Acquire, EMemoryOrder::Acquire );)
			{
				// another thread has been allocated this chunk
				if_unlikely( expected != null and expected != data )
				{
					// TODO: memleak if used linear allocator
					_allocator.Deallocate( data, SizeAndAlignOf<ValueChunk_t> );
					data = expected;
					break;
				}

				ThreadUtils::Pause();
			}
		}

		// find available index
		for (uint j = 0; j < HighWaitCount; ++j)
		{
			HiLevelBits_t	available	= ~info.hiLevel.load();			// 1 - unassigned
			int				idx			= BitScanForward( available );	// first 1 bit

			for (; idx >= 0;)
			{
				ASSERT( usize(idx) < info.lowLevel.size() );

				if ( _AssignInLowLevel( OUT outIndex, chunkIndex, idx, *data, ctor ))
					return true;

				available	&= ~(HiLevelBits_t{1} << idx);	// 1 -> 0
				idx			= BitScanForward( available );	// first 1 bit
			}

			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	_AssignInLowLevel
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	template <typename FN>
	inline bool  LfIndexedPool2<V,I,CS,MC,A>::_AssignInLowLevel (OUT Index_t &outIndex, int chunkIndex, int hiLevelIndex, ValueChunk_t &data, const FN &ctor) __NE___
	{
		auto&			info		= _chunkInfo[ chunkIndex ];
		auto&			level		= info.lowLevel[ hiLevelIndex ];
		auto&			created		= info.created[ hiLevelIndex ];
		LowLevelBits_t	available	= level.load();						// 0 - unassigned
		int				idx			= BitScanForward( ~available );		// first 0 bit

		for (; idx >= 0;)
		{
			const LowLevelBits_t	mask = (LowLevelBits_t{1} << idx);

			if ( level.CAS( INOUT available, available | mask, EMemoryOrder::Acquire, EMemoryOrder::Relaxed ))	// 0 -> 1
			{
				if_unlikely( available == ~mask )
					_UpdateHiLevel( chunkIndex, hiLevelIndex );

				outIndex = Index_t(idx) | (Index_t(hiLevelIndex) * LowLevel_Count) | (Index_t(chunkIndex) * ChunkSize);

				// if element is not created
				if ( (created.load() & mask) == 0 )
				{
					ctor( &data[ idx + hiLevelIndex * LowLevel_Count ], outIndex );

					CHECK( (created.fetch_or( mask ) & mask) == 0 );	// 0 -> 1
				}
				return true;
			}

			idx = BitScanForward( ~available );		// first 0 bit
			ThreadUtils::Pause();
		}
		return false;
	}

/*
=================================================
	_UpdateHiLevel
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	inline void  LfIndexedPool2<V,I,CS,MC,A>::_UpdateHiLevel (int chunkIndex, int hiLevelIndex) __NE___
	{
		auto&	info	= _chunkInfo[ chunkIndex ];
		auto&	level	= info.lowLevel[ hiLevelIndex ];

		// update high level
		EXLOCK( info.hiLevelGuard );

		// low level may be changed at any time so check it inside spinlock
		if_unlikely( level.load() == UMax )
		{
			const auto		hi_bit	= (HiLevelBits_t{1} << hiLevelIndex);
			constexpr auto	hi_max	= MaxHighLevel | InitialHighLevel;

			EXLOCK( _topLevelGuard );

			// update top level
			if_unlikely( info.hiLevel.Or( hi_bit ) == hi_max )	// 0 -> 1
			{
				_topLevel.fetch_or( TopLevelBits_t{1} << chunkIndex );	// 0 -> 1

				ASSERT( info.hiLevel.load() >= MaxHighLevel );
			}
		}
	}

/*
=================================================
	AssignAt
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	template <typename FN>
	inline bool  LfIndexedPool2<V,I,CS,MC,A>::AssignAt (Index_t index, OUT Value_t* &outValue, FN &&ctor) __NE___
	{
		if_unlikely( index >= capacity() )
			return false;

		const uint		chunk_idx	= index / ChunkSize;
		const uint		arr_idx		= index % ChunkSize;
		const uint		hi_lvl_idx	= arr_idx / LowLevel_Count;
		const uint		low_lvl_idx	= arr_idx % LowLevel_Count;
		LowLevelBits_t	mask		= LowLevelBits_t{1} << low_lvl_idx;
		auto&			info		= _chunkInfo[ chunk_idx ];
		auto&			created		= info.created[ hi_lvl_idx ];
		auto&			level		= info.lowLevel[ hi_lvl_idx ];
		ValueChunk_t*	data		= _chunkData[ chunk_idx ].load();

		// allocate
		if_unlikely( data == null )
		{
			data = Cast<ValueChunk_t>(_allocator.Allocate( SizeAndAlignOf<ValueChunk_t> ));
			CHECK_ERR( data != null );

			// set new pointer and invalidate cache
			for (ValueChunk_t* expected = null;
				 not _chunkData[ chunk_idx ].CAS( INOUT expected, data, EMemoryOrder::Acquire, EMemoryOrder::Acquire );)
			{
				// another thread already allocated this chunk
				if_unlikely( expected != null and expected != data )
				{
					// TODO: memleak if used linear allocator
					_allocator.Deallocate( data, SizeAndAlignOf<ValueChunk_t> );
					data = expected;
					break;
				}

				ThreadUtils::Pause();
			}
		}

		LowLevelBits_t	old_bits = level.fetch_or( mask );	// 0 -> 1

		if ( old_bits & mask )
		{
			// another thread already assigned this bit.
			// wait until the value is initialized.
			for (uint i = 0; (created.load() & mask) != mask; ++i)
			{
				if_unlikely( i > ThreadUtils::SpinBeforeLock() )
				{
					i = 0;
					ThreadUtils::Sleep_500us();
				}
				ThreadUtils::Pause();
			}

			// invalidate cache to make visible all changes on value
			MemoryBarrier( EMemoryOrder::Acquire );
		}
		else
		{
			// if assigned
			if_unlikely( old_bits == ~mask )
				_UpdateHiLevel( chunk_idx, hi_lvl_idx );

			if ( not (created.fetch_or( mask ) & mask) )	// 0 -> 1
			{
				ctor( &data[arr_idx], index );

				// other threads may access by this index without sync, only with cache invalidation
				MemoryBarrier( EMemoryOrder::Release );
			}
			else
			{
				// already created.
				// invalidate cache to make visible all changes on value.
				MemoryBarrier( EMemoryOrder::Acquire );
			}
		}

		outValue = &data[arr_idx];
		return true;
	}

/*
=================================================
	Unassign
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	inline bool  LfIndexedPool2<V,I,CS,MC,A>::Unassign (Index_t index) __NE___
	{
		if_unlikely( index >= capacity() )
			return false;

		const uint				chunk_idx	= index / ChunkSize;
		const uint				hi_lvl_idx	= (index % ChunkSize) / LowLevel_Count;
		const uint				low_lvl_idx	= (index % ChunkSize) % LowLevel_Count;
		const LowLevelBits_t	mask		= LowLevelBits_t{1} << low_lvl_idx;
		auto&					info		= _chunkInfo[ chunk_idx ];
		auto&					level		= info.lowLevel[ hi_lvl_idx ];
		const LowLevelBits_t	old_bits	= level.fetch_and( ~mask );	// 1 -> 0

		if ( not (old_bits & mask) )
			return false;	// was not assigned

		// update high level bits
		if_unlikely( old_bits == UMax )
		{
			EXLOCK( info.hiLevelGuard );

			// low level may be changed at any time so check it inside spinlock
			if_unlikely( level.load() != UMax )
			{
				const auto		hi_bit	= (HiLevelBits_t{1} << hi_lvl_idx);
				constexpr auto	hi_max	= MaxHighLevel | InitialHighLevel;

				EXLOCK( _topLevelGuard );

				// update top level
				if_unlikely( info.hiLevel.fetch_and( ~hi_bit ) == hi_max )	// 1 -> 0
				{
					TopLevelBits_t	top_bit	= TopLevelBits_t{1} << chunk_idx;
					TopLevelBits_t	prev	= _topLevel.fetch_and( ~top_bit );	// 1 -> 0
					Unused( prev );
					ASSERT( !!( prev & top_bit ));
					ASSERT( info.hiLevel.load() != hi_max );
				}
			}
		}

		return true;
	}

/*
=================================================
	IsAssigned
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	inline bool  LfIndexedPool2<V,I,CS,MC,A>::IsAssigned (Index_t index) __NE___
	{
		if ( index < capacity() )
		{
			const uint		chunk_idx	= index / ChunkSize;
			const uint		elem_idx	= (index % ChunkSize) / LowLevel_Count;
			const uint		bit_idx		= (index % ChunkSize) % LowLevel_Count;
			LowLevelBits_t	mask		= LowLevelBits_t{1} << bit_idx;
			LowLevelBits_t	bits		= _chunkInfo[ chunk_idx ].lowLevel[ elem_idx ].load();
			return (bits & mask);
		}
		return false;
	}

/*
=================================================
	operator []
----
	you must invalidate cache before reading data
	and flush cache after writing
=================================================
*/
	template <typename V, typename I, usize CS, usize MC, typename A>
	inline V&  LfIndexedPool2<V,I,CS,MC,A>::operator [] (Index_t index) __NE___
	{
		ASSERT( index < capacity() );
		ASSERT( IsAssigned( index ));

		const uint		chunk_idx	= index / ChunkSize;
		const uint		elem_idx	= index % ChunkSize;
		ValueChunk_t*	chunk		= _chunkData[ chunk_idx ].load();

		NonNull( chunk );
		return (*chunk)[ elem_idx ];
	}


} // AE::Threading
