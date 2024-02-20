// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Threading
{

/*
=================================================
    constructor
=================================================
*/
    template <usize CS, usize MC>
    LfStaticBlockAllocator<CS,MC>::LfStaticBlockAllocator (void*    storage,
                                                           Bytes    storageSize,
                                                           Bytes    blockSize,
                                                           Bytes    blockAlign) __NE___ :
        _storage{ storage },
        _blockSize{ blockSize },
        _blockAlign{ blockAlign }
    {
        ASSERT( blockSize > 0 );
        ASSERT_Eq( BlockSize(), blockSize );
        ASSERT_Eq( BlockAlign(), blockAlign );

        CHECK( _storage );
        CHECK_Eq( storageSize, MaxMemorySize() );

        StaticArray< TopLevelBits_t, TopLevel_Count >   top_bits = {};

        for (usize i = 0; i < _bottomChunks.size(); ++i)
        {
            auto&   chunk   = _bottomChunks[i];
            auto&   top     = top_bits[ i / CT_SizeOfInBits<TopLevelBits_t> ];

            for (usize j = 0; j < HiLevel_Count; ++j)
            {
                chunk.lowLevel[j].store( 0 );
            }

            chunk.hiLevel.store( InitialHighLevel );    // set 0 bit for working range, 1 bit for unused bits

            top |= TopLevelBits_t{1} << (i % CT_SizeOfInBits<TopLevelBits_t>);
        }

        for (usize i = 0; i < _topChunks.size(); ++i)
        {
            _topChunks[i].assigned.store( ~top_bits[i] );   // set 0 bit for working range, 1 bit for unused bits
        }
    }

/*
=================================================
    _PtrOffset
=================================================
*/
    template <usize CS, usize MC>
    Bytes  LfStaticBlockAllocator<CS,MC>::_PtrOffset (const uint chunkIndex, const uint idxInChunk) C_NE___
    {
        ASSERT( chunkIndex < MaxChunks );
        ASSERT( idxInChunk < ChunkSize );

        return chunkIndex * LargeBlockSize() + BlockSize() * idxInChunk;
    }

/*
=================================================
    Release
=================================================
*/
    template <usize CS, usize MC>
    void  LfStaticBlockAllocator<CS,MC>::Release (Bool checkMemLeak) __NE___
    {
        for (usize i = 0; i < MaxChunks; ++i)
        {
            auto&   chunk       = _bottomChunks[i];
            auto&   top_chunk   = _topChunks[ i / CT_SizeOfInBits<TopLevelBits_t> ];

            EXLOCK( chunk.hiLevelGuard );

            HiLevelBits_t   old_hi_level = chunk.hiLevel.exchange( InitialHighLevel );

            if ( checkMemLeak )
            {
                CHECK( old_hi_level == InitialHighLevel );  // some blocks is still allocated

                TopLevelBits_t  top_bits    = top_chunk.assigned.load();
                TopLevelBits_t  top_lvl_bit = TopLevelBits_t{1} << (i % CT_SizeOfInBits<TopLevelBits_t>);
                CHECK( not (top_bits & top_lvl_bit) );
            }

            for (usize j = 0; j < HiLevel_Count; ++j)
            {
                LowLevelBits_t  old_low_level = chunk.lowLevel[j].exchange( 0 );

                if ( checkMemLeak )
                    CHECK( old_low_level == 0 );    // some blocks is still allocated
            }
        }
    }

/*
=================================================
    AllocBlock
=================================================
*/
    template <usize CS, usize MC>
    typename LfStaticBlockAllocator<CS,MC>::Ptr_t
        LfStaticBlockAllocator<CS,MC>::AllocBlock () __NE___
    {
        for (usize i = 0; i < _topChunks.size(); ++i)
        {
            auto&           top_chunk   = _topChunks[i];
            TopLevelBits_t  available   = ~top_chunk.assigned.load();   // 1 - unassigned
            int             idx         = BitScanForward( available );  // first 1 bit

            for (; idx >= 0;)
            {
                Ptr_t   ptr = _Alloc( uint(idx + i * CT_SizeOfInBits<TopLevelBits_t>) );

                if_likely( ptr != null )
                    return ptr;

                available   &= ~(TopLevelBits_t{1} << idx);     // 1 -> 0
                idx         = BitScanForward( available );      // first 1 bit
            }
        }

        return null;
    }

/*
=================================================
    _Alloc
=================================================
*/
    template <usize CS, usize MC>
    typename LfStaticBlockAllocator<CS,MC>::Ptr_t
        LfStaticBlockAllocator<CS,MC>::_Alloc (const uint chunkIndex) __NE___
    {
        BottomChunk&    chunk   = _bottomChunks[ chunkIndex ];

        // find available index in high level
        for (uint j = 0; j < HighWaitCount; ++j)
        {
            HiLevelBits_t   hi_available    = ~chunk.hiLevel.load();            // 1 - unassigned
            int             hi_lvl_idx      = BitScanForward( hi_available );   // first 1 bit

            for (; hi_lvl_idx >= 0;)
            {
                ASSERT( usize(hi_lvl_idx) < chunk.lowLevel.size() );

                // find available index in low level
                auto&           level           = chunk.lowLevel[ hi_lvl_idx ];
                LowLevelBits_t  low_available   = level.load();                     // 0 - unassigned
                int             low_lvl_idx     = BitScanForward( ~low_available ); // first 0 bit

                for (; low_lvl_idx >= 0;)
                {
                    const LowLevelBits_t    low_lvl_bit = (LowLevelBits_t{1} << low_lvl_idx);

                    if_likely( level.CAS( INOUT low_available, low_available | low_lvl_bit ))   // 0 -> 1
                    {
                        // update high level
                        if_unlikely( low_available == ~low_lvl_bit )
                        {
                            bool    update_top  = false;
                            {
                                EXLOCK( chunk.hiLevelGuard );

                                // low level value may be changed at any time so check it inside spinlock
                                if ( level.load() == UMax )
                                {
                                    const auto  hi_lvl_bit  = (HiLevelBits_t{1} << hi_lvl_idx);

                                    update_top = (chunk.hiLevel.Or( hi_lvl_bit ) == MaxHighLevel);  // 0 -> 1
                                }
                            }

                            if_unlikely( update_top )
                            {
                                auto&   top_chunk = _topChunks[ chunkIndex / CT_SizeOfInBits<TopLevelBits_t> ];

                                EXLOCK( top_chunk.assignedGuard );

                                if ( chunk.hiLevel.load() == MaxHighLevel )
                                {
                                    const auto  top_lvl_bit = TopLevelBits_t{1} << (chunkIndex % CT_SizeOfInBits<TopLevelBits_t>);

                                    top_chunk.assigned.fetch_or( top_lvl_bit ); // 0 -> 1
                                }
                            }
                        }

                        const uint  idx_in_chunk = hi_lvl_idx * LowLevel_Count + low_lvl_idx;
                        ASSERT( idx_in_chunk < ChunkSize );

                        Ptr_t   result = _storage + _PtrOffset( chunkIndex, idx_in_chunk );

                        ASSERT( CheckPointerAlignment( result, usize(_blockAlign) ));
                        DEBUG_ONLY( DbgInitMem( result, BlockSize() ));
                        return result;
                    }

                    low_lvl_idx = BitScanForward( ~low_available ); // first 0 bit
                    ThreadUtils::Pause();
                }

                hi_available &= ~(HiLevelBits_t{1} << hi_lvl_idx);  // 1 -> 0
                hi_lvl_idx = BitScanForward( hi_available );        // first 1 bit
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
    template <usize CS, usize MC>
    bool  LfStaticBlockAllocator<CS,MC>::DeallocBlock (void* ptr) __NE___
    {
        const Bytes block_size  = LargeBlockSize();

        for (uint i = 0; i < MaxChunks; ++i)
        {
            auto&   chunk   = _bottomChunks[i];
            void*   mem     = _storage + _PtrOffset( i, 0 );

            if ( ptr < mem or ptr >= mem + block_size )
                continue;

            const uint  idx = uint((Bytes{ptr} - Bytes{mem}) / _blockSize);
            ASSERT( idx < ChunkSize );

            const uint      hi_lvl_idx  = idx / LowLevel_Count;
            const uint      low_lvl_idx = idx % LowLevel_Count;
            LowLevelBits_t  low_bit     = LowLevelBits_t{1} << low_lvl_idx;
            auto&           level       = chunk.lowLevel[ hi_lvl_idx ];
            const uint      idx_in_chunk= hi_lvl_idx * LowLevel_Count + low_lvl_idx;

            ASSERT( ptr == mem + BlockSize() * idx_in_chunk );  Unused( idx_in_chunk );
            DEBUG_ONLY( DbgFreeMem( ptr, BlockSize() ));

            LowLevelBits_t  old_bits = level.fetch_and( ~low_bit ); // 1 -> 0

            if_unlikely( not (old_bits & low_bit) )
            {
                AE_LOGE( "failed to deallocate memory: block is already deallocated" );
                return false;
            }

            // update high level bits
            if_unlikely( old_bits == UMax )
            {
                bool    update_top  = false;
                {
                    EXLOCK( chunk.hiLevelGuard );

                    // low level value may be changed at any time so check it inside spinlock
                    if ( level.load() != UMax )
                    {
                        const auto  hi_lvl_bit  = (HiLevelBits_t{1} << hi_lvl_idx);

                        update_top = (chunk.hiLevel.fetch_and( ~hi_lvl_bit ) == MaxHighLevel);  // 1 -> 0
                    }
                }

                if_unlikely( update_top )
                {
                    auto&   top_chunk = _topChunks[ i / CT_SizeOfInBits<TopLevelBits_t> ];

                    EXLOCK( top_chunk.assignedGuard );

                    if ( chunk.hiLevel.load() != MaxHighLevel )
                    {
                        const auto  top_lvl_bit = TopLevelBits_t{1} << (i % CT_SizeOfInBits<TopLevelBits_t>);

                        top_chunk.assigned.fetch_and( ~top_lvl_bit );   // 1 -> 0
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
    template <usize CS, usize MC>
    Bytes  LfStaticBlockAllocator<CS,MC>::AllocatedSize () C_NE___
    {
        const Bytes block_size  = LargeBlockSize();
        Bytes       result;

        for (uint i = 0; i < MaxChunks; ++i)
        {
            auto&   chunk   = _bottomChunks[i];

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
    template <usize CS, usize MC>
    void*  LfStaticBlockAllocator<CS,MC>::Allocate (const SizeAndAlign sizeAndAlign) __NE___
    {
        CHECK_ERR(  sizeAndAlign.size  <= BlockSize() and
                    sizeAndAlign.align <= BlockAlign() );
        return AllocBlock();
    }

/*
=================================================
    Deallocate
=================================================
*/
    template <usize CS, usize MC>
    void  LfStaticBlockAllocator<CS,MC>::Deallocate (void* ptr, const SizeAndAlign sizeAndAlign) __NE___
    {
        ASSERT( sizeAndAlign.size  <= BlockSize() and
                sizeAndAlign.align <= BlockAlign() );
        Unused( sizeAndAlign );
        CHECK( DeallocBlock( ptr ));
    }

} // AE::Threading
