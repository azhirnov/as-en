// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Radian.h"

namespace AE::Math
{

    //
    // Spherical Coordinates
    //

    template <typename T>
    struct TSpherical
    {
        StaticAssert( IsFloatPoint<T> );

    // types
    public:
        using Value_t   = T;
        using Angle_t   = TRadian< T >;
        using Self      = TSpherical< T >;
        using Vec3_t    = Vec< T, 3 >;


    // variables
    public:
        Angle_t     phi;        // azimuthal angle (horizontal - X)
        Angle_t     theta;      // polar angle (vertical - Y)


    // methods
    public:
        constexpr TSpherical ()                                                 __NE___ {}
        constexpr TSpherical (T phi, T theta)                                   __NE___ : phi{phi}, theta{theta} {}
        constexpr TSpherical (Angle_t phi, Angle_t theta)                       __NE___ : phi{phi}, theta{theta} {}
        constexpr explicit TSpherical (const Vec<T,2> &angle)                   __NE___ : phi{angle.y}, theta{angle.x} {}
        constexpr explicit TSpherical (const Vec<Angle_t, 2> &angle)            __NE___ : phi{angle.y}, theta{angle.x} {}

        ND_ constexpr explicit operator Vec<T,2> ()                             C_NE___ { return Vec<T,2>{ T{phi}, T{theta} }; }
        ND_ constexpr explicit operator Vec<Angle_t,2> ()                       C_NE___ { return Vec<Angle_t,2>{ phi, theta }; }

        ND_ constexpr Self  operator + (const Self &rhs)                        C_NE___;
        ND_ constexpr Self  operator - (const Self &rhs)                        C_NE___;

        ND_ static Pair<Self, Value_t>  FromCartesian (const Vec3_t &cartesian) __NE___;
        ND_ Vec3_t                      ToCartesian ()                          C_NE___;
        ND_ Vec3_t                      ToCartesian (Value_t radius)            C_NE___;
    };


    using Spherical     = TSpherical< float >;
    using SphericalD    = TSpherical< double >;



/*
=================================================
    operator +, operator -
=================================================
*/
    template <typename T>
    constexpr TSpherical<T>  TSpherical<T>::operator + (const Self &rhs) C_NE___
    {
        // TODO: wrap?
        return Self{ phi + rhs.phi, theta + rhs.theta };
    }

    template <typename T>
    constexpr TSpherical<T>  TSpherical<T>::operator - (const Self &rhs) C_NE___
    {
        // TODO: wrap?
        return Self{ phi - rhs.phi, theta - rhs.theta };
    }

/*
=================================================
    FromCartesian
=================================================
*/
    template <typename T>
    Pair<TSpherical<T>, T>  TSpherical<T>::FromCartesian (const Vec<T,3> &cartesian) __NE___
    {
        const T     radius  = Length( cartesian );
        const T     len     = IsZero( radius ) ? T{1} : radius;
        const auto  c       = cartesian / len;

        TSpherical<T>   spherical;
        spherical.theta = ACos( c.y );
        spherical.phi   = ATan( c.z, c.x );

        return { spherical, radius };
    }

/*
=================================================
    ToCartesian
=================================================
*/
    template <typename T>
    Vec<T,3>  TSpherical<T>::ToCartesian () C_NE___
    {
        Vec<T,3>    cartesian;
        const T     sin_t   = Sin( theta );

        cartesian.x = sin_t * Cos( phi );
        cartesian.y = Cos( theta );
        cartesian.z = sin_t * Sin( phi );
        return cartesian;
    }

    template <typename T>
    Vec<T,3>  TSpherical<T>::ToCartesian (T radius) C_NE___
    {
        return ToCartesian() * radius;
    }

/*
=================================================
    Lerp
=================================================
*/
    template <typename T>
    TSpherical<T>  Lerp (const TSpherical<T> &lhs, const TSpherical<T> &rhs, const T factor) __NE___
    {
        TSpherical<T>   result;
        result.theta    = Math::Lerp( lhs.theta, rhs.theta, factor );
        result.phi      = Math::Lerp( lhs.phi,   rhs.phi,   factor );
        return result;
    }

/*
=================================================
    Equal / BitEqual
=================================================
*/
    template <typename T>
    ND_ bool2  Equal (const TSpherical<T> &lhs, const TSpherical<T> &rhs, const T err = Epsilon<T>()) __NE___
    {
        return bool2{ Math::Equal( lhs.phi, rhs.phi, err ),
                      Math::Equal( lhs.theta, rhs.theta, err )};
    }

    template <typename T>
    ND_ bool2  Equal (const TSpherical<T> &lhs, const TSpherical<T> &rhs, const Percent err) __NE___
    {
        return bool2{ Math::Equal( lhs.phi, rhs.phi, err ),
                      Math::Equal( lhs.theta, rhs.theta, err )};
    }

    template <typename T>
    ND_ bool2  BitEqual (const TSpherical<T> &lhs, const TSpherical<T> &rhs, const EnabledBitCount bitCount) __NE___
    {
        return bool2{ Math::BitEqual( lhs.phi, rhs.phi, bitCount ),
                      Math::BitEqual( lhs.theta, rhs.theta, bitCount )};
    }

    template <typename T>
    ND_ bool2  BitEqual (const TSpherical<T> &lhs, const TSpherical<T> &rhs) __NE___
    {
        return bool2{ Math::BitEqual( lhs.phi, rhs.phi ),
                      Math::BitEqual( lhs.theta, rhs.theta )};
    }


} // AE::Math


namespace AE::Base
{
    template <typename T>   struct TMemCopyAvailable< TSpherical<T> >       { static constexpr bool  value = IsMemCopyAvailable<T>; };
    template <typename T>   struct TZeroMemAvailable< TSpherical<T> >       { static constexpr bool  value = IsZeroMemAvailable<T>; };
    template <typename T>   struct TTriviallySerializable< TSpherical<T> >  { static constexpr bool  value = IsTriviallySerializable<T>; };

} // AE::Base
