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
    class StackAllocator< AllocatorType, MaxBlocks, true > final : public IAllocatorTS
    {
    // types
    private:
        using BaseAllocator_t   = StackAllocator< AllocatorType, MaxBlocks, false >;
    public:
        using Self              = StackAllocator< AllocatorType, MaxBlocks, true >;
        using Allocator_t       = AllocatorType;
        using Bookmark          = typename BaseAllocator_t::Bookmark;


    // variables
    private:
        Mutex               _guard;
        BaseAllocator_t     _base;


    // methods
    public:
        StackAllocator ()                                               __NE___ {}
        StackAllocator (Self &&other)                                   __NE___;
        explicit StackAllocator (const Allocator_t &alloc)              __NE___ : _base{ alloc } {}
        ~StackAllocator ()                                              __NE___ { Release(); }

            Self&       operator = (Self &&rhs)                         __NE___;

            void        SetBlockSize (Bytes size)                       __NE___;

        ND_ Bookmark    Push ()                                         __NE___;
            void        Pop (Bookmark bm)                               __NE___;

            void        Discard ()                                      __NE___;
            void        Release ()                                      __NE___;


        // IAllocator //
        ND_ void*       Allocate (const SizeAndAlign sizeAndAlign)      __NE_OV;

            void        Deallocate (void* ptr)                          __NE_OV { Deallocate( ptr, 1_b ); }
            void        Deallocate (void* ptr, Bytes size)              __NE_OV;
            void        Deallocate (void* ptr, const SizeAndAlign sa)   __NE_OV { Deallocate( ptr, sa.size ); }

            using IAllocator::Allocate;
    };



/*
=================================================
    constructor
=================================================
*/
    template <typename A, uint MB>
    StackAllocator<A,MB,true>::StackAllocator (Self &&other) __NE___
    {
        EXLOCK( _guard, other._guard );
        _base = RVRef(other._base);
    }

/*
=================================================
    operator =
=================================================
*/
    template <typename A, uint MB>
    StackAllocator<A,MB,true>&  StackAllocator<A,MB,true>::operator = (Self &&rhs) __NE___
    {
        EXLOCK( _guard, rhs._guard );
        _base = RVRef(rhs._base);
        return *this;
    }

/*
=================================================
    SetBlockSize
=================================================
*/
    template <typename A, uint MB>
    void  StackAllocator<A,MB,true>::SetBlockSize (Bytes size) __NE___
    {
        EXLOCK( _guard );
        return _base.SetBlockSize( size );
    }

/*
=================================================
    Allocate
=================================================
*/
    template <typename A, uint MB>
    void*  StackAllocator<A,MB,true>::Allocate (const SizeAndAlign sizeAndAlign) __NE___
    {
        EXLOCK( _guard );
        return _base.Allocate( sizeAndAlign );
    }

/*
=================================================
    Deallocate
=================================================
*/
    template <typename A, uint MB>
    void  StackAllocator<A,MB,true>::Deallocate (void* ptr, Bytes size) __NE___
    {
    #ifdef AE_DEBUG
        EXLOCK( _guard );
        _base.Deallocate( ptr, size );
    #else
        Unused( ptr, size );
    #endif
    }

/*
=================================================
    Push
=================================================
*/
    template <typename A, uint MB>
    typename StackAllocator<A,MB,true>::Bookmark  StackAllocator<A,MB,true>::Push () __NE___
    {
        EXLOCK( _guard );
        return _base.Push();
    }

/*
=================================================
    Pop
=================================================
*/
    template <typename A, uint MB>
    void  StackAllocator<A,MB,true>::Pop (Bookmark bm) __NE___
    {
        EXLOCK( _guard );
        return _base.Pop( bm );
    }

/*
=================================================
    Discard
=================================================
*/
    template <typename A, uint MB>
    void  StackAllocator<A,MB,true>::Discard () __NE___
    {
        EXLOCK( _guard );
        return _base.Discard();
    }

/*
=================================================
    Release
=================================================
*/
    template <typename A, uint MB>
    void  StackAllocator<A,MB,true>::Release () __NE___
    {
        EXLOCK( _guard );
        return _base.Release();
    }


} // AE::Base
