// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Math.h"
#include "base/Math/Vec.h"

namespace AE::Math
{

    //
    // Radians
    //

    template <typename T>
    struct TRadians
    {
        STATIC_ASSERT( IsScalar<T> and IsFloatPoint<T> );

    // types
    public:
        using Self      = TRadians<T>;
        using Value_t   = T;


    // variables
    private:
        T       _value;


    // methods
    public:
        constexpr TRadians ()                                   __NE___ : _value{} {}
        constexpr explicit TRadians (T val)                     __NE___ : _value{val} {}

        constexpr TRadians (const Self &)                       __NE___ = default;
        constexpr TRadians (Self &&)                            __NE___ = default;

        ND_ constexpr explicit operator T ()                    C_NE___ { return _value; }

        ND_ constexpr static Self   Pi ()                       __NE___ { return Self{T( 3.14159265358979323846 )}; }
        ND_ constexpr static Self   Pi2 ()                      __NE___ { return Pi() * T(2); }
        ND_ constexpr static T      DegToRad ()                 __NE___ { return T(0.01745329251994329576923690768489); }
        ND_ constexpr static T      RadToDeg ()                 __NE___ { return T(57.295779513082320876798154814105); }

            Self&  operator = (const Self &)                    __NE___ = default;
            Self&  operator = (Self &&)                         __NE___ = default;

        ND_ constexpr Self   operator - ()                      C_NE___ { return Self{ -_value }; }

        ND_ constexpr Self   WrapToPi ()                        C_NE___ { return Self{Wrap( _value, -Pi()._value, Pi()._value  )}; }
        ND_ constexpr Self   WrapTo0_2Pi ()                     C_NE___ { return Self{Wrap( _value, 0, Pi2()._value  )}; }

            constexpr Self&  operator += (const Self rhs)       __NE___ { _value += rhs._value;  return *this; }
            constexpr Self&  operator -= (const Self rhs)       __NE___ { _value -= rhs._value;  return *this; }
            constexpr Self&  operator *= (const Self rhs)       __NE___ { _value *= rhs._value;  return *this; }
            constexpr Self&  operator /= (const Self rhs)       __NE___ { _value /= rhs._value;  return *this; }

            constexpr Self&  operator += (const T rhs)          __NE___ { _value += rhs;  return *this; }
            constexpr Self&  operator -= (const T rhs)          __NE___ { _value -= rhs;  return *this; }
            constexpr Self&  operator *= (const T rhs)          __NE___ { _value *= rhs;  return *this; }
            constexpr Self&  operator /= (const T rhs)          __NE___ { _value /= rhs;  return *this; }

        ND_ constexpr Self   operator + (const Self rhs)        C_NE___ { return Self{ _value + rhs._value }; }
        ND_ constexpr Self   operator - (const Self rhs)        C_NE___ { return Self{ _value - rhs._value }; }
        ND_ constexpr Self   operator * (const Self rhs)        C_NE___ { return Self{ _value * rhs._value }; }
        ND_ constexpr Self   operator / (const Self rhs)        C_NE___ { return Self{ _value / rhs._value }; }

        ND_ constexpr Self   operator + (const T rhs)           C_NE___ { return Self{ _value + rhs }; }
        ND_ constexpr Self   operator - (const T rhs)           C_NE___ { return Self{ _value - rhs }; }
        ND_ constexpr Self   operator * (const T rhs)           C_NE___ { return Self{ _value * rhs }; }
        ND_ constexpr Self   operator / (const T rhs)           C_NE___ { return Self{ _value / rhs }; }

        ND_ constexpr bool  operator == (const Self rhs)        C_NE___ { return _value == rhs._value; }
        ND_ constexpr bool  operator != (const Self rhs)        C_NE___ { return _value != rhs._value; }
        ND_ constexpr bool  operator >  (const Self rhs)        C_NE___ { return _value >  rhs._value; }
        ND_ constexpr bool  operator <  (const Self rhs)        C_NE___ { return _value <  rhs._value; }
        ND_ constexpr bool  operator >= (const Self rhs)        C_NE___ { return _value >= rhs._value; }
        ND_ constexpr bool  operator <= (const Self rhs)        C_NE___ { return _value <= rhs._value; }

        ND_ friend constexpr Self  operator + (T lhs, Self rhs) __NE___ { return Self{ lhs + rhs._value }; }
        ND_ friend constexpr Self  operator - (T lhs, Self rhs) __NE___ { return Self{ lhs - rhs._value }; }
        ND_ friend constexpr Self  operator * (T lhs, Self rhs) __NE___ { return Self{ lhs * rhs._value }; }
        ND_ friend constexpr Self  operator / (T lhs, Self rhs) __NE___ { return Self{ lhs / rhs._value }; }

        ND_ static constexpr Self  FromDeg (T value)            __NE___ { return Self{ DegToRad() * value }; }
    };


    using Rad           = TRadians<float>;
    using RadiansF      = TRadians<float>;
    using RadiansD      = TRadians<double>;

    template <typename T, int I, glm::qualifier Q>
    using TRadianVec    = TVec< TRadians<T>, I, Q >;

    template <typename T, int I>
    using RadianVec     = Vec< TRadians<T>, I >;

    inline static constexpr Rad  Pi = Rad::Pi();

    ND_ constexpr Rad  operator "" _rad (long double value)         __NE___ { return Rad{ Rad::Value_t(value) }; }
    ND_ constexpr Rad  operator "" _rad (unsigned long long value)  __NE___ { return Rad{ Rad::Value_t(value) }; }

