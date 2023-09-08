// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorFwdDecl.h"
#include "base/Memory/AllocatorHelper.h"
#include "base/Containers/FixedArray.h"

namespace AE::Base
{

    //
    // Linear Allocator
    //

    template <typename AllocatorType, uint MaxBlocks>
    class LinearAllocator< AllocatorType, MaxBlocks, false > final : public MovableOnly
    {
    // types
    public:
        using Allocator_t   = AllocatorType;
        using Self          = LinearAllocator< AllocatorType, MaxBlocks, false >;

        static constexpr bool   IsThreadSafe = false;

    private:
        struct Block
        {
            void *      ptr         = null;
            Bytes       size;       // used memory size
            Bytes       capacity;   // size of block
        };
        using Blocks_t  = FixedArray< Block, MaxBlocks >;

        using Helper_t  = AllocatorHelper< EAllocatorType::Linear >;

        static constexpr Bytes  _Padding    = 0_b;


    // variables
    private:
        Blocks_t            _blocks;
        Bytes               _blockSize  = SmallAllocationSize;

        NO_UNIQUE_ADDRESS
         Allocator_t        _alloc;


    // methods
    public:
        LinearAllocator ()                                  __NE___ {}
        LinearAllocator (Self &&other)                      __NE___ ;
        explicit LinearAllocator (const Allocator_t &alloc) __NE___ : _alloc{alloc}             {}
        explicit LinearAllocator (Bytes blockSize)          __NE___ : _blockSize{ blockSize }   {}
        ~LinearAllocator ()                                 __NE___ { Release(); }

        Self&  operator = (Self &&rhs)                      __NE___;

        void  SetBlockSize (Bytes size)                     __NE___ { _blockSize = size; }

        template <typename T>
        ND_ T*      Allocate (usize count = 1)              __NE___ { return Cast<T>( Allocate( SizeAndAlign{ SizeOf<T> * count, AlignOf<T> })); }
        ND_ void*   Allocate (const SizeAndAlign)           __NE___;

        void  Deallocate (void *, Bytes)                    __NE___ {}
        void  Deallocate (void *, const SizeAndAlign)       __NE___ {}

        void  Discard ()                                    __NE___;
        void  Release ()                                    __NE___;

        ND_ Bytes               TotalSize ()                C_NE___;
        ND_ ArrayView<Block>    GetBlocks ()                C_NE___ { return _blocks; }
    };



/*
=================================================
    constructor
=================================================
*/
    template <typename A, uint MB>
    LinearAllocator<A,MB,false>::LinearAllocator (Self &&other) __NE___ :
        _blocks{ RVRef(other._blocks) },
        _blockSize{ other._blockSize },
        _alloc{ RVRef(other._alloc) }
    {}

/*
=================================================
    operator =
=================================================
*/
    template <typename A, uint MB>
    LinearAllocator<A,MB,false>&  LinearAllocator<A,MB,false>::operator = (Self &&rhs) __NE___
    {
        Release();
        _blocks     = RVRef(rhs._blocks);
        _blockSize  = rhs._blockSize;
        _alloc      = RVRef(rhs._alloc);
        return *this;
    }

/*
=================================================
    Allocate
=================================================
*/
    template <typename A, uint MB>
    void*  LinearAllocator<A,MB,false>::Allocate (const SizeAndAlign sizeAndAlign) __NE___
    {
        ASSERT( sizeAndAlign.size > 0 );
        ASSERT( sizeAndAlign.align > 0 );

        for (auto& block : _blocks)
        {
            Bytes   offset  = AlignUp( Bytes{block.ptr} + (block.size != 0 ? _Padding + block.size : 0_b), sizeAndAlign.align ) - Bytes{block.ptr};

            if_likely( offset + sizeAndAlign.size <= block.capacity )
            {
                block.size = offset + sizeAndAlign.size;
                Helper_t::OnAllocate( block.ptr + offset, sizeAndAlign );
                return block.ptr + offset;
            }
        }

        if_unlikely( _blocks.size() == _blocks.capacity() )
        {
            //DBG_WARNING( "overflow" );
            return null;
        }

        Bytes   block_size  = _blockSize * (1 + _blocks.size() / 2);
                block_size  = sizeAndAlign.size*2 < block_size ? block_size : sizeAndAlign.size*2;
        void*   ptr         = _alloc.Allocate( SizeAndAlign{ block_size, DefaultAllocatorAlign });

        if_unlikely( ptr == null )
        {
            DBG_WARNING( "failed to allocate memory" );
            return null;
        }

        auto&   block   = _blocks.emplace_back( Block{ ptr, 0_b, block_size });
        Bytes   offset  = AlignUp( Bytes{block.ptr}, sizeAndAlign.align ) - Bytes{block.ptr};

        DEBUG_ONLY( DbgInitMem( block.ptr, block.capacity ));

        block.size = offset + sizeAndAlign.size;
        Helper_t::OnAllocate( block.ptr + offset, sizeAndAlign );
        return block.ptr + offset;
    }

/*
=================================================
    Discard
=================================================
*/
    template <typename A, uint MB>
    void  LinearAllocator<A,MB,false>::Discard () __NE___
    {
        for (auto& block : _blocks)
        {
            block.size = 0_b;
            DEBUG_ONLY( DbgInitMem( block.ptr, block.capacity ));
        }
    }

/*
=================================================
    Release
=================================================
*/
    template <typename A, uint MB>
    void  LinearAllocator<A,MB,false>::Release () __NE___
    {
        for (auto& block : _blocks) {
            _alloc.Deallocate( block.ptr, SizeAndAlign{ block.capacity, DefaultAllocatorAlign });
        }
        _blocks.clear();
    }

/*
=================================================
    TotalSize
=================================================
*/
    template <typename A, uint MB>
    Bytes  LinearAllocator<A,MB,false>::TotalSize () C_NE___
    {
        Bytes   size;
        for (auto& block : _blocks) {
            size += block.capacity;
        }
        return size;
    }


} // AE::Base
