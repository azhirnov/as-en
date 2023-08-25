// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Warning:
        This container does not flush/invalidate cache, all atomic operations has relaxed memory order.
*/

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Primitives/Atomic.h"
#endif

namespace AE::Threading
{

    //
    // Lock-free Static Indexed Pool
    //

    template <typename ValueType,
              typename IndexType,
              usize Count,
              typename AllocatorType = UntypedAllocator
             >
    class LfStaticIndexedPool final : public Noncopyable
    {
        STATIC_ASSERT( Count > 0 and IsAligned( Count, 32 ));
        STATIC_ASSERT( MaxValue<IndexType>() >= Count );
        STATIC_ASSERT( AllocatorType::IsThreadSafe );

    // types
    public:
        using Self          = LfStaticIndexedPool< ValueType, IndexType, Count, AllocatorType >;
        using Index_t       = IndexType;
        using Value_t       = ValueType;
        using Allocator_t   = AllocatorType;

    private:
        static constexpr usize  ChunkSize   = Count < 32*12 ? 32 : 64;
        static constexpr usize  ChunksCount = Count / ChunkSize;

        STATIC_ASSERT( IsAligned( Count, ChunkSize ));

        using Bitfield_t    = Conditional< (ChunkSize <= 32), uint, ulong >;

        struct alignas(AE_CACHE_LINE) Chunk
        {
            Atomic< Bitfield_t >                assigned    {0};    // 1 - assigned, 0 - unassigned
            StaticArray< Value_t, ChunkSize >   arr         {};
        };
        using ChunkArray_t  = StaticArray< Chunk, ChunksCount >;

        static constexpr uint   NumAttempts         = 8;
        static constexpr usize  ThreadToChunkMask   =   ChunksCount < 4 ?   0 :
                                                        ChunksCount < 8 ?   ToBitMask<usize>( 2u ) :
                                                        ChunksCount < 16 ?  ToBitMask<usize>( 3u ) :
                                                                            ToBitMask<usize>( 4u );
        static constexpr usize  ChunkIdxStep        =   ChunksCount < 4 ?   1 :
                                                        ChunksCount < 10 ?  3 :
                                                                            5;
        STATIC_ASSERT( ThreadToChunkMask < ChunksCount );


    // variables
    private:
        Ptr<ChunkArray_t>   _arr;

        NO_UNIQUE_ADDRESS
         Allocator_t        _allocator;


    // methods
    public:
        explicit LfStaticIndexedPool (const Allocator_t &alloc = Allocator_t{}) __NE___;
        ~LfStaticIndexedPool ()                         __NE___ { Release( True{"check for assigned"} ); }

            void  Release (Bool checkForAssigned)       __NE___;

            template <typename FN>
            void  UnassignAll (FN && fn)                noexcept(IsNothrowInvocable<FN>);

            template <typename FN>
            void  ForEach (FN && fn)                    noexcept(IsNothrowInvocable<FN>);

        ND_ auto  Assign ()                             __NE___ { Index_t idx;  return Assign( OUT idx ) ? idx : UMax; }
        ND_ bool  Assign (OUT Index_t &outIndex)        __NE___;
            void  Unassign (Index_t index)              __NE___;
        ND_ bool  IsAssigned (Index_t index)            C_NE___;

        ND_ Value_t&        operator [] (Index_t index) __NE___;
        ND_ Value_t const&  operator [] (Index_t index) C_NE___ { return const_cast<Self*>(this)->operator[]( index ); }

        ND_ Value_t*        At (Index_t index)          __NE___;
        ND_ Value_t const*  At (Index_t index)          C_NE___ { return const_cast<Self*>(this)->At( index ); }

        ND_ usize           size ()                     C_NE___;
        ND_ bool            empty ()                    C_NE___ { return size() == 0; }

        ND_ static constexpr usize  capacity ()         __NE___ { return Count; }
        ND_ static constexpr Bytes  DynamicSize ()      __NE___ { return SizeOf<ChunkArray_t>; }
    };

} // AE::Threading

//#include "threading/Containers/LfStaticIndexedPool.inl.h"
