// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Camera.h"
#include "base/Math/Frustum.h"
#include "base/Math/Radians.h"

namespace AE::Math
{

    //
    // Camera for first person view
    //

    template <typename T>
    struct TFPVCamera final
    {
    // types
    private:
        using Camera_t      = TCamera<T>;
        using Frustum_t     = TFrustum<T>;
        using Self          = TFPVCamera<T>;
        using Quat_t        = typename Camera_t::Quat_t;
        using Vec2_t        = typename Camera_t::Vec2_t;
        using Vec3_t        = typename Camera_t::Vec3_t;
        using Mat4_t        = typename Camera_t::Mat4_t;
        using Rad_t         = TRadians<T>;
        using Rad2_t        = RadianVec<T,2>;
        using Transform_t   = typename Camera_t::Transform_t;


    // variables
    private:
        Camera_t    _camera;
        Frustum_t   _frustum;

        Rad_t       _vertAngle  {T{0}};


    // methods
    public:
        TFPVCamera ()                                                       __NE___ {}

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

            void    SetPerspective (Rad fovY, T aspect, T zNear, T zFar)    __NE___;

            Self&   Rotate (Rad_t horizontal, Rad_t vertical)               __NE___;
            Self&   Rotate (const Rad2_t &v)                                __NE___ { return Rotate( v.x, v.y ); }

            Self&   Move (const Vec3_t &delta)                              __NE___;
            Self&   Move3D (const Vec3_t &delta)                            __NE___;    // free FPS camera

        ND_ Vec3_t  Transform (const Vec3_t &delta)                         C_NE___;
        ND_ Vec3_t  Transform3D (const Vec3_t &delta)                       C_NE___;    // free FPS camera

            Self&   SetPosition (const Vec3_t &pos)                         __NE___;
            Self&   SetRotation (const Quat_t &q)                           __NE___;

    private:
            void    _LimitRotation (INOUT Rad_t &horizontal, INOUT Rad_t &vertical);
    };


/*
=================================================
    SetPerspective
=================================================
*/
    template <typename T>
    void  TFPVCamera<T>::SetPerspective (Rad fovY, T aspect, T zNear, T zFar) __NE___
    {
        _camera.SetPerspective( fovY, aspect, Vec2_t{zNear, zFar} );
        _frustum.Setup( _camera );
    }

/*
=================================================
    Rotate
=================================================
*/
    template <typename T>
    TFPVCamera<T>&  TFPVCamera<T>::Rotate (Rad_t horizontal, Rad_t vertical) __NE___
    {
        _LimitRotation( INOUT horizontal, INOUT vertical );

        if ( IsZero( vertical ) & IsZero( horizontal ))
            return *this;

        Quat_t&     q = _camera.transform.orientation;

        q = Quat_t::RotateX( vertical ) * q * Quat_t::RotateY( -horizontal );
        q.Normalize();

        _frustum.Setup( _camera );
        return *this;
    }

/*
=================================================
    _LimitRotation
=================================================
*/
    template <typename T>
    void  TFPVCamera<T>::_LimitRotation (INOUT Rad_t &, INOUT Rad_t &vertical)
    {
        static constexpr Rad_t  angle = 80_deg;

        vertical = Clamp( _vertAngle + vertical, -angle, angle ) - _vertAngle;

        _vertAngle += vertical;
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
    TFPVCamera<T>&  TFPVCamera<T>::Move (const Vec3_t &delta) __NE___
    {
        _camera.transform.position += Transform( delta );
        return *this;
    }

    template <typename T>
    typename TFPVCamera<T>::Vec3_t
        TFPVCamera<T>::Transform (const Vec3_t &delta) C_NE___
    {
        const Quat_t    view_q  = _camera.Orientation();
        const Vec3_t    up_dir  = _camera.UpDir();
        const Vec3_t    axis_x  = view_q.AxisX();
        const Vec3_t    forward = -Normalize( Cross( up_dir, axis_x ));
        Vec3_t          pos;

        pos  = forward * delta.x;
        pos += axis_x  * delta.y;
        pos += up_dir  * delta.z;
        return pos;
    }

/*
=================================================
    Move3D / Transform3D
----
    x - forward/backward
    y - side
    z - up/down
=================================================
*/
    template <typename T>
    TFPVCamera<T>&  TFPVCamera<T>::Move3D (const Vec3_t &delta) __NE___
    {
        _camera.transform.position += Transform3D( delta );
        return *this;
    }

    template <typename T>
    typename TFPVCamera<T>::Vec3_t
        TFPVCamera<T>::Transform3D (const Vec3_t &delta) C_NE___
    {
        const Quat_t    view_q  = _camera.Orientation();
        const Vec3_t    up_dir  = _camera.UpDir();
        const Vec3_t    axis_x  = view_q.AxisX();
        const Vec3_t    axis_z  = view_q.AxisZ();
        Vec3_t          pos;

        pos  = axis_z * delta.x;
        pos += axis_x * delta.y;
        pos += up_dir * delta.z;
        return pos;
    }

/*
=================================================
    SetPosition
=================================================
*/
    template <typename T>
    TFPVCamera<T>&  TFPVCamera<T>::SetPosition (const Vec3_t &pos) __NE___
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
    TFPVCamera<T>&  TFPVCamera<T>::SetRotation (const Quat_t &q) __NE___
    {
        _camera.transform.orientation = q;
        return *this;
    }


} // AE::Math
