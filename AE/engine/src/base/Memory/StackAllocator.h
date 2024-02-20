// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/AllocatorFwdDecl.h"
#include "base/Memory/AllocatorHelper.h"
#include "base/Containers/FixedArray.h"
#include "base/CompileTime/Math.h"

namespace AE::Base
{

    //
    // Stack Allocator
    //

    template <typename AllocatorType,
              uint MaxBlocks
             >
    class StackAllocator< AllocatorType, MaxBlocks, false > final : public IAllocator
    {
        StaticAssert( MaxBlocks > 0 );

    // types
    public:
        using Self          = StackAllocator< AllocatorType, MaxBlocks, false >;
        using Allocator_t   = AllocatorType;

        enum class Bookmark : usize {};

        struct AutoReleaseBookmark
        {
            friend class StackAllocator;
        private:
            Self &          _ref;
            const Bookmark  _bm;

            AutoReleaseBookmark (Self &ref) __NE___ : _ref{ref}, _bm{ref.Push()} {}
        public:
            ~AutoReleaseBookmark ()         __NE___ { _ref.Pop( _bm ); }
        };

    private:
        struct Block
        {
            void *      ptr         = null;
            Bytes       size;       // used memory size
            Bytes       capacity;   // size of block
        };

        using Blocks_t          = FixedArray< Block, MaxBlocks >;
        using BookmarkStack_t   = FixedArray< Bookmark, 32 >;

        using Helper_t          = AllocatorHelper< EAllocatorType::Stack >;

        static constexpr uint   _PtrOffset      = CT_CeilIntLog2< MaxBlocks >;
        static constexpr usize  _BlockIndexMask = (1u << _PtrOffset) - 1;


    // variables
    private:
        Blocks_t            _blocks;
        BookmarkStack_t     _bookmarks;
        Bytes               _blockSize  = SmallAllocationSize;

        NO_UNIQUE_ADDRESS
         Allocator_t        _alloc;


    // methods
    public:
        StackAllocator ()                                           __NE___ {}
        StackAllocator (Self &&other)                               __NE___;
        StackAllocator (const Self &)                               = delete;
        explicit StackAllocator (const Allocator_t &alloc)          __NE___: _alloc{alloc} {}

        ~StackAllocator ()                                          __NE_OV { Release(); }

            Self&   operator = (const Self &)                       = delete;
            Self&   operator = (Self &&rhs)                         __NE___;

            void    SetBlockSize (Bytes size)                       __NE___ { _blockSize = size; }

            bool    Commit (Bookmark bm, Bytes size)                __NE___;

        ND_ auto    PushAuto ()                                     __NE___ { return AutoReleaseBookmark{ *this }; }

        ND_ Bookmark Push ()                                        __NE___;
            void     Pop (Bookmark bm)                              __NE___;

            void    Release ()                                      __NE___;

        ND_ Bytes   TotalSize ()                                    C_NE___;


        // IAllocator //
        ND_ void*   Allocate (const SizeAndAlign)                   __NE_OV;
            using   IAllocator::Allocate;

            void    Deallocate (void* ptr)                          __NE_OV { Deallocate( ptr, 1_b ); }
            void    Deallocate (void* ptr, Bytes size)              __NE_OV;
            void    Deallocate (void* ptr, const SizeAndAlign sa)   __NE_OV { Deallocate( ptr, sa.size ); }

            void    Discard ()                                      __NE_OV;


