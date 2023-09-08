// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Threading
{

/*
=================================================
    constructor
=================================================
*/
    template <usize CS, usize MC, typename BA, typename GA>
    LfFixedBlockAllocator3<CS,MC,BA,GA>::LfFixedBlockAllocator3 (Bytes blockSize,
                                                                 Bytes blockAlign,
                                                                 const BlockAllocator_t&    blockAlloc,
                                                                 const GenAllocator_t&      genAlloc) __NE___ :
        _blockSize{ blockSize },
        _blockAlign{ blockAlign },
        _blockAlloc{ blockAlloc },
        _genAlloc{ genAlloc }
    {
        ASSERT( BlockSize() == blockSize );
        ASSERT( BlockAlign() == blockAlign );

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
            chunk.memBlock.store( null );

            top |= TopLevelBits_t{1} << (i % CT_SizeOfInBits<TopLevelBits_t>);

            #if AE_LFFIXEDBLOCKALLOC_DEBUG
            chunk.dbgInfo.store( null );
            #endif
        }

        for (usize i = 0; i < _topChunks.size(); ++i)
        {
            _topChunks[i].assigned.store( ~top_bits[i] );   // set 0 bit for working range, 1 bit for unused bits
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
    template <usize CS, usize MC, typename BA, typename GA>
    void  LfFixedBlockAllocator3<CS,MC,BA,GA>::Release (bool checkMemLeak) __NE___
    {
        MemoryBarrier( EMemoryOrder::Acquire );

        for (usize i = 0; i < MaxChunks; ++i)
        {
            auto&   chunk       = _bottomChunks[i];
            void*   ptr         = chunk.memBlock.exchange( null );
            auto&   top_chunk   = _topChunks[ i / CT_SizeOfInBits<TopLevelBits_t> ];

            if ( ptr != null )
                _blockAlloc.Deallocate( ptr, SizeAndAlign{ LargeBlockSize(), BlockAlign() });

            EXLOCK( chunk.hiLevelGuard );

            HiLevelBits_t   old_hi_level = chunk.hiLevel.exchange( InitialHighLevel );

            if ( checkMemLeak )
            {
                CHECK( old_hi_level == InitialHighLevel );  // some blocks is still allocated

                TopLevelBits_t  top_bits    = top_chunk.assigned.load();
                TopLevelBits_t  top_lvl_bit = TopLevelBits_t{1} << (i % CT_SizeOfInBits<TopLevelBits_t>);
                CHECK( not (top_bits & top_lvl_bit) );
            }

            #if AE_LFFIXEDBLOCKALLOC_DEBUG
            SourceLoc*  dbg_info = chunk.dbgInfo.exchange( null );
            #endif

            for (usize j = 0; j < HiLevel_Count; ++j)
            {
                LowLevelBits_t  old_low_level = chunk.lowLevel[j].exchange( 0 );

                if ( checkMemLeak )
                {
                    CHECK( old_low_level == 0 );    // some blocks is still allocated

                    // dump allocated memory blocks
                    #if AE_LFFIXEDBLOCKALLOC_DEBUG
                    if ( dbg_info != null )
                    {
                        int     low_idx = BitScanForward( old_low_level );  // first 1 bit

                        for (; low_idx >= 0;)
                        {
                            auto&   dbg = dbg_info[ low_idx + j * HiLevel_Count ];

                            AE_LOGI( "Leaked memory block: "s << ToString( BlockSize() ), dbg.file, dbg.line );

                            old_low_level   &= (LowLevelBits_t{1} << low_idx);  // 1 -> 0
                            low_idx         = BitScanForward( old_low_level );  // first 1 bit
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

        ulong   atom_cnt    = _dbgCounter.exchange( 0 );
        ulong   lock_cnt    = _dbgLockCounter.exchange( 0 );

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
    typename LfFixedBlockAllocator3<CS,MC,BA,GA>::Ptr_t
        LfFixedBlockAllocator3<CS,MC,BA,GA>::Alloc (const SourceLoc &loc) __NE___
    {
        struct Dbg
        {
            ulong   counter = 0;
            ulong   locks   = 0;
            Self &  ref;

            Dbg (Self& r) : ref{r}
            {}

            ~Dbg ()
            {
                ref._dbgCounter.fetch_add( counter );
                ref._dbgLockCounter.fetch_add( locks );
            }
        } dbg{ *this };

        for (usize i = 0; i < _topChunks.size(); ++i, ++dbg.counter)
        {
            auto&           top_chunk   = _topChunks[i];
            TopLevelBits_t  available   = ~top_chunk.assigned.load();   // 1 - unassigned
            int             idx         = BitScanForward( available );  // first 1 bit

            for (; idx >= 0; ++dbg.counter)
            {
                Ptr_t   ptr = _Alloc( uint(idx + i * CT_SizeOfInBits<TopLevelBits_t>), loc, INOUT dbg.counter, INOUT dbg.locks );

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
    template <usize CS, usize MC, typename BA, typename GA>
    typename LfFixedBlockAllocator3<CS,MC,BA,GA>::Ptr_t
        LfFixedBlockAllocator3<CS,MC,BA,GA>::_Alloc (const uint chunkIndex, const SourceLoc &loc, INOUT ulong& dbgCounter, INOUT ulong& lockCounter) __NE___
    {
        BottomChunk&    chunk   = _bottomChunks[ chunkIndex ];
        auto*           ptr     = chunk.memBlock.load( EMemoryOrder::Acquire );

        #if AE_LFFIXEDBLOCKALLOC_DEBUG
        SourceLoc*  dbg_info = chunk.dbgInfo.load( EMemoryOrder::Acquire );
        #endif

        // allocate new block
        if_unlikely( ptr == null )
        {
            // memory must be already visible for all threads
            void*   new_block = _blockAlloc.Allocate( SizeAndAlign{ LargeBlockSize(), BlockAlign() });

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
                    SourceLoc*  new_dbg_info = Cast<SourceLoc>( _genAlloc.Allocate( SizeAndAlign{ _DbgInfoSize(), _DbgInfoAlign() }));

                    if ( new_dbg_info != null )
                    {
                        ZeroMem( OUT new_dbg_info, _DbgInfoSize() );

                        SourceLoc*  old_dbg_info = chunk.dbgInfo.exchange( new_dbg_info );
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
            HiLevelBits_t   hi_available    = ~chunk.hiLevel.load();            // 1 - unassigned
            int             hi_lvl_idx      = BitScanForward( hi_available );   // first 1 bit

            for (; hi_lvl_idx >= 0; ++dbgCounter)
            {
                ASSERT( usize(hi_lvl_idx) < chunk.lowLevel.size() );

                // find available index in low level
                auto&           level           = chunk.lowLevel[ hi_lvl_idx ];
                LowLevelBits_t  low_available   = level.load();                     // 0 - unassigned
                int             low_lvl_idx     = BitScanForward( ~low_available ); // first 0 bit

                for (; low_lvl_idx >= 0; ++dbgCounter)
                {
                    const LowLevelBits_t    low_lvl_bit = (LowLevelBits_t{1} << low_lvl_idx);

                    if ( level.CAS( INOUT low_available, low_available | low_lvl_bit )) // 0 -> 1
                    {
                        // update high level
                        if_unlikely( low_available == ~low_lvl_bit )
                        {
                            bool    update_top  = false;
                            {
                                EXLOCK( chunk.hiLevelGuard );
                                ++lockCounter;

                                // low level value may be changed at any time so check it inside spinlock
                                if ( level.load() == UMax )
                                {
                                    const auto  hi_lvl_bit  = (HiLevelBits_t{1} << hi_lvl_idx);

                                    update_top = (chunk.hiLevel.Or( hi_lvl_bit ) == MaxHighLevel);  // 0 -> 1
                                    ++dbgCounter;
                                }
                            }

                            if_unlikely( update_top )
                            {
                                auto&   top_chunk = _topChunks[ chunkIndex / CT_SizeOfInBits<TopLevelBits_t> ];

                                EXLOCK( top_chunk.assignedGuard );
                                ++lockCounter;

                                if ( chunk.hiLevel.load() == MaxHighLevel )
                                {
                                    const auto  top_lvl_bit = TopLevelBits_t{1} << (chunkIndex % CT_SizeOfInBits<TopLevelBits_t>);

                                    top_chunk.assigned.fetch_or( top_lvl_bit ); // 0 -> 1
                                    ++dbgCounter;
                                }
                            }
                        }

                        const uint  idx_in_chunk    = hi_lvl_idx * LowLevel_Count + low_lvl_idx;
                        ASSERT( idx_in_chunk < ChunkSize );

                        #if AE_LFFIXEDBLOCKALLOC_DEBUG
                        if ( dbg_info != null )
                        {
                            dbg_info[idx_in_chunk] = loc;
                            MemoryBarrier( EMemoryOrder::Release );
                        }
                        #endif
                        Unused( loc );

                        void*   result = ptr + BlockSize() * idx_in_chunk;

                        ASSERT( CheckPointerAlignment( result, usize(_blockAlign) ));
                        DEBUG_ONLY( DbgInitMem( result, BlockSize() ));
                        return Ptr_t{result};
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
    Dealloc
=================================================
*/
    template <usize CS, usize MC, typename BA, typename GA>
    bool  LfFixedBlockAllocator3<CS,MC,BA,GA>::Dealloc (void* ptr) __NE___
    {
        const Bytes block_size  = LargeBlockSize();

        for (uint i = 0; i < MaxChunks; ++i)
        {
            auto&   chunk   = _bottomChunks[i];
            void*   mem     = chunk.memBlock.load();

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
    template <usize CS, usize MC, typename BA, typename GA>
    Bytes  LfFixedBlockAllocator3<CS,MC,BA,GA>::AllocatedSize () C_NE___
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


} // AE::Threading
