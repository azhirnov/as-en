// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"

namespace AE::Graphics
{

    //
    // Image Array Layer
    //

    struct ImageLayer
    {
    // variables
    private:
        ushort      _value  = 0;


    // methods
    public:
        constexpr ImageLayer ()                                     __NE___ {}

        template <typename T, ENABLEIF( IsUnsignedInteger<T> )>
        explicit constexpr ImageLayer (T value)                     __NE___ : _value( CheckCast<ushort>(value)) {}

        ND_ constexpr uint  Get ()                                  C_NE___ { return _value; }

        ND_ constexpr bool  operator == (const ImageLayer &rhs)     C_NE___ { return _value == rhs._value; }
        ND_ constexpr bool  operator != (const ImageLayer &rhs)     C_NE___ { return _value != rhs._value; }
        ND_ constexpr bool  operator >  (const ImageLayer &rhs)     C_NE___ { return _value >  rhs._value; }
        ND_ constexpr bool  operator <  (const ImageLayer &rhs)     C_NE___ { return _value <  rhs._value; }
        ND_ constexpr bool  operator >= (const ImageLayer &rhs)     C_NE___ { return _value >= rhs._value; }
        ND_ constexpr bool  operator <= (const ImageLayer &rhs)     C_NE___ { return _value <= rhs._value; }

        ND_ ImageLayer      operator + (const ImageLayer &rhs)      C_NE___ { return ImageLayer{ Get() + rhs.Get() }; }
            ImageLayer&     operator ++ ()                          __NE___ { ++_value;  return *this; }

            ImageLayer&     operator *= (uint rhs)                  __NE___ { _value = CheckCast<ushort>(_value * rhs);  return *this; }
    };


    ND_ inline constexpr ImageLayer operator "" _layer (unsigned long long value)   __NE___ { return ImageLayer{ uint(value) }; }


} // AE::Graphics

namespace AE::Base
{
    template <> struct TMemCopyAvailable< AE::Graphics::ImageLayer >        { static constexpr bool  value = true; };
    template <> struct TZeroMemAvailable< AE::Graphics::ImageLayer >        { static constexpr bool  value = true; };
    template <> struct TTriviallySerializable< AE::Graphics::ImageLayer >   { static constexpr bool  value = true; };

} // AE::Base


template <>
struct std::hash< AE::Graphics::ImageLayer >
{
    ND_ size_t  operator () (const AE::Graphics::ImageLayer &value) C_NE___
    {
        return size_t(AE::Base::HashOf( value.Get() ));
    }
};
