// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Primitives/SpinLock.h"
# include "threading/Primitives/DataRaceCheck.h"
#endif

namespace AE::Threading
{

    template <typename T,
              usize ChunkCapacity_v
             >
    class LfChunkList final : public Noncopyable
    {
        StaticAssert( ChunkCapacity_v > 0 );

    // types
    public:
        using Self          = LfChunkList< T, ChunkCapacity_v >;
        using Value_t       = T;

    private:
        using Count_t       = ByteSizeToUInt< sizeof(void*)/2 >;
        using BaseList_t    = ChunkList< Conditional< IsAtomic<T>, AtomicInternalType<T>, T >>;
        using BaseChunk_t   = typename BaseList_t::Chunk;
        using CopyPolicy_t  = CopyPolicy::AutoDetect<T>;

        static constexpr usize  _Align          = Max( AE_CACHE_LINE, alignof(T) );
        static constexpr usize  _HeaderSize     = AlignUp( sizeof(void*)*2, alignof(T) );
        static constexpr usize  _ChunkCapacity  = ChunkCapacity_v;
        static constexpr usize  _ChunkSize      = _HeaderSize + sizeof(T) * _ChunkCapacity;
        static constexpr uint   _MaxAttempts    = 4;


        struct alignas(_Align) Chunk final : public Noncopyable
        {
        // variables
            Atomic< Chunk *>    next        {null};
            Atomic< Count_t >   count       {0};                // TODO: can be > capacity
            const Count_t       capacity    = _ChunkCapacity;   // only for compatibility with 'ChunkList', use '_ChunkCapacity' instead
            union {
                T               _data  [1];                     // needed to put elements with correct alignment
                char            _data2 [1];
            };

        // methods
            Chunk ()                            __NE___ {}
            ~Chunk ()                           __NE___ { CopyPolicy_t::Destroy( _data, Min( count.load(), _ChunkCapacity )); }

            ND_ bool    IsEmpty ()              C_NE___ { return count.load() == 0; }
            ND_ bool    IsFull ()               C_NE___ { return count.load() >= _ChunkCapacity; }

            ND_ T*      Data ()                 __NE___ { return _data; }
            ND_ T&      operator [] (usize i)   __NE___ { ASSERT( i < count.load() );  return _data[i]; }
        };


        StaticAssert( sizeof(Chunk) == sizeof(BaseChunk_t) );   // incorrect because of align
        StaticAssert( alignof(Chunk) == alignof(BaseChunk_t) );
        StaticAssert( offsetof( Chunk, count ) == offsetof( BaseChunk_t, count ));
        StaticAssert( offsetof( Chunk, capacity ) == offsetof( BaseChunk_t, capacity ));
        StaticAssert64( (_ChunkSize % alignof(Chunk)) < sizeof(T) );


    // variables
    private:
        Atomic< Chunk *>    _last   {null};
        Chunk *             _first  {null};

        DRC_ONLY( RWDataRaceCheck   _drCheck;)


    // methods
    public:
        LfChunkList ()                                          __NE___ {}
        ~LfChunkList ()                                         __NE___;

        ND_ BaseList_t  Release ()                              __NE___;

        template <typename Allocator>
        ND_ bool  Init (Allocator &alloc)                       __NE___;

        template <typename Allocator,
                  typename ...Args>
        ND_ bool  Emplace (Allocator &alloc, Args&& ...args)    __NE___;

        template <typename Allocator>
            void  Destroy (Allocator &alloc)                    __NE___;

        ND_ static constexpr Bytes  ChunkSize ()                __NE___ { return Bytes{_ChunkSize}; }

