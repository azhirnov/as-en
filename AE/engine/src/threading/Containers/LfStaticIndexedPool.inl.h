// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Threading
{
/*
=================================================
    constructor
=================================================
*/
    template <typename V, typename I, usize C, typename A>
    LfStaticIndexedPool<V,I,C,A>::LfStaticIndexedPool (const Allocator_t &alloc) __NE___ :
        _allocator{ alloc }
    {
        _arr = Cast<ChunkArray_t>( _allocator.Allocate( SizeAndAlignOf<ChunkArray_t> ));
        CHECK( _arr != null );

        if ( _arr != null )
            PlacementNew<ChunkArray_t>( OUT _arr.get() );
    }

/*
=================================================
    ForEach
----
    Must be externally synchronized
=================================================
*/
    template <typename V, typename I, usize C, typename A>
    template <typename FN>
    void  LfStaticIndexedPool<V,I,C,A>::ForEach (FN && fn) noexcept(IsNothrowInvocable<FN>)
    {
        if ( _arr == null )
            return;

        for (usize i = 0; i < ChunksCount; ++i)
        {
            auto&   chunk = (*_arr)[i];

            for (usize j = 0; j < ChunkSize; ++j)
            {
                fn( chunk.arr[j], Index_t(i*ChunkSize + j) );
            }
        }
    }

/*
=================================================
    Release
----
    Must be externally synchronized
=================================================
*/
    template <typename V, typename I, usize C, typename A>
    void  LfStaticIndexedPool<V,I,C,A>::Release (Bool checkForAssigned) __NE___
    {
        if ( _arr == null )
            return;

        if_unlikely( checkForAssigned )
        {
            for (usize i = 0; i < ChunksCount; ++i)
            {
                auto&   chunk = (*_arr)[i];
                CHECK( chunk.assigned.load() == 0 );    // some items is still assigned
            }
        }

        PlacementDelete( INOUT *_arr );

        _allocator.Deallocate( _arr.get(), SizeAndAlign{ SizeOf<ChunkArray_t>, AlignOf<ChunkArray_t> });
        _arr = null;
    }

/*
=================================================
    UnassignAll
----
    Must be externally synchronized
=================================================
*/
    template <typename V, typename I, usize C, typename A>
    template <typename FN>
    void  LfStaticIndexedPool<V,I,C,A>::UnassignAll (FN && fn) noexcept(IsNothrowInvocable<FN>)
    {
        if ( _arr == null )
            return;

        for (usize i = 0; i < ChunksCount; ++i)
        {
            auto&       chunk   = (*_arr)[i];
            Bitfield_t  bits    = chunk.assigned.exchange( 0 );

            if ( bits == 0 )
                continue;

            int     idx = BitScanForward( bits );   // first 1 bit

            for (; idx >= 0;)
            {
                Bitfield_t  mask = Bitfield_t{1} << idx;

                fn( chunk.arr[idx] );

                bits &= ~mask;                  // 1 -> 0
                idx  = BitScanForward( bits );  // first 1 bit
            }
        }
    }

/*
=================================================
    Assign
=================================================
*/
    template <typename V, typename I, usize C, typename A>
    bool  LfStaticIndexedPool<V,I,C,A>::Assign (OUT Index_t &outIndex) __NE___
    {
        CHECK_ERR( _arr != null );

        const usize     initial_idx = ThreadUtils::GetIntID() & ThreadToChunkMask;

        for (uint a = 0; a < NumAttempts; ++a)
        {
            for (usize i = 0; i < ChunksCount; ++i)
            {
                usize       chunk_idx   = (initial_idx + i * ChunkIdxStep) % ChunksCount;
                auto&       chunk       = (*_arr)[ chunk_idx ];
                Bitfield_t  bits        = chunk.assigned.load();    // 0 - unassigned
                int         idx         = BitScanForward( ~bits );  // first 0 bit

                for (; idx >= 0;)
                {
                    Bitfield_t  new_bit = Bitfield_t{1} << idx;

                    // try to acquire index
                    if_likely( chunk.assigned.CAS( INOUT bits, bits | new_bit ))    // 0 -> 1
                    {
                        outIndex = CheckCast<Index_t>( chunk_idx * ChunkSize + idx );
                        return true;
                    }

                    idx = BitScanForward( ~bits );  // first 0 bit
                    ThreadUtils::Pause();
                }
            }

            ThreadUtils::Pause();
        }
        return false;
    }

/*
=================================================
    Unassign
=================================================
*/
    template <typename V, typename I, usize C, typename A>
    void  LfStaticIndexedPool<V,I,C,A>::Unassign (Index_t index) __NE___
    {
        ASSERT( _arr != null );
        ASSERT( index < capacity() );

        const uint  chunk_idx   = index / ChunkSize;
        const uint  bit_idx     = index % ChunkSize;
        Bitfield_t  mask        = Bitfield_t{1} << bit_idx;
        Bitfield_t  old_bits    = (*_arr)[ chunk_idx ].assigned.fetch_and( ~mask ); // 1 -> 0
        Unused( old_bits );
        ASSERT( old_bits & mask );  // prev bit must be 1
    }

/*
=================================================
    IsAssigned
=================================================
*/
    template <typename V, typename I, usize C, typename A>
    bool  LfStaticIndexedPool<V,I,C,A>::IsAssigned (Index_t index) C_NE___
    {
        if_likely( (_arr != null) & (index < capacity()) )
        {
            const uint  chunk_idx   = index / ChunkSize;
            const uint  bit_idx     = index % ChunkSize;
            Bitfield_t  mask        = Bitfield_t{1} << bit_idx;
            Bitfield_t  bits        = (*_arr)[ chunk_idx ].assigned.load();
            return (bits & mask);   // 1 if assigned
        }
        return false;
    }

/*
=================================================
    operator []
=================================================
*/
    template <typename V, typename I, usize C, typename A>
    typename LfStaticIndexedPool<V,I,C,A>::Value_t&
        LfStaticIndexedPool<V,I,C,A>::operator [] (Index_t index) __NE___
    {
        ASSERT( _arr != null );
        ASSERT( index < capacity() );
        ASSERT( IsAssigned( index ));

        const uint      chunk_idx   = index / ChunkSize;
        const uint      bit_idx     = index % ChunkSize;
        auto&           chunk       = (*_arr)[ chunk_idx ];

        return chunk.arr[ bit_idx ];
    }

/*
=================================================
    At
=================================================
*/
    template <typename V, typename I, usize C, typename A>
    typename LfStaticIndexedPool<V,I,C,A>::Value_t*
        LfStaticIndexedPool<V,I,C,A>::At (Index_t index) __NE___
    {
        if_likely( _arr != null and index < capacity() )
        {
            const uint      chunk_idx   = index / ChunkSize;
            const uint      bit_idx     = index % ChunkSize;
            auto&           chunk       = (*_arr)[ chunk_idx ];

            return &chunk.arr[ bit_idx ];
        }
        return null;
    }

/*
=================================================
    size
=================================================
*/
    template <typename V, typename I, usize C, typename A>
    usize  LfStaticIndexedPool<V,I,C,A>::size () C_NE___
    {
        if_unlikely( _arr == null )
            return 0;

        usize   count = 0;

        for (auto& chunk : *_arr) {
            count += BitCount( chunk.assigned.load() );
        }
        return count;
    }

} // AE::Threading
