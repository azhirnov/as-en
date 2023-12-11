// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    thread-safe: yes
*/

#pragma once

#include "threading/Common.h"

namespace AE::Base
{

    //
    // Linear Allocator
    //

    template <typename AllocatorType, uint MaxBlocks>
    class LinearAllocator< AllocatorType, MaxBlocks, true > final : public IAllocatorTS
    {
    // types
    private:
        using BaseAlloc_t   = LinearAllocator< AllocatorType, MaxBlocks, false >;
    public:
        using Allocator_t   = AllocatorType;
        using Self          = LinearAllocator< AllocatorType, MaxBlocks, true >;


    // variables
    private:
        Mutex               _guard;
        BaseAlloc_t         _base;


    // methods
    public:
        LinearAllocator ()                                          __NE___ {}
        LinearAllocator (Self &&other)                              __NE___;
        explicit LinearAllocator (const Allocator_t &alloc)         __NE___ : _base{ alloc } {}
        explicit LinearAllocator (Bytes blockSize)                  __NE___ : _base{ blockSize } {}
        ~LinearAllocator ()                                         __NE___ { Release(); }

            Self&   operator = (Self &&rhs)                         __NE___;

            void    SetBlockSize (Bytes size)                       __NE___;
            void    Discard ()                                      __NE___;
            void    Release ()                                      __NE___;


        // IAllocator //
        ND_ void*   Allocate (const SizeAndAlign sizeAndAlign)      __NE_OV;

            void    Deallocate (void* ptr)                          __NE_OV { Deallocate( ptr, 1_b ); }
            void    Deallocate (void* ptr, Bytes size)              __NE_OV;
            void    Deallocate (void* ptr, const SizeAndAlign sa)   __NE_OV { Deallocate( ptr, sa.size ); }
    };



/*
=================================================
    constructor
=================================================
*/
    template <typename A, uint MB>
    LinearAllocator<A,MB,true>::LinearAllocator (Self &&other) __NE___
    {
        EXLOCK( _guard, other._guard );
        _base = RVRef(other._base);
    }

    template <typename A, uint MB>
    LinearAllocator<A,MB,true>&  LinearAllocator<A,MB,true>::operator = (Self &&rhs) __NE___
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
    void  LinearAllocator<A,MB,true>::SetBlockSize (Bytes size) __NE___
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
    void*  LinearAllocator<A,MB,true>::Allocate (const SizeAndAlign sizeAndAlign) __NE___
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
    void  LinearAllocator<A,MB,true>::Deallocate (void* ptr, Bytes size) __NE___
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
    Discard
=================================================
*/
    template <typename A, uint MB>
    void  LinearAllocator<A,MB,true>::Discard () __NE___
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
    void  LinearAllocator<A,MB,true>::Release () __NE___
    {
        EXLOCK( _guard );
        return _base.Release();
    }


} // AE::Base