    private:
            void  _UpdateLast ()                                __NE___;
    };



/*
=================================================
    Release
----
    Must be externally synchronized.
=================================================
*/
    template <typename T, usize S>
    typename LfChunkList<T,S>::BaseList_t  LfChunkList<T,S>::Release () __NE___
    {
        DRC_EXLOCK( _drCheck );

        BaseList_t  result { Cast<BaseChunk_t>( _first )};

        _first = null;
        _last.store( null );

        return result;
    }

/*
=================================================
    Init
----
    Must be externally synchronized.
=================================================
*/
    template <typename T, usize S>
    template <typename Allocator>
    bool  LfChunkList<T,S>::Init (Allocator &alloc) __NE___
    {
        DRC_EXLOCK( _drCheck );

        ASSERT( _last.load() == null );
        ASSERT( _first == null );

        Chunk*  new_chunk = Cast<Chunk>( alloc.Allocate( SizeAndAlign{ ChunkSize(), AlignOf<Chunk> }));
        if_likely( new_chunk != null )
        {
            PlacementNew<Chunk>( OUT new_chunk );

            _first  = new_chunk;
            _last.store( new_chunk );

            return true;
        }
        return false;
    }

/*
=================================================
    Emplace
----
    Returns 'true' if new element is successfully added to chunk.
    Returns 'false' if not enough space in last chunk and failed to allocate a new chunk.
=================================================
*/
    template <typename T, usize S>
    template <typename Allocator, typename ...Args>
    bool  LfChunkList<T,S>::Emplace (Allocator &alloc, Args&& ...args) __NE___
    {
        // allocator can be externally synchronized
        //StaticAssert( IsThreadSafeAllocator< Allocator >);

        DRC_SHAREDLOCK( _drCheck );

        Chunk*  chunk = _last.load();

        for (uint attempt = 0; attempt < _MaxAttempts; ++attempt)
        {
            ASSERT( chunk != null );

            if_likely( Count_t idx = chunk->count.fetch_add( 1 );  idx < _ChunkCapacity )   // TODO: CAS ?
            {
                if constexpr( IsAtomic<T> )
                {
                    // We don't need acquire/release for atomic operations.
                    // In 'Release()' we need to add memory barrier anyway.
                    chunk->Data()[idx].store( FwdArg<Args>(args)... );
                }
                else
                {
                    // load previous content
                    //MemoryBarrier( EMemoryOrder::Acquire );  // skip because previous content will be discarded

                    PlacementNew<T>( OUT chunk->Data() + idx, FwdArg<Args>(args)... );

                    // make changes visible for all threads
                    MemoryBarrier( EMemoryOrder::Release );
                }
                return true;
            }

            // another thread may already add the next chunk
            if_unlikely( Chunk* next = chunk->next.load(); next != null )
            {
                chunk = next;
                _UpdateLast();
                continue;   // try again
            }

            // allocate memory for chunk
            Chunk*  new_chunk = Cast<Chunk>( alloc.Allocate( SizeAndAlign{ ChunkSize(), AlignOf<Chunk> }));
            if_unlikely( new_chunk == null )
                continue;   // try again

            // create chunk and flush cache before adding chunk to list
            PlacementNew<Chunk>( OUT new_chunk );
            MemoryBarrier( EMemoryOrder::Release );

            // add new chunk to list
            {
                Chunk*  next = null;
                Chunk*  curr = chunk;

                for (uint i = 0; i < ThreadUtils::SpinBeforeLock(); ++i)
                {
                    if_likely( curr->next.CAS( INOUT next, new_chunk ))
                        break;

                    // 'CAS' can return false even if 'next' is null
                    if_likely( next != null )
                    {
                        // we need to attach 'new_chunk' to any subsequent chunk to avoid mem leak
                        curr = next;
                        next = null;
                    }

                    ThreadUtils::Pause();
                }
            }

            // 'chunk->next' may not equal to 'new_chunk'
            chunk = chunk->next.load();
            _UpdateLast();
        }
        return false;
    }

/*
=================================================
    _UpdateLast
=================================================
*/
    template <typename T, usize S>
    forceinline void  LfChunkList<T,S>::_UpdateLast () __NE___
    {
        Chunk*  curr = _last.load();
        ASSERT( curr != null );

        Chunk*  chunk = curr;

        // move to last with available space
        for (Chunk* next = chunk->next.load();
             (next != null) and (chunk->count.load() >= _ChunkCapacity);)
        {
            chunk   = next;
            next    = chunk->next.load();
        }

        Unused( _last.CAS_Loop( INOUT curr, chunk ));
    }

/*
=================================================
    Destroy
----
    Must be externally synchronized.
=================================================
*/
    template <typename T, usize S>
    template <typename Allocator>
    void  LfChunkList<T,S>::Destroy (Allocator &alloc) __NE___
    {
        DRC_EXLOCK( _drCheck );

        for (Chunk* chunk = _first; chunk != null;)
        {
            Chunk*  ptr     = chunk;
                    chunk   = chunk->next.load();

            ptr->~Chunk();
            alloc.Deallocate( ptr, SizeAndAlign{ ChunkSize(), AlignOf<Chunk> });
        }

        _first  = null;
        _last.store( null );
    }

/*
=================================================
    destructor
=================================================
*/
    template <typename T, usize S>
    LfChunkList<T,S>::~LfChunkList () __NE___
    {
        // use 'Release()' or 'Destroy()' to avoid mem leak

        DRC_EXLOCK( _drCheck );
        ASSERT( _last.load() == null );
        ASSERT( _first == null );
    }


} // AE::Threading
