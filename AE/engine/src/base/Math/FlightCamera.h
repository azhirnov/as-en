// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Camera.h"
#include "base/Math/Frustum.h"

namespace AE::Math
{

    //
    // Camera for flight simulator
    //

    template <typename T>
    struct TFlightCamera final
    {
    // types
    private:
        using Camera_t      = TCamera<T>;
        using Frustum_t     = TFrustum<T>;
        using Self          = TFlightCamera<T>;
        using Quat_t        = typename Camera_t::Quat_t;
        using Vec2_t        = typename Camera_t::Vec2_t;
        using Vec3_t        = typename Camera_t::Vec3_t;
        using Mat4_t        = typename Camera_t::Mat4_t;
        using Rad_t         = TRadian<T>;
        using Rad3_t        = RadianVec<T, 3>;
        using Transform_t   = typename Camera_t::Transform_t;


    // variables
    private:
        Camera_t    _camera;
        Frustum_t   _frustum;


    // methods
    public:
        TFlightCamera ()                                                    __NE___ {}

        ND_ Frustum_t const&    Frustum ()                                  C_NE___ { return _frustum; }
        ND_ Transform_t const&  Transform ()                                C_NE___ { return _camera.transform; }
        ND_ Vec3_t const&       Position ()                                 C_NE___ { return _camera.transform.position; }
        ND_ Quat_t const&       Orientation ()                              C_NE___ { return _camera.transform.orientation; }
        ND_ Mat4_t const&       Projection ()                               C_NE___ { return _camera.projection; }

        ND_ Mat4_t              ToModelViewProjMatrix ()                    C_NE___ { return _camera.ToModelViewProjMatrix(); }
        ND_ Mat4_t              ToViewProjMatrix ()                         C_NE___ { return _camera.ToViewProjMatrix(); }
        ND_ Mat4_t              ToViewMatrix ()                             C_NE___ { return _camera.ToViewMatrix(); }
        ND_ Mat4_t              ToModelMatrix ()                            C_NE___ { return _camera.ToModelMatrix(); }
        ND_ Mat4_t              ToModelViewMatrix ()                        C_NE___ { return _camera.ToModelViewMatrix(); }

            void    SetPerspective (Rad fovY, T aspect,
                                    T zNear, T zFar,
                                    Bool reverseZ = False{})                __NE___;

            Self&   Rotate (Rad_t yaw, Rad_t pitch, Rad_t roll)             __NE___;
            Self&   Rotate (const Rad3_t &v)                                __NE___ { return Rotate( v.x, v.y, v.z ); }

            Self&   Move (const Vec3_t &delta)                              __NE___;

        ND_ Vec3_t  Transform (const Vec3_t &delta)                         C_NE___;

            Self&   SetPosition (const Vec3_t &pos)                         __NE___;
            Self&   SetRotation (const Quat_t &q)                           __NE___;
    };


/*
=================================================
    SetPerspective
=================================================
*/
    template <typename T>
    void  TFlightCamera<T>::SetPerspective (Rad fovY, T aspect, T zNear, T zFar, Bool reverseZ) __NE___
    {
        _camera.SetPerspective( fovY, aspect, Vec2_t{zNear, zFar}, reverseZ );
        _frustum.Setup( _camera );
    }

/*
=================================================
    SetPosition
=================================================
*/
    template <typename T>
    TFlightCamera<T>&  TFlightCamera<T>::SetPosition (const Vec3_t &pos) __NE___
    {
        _camera.transform.position = pos;
        return *this;
    }

/*
=================================================
    SetRotation
=================================================
*/
    template <typename T>
    TFlightCamera<T>&  TFlightCamera<T>::SetRotation (const Quat_t &q) __NE___
    {
        _camera.transform.orientation = q;
        return *this;
    }

/*
=================================================
    Rotate
=================================================
*/
    template <typename T>
    TFlightCamera<T>&  TFlightCamera<T>::Rotate (Rad_t yaw, Rad_t pitch, Rad_t roll) __NE___
    {
        if ( IsZero( yaw ) & IsZero( pitch ) & IsZero( roll ))
            return *this;

        Quat_t& q = _camera.transform.orientation;

        q = Quat_t::RotateX( pitch ) *
            Quat_t::RotateZ( roll ) *
            Quat_t::RotateY( -yaw ) * q;
        q.Normalize();

        _frustum.Setup( _camera );
        return *this;
    }

/*
=================================================
    Move / Transform
----
    x - forward/backward
    y - side
    z - up/down
=================================================
*/
    template <typename T>
    TFlightCamera<T>&  TFlightCamera<T>::Move (const Vec3_t &delta) __NE___
    {
        _camera.transform.position += Transform( delta );
        return *this;
    }

    template <typename T>
    typename TFlightCamera<T>::Vec3_t
        TFlightCamera<T>::Transform (const Vec3_t &delta) C_NE___
    {
        const Quat_t    view_q  = _camera.Orientation();
        const Vec3_t    up_dir  = _camera.UpDir();
        const Vec3_t    axis_x  = view_q.AxisX();
        const Vec3_t    axis_z  = view_q.AxisZ();
        Vec3_t          pos;

        pos  = axis_x *  delta.y;
        pos += up_dir *  delta.z;
        pos += axis_z * -delta.x;
        return pos;
    }


} // AE::Math
