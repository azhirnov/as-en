// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# include "threading/Primitives/Atomic.h"
# include "threading/Primitives/SyncEvent.h"
#endif

namespace AE::Threading
{
    using namespace AE::Math;

    //
    // Lock-Free Linear Allocator
    //

    template <usize BlockSize_v     = usize(SmallAllocationSize),   // in bytes
              usize MemAlign        = 8,
              usize MaxBlocks       = 16,
              typename AllocatorType = UntypedAllocator
             >
    class LfLinearAllocator final : public Noncopyable
    {
        STATIC_ASSERT( BlockSize_v > 0 );
        STATIC_ASSERT( MaxBlocks > 0 and MaxBlocks < 64 );

    // types
    public:
        using Self          = LfLinearAllocator< BlockSize_v, MemAlign, MaxBlocks, AllocatorType >;
        using Index_t       = uint;
        using Allocator_t   = AllocatorType;

        static constexpr bool   IsThreadSafe = true;

    private:
        struct alignas(AE_CACHE_LINE) MemBlock
        {
            Atomic< void *>     mem     {null};
            Atomic< usize >     size    {0};            // <= _Capacity
        };
        using MemBlocks_t = StaticArray< MemBlock, MaxBlocks >;

        static constexpr Bytes  _Capacity {BlockSize_v};
        static constexpr Bytes  _MemAlign {MemAlign};


    // variables
    private:
        MemBlocks_t         _blocks = {};

        Mutex               _allocGuard;

        NO_UNIQUE_ADDRESS
         Allocator_t        _allocator;


    // methods
    public:
        explicit LfLinearAllocator (const Allocator_t &alloc = Allocator_t{}) __NE___;
        ~LfLinearAllocator ()                                               __NE___ { Release(); }


        // must be externally synchronized
            void   Release ()                                               __NE___;
            void   Discard ()                                               __NE___;
        ND_ Bytes  CurrentSize ()                                           C_NE___;


        template <typename T>
        ND_ T*      Allocate (usize count = 1)                              __NE___ { return Cast<T>( Allocate( SizeAndAlignOf<T> * count )); }
        ND_ void*   Allocate (Bytes size)                                   __NE___ { return Allocate( SizeAndAlign{ size, DefaultAllocatorAlign }); }
        ND_ void*   Allocate (const SizeAndAlign sizeAndAlign)              __NE___;

        // only for debugging
        void  Deallocate (void *ptr, const SizeAndAlign sizeAndAlign)       __NE___;
        void  Deallocate (void *ptr, const Bytes size)                      __NE___ { Deallocate( ptr, SizeAndAlign{ size, 1_b }); }
        void  Deallocate (void *ptr)                                        __NE___ { Unused( ptr ); }


        ND_ static constexpr Bytes  BlockSize ()                            __NE___ { return _Capacity; }
        ND_ static constexpr Bytes  MaxSize ()                              __NE___ { return MaxBlocks * _Capacity; }
    };


} // AE::Threading

//#include "threading/Memory/LfLinearAllocator.inl.h"
