// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Math/Percent.h"

namespace AE::Math
{
    template <typename T>
    static constexpr T EulerNumber = T( 2.71828182845904523536 );

/*
=================================================
    AdditionIsSafe
=================================================
*/
    template <typename T1, typename T2>
    ND_ constexpr EnableIf<IsSignedInteger<T1> and IsSignedInteger<T2>, bool>  AdditionIsSafe (const T1 a, const T2 b) __NE___
    {
        STATIC_ASSERT( IsScalar<T1> and IsScalar<T2> );

        using T = decltype(a + b);

        const T x   = T(a);
        const T y   = T(b);
        const T min = MinValue<T>();
        const T max = MaxValue<T>();

        bool    overflow =  ((y > 0) & (x > max - y))   |
                            ((y < 0) & (x < min - y));
        return not overflow;
    }

/*
=================================================
    AdditionIsSafe
=================================================
*/
    template <typename T1, typename T2>
    ND_ constexpr EnableIf<IsUnsignedInteger<T1> and IsUnsignedInteger<T2>, bool>  AdditionIsSafe (const T1 a, const T2 b) __NE___
    {
        STATIC_ASSERT( IsScalar<T1> and IsScalar<T2> );

        using T = decltype(a + b);

        const T x   = T(a);
        const T y   = T(b);

        return (x + y) >= (x | y);
    }

/*
=================================================
    Average
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, T>  Average (T begin, T end) __NE___
    {
        if constexpr( IsFloatPoint<T> )
            return (begin * T{0.5}) + (end * T{0.5});
        else
        if constexpr( IsUnsignedInteger<T> )
            return (begin >> 1) + (end >> 1) + (((begin & 1) + (end & 1)) >> 1);
        else
        if constexpr( IsSignedInteger<T> )
            return (begin / 2) + (end / 2) + ((begin % 2) + (end % 2)) / 2;
    }

    template <typename T>
    ND_ constexpr TBytes<T>  Average (TBytes<T> begin, TBytes<T> end) __NE___
    {
        return TBytes<T>{ Average( T{begin}, T{end} )};
    }

/*
=================================================
    AlignDown
=================================================
*/
    template <typename T0, typename T1>
    ND_ constexpr auto  AlignDown (const T0 &value, const T1 &align) __NE___
    {
        ASSERT( align > 0 );
        if constexpr( IsPointer<T0> )
        {
            Bytes   byte_align{ align };
            return BitCast<T0>(usize( (BitCast<usize>(value) / byte_align) * byte_align ));
        }
        else
            return (value / align) * align;
    }

/*
=================================================
    AlignUp
=================================================
*/
    template <typename T0, typename T1>
    ND_ constexpr auto  AlignUp (const T0 &value, const T1 &align) __NE___
    {
        ASSERT( align > 0 );
        if constexpr( IsPointer<T0> )
        {
            Bytes   byte_align{ align };
            return BitCast<T0>(usize( ((BitCast<usize>(value) + byte_align-1) / byte_align) * byte_align ));
        }
        else
            return ((value + align-1) / align) * align;
    }

/*
=================================================
    IsMultipleOf
=================================================
*/
    template <typename T0, typename T1>
    ND_ constexpr bool  IsMultipleOf (const T0 &value, const T1 &align) __NE___
    {
        ASSERT( align > 0 );
        if constexpr( IsPointer<T0> )
        {
            return BitCast<usize>(value) % Bytes{align} == 0;
        }
        else
            return value % align == 0;
    }

/*
=================================================
    All/Any
=================================================
*/
    ND_ forceinline constexpr bool  All (const bool &value) __NE___
    {
        return value;
    }

