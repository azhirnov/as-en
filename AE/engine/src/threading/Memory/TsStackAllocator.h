// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe: yes
*/

#pragma once

#include "threading/Common.h"

namespace AE::Base
{

    //
    // Stack Allocator
    //

    template <typename AllocatorType, uint MaxBlocks>
    class StackAllocator< AllocatorType, MaxBlocks, true > final : public MovableOnly
    {
    // types
    private:
        using BaseAllocator_t   = StackAllocator< AllocatorType, MaxBlocks, false >;
    public:
        using Self              = StackAllocator< AllocatorType, MaxBlocks, true >;
        using Allocator_t       = AllocatorType;
        using Bookmark          = typename BaseAllocator_t::Bookmark;

        static constexpr bool   IsThreadSafe = true;


    // variables
    private:
        Mutex               _guard;
        BaseAllocator_t     _base;


    // methods
    public:
        StackAllocator ()                                       __NE___ {}
        explicit StackAllocator (const Allocator_t &alloc)      __NE___ : _base{ alloc } {}

        ~StackAllocator ()                                      __NE___ { Release(); }


        StackAllocator (Self &&other)                           __NE___
        {
            EXLOCK( _guard, other._guard );
            _base = RVRef(other._base);
        }

        Self&  operator = (Self &&rhs)                          __NE___
        {
            EXLOCK( _guard, rhs._guard );
            _base = RVRef(rhs._base);
            return *this;
        }


        void  SetBlockSize (Bytes size)                         __NE___
        {
            EXLOCK( _guard );
            return _base.SetBlockSize( size );
        }


        ND_ void*  Allocate (const SizeAndAlign sizeAndAlign)   __NE___
        {
            EXLOCK( _guard );
            return _base.Allocate( sizeAndAlign );
        }


        template <typename T>
        ND_ T*  Allocate (usize count = 1)                      __NE___
        {
            EXLOCK( _guard );
            return _base.template Allocate<T>( count );
        }


        void  Deallocate (void* ptr, const SizeAndAlign sizeAndAlign) __NE___
        {
            Unused( ptr, sizeAndAlign );
        }


        ND_ Bookmark  Push ()                                   __NE___
        {
            EXLOCK( _guard );
            return _base.Push();
        }


        void  Pop (Bookmark bm)                                 __NE___
        {
            EXLOCK( _guard );
            return _base.Pop( bm );
        }


        void  Discard ()                                        __NE___
        {
            EXLOCK( _guard );
            return _base.Discard();
        }

        void  Release ()                                        __NE___
        {
            EXLOCK( _guard );
            return _base.Release();
        }
    };

} // AE::Base
