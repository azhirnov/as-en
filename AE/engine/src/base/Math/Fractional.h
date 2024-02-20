// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Math
{

    //
    // Fractional
    //

    template <typename T>
    struct Fractional
    {
    // types
    public:
        using Self  = Fractional<T>;


    // variables
    public:
        T   num {0};        // numerator
        T   den {1};        // denominator


    // methods
    public:
        constexpr Fractional ()                                 __NE___ {}

        constexpr explicit Fractional (T inNum, T inDenom = T{1})   __NE___
        {
            ASSERT( inDenom > 0 );
            const T gcd = _GreatestCommonDivisor( inNum, inDenom );
            if ( gcd != 0 ) {
                num = inNum / gcd;
                den = inDenom / gcd;
            }
        }

        constexpr Fractional (const Self &)                     __NE___ = default;

        constexpr Self&  operator = (const Self &)              __NE___ = default;


        ND_ constexpr Self  operator - ()                       C_NE___
        {
            return Self{ -num, den };
        }

        ND_ constexpr Self  operator + (const Self &rhs)        C_NE___
        {
            return Self{ this->num * rhs.den + rhs.num * den, this->den * rhs.den };
        }

        ND_ constexpr Self  operator - (const Self &rhs)        C_NE___
        {
            return Self{ this->num * rhs.den - rhs.num * den, this->den * rhs.den };
        }

        ND_ constexpr Self  operator * (const Self &rhs)        C_NE___
        {
            return Self{ this->num * rhs.num, this->den * rhs.den };
        }

        ND_ constexpr Self  operator / (const Self &rhs)        C_NE___
        {
            return Self{ this->num * rhs.den, this->den * rhs.num };
        }

        ND_ constexpr Self  Pow (uint value)                    C_NE___
        {
            Self    result;
            result.num  = this->num;
            result.den  = this->den;

            for (uint i = 0; i < value; ++i) {
                result.num  *= this->num;
                result.den  *= this->den;
            }
            return result;
        }

        ND_ constexpr bool  operator == (const Self &rhs)       C_NE___
        {
            return  ((this->num == T{0})    and (rhs.num == T{0}))  or
                    ((this->num == rhs.num) and (this->den == rhs.den));
        }

        ND_ constexpr bool  IsZero ()                           C_NE___ { return num == T{0}; }

        ND_ constexpr bool  IsInteger ()                        C_NE___ { return den == T{1}; }

        ND_ constexpr bool  IsPositive ()                       C_NE___ { return num >  T{0}; }
        ND_ constexpr bool  IsPositiveOrZero ()                 C_NE___ { return num >= T{0}; }
        ND_ constexpr bool  IsNegative ()                       C_NE___ { return num <  T{0}; }
        ND_ constexpr bool  IsNegativeOrZero ()                 C_NE___ { return num <= T{0}; }

        ND_ constexpr T     Numerator ()                        C_NE___ { return num; }
        ND_ constexpr T     Denominator ()                      C_NE___ { return den; }

        template <typename R>
        ND_ constexpr EnableIf<IsFloatPoint<R>, R>  ToFloat ()  C_NE___ { return R(num) / R(den); }


        // Rounding:
        // RTS - round to smaller, similar to floor(val * float(frac))
        // RTN - round to nearest, similar to round(val * float(frac))
        // RTL - round to larger,  similar to ceil(val * float(frac))

        template <typename R>
        ND_ constexpr R  Mul_RTS (const R &val)                 C_NE___ { return (val * num) / den; }

        template <typename R>
        ND_ constexpr R  Mul_RTN (const R &val)                 C_NE___ { return ((val * num) + R(den/2)) / den; }

        template <typename R>
        ND_ constexpr R  Mul_RTL (const R &val)                 C_NE___ { return ((val * num) + R(den-1)) / den; }


        template <typename R>
        ND_ constexpr R  Div_RTS (const R &val)                 C_NE___ { ASSERT( num != 0 );  return (val * den) / num; }

        template <typename R>
        ND_ constexpr R  Div_RTN (const R &val)                 C_NE___ { ASSERT( num != 0 );  return ((val * den) + R(num/2)) / num; }

        template <typename R>
        ND_ constexpr R  Div_RTL (const R &val)                 C_NE___ { ASSERT( num != 0 );  return ((val * den) + R(num-1)) / num; }


    private:
        ND_ static constexpr T  _GreatestCommonDivisor (T value1, T value2) __NE___
        {
            return value2 != 0 ? _GreatestCommonDivisor( value2, value1 % value2 ) : value1;
        }
    };


    using FractionalI   = Fractional< int >;
    using FractionalU   = Fractional< uint >;

    using FractionalI16 = Fractional< sshort >;
    using FractionalU16 = Fractional< ushort >;


} // AE::Math