    ND_ forceinline constexpr bool  Any (const bool &value) __NE___
    {
        return value;
    }

/*
=================================================
    Square
=================================================
*/
    template <typename T>
    ND_ constexpr T  Square (const T &value) __NE___
    {
        return value * value;
    }

/*
=================================================
    Sqrt
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  Sqrt (const T &value) __NE___
    {
        return std::sqrt( value );
    }

/*
=================================================
    Abs
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, T>  Abs (const T &x) __NE___
    {
        return std::abs( x );
    }

/*
=================================================
    Epsilon
=================================================
*/
    template <typename T>
    ND_ constexpr  EnableIf<IsScalar<T>, T>  Epsilon () __NE___
    {
        return std::numeric_limits<T>::epsilon() * T(2);
    }

/*
=================================================
    Equals (scalar)
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, bool>  Equals (const T &lhs, const T &rhs, const T err = Epsilon<T>()) __NE___
    {
        if constexpr( IsUnsignedInteger<T> )
        {
            return lhs < rhs ? ((rhs - lhs) <= err) : ((lhs - rhs) <= err);
        }else
            return Abs( lhs - rhs ) <= err;
    }

/*
=================================================
    Equals (Optional)
=================================================
*/
    template <typename T>
    ND_ constexpr bool  Equals (const Optional<T> &lhs, const Optional<T> &rhs) __NE___
    {
        return  lhs.has_value() == rhs.has_value()  and
                (lhs.has_value() ? All( *lhs == *rhs ) : false);
    }

/*
=================================================
    IsZero / IsNotZero
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, bool>  IsZero (const T &x) __NE___
    {
        return Equals( x, T{0}, Epsilon<T>() );
    }

    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, bool>  IsNotZero (const T &x) __NE___
    {
        return not IsZero( x );
    }

/*
=================================================
    Equals
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, bool>  Equals (const T &lhs, const T &rhs, const Percent err) __NE___
    {
        T   pct = std::abs( std::min( lhs, rhs ) / std::max( lhs, rhs ) - T{1});

        return  std::abs( lhs - rhs ) <= Epsilon<T>() or    // for zero
                pct <= T(err.GetFraction());
    }

/*
=================================================
    Floor / Ceil / Trunc
=================================================
*/
    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Floor (const T& x) __NE___
    {
        return std::floor( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Ceil (const T& x) __NE___
    {
        return std::ceil( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Trunc (const T& x) __NE___
    {
    #if 1
        return std::trunc( x );
    #else
        return x > T{0} ? Floor(x) : Ceil(x);
    #endif
    }

/*
=================================================
    Round / RoundToInt / RoundToUint
=================================================
*/
    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Round (const T& x) __NE___
    {
        return std::round( x );
    }

    template <typename T>
    ND_ auto  RoundToInt (const T& x) __NE___
    {
        STATIC_ASSERT( IsFloatPoint<T> );

        if constexpr( sizeof(T) >= sizeof(slong) )
            return slong(std::round( x ));

        if constexpr( sizeof(T) >= sizeof(int32_t) )
            return int32_t(std::round( x ));
    }

    template <typename T>
    ND_ auto  RoundToUint (const T& x) __NE___
    {
        STATIC_ASSERT( IsFloatPoint<T> );

        if constexpr( sizeof(T) >= sizeof(ulong) )
            return ulong(std::round( x ));

        if constexpr( sizeof(T) >= sizeof(uint) )
            return uint(std::round( x ));
    }

/*
=================================================
    Fract
----
    GLSL-style fract which returns value in range [0; 1)
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  Fract (const T& x) __NE___
    {
        return x - Floor( x );
    }

/*
=================================================
    IsIntersects
----
    1D intersection check
=================================================
*/
    template <typename T>
    ND_ constexpr bool  IsIntersects (const T& begin1, const T& end1,
                                      const T& begin2, const T& end2) __NE___
    {
        STATIC_ASSERT( IsScalar<T> or IsPointer<T> or IsBytes<T> );
        return (end1 > begin2) & (begin1 < end2);
    }

/*
=================================================
    IsCompletelyInside
=================================================
*/
    template <typename T>
    ND_ constexpr bool  IsCompletelyInside (const T& largeBlockBegin, const T& largeBlockEnd,
                                            const T& smallBlockBegin, const T& smallBlockEnd) __NE___
    {
        STATIC_ASSERT( IsScalar<T> or IsPointer<T> or IsBytes<T> );
        return (smallBlockBegin >= largeBlockBegin) & (smallBlockEnd <= largeBlockEnd);
    }

/*
=================================================
    GetIntersection
=================================================
*/
    template <typename T>
    ND_ constexpr bool  GetIntersection (const T& begin1, const T& end1,
                                         const T& begin2, const T& end2,
                                         OUT T& outBegin, OUT T& outEnd) __NE___
    {
        STATIC_ASSERT( IsScalar<T> or IsPointer<T> or IsBytes<T> );
        outBegin = Max( begin1, begin2 );
        outEnd   = Min( end1, end2 );
        return outBegin < outEnd;
    }

/*
=================================================
    Lerp
----
    linear interpolation
=================================================
*/
    template <typename T, typename B>
    ND_ constexpr EnableIf<IsScalar<T>, T>  Lerp (const T& x, const T& y, const B& factor) __NE___
    {
        //return T(factor) * (y - x) + x;
        return x * (T{1} - T(factor)) + y * T(factor);
    }

/*
=================================================
    Ln / Log / Log2 / Log10
=================================================
*/
    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Ln (const T& x) __NE___
    {
        ASSERT( x >= T{0} );
        return std::log( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Log2 (const T& x) __NE___
    {
        ASSERT( x >= T{0} );
        return std::log2( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Log10 (const T& x) __NE___
    {
        ASSERT( x >= T{0} );
        return std::log10( x );
    }

    template <auto Base, typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Log (const T& x) __NE___
    {
        static constexpr auto log_base = std::log( Base );
        return Ln( x ) / log_base;
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Log (const T& x, const T& base) __NE___
    {
        return Ln( x ) / Ln( base );
    }

/*
=================================================
    Pow / Exp / Exp2 / Exp10 / ExpMinus1
=================================================
*/
    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Pow (const T& base, const T& power) __NE___
    {
        ASSERT( base >= T{0} or power == Floor(power) );    // if 'base' < 0 and 'power' not integer then result is NaN
        return std::pow( base, power );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Exp (const T& x) __NE___
    {
        return std::exp( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Exp2 (const T& x) __NE___
    {
        return std::exp2( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Exp10 (const T& x) __NE___
    {
        return Pow( T(10), x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  ExpMinus1 (const T& x) __NE___
    {
        return std::expm1( x );
    }

/*
=================================================
    Sign / SignOrZero
----
    extract sign and return same type as input
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, T>  Sign (const T &value) __NE___
    {
        if constexpr( IsSigned<T> )
            return value < T{0} ? T{-1} : T{1};
        else
            return T{1};
    }

    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, T>  SignOrZero (const T &value) __NE___
    {
        if constexpr( IsSigned<T> )
            return value < T{0} ? T{-1} : value > T{0} ? T{1} : T{0};
        else
            return value > T{0} ? T{1} : T{0};
    }

/*
=================================================
    HasSign
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, bool>  HasSign (const T &value) __NE___
    {
        if constexpr( IsSigned<T> )
            return value < T{0};
        else
            return false;
    }

/*
=================================================
    AnyEqual
=================================================
*/
    template <typename Lhs, typename Rhs0, typename ...Rhs>
    ND_ constexpr bool  AnyEqual (const Lhs &lhs, const Rhs0 &rhs0, const Rhs& ...rhs) __NE___
    {
        if constexpr( sizeof... (Rhs) == 0 )
            return All( lhs == rhs0 );
        else
            return All( lhs == rhs0 ) | AnyEqual( lhs, rhs... );
    }

/*
=================================================
    DivCeil
=================================================
*/
    template <typename T1, typename T2>
    ND_ constexpr T1  DivCeil (const T1 &x, const T2 &divider) __NE___
    {
        return (x + divider - T1{1}) / divider;
    }

/*
=================================================
    IsInfinity / IsNaN / IsFinite
=================================================
*/
    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, bool>  IsInfinity (const T &x) __NE___
    {
        return std::isinf( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, bool>  IsNaN (const T &x) __NE___
    {
        return std::isnan( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, bool>  IsFinite (const T &x) __NE___
    {
        return std::isfinite( x );
    }

/*
=================================================
    Exchange
=================================================
*/
    template <typename T1, typename T2>
    ND_ constexpr T1  Exchange (INOUT T1 &lhs, const T2 &rhs) __NE___
    {
        T1  tmp = lhs;
        lhs = rhs;
        return tmp;
    }

/*
=================================================
    FusedMulAdd
----
    faster and more precise version of (a * b) + c
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  FusedMulAdd (const T a, const T b, const T c) __NE___
    {
        return std::fma( a, b, c );
    }

/*
=================================================
    LinearStep / SmoothStep / BumpStep / SmoothBumpStep
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  LinearStep (const T& x, const T& edge0, const T& edge1) __NE___
    {
        ASSERT( edge0 < edge1 );
        return Saturate( (x - edge0) / (edge1 - edge0) );
    }

    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  SmoothStep (const T& x, const T& edge0, const T& edge1) __NE___
    {
        ASSERT( edge0 < edge1 );
        T t = Saturate( (x - edge0) / (edge1 - edge0) );
        return t * t * (T(3) - T(2) * t);
    }

    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  BumpStep (const T& x, const T& edge0, const T& edge1) __NE___
    {
        ASSERT( edge0 < edge1 );
        return T(1) - Abs( Saturate( (x - edge0) / (edge1 - edge0) ) - T(0.5) ) * T(2);
    }

    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  SmoothBumpStep (const T& x, const T& edge0, const T& edge1) __NE___
    {
        ASSERT( edge0 < edge1 );
        T   t = BumpStep( x, edge0, edge1 );
        return t * t * (T(3) - T(2) * t);
    }

/*
=================================================
    ToUNorm / ToSNorm
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  ToUNorm (const T snorm) __NE___
    {
        return snorm * T(0.5) + T(0.5);
    }

    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  ToSNorm (const T unorm) __NE___
    {
        return unorm * T(2.0) - T(1.0);
    }


} // AE::Math
