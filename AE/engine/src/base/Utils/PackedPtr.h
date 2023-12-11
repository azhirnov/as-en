// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    ref: https://stackoverflow.com/questions/16198700/using-the-extra-16-bits-in-64-bit-pointers

    With 64 byte align:
    - x64 uses only 48 bits (47 for user space).
    - minus 6 bits for 64 byte alignment.
    - 41..42 bits for pointer, 22..23 bits for extra data which is 4M.
*/

#pragma once

#include "base/Common.h"
#include "base/CompileTime/Math.h"
#include "base/Math/BitMath.h"
#include "base/Math/Math.h"
#include "base/Math/POTValue.h"

namespace AE::Base
{

    //
    // Packed Pointer
    //

    template <typename T,
              usize Align_v>
    class PackedPtr
    {
        StaticAssert( IsPowerOfTwo( Align_v ));

    // types
    public:
        using Value_t   = T;
        using Self      = PackedPtr< T, Align_v >;

    private:
        static constexpr uint   _MaxPtrBits = sizeof(void*) >= 8 ? 48 : 32;     // can be 47 bits
        static constexpr uint   _AlignPOT   = uint(CT_IntLog2< Align_v >);
        static constexpr uint   _PtrBits    = _MaxPtrBits - _AlignPOT;
        static constexpr usize  _PtrMask    = (usize{1} << _PtrBits) - 1;
        static constexpr uint   _ExtraBits  = CT_SizeOfInBits<usize> - _PtrBits;
        static constexpr usize  _ExtraMax   = (usize{1} << _ExtraBits) - 1;     // mask without offset


    // variables
    private:
        usize   _value  = 0;


    // methods
    public:
        PackedPtr ()                                    __NE___ {}
        PackedPtr (const Self &)                        __NE___ = default;
        PackedPtr (Self &&)                             __NE___ = default;

            Self&  operator = (const Self &)            __NE___ = default;
            Self&  operator = (Self &&)                 __NE___ = default;

        ND_ T*          Ptr ()                          __NE___ { return BitCast<T*>( (_value & _PtrMask) << _AlignPOT ); }
        ND_ T const*    Ptr ()                          C_NE___ { return BitCast<T const*>( (_value & _PtrMask) << _AlignPOT ); }

            void        Ptr (T* value)                  __NE___;

        ND_ usize       Extra ()                        C_NE___ { return _value >> _PtrBits; }
            void        Extra (usize value)             __NE___;

        ND_ static constexpr usize      ExtraBits ()    __NE___ { return _ExtraBits; }
        ND_ static constexpr usize      ExtraMax ()     __NE___ { return _ExtraMax; }
        ND_ static constexpr POTBytes   Align ()        __NE___ { return POTBytes{ PowerOfTwo{ _AlignPOT }}; }
    };


/*
=================================================
    Ptr
=================================================
*/
    template <typename T, usize A>
    void  PackedPtr<T,A>::Ptr (T* value) __NE___
    {
        usize   v = usize(value);
        ASSERT( IsMultipleOf( v, A ));

        v >>= _AlignPOT;
        ASSERT( v <= _PtrMask );

        _value &= _PtrMask;
        _value |= (v & _PtrMask);
    }

/*
=================================================
    Extra
=================================================
*/
    template <typename T, usize A>
    void  PackedPtr<T,A>::Extra (usize value) __NE___
    {
        ASSERT( value <= _ExtraMax );

        _value &= ~_PtrMask;    // remove extra
        _value |= value << _PtrBits;
    }


} // AE::Base
