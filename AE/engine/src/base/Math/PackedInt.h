// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/Math.h"

namespace AE::Math
{

    //
    // Packed Integer
    //

    template <typename T>
    struct PackedInt
    {
        StaticAssert( IsUnsignedInteger<T> );

    // types
    public:
        using Self      = PackedInt<T>;
        using Value_t   = T;
        using HalfInt_t = ByteSizeToUInt< sizeof(T)/2 >;

        StaticAssert( sizeof(Value_t) == sizeof(HalfInt_t)*2 );

        static constexpr uint   _Offset = CT_SizeOfInBits< HalfInt_t >;
        static constexpr uint   _Mask   = ToBitMask< HalfInt_t >( _Offset );


    // variables
    public:
        HalfInt_t   hi  = 0;
        HalfInt_t   lo  = 0;


    // methods
    public:
        constexpr PackedInt ()              __NE___ {}
        constexpr PackedInt (const Self &)  __NE___ = default;

        constexpr PackedInt (Value_t val)   __NE___ :
            hi{HalfInt_t( val >> _Offset )},
            lo{HalfInt_t{ val & _Mask }}
        {}

        Self&  operator = (const Self &)    __NE___ = default;

        Self&  operator = (Value_t val)     __NE___
        {
            hi = HalfInt_t( val >> _Offset );
            lo = HalfInt_t( val & _Mask );
            return *this;
        }

        ND_ explicit operator Value_t ()    C_NE___ { return (Value_t{hi} << _Offset) | Value_t{lo}; }
    };

    using packed_ulong  = PackedInt<ulong>;


} // AE::Math


namespace AE::Base
{
    template <typename T>   struct TMemCopyAvailable< PackedInt<T> >        { static constexpr bool  value = IsMemCopyAvailable<T>; };
    template <typename T>   struct TZeroMemAvailable< PackedInt<T> >        { static constexpr bool  value = IsZeroMemAvailable<T>; };
    template <typename T>   struct TTriviallySerializable< PackedInt<T> >   { static constexpr bool  value = IsTriviallySerializable<T>; };

} // AE::Base
