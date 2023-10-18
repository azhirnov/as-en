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
    class LinearAllocator< AllocatorType, MaxBlocks, true > final : public MovableOnly
    {
    // types
    private:
        using BaseAlloc_t   = LinearAllocator< AllocatorType, MaxBlocks, false >;
    public:
        using Allocator_t   = AllocatorType;
        using Self          = LinearAllocator< AllocatorType, MaxBlocks, true >;

        static constexpr bool   IsThreadSafe = true;


    // variables
    private:
        Mutex               _guard;
        BaseAlloc_t         _base;


    // methods
    public:
        LinearAllocator ()                                      __NE___ {}
        LinearAllocator (Self &&other)                          __NE___;
        explicit LinearAllocator (const Allocator_t &alloc)     __NE___ : _base{ alloc } {}
        explicit LinearAllocator (Bytes blockSize)              __NE___ : _base{ blockSize } {}
        ~LinearAllocator ()                                     __NE___ { Release(); }

        Self& operator = (Self &&rhs)                           __NE___;

        ND_ void*  Allocate (const SizeAndAlign sizeAndAlign)   __NE___;

        template <typename T>
        ND_ T*  Allocate (usize count = 1)                      __NE___;

        void  Deallocate (void* ptr)                            __NE___ { Deallocate( ptr, 1_b ); }
        void  Deallocate (void* ptr, Bytes size)                __NE___;
        void  Deallocate (void* ptr, const SizeAndAlign sa)     __NE___ { Deallocate( ptr, sa.size ); }

        void  SetBlockSize (Bytes size)                         __NE___;
        void  Discard ()                                        __NE___;
        void  Release ()                                        __NE___;
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

    template <typename A, uint MB>
    template <typename T>
    T*  LinearAllocator<A,MB,true>::Allocate (usize count) __NE___
    {
        EXLOCK( _guard );
        return _base.template Allocate<T>( count );
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
