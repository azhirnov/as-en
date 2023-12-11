// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/GLM.h"

namespace AE::Math
{

    //
    // Large Bool (Bool32)
    //

    struct Bool32
    {
    private:
        uint    _value  = 0;

    private:
        constexpr explicit Bool32 (uint val)                __NE___ : _value{val} {}

    public:
        constexpr Bool32 ()                                 __NE___ = default;
        constexpr Bool32 (bool val)                         __NE___ : _value{uint(val)} {}

        constexpr Bool32 (Bool32 &&)                        __NE___ = default;
        constexpr Bool32 (const Bool32 &)                   __NE___ = default;

        constexpr Bool32&  operator = (Bool32 &&)           __NE___ = default;
        constexpr Bool32&  operator = (const Bool32 &)      __NE___ = default;
        constexpr Bool32&  operator = (bool rhs)            __NE___ { _value = uint(rhs);  return *this; }

        ND_ constexpr Bool32  operator not ()               C_NE___ { return Bool32{ ~_value }; }

        ND_ constexpr Bool32  operator | (Bool32 rhs)       C_NE___ { return Bool32{ _value | rhs._value }; }
        ND_ constexpr Bool32  operator & (Bool32 rhs)       C_NE___ { return Bool32{ _value & rhs._value }; }
        ND_ constexpr Bool32  operator ^ (Bool32 rhs)       C_NE___ { return Bool32{ _value ^ rhs._value }; }

        ND_ constexpr Bool32  operator || (Bool32 rhs)      C_NE___ { return Bool32{ _value || rhs._value }; }
        ND_ constexpr Bool32  operator && (Bool32 rhs)      C_NE___ { return Bool32{ _value && rhs._value }; }

        ND_ constexpr Bool32&  operator |= (Bool32 rhs)     __NE___ { _value |= rhs._value;  return *this; }
        ND_ constexpr Bool32&  operator &= (Bool32 rhs)     __NE___ { _value &= rhs._value;  return *this; }
        ND_ constexpr Bool32&  operator ^= (Bool32 rhs)     __NE___ { _value ^= rhs._value;  return *this; }

        ND_ constexpr operator bool ()                      C_NE___ { return _value != 0; }
    };

} // AE::Math


namespace AE::Base
{
    template <> struct TMemCopyAvailable< Bool32 >      { static constexpr bool  value = true; };
    template <> struct TZeroMemAvailable< Bool32 >      { static constexpr bool  value = true; };
    template <> struct TTriviallySerializable< Bool32 > { static constexpr bool  value = true; };

} // AE::Base
