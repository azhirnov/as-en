// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Transformation.h"
#include "base/Math/Radians.h"
#include "base/Math/Rectangle.h"

namespace AE::Math
{

    //
    // Camera
    //

    template <typename T>
    struct CameraTempl
    {
    // types
        using Transform_t   = Transformation<T>;
        using Mat4_t        = typename Transform_t::Mat4_t;
        using Vec3_t        = typename Transform_t::Vec3_t;
        using Quat_t        = typename Transform_t::Quat_t;
        using Rect_t        = Rectangle<T>;
        using Vec2_t        = Vec< T, 2 >;
        using Radians_t     = RadiansTempl<T>;
        using Self          = CameraTempl<T>;
        using Value_t       = T;


    // variables
        Transform_t     transform;
        Mat4_t          projection;


    // methods
        CameraTempl ()                                                                      __NE___ {}

        ND_ Quat_t const&   Orientation ()                                                  C_NE___ { return transform.orientation; }

        ND_ Mat4_t  ToModelViewProjMatrix ()                                                C_NE___ { return projection * ToModelViewMatrix(); }
        ND_ Mat4_t  ToViewProjMatrix ()                                                     C_NE___ { return projection * ToViewMatrix(); }
        ND_ Mat4_t  ToViewMatrix ()                                                         C_NE___ { return transform.ToRotationMatrix(); }
        ND_ Mat4_t  ToModelMatrix ()                                                        C_NE___ { return Mat4_t::Translated( transform.position ); }

        ND_ Mat4_t  ToModelViewMatrix ()                                                    C_NE___
        {
            Mat4_t  orient_mat      { transform.orientation };
            Mat4_t  translate_mat   = Mat4_t::Translated( transform.position );
            //Mat4_t scale_mat      = Mat4_t::Scaled( Vec3_t{ transform.scale });
            return orient_mat * translate_mat;
        }


        // for transformation
        Self&   Move (const Vec3_t &delta)                                                  __NE___
        {
            transform.Move( delta );
            return *this;
        }

        Self&  SetPosition (const Vec3_t &value)                                            __NE___
        {
            transform.position = value;
            return *this;
        }

        Self&   Rotate (const Quat_t &delta)                                                __NE___
        {
            transform.Rotate( delta );
            return *this;
        }

        Self&  Rotate (Radians_t angle, const Vec3_t &normal)                               __NE___
        {
            transform.Rotate( Quat_t::Identity().Rotate( angle, normal ));
            return *this;
        }

        Self&  SetOrientation (const Quat_t &value)                                         __NE___
        {
            transform.orientation = value;
            return *this;
        }


        // for projection
        Self&  SetOrtho (const Rect_t &viewport, const Vec2_t &range)                       __NE___
        {
            projection = Mat4_t::Ortho( viewport, range );
            return *this;
        }

        Self&  SetPerspective (Radians_t fovY, Value_t aspect, Value_t zNear)               __NE___
        {
            projection = Mat4_t::InfinitePerspective( fovY, aspect, zNear );
            return *this;
        }

        Self&  SetPerspective (Radians_t fovY, Value_t aspect, const Vec2_t &range)         __NE___
        {
            projection = Mat4_t::Perspective( fovY, aspect, range );
            return *this;
        }

        Self&  SetPerspective (Radians_t fovY, const Vec2_t &viewport, const Vec2_t &range) __NE___
        {
            projection = Mat4_t::Perspective( fovY, viewport, range );
            return *this;
        }

        Self&  SetFrustum (const Rect_t &viewport, const Vec2_t &range)                     __NE___
        {
            projection = Mat4_t::Frustum( viewport, range );
            return *this;
        }

        static const Vec3_t  UpDir ()                                                       __NE___ { return Vec3_t( T(0), T(1), T(0) ); }
        static const Vec3_t  RightDir ()                                                    __NE___ { return Vec3_t( T(1), T(0), T(0) ); }
        static const Vec3_t  ForwardDir ()                                                  __NE___ { return Vec3_t( T(0), T(0), T(1) ); }
    };


} // AE::Math
