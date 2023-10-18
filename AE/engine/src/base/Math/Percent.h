// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"

namespace AE::Math
{

    //
    // Percent
    //

    template <typename T>
    struct TPercent
    {
    // types
    public:
        using Value_t   = T;
        using Self      = TPercent<T>;


    // variables
    private:
        T       _value;


    // methods
    public:
        constexpr TPercent ()                                               __NE___ = default;
        explicit constexpr TPercent (T val)                                 __NE___ : _value{val} {}


        ND_ constexpr T GetPercent ()                                       C_NE___ { return _value * T{100}; }             // 0..100%
        ND_ constexpr T GetFraction ()                                      C_NE___ { return _value; }                      // 0..1

        template <typename B>
        ND_ constexpr EnableIf<IsFloatPoint<B>, B>  Of (const B &value)     C_NE___ { return value * B{GetFraction()}; }

        ND_ static constexpr Self   FromPercent (T value)                   __NE___ { return Self{ value * T{0.01} }; }
        ND_ static constexpr Self   FromFraction (T value)                  __NE___ { return Self{ value }; }
    };


    using Percent   = TPercent< float >;
    using PercentD  = TPercent< double >;

    ND_ constexpr Percent operator "" _pct (long double value)          { return Percent{ Percent::FromPercent( Percent::Value_t( value ))}; }
    ND_ constexpr Percent operator "" _pct (unsigned long long value)   { return Percent{ Percent::FromPercent( Percent::Value_t( value ))}; }


} // AE::Math