    private:
        ND_ static Pair<usize, usize>  _UnpackBookmark (Bookmark bm) __NE___;
    };



/*
=================================================
    constructor
=================================================
*/
    template <typename A, uint B>
    StackAllocator<A,B,false>::StackAllocator (Self &&other) __NE___ :
        _blocks{ RVRef(other._blocks) },
        _bookmarks{ RVRef(other._bookmarks) },
        _blockSize{ other._blockSize },
        _alloc{ RVRef(other._alloc) }
    {}

/*
=================================================
    operator =
=================================================
*/
    template <typename A, uint B>
    StackAllocator<A,B,false>&  StackAllocator<A,B,false>::operator = (Self &&rhs) __NE___
    {
        Release();
        _blocks     = RVRef(rhs._blocks);
        _bookmarks  = RVRef(rhs._bookmarks);
        _blockSize  = rhs._blockSize;
        _alloc      = RVRef(rhs._alloc);
        return *this;
    }

/*
=================================================
    Allocate
=================================================
*/
    template <typename A, uint B>
    void*  StackAllocator<A,B,false>::Allocate (const SizeAndAlign sizeAndAlign) __NE___
    {
        usize   idx = _blocks.size() ? 0u : UMax;

        if ( _bookmarks.size() )
        {
            usize   off = 0;
            std::tie( idx, off ) = _UnpackBookmark( _bookmarks.back() );

            ASSERT( idx >= _blocks.size() or _blocks[idx].size >= off );
        }

        for (; idx < _blocks.size(); ++idx)
        {
            auto&   block   = _blocks[idx];
            Bytes   offset  = AlignUp( usize(block.ptr) + block.size, sizeAndAlign.align ) - usize(block.ptr);

            if_likely( sizeAndAlign.size <= (block.capacity - offset) )
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

        Bytes   block_size  = _blockSize * (1 + _blocks.size()/2);
                block_size  = sizeAndAlign.size*2 < block_size ? block_size : block_size*2;
        void*   ptr         = _alloc.Allocate( SizeAndAlign{ block_size, DefaultAllocatorAlign });

        if_unlikely( ptr == null )
        {
            DBG_WARNING( "failed to allocate memory" );
            return null;
        }

        auto&   block       = _blocks.emplace_back(Block{ ptr, 0_b, block_size });
        Bytes   offset      = AlignUp( usize(block.ptr) + block.size, sizeAndAlign.align ) - usize(block.ptr);

        DEBUG_ONLY( DbgInitMem( block.ptr, block.capacity ));

        block.size = offset + sizeAndAlign.size;
        Helper_t::OnAllocate( block.ptr + offset, sizeAndAlign );
        return block.ptr + offset;
    }

/*
=================================================
    Deallocate
=================================================
*/
    template <typename A, uint B>
    void    StackAllocator<A,B,false>::Deallocate (void* ptr, Bytes size) __NE___
    {
    #ifdef AE_DEBUG
        for (auto& block : _blocks)
        {
            if ( IsIntersects( ptr, ptr + size, block.ptr, block.ptr + block.size ))
            {
                CHECK( ptr + size <= block.ptr + block.size );
                return;
            }
            AE_LOG_DBG( "'ptr' is not belong to this allocator" );
        }
    #else
        Unused( ptr, size );
    #endif
    }

/*
=================================================
    Push
=================================================
*/
    template <typename A, uint B>
    typename StackAllocator<A,B,false>::Bookmark  StackAllocator<A,B,false>::Push () __NE___
    {
        Bookmark    bm = Bookmark(0);

        if_likely( _blocks.size() )
        {
            bm = Bookmark( (_blocks.size()-1) | (usize(_blocks.back().size) << _PtrOffset) );
        }

        if_unlikely( _bookmarks.size() == _bookmarks.capacity() )
        {
            DBG_WARNING( "overflow" );
            return Bookmark(~0ull);
        }

        _bookmarks.push_back( bm );
        return bm;
    }

/*
=================================================
    Pop
=================================================
*/
    template <typename A, uint B>
    void  StackAllocator<A,B,false>::Pop (Bookmark bm) __NE___
    {
        for (usize i = 0; i < _bookmarks.size(); ++i)
        {
            if ( _bookmarks[i] == bm )
            {
                auto[idx, off] = _UnpackBookmark( bm );

                for (; idx < _blocks.size(); ++idx)
                {
                    auto& block = _blocks[idx];
                    block.size = Bytes{off};

                    DEBUG_ONLY( DbgInitMem( block.ptr + block.size, block.capacity - block.size ));
                    off = 0;
                }

                _bookmarks.resize( i );
                return;
            }
        }
    }

/*
=================================================
    Commit
=================================================
*/
    template <typename A, uint B>
    bool  StackAllocator<A,B,false>::Commit (Bookmark bm, Bytes size) __NE___
    {
        if_likely( not _bookmarks.empty() and _bookmarks.back() == bm )
        {
            auto[idx, off]  = _UnpackBookmark( bm );
            auto&   block   = _blocks.back();

            block.size = Bytes{off} + size;

            DEBUG_ONLY( DbgInitMem( block.ptr + block.size, block.capacity - block.size ));
            return true;
        }
        return false;
    }

/*
=================================================
    Discard
=================================================
*/
    template <typename A, uint B>
    void  StackAllocator<A,B,false>::Discard () __NE___
    {
        for (auto& block : _blocks)
        {
            block.size = 0_b;
            DEBUG_ONLY( DbgInitMem( block.ptr, block.capacity ));
        }
        _bookmarks.clear();
    }

/*
=================================================
    Release
=================================================
*/
    template <typename A, uint B>
    void  StackAllocator<A,B,false>::Release () __NE___
    {
        for (auto& block : _blocks) {
            _alloc.Deallocate( block.ptr, SizeAndAlign{ block.capacity, DefaultAllocatorAlign });
        }
        _blocks.clear();
        _bookmarks.clear();
    }

/*
=================================================
    _UnpackBookmark
=================================================
*/
    template <typename A, uint B>
    Pair<usize, usize>  StackAllocator<A,B,false>::_UnpackBookmark (Bookmark bm) __NE___
    {
        usize   u   = BitCast<usize>( bm );
        usize   idx = u & _BlockIndexMask;
        usize   off = u >> _PtrOffset;
        return { idx, off };
    }

/*
=================================================
    TotalSize
=================================================
*/
    template <typename A, uint B>
    Bytes  StackAllocator<A,B,false>::TotalSize () C_NE___
    {
        Bytes   size;
        for (auto& block : _blocks) {
            size += block.capacity;
        }
        return size;
    }


} // AE::Base
