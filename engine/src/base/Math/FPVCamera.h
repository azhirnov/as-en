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
    struct FPVCameraTempl final
    {
    // types
    private:
        using Camera_t  = CameraTempl<T>;
        using Frustum_t = FrustumTempl<T>;
        using Self      = FPVCameraTempl<T>;
        using Quat_t    = typename Camera_t::Quat_t;
        using Vec2_t    = typename Camera_t::Vec2_t;
        using Vec3_t    = typename Camera_t::Vec3_t;
        using Mat4_t    = typename Camera_t::Mat4_t;
        using Rad_t     = RadiansTempl<T>;
        using Rad3_t    = RadianVec<T,3>;


    // variables
    private:
        Camera_t    _camera;
        Frustum_t   _frustum;


    // methods
    public:
        FPVCameraTempl ()                                           __NE___ {}

        ND_ Camera_t const&     GetCamera ()                        C_NE___ { return _camera; }
        ND_ Frustum_t const&    GetFrustum ()                       C_NE___ { return _frustum; }

        void  SetPerspective (Rad fovY, T aspect, T zNear, T zFar)  __NE___;

        Self&  Rotate (Rad_t horizontal, Rad_t vertical)            __NE___;
        Self&  Rotate (const Rad3_t &v)                             __NE___ { return Rotate( v.x, v.y ); }

        Self&  Move (const Vec3_t &delta)                           __NE___;
        Self&  Move3D (const Vec3_t &delta)                         __NE___;    // free FPS camera

        ND_ Vec3_t  Transform (const Vec3_t &delta)                 C_NE___;
        ND_ Vec3_t  Transform3D (const Vec3_t &delta)               C_NE___;    // free FPS camera

        Self&  SetPosition (const Vec3_t &pos)                      __NE___;
        Self&  SetRotation (const Quat_t &q)                        __NE___;
    };


/*
=================================================
    SetPerspective
=================================================
*/
    template <typename T>
    void  FPVCameraTempl<T>::SetPerspective (Rad fovY, T aspect, T zNear, T zFar) __NE___
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
    FPVCameraTempl<T>&  FPVCameraTempl<T>::Rotate (Rad_t horizontal, Rad_t vertical) __NE___
    {
        horizontal = -horizontal;   // TODO: remove?

        Quat_t& q       = _camera.transform.orientation;
        bool    has_ver = not Equals( vertical, Rad_t(0) );
        bool    has_hor = not Equals( horizontal, Rad_t(0) );

        if ( not (has_hor or has_ver) )
            return *this;

        //_LimitRotation( INOUT horizontal, INOUT vertical );   // TODO

        if ( has_ver )
            q = Quat_t::RotateX( vertical ) * q;

        if ( has_hor )
            q = q * Quat_t::RotateY( horizontal );

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
    FPVCameraTempl<T>&  FPVCameraTempl<T>::Move (const Vec3_t &delta) __NE___
    {
        _camera.transform.position += Transform( delta );
        return *this;
    }

    template <typename T>
    typename FPVCameraTempl<T>::Vec3_t
        FPVCameraTempl<T>::Transform (const Vec3_t &delta) C_NE___
    {
        const Mat4_t    view_mat    = _camera.ToViewMatrix();   // TODO: optimize?
        const Vec3_t    up_dir      = _camera.UpDir();
        const Vec3_t    axis_x      { view_mat[0][0], view_mat[1][0], view_mat[2][0] };
        const Vec3_t    forwards    = -Normalize( Cross( up_dir, axis_x ));
        Vec3_t          pos;

        pos  = forwards * delta.x;
        pos += axis_x   * delta.y;
        pos += up_dir   * delta.z;
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
    FPVCameraTempl<T>&  FPVCameraTempl<T>::Move3D (const Vec3_t &delta) __NE___
    {
        _camera.transform.position += Transform3D( delta );
        return *this;
    }

    template <typename T>
    typename FPVCameraTempl<T>::Vec3_t
        FPVCameraTempl<T>::Transform3D (const Vec3_t &delta) C_NE___
    {
        const Mat4_t    view_mat    = _camera.ToViewMatrix();   // TODO: optimize?
        const Vec3_t    up_dir      = _camera.UpDir();
        const Vec3_t    axis_x      { view_mat[0][0], view_mat[1][0], view_mat[2][0] };
        const Vec3_t    axis_z      { view_mat[0][2], view_mat[1][2], view_mat[2][2] };
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
    FPVCameraTempl<T>&  FPVCameraTempl<T>::SetPosition (const Vec3_t &pos) __NE___
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
    FPVCameraTempl<T>&  FPVCameraTempl<T>::SetRotation (const Quat_t &q) __NE___
    {
        _camera.transform.orientation = q;
        return *this;
    }


} // AE::Math
