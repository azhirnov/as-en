// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Transformation.h"

namespace AE::Math
{
    template <typename T> struct AxisAlignedBoundingBox;



    //
    // Sphere
    //

    template <typename T>
    struct BoundingSphere
    {
    // types
        using Self      = BoundingSphere<T>;
        using Vec3_t    = Vec<T,3>;
        using Value_t   = T;


    // variables
        Vec3_t      center  {T{0}};
        Value_t     radius  = T{0};


    // methods
        BoundingSphere ()                                               __NE___ {}
        BoundingSphere (const Vec3_t &center, T radius)                 __NE___ : center{center}, radius{abs(radius)} {}


        Self&  Move (const Vec3_t &delta)                               __NE___ { center += delta;  return *this; }
        Self&  Scale (T scale)                                          __NE___ { radius *= Abs(scale);  return *this; }


        Self&  Transform (const Transformation<T> &tr)                  __NE___
        {
            center  = tr.ToGlobalPosition( center );
            radius *= tr.scale;
            return *this;
        }

        ND_ bool  IsIntersects (const BoundingSphere<T> &other)         C_NE___
        {
            const T     dist = radius + other.radius;
            constexpr T err  = Epsilon<T>();

            return  Distance2( center, other.center ) < (dist * dist + err);
        }

        ND_ bool  IsIntersects (const AxisAlignedBoundingBox<T> &aabb)  C_NE___;

        ND_ AxisAlignedBoundingBox<T>  ToAABB ()                        C_NE___;
    };


    using Sphere = BoundingSphere<float>;

} // AE::Math