    ND_ constexpr Rad  operator "" _deg (long double value)         __NE___ { return Rad::FromDeg( Rad::Value_t(value) ); }
    ND_ constexpr Rad  operator "" _deg (unsigned long long value)  __NE___ { return Rad::FromDeg( Rad::Value_t(value) ); }


/*
=================================================
    Abs
=================================================
*/
    template <typename T>
    ND_ TRadians<T>  Abs (const TRadians<T>& x) __NE___
    {
        return TRadians<T>{ std::abs( T(x) )};
    }

/*
=================================================
    Sin
=================================================
*/
    template <typename T>
    ND_ T  Sin (const TRadians<T>& x) __NE___
    {
        return std::sin( T(x) );
    }

/*
=================================================
    Cos
=================================================
*/
    template <typename T>
    ND_ T  Cos (const TRadians<T>& x) __NE___
    {
        return std::cos( T(x) );
    }

/*
=================================================
    SinCos
=================================================
*/
    template <typename T>
    ND_ Vec<T,2>  SinCos (const TRadians<T>& x) __NE___
    {
        return Vec<T,2>{ std::sin( T(x) ), std::cos( T(x) )};
    }

/*
=================================================
    ASin
=================================================
*/
    template <typename T>
    ND_ TRadians<T>  ASin (const T& x) __NE___
    {
        ASSERT( x >= T{-1} and x <= T{1} );

        return TRadians<T>{std::asin( x )};
    }

/*
=================================================
    ACos
=================================================
*/
    template <typename T>
    ND_ TRadians<T>  ACos (const T& x) __NE___
    {
        ASSERT( x >= T{-1} and x <= T{1} );

        return TRadians<T>{std::acos( x )};
    }

/*
=================================================
    SinH
=================================================
*/
    template <typename T>
    ND_ T  SinH (const TRadians<T>& x) __NE___
    {
        return std::sinh( T(x) );
    }

/*
=================================================
    CosH
=================================================
*/
    template <typename T>
    ND_ T  CosH (const TRadians<T>& x) __NE___
    {
        return std::cosh( T(x) );
    }

/*
=================================================
    ASinH
=================================================
*/
    template <typename T>
    ND_ TRadians<T>  ASinH (const T& x) __NE___
    {
        return TRadians<T>( SignOrZero( x ) * Ln( x + Sqrt( (x*x) + T{1} )));
    }

/*
=================================================
    ACosH
=================================================
*/
    template <typename T>
    ND_ TRadians<T>  ACosH (const T& x) __NE___
    {
        ASSERT( x >= T{1} );
        return TRadians<T>{ Ln( x + Sqrt( (x*x) - T{1} ))};
    }

/*
=================================================
    Tan
=================================================
*/
    template <typename T>
    ND_ T  Tan (const TRadians<T>& x) __NE___
    {
        return std::tan( T(x) );
    }

/*
=================================================
    CoTan
=================================================
*/
    template <typename T>
    ND_ T  CoTan (const TRadians<T>& x) __NE___
    {
        return SafeDiv( T{1}, Tan( x ), T{0} );
    }

/*
=================================================
    TanH
=================================================
*/
    template <typename T>
    ND_ T  TanH (const TRadians<T>& x) __NE___
    {
        return std::tanh( T(x) );
    }

/*
=================================================
    CoTanH
=================================================
*/
    template <typename T>
    ND_ T  CoTanH (const TRadians<T>& x) __NE___
    {
        return SafeDiv( T{1}, TanH( x ), T{0} );
    }

/*
=================================================
    ATan
=================================================
*/
    template <typename T>
    ND_ TRadians<T>  ATan (const T& y_over_x) __NE___
    {
        return TRadians<T>{ std::atan( y_over_x )};
    }

/*
=================================================
    ATan
=================================================
*/
    template <typename T>
    ND_ TRadians<T>  ATan (const T& y, const T& x) __NE___
    {
        return TRadians<T>{ std::atan2( y, x )};
    }

/*
=================================================
    ACoTan
=================================================
*/
    template <typename T>
    ND_ TRadians<T>  ACoTan (const T& x) __NE___
    {
        return TRadians<T>{ SafeDiv( T{1}, ATan( x ), T{0} )};
    }

/*
=================================================
    ATanH
=================================================
*
    template <typename T>
    ND_ Radians<T>  ATanH (const T& x) __NE___
    {
        ASSERT( x > T{-1} and x < T{1} );

        if ( Abs(x) == T{1} )   return Infinity<T>();   else
        if ( Abs(x) > T{1} )    return NaN<T>();        else
                                return Ln( (T{1} + x) / (T{1} - x) ) / T(2);
    }

/*
=================================================
    ACoTanH
=================================================
*/
    template <typename T>
    ND_ TRadians<T>  ACoTanH (const T& x) __NE___
    {
        return TRadians<T>{ SafeDiv( T{1}, ATanH( x ), T{0} )};
    }

/*
=================================================
    Equals
=================================================
*/
    template <typename T>
    ND_ constexpr bool  Equals (const TRadians<T> &lhs, const TRadians<T> &rhs, const T &err = Epsilon<T>()) __NE___
    {
        return Equals( T{lhs}, T{rhs}, err );
    }

/*
=================================================
    IsZero / IsNotZero
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, bool>  IsZero (const TRadians<T> &x) __NE___
    {
        return IsZero( T{x} );
    }

    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, bool>  IsNotZero (const TRadians<T> &x) __NE___
    {
        return IsNotZero( T{x} );
    }


} // AE::Math


namespace AE::Base
{
    template <typename T>   struct TMemCopyAvailable< TRadians<T> >     { static constexpr bool  value = IsMemCopyAvailable<T>; };
    template <typename T>   struct TZeroMemAvailable< TRadians<T> >     { static constexpr bool  value = IsZeroMemAvailable<T>; };
    template <typename T>   struct TTriviallySerializable< TRadians<T> >{ static constexpr bool  value = IsTriviallySerializable<T>; };

} // AE::Base
