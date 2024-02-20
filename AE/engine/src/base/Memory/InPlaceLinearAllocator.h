// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/IAllocator.h"
#include "base/Containers/FixedArray.h"

namespace AE::Base
{

    //
    // In Place Linear Allocator
    //

    template <usize Size_v>
    class InPlaceLinearAllocator final : public IAllocator
    {
        StaticAssert( Size_v > 0 );
        StaticAssert( Size_v < MaxValue<ushort>() );

    // types
    public:
        using Self  = InPlaceLinearAllocator< Size_v >;


    // variables
    private:
        Bytes16u    _size;
        char        _memory [Size_v];


    // methods
    public:
        InPlaceLinearAllocator ()                                   __NE___ {}
        ~InPlaceLinearAllocator ()                                  __NE_OV { ASSERT( _size == 0 ); }

        InPlaceLinearAllocator (Self &&)                            = delete;
            Self&   operator = (Self &&)                            = delete;

        ND_ static constexpr Bytes  Capacity ()                     __NE___ { return Bytes{Size_v}; }


        // IAllocator //
        ND_ void*   Allocate (const SizeAndAlign)                   __NE_OV;
            using   IAllocator::Allocate;

            void    Deallocate (void* ptr)                          __NE_OV { Deallocate( ptr, 1_b ); }
            void    Deallocate (void* ptr, Bytes size)              __NE_OV;
            void    Deallocate (void* ptr, const SizeAndAlign sa)   __NE_OV { Deallocate( ptr, sa.size ); }

            void    Discard ()                                      __NE_OV;


    private:
        ND_ void*   _Begin ()                                       __NE___ { return &_memory[0]; }
        ND_ void*   _End ()                                         __NE___ { return &_memory[Size_v]; }
    };



/*
=================================================
    Allocate
=================================================
*/
    template <usize S>
    void*  InPlaceLinearAllocator<S>::Allocate (const SizeAndAlign sizeAndAlign) __NE___
    {
        ASSERT( sizeAndAlign.size > 0 );
        ASSERT( sizeAndAlign.align > 0 );

        Bytes   offset = AlignUp( Bytes{_Begin()} + _size, sizeAndAlign.align ) - Bytes{_Begin()};

        if_likely( offset + sizeAndAlign.size <= Capacity() )
        {
            _size = offset + sizeAndAlign.size;
            return _Begin() + offset;
        }
        return null;
    }

/*
=================================================
    Deallocate
=================================================
*/
    template <usize S>
    void  InPlaceLinearAllocator<S>::Deallocate (void* ptr, Bytes size) __NE___
    {
    #ifdef AE_DEBUG
        if ( IsIntersects( ptr, ptr + size, _Begin(), _End() ))
        {
            CHECK( ptr + size <= _End() );
        }else
            AE_LOG_DBG( "'ptr' is not belong to this allocator" );
    #else
        Unused( ptr, size );
    #endif
    }

/*
=================================================
    Discard
=================================================
*/
    template <usize S>
    void  InPlaceLinearAllocator<S>::Discard () __NE___
    {
        DEBUG_ONLY( DbgInitMem( _Begin(), _size ));
        _size = 0_b;
    }


} // AE::Base
