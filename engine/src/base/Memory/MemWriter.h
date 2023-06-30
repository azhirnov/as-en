// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Math/Math.h"
#include "base/Memory/MemUtils.h"

namespace AE::Base
{

    //
    // Memory Writer
    //

    struct MemWriter
    {
    // variables
    private:
        void *      _ptr    = null;
        usize       _offset = 0;
        usize       _size;

        static constexpr Bytes  _MaxAlign {1u << 8};


    // methods
    public:
        MemWriter (void *ptr, Bytes size)                   __NE___ : _ptr{ptr}, _size{usize(size)} { ASSERT( Bytes{ptr} > _MaxAlign ); }   // IsAllocated() == true
        explicit MemWriter (Bytes align)                    __NE___ : _ptr{align}, _size{UMax}      { ASSERT( align <= _MaxAlign ); }       // IsAllocated() == false


        void  AlignTo (Bytes align)                         __NE___
        {
            Unused( Reserve( 0_b, align ));
        }

        ND_ void*  Reserve (Bytes size, Bytes align)        __NE___
        {
            ASSERT( _ptr != null );
            usize   result = AlignUp( usize(_ptr) + _offset, usize(align) );

            _offset = (result - usize(_ptr)) + usize(size);
            ASSERT( _offset <= _size );     // TODO: throw?

            return BitCast<void *>( result );
        }


        template <typename T>
        ND_ T&  Reserve ()                                  __NE___
        {
            return *Cast<T>( Reserve( SizeOf<T>, AlignOf<T> ));
        }

        template <typename T, typename ...Args>
        ND_ T&  Emplace (Args&& ...args)                    __Th___
        {
            ASSERT( IsAllocated() );
            return *PlacementNew<T>( &Reserve<T>(), FwdArg<Args>( args )... ); // throw
        }

        template <typename T, typename ...Args>
        ND_ T&  EmplaceSized (Bytes size, Args&& ...args)   __Th___
        {
            ASSERT( IsAllocated() );
            ASSERT( size >= SizeOf<T> );
            return *PlacementNew<T>( Reserve( size, AlignOf<T> ), FwdArg<Args>( args )... );  // throw
        }


        template <typename T>
        ND_ T*  ReserveArray (usize count)                  __NE___
        {
            return count ? Cast<T>( Reserve( SizeOf<T> * count, AlignOf<T> )) : null;
        }

        template <typename T, typename ...Args>
        ND_ T*  EmplaceArray (usize count, Args&& ...args)  __Th___
        {
            ASSERT( IsAllocated() );
            T*  result = ReserveArray<T>( count );

            for (usize i = 0; i < count; ++i) {
                PlacementNew<T>( result + i, FwdArg<Args>( args )... );  // throw
            }
            return result;
        }


        void  Clear ()                                      __NE___
        {
            ASSERT( IsAllocated() );
            ZeroMem( _ptr, Bytes{_size} );
        }


        ND_ Bytes  OffsetOf (void *ptr, Bytes defaultValue = UMax) C_NE___
        {
            if ( ptr ) {
                ASSERT( ptr >= _ptr and ptr < _ptr + Bytes{_size} );
                return Bytes{usize(ptr) - usize(_ptr)};
            }
            return defaultValue;
        }

        ND_ bool    IsAllocated ()                          C_NE___ { return Bytes{_ptr} > _MaxAlign; }
        ND_ Bytes   AllocatedSize ()                        C_NE___ { return Bytes{_offset}; }
        ND_ Bytes   MaxSize ()                              C_NE___ { return Bytes{_size}; }
        ND_ void*   Data ()                                 C_NE___ { return _ptr; }
    };


} // AE::Base
