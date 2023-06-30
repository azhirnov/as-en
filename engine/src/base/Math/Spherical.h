// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Radians.h"

namespace AE::Math
{

    //
    // Spherical Coordinates
    //

    template <typename T>
    struct SphericalTempl
    {
        STATIC_ASSERT( IsFloatPoint<T> );

    // types
    public:
        using Value_t   = T;
        using Angle_t   = RadiansTempl< T >;
        using Self      = SphericalTempl< T >;
        using Vec3_t    = Vec< T, 3 >;


    // variables
    public:
        Angle_t     theta;      // polar angle (vertical)
        Angle_t     phi;        // azimuthal angle (horizontal)


    // methods
    public:
        constexpr SphericalTempl ()                                             __NE___ {}
        constexpr SphericalTempl (T theta, T phi)                               __NE___ : theta{theta}, phi{phi} {}
        constexpr SphericalTempl (Angle_t theta, Angle_t phi)                   __NE___ : theta{theta}, phi{phi} {}
        constexpr explicit SphericalTempl (const Vec<T,2> &angle)               __NE___ : theta{angle.x}, phi{angle.y} {}
        constexpr explicit SphericalTempl (const Vec<Angle_t, 2> &angle)        __NE___ : theta{angle.x}, phi{angle.y} {}

        ND_ constexpr explicit operator Vec<T,2> ()                             C_NE___ { return Vec<T,2>{ T(theta), T(phi) }; }
        ND_ constexpr explicit operator Vec<Angle_t,2> ()                       C_NE___ { return Vec<Angle_t,2>{ theta, phi }; }

        ND_ constexpr Self  operator + (const Self &rhs)                        C_NE___;
        ND_ constexpr Self  operator - (const Self &rhs)                        C_NE___;

        ND_ static Pair<Self, Value_t>  FromCartesian (const Vec3_t &cartesian) __NE___;
        ND_ Vec3_t                      ToCartesian ()                          C_NE___;
        ND_ Vec3_t                      ToCartesian (Value_t radius)            C_NE___;
    };


    using SphericalF    = SphericalTempl< float >;
    using SphericalD    = SphericalTempl< double >;



/*
=================================================
    operator +, operator -
=================================================
*/
    template <typename T>
    inline constexpr SphericalTempl<T>  SphericalTempl<T>::operator + (const Self &rhs) C_NE___
    {
        return Self{ theta + rhs.theta, phi + rhs.phi };
    }

    template <typename T>
    inline constexpr SphericalTempl<T>  SphericalTempl<T>::operator - (const Self &rhs) C_NE___
    {
        return Self{ theta - rhs.theta, phi - rhs.phi };
    }

/*
=================================================
    FromCartesian
=================================================
*/
    template <typename T>
    inline Pair<SphericalTempl<T>, T>  SphericalTempl<T>::FromCartesian (const Vec<T,3> &cartesian) __NE___
    {
        const T radius  = Length( cartesian );
        const T len     = Equals( radius, T{0} ) ? T{1} : radius;

        SphericalTempl<T>   spherical{  ACos( cartesian.z / len ),
                                        ATan( cartesian.y, len )};
        return { spherical, radius };
    }

/*
=================================================
    ToCartesian
=================================================
*/
    template <typename T>
    inline Vec<T,3>  SphericalTempl<T>::ToCartesian () C_NE___
    {
        Vec<T,3>    cartesian;

        const T     sin_phi = Sin( phi );
        cartesian.x = sin_phi * Cos( theta );
        cartesian.y = sin_phi * Sin( theta );
        cartesian.z = Cos( phi );

        return cartesian;
    }

    template <typename T>
    inline Vec<T,3>  SphericalTempl<T>::ToCartesian (T radius) C_NE___
    {
        return ToCartesian() * radius;
    }


} // AE::Math
