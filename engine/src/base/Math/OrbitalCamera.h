// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Camera.h"
#include "base/Math/Frustum.h"
#include "base/Math/Radians.h"

namespace AE::Math
{

    //
    // Camera for third person view
    //

    template <typename T>
    struct OrbitalCameraTempl final
    {
    // types
    private:
        using Camera_t      = CameraTempl<T>;
        using Frustum_t     = FrustumTempl<T>;
        using Self          = OrbitalCameraTempl<T>;
        using Quat_t        = typename Camera_t::Quat_t;
        using Vec2_t        = typename Camera_t::Vec2_t;
        using Vec3_t        = typename Camera_t::Vec3_t;
        using Mat4_t        = typename Camera_t::Mat4_t;
        using Rad_t         = RadiansTempl<T>;
        using Rad2_t        = RadianVec<T,2>;
        using Transform_t   = typename Camera_t::Transform_t;


    // variables
    private:
        Camera_t    _camera;
        T           _offset     = T{0};
        Frustum_t   _frustum;


    // methods
    public:
        OrbitalCameraTempl ()                                               __NE___ {}

        ND_ Frustum_t const&    Frustum ()                                  C_NE___ { return _frustum; }
        ND_ Vec3_t              Position ()                                 C_NE___ { return CameraPosition() + CameraOffset(); }
        ND_ Quat_t const&       Orientation ()                              C_NE___ { return _camera.transform.orientation; }
        ND_ Mat4_t const&       Projection ()                               C_NE___ { return _camera.projection; }

        ND_ Vec3_t              CameraOffset ()                             C_NE___ { return Orientation() * Vec3_t{ T{0}, T{0}, -_offset }; }
        ND_ Vec3_t const&       CameraPosition ()                           C_NE___ { return _camera.transform.position; }

        ND_ Mat4_t              ToModelViewProjMatrix ()                    C_NE___ { return Projection() * ToModelViewMatrix(); }
        ND_ Mat4_t              ToViewProjMatrix ()                         C_NE___ { return Projection() * ToViewMatrix(); }
        ND_ Mat4_t              ToViewMatrix ()                             C_NE___ { return Mat4_t{ Orientation().Inversed() }; }
        ND_ Mat4_t              ToModelMatrix ()                            C_NE___ { return _camera.ToModelMatrix(); }
        ND_ Mat4_t              ToModelViewMatrix ()                        C_NE___ { return ToViewMatrix() * ToModelMatrix(); }

            void    SetPerspective (Rad fovY, T aspect, T zNear, T zFar)    __NE___;

            void    SetOffset (T value)                                     __NE___ { _offset = Max( value, T{0} ); }
            void    AddOffset (T delta)                                     __NE___ { _offset = Max( _offset + delta, T{0} ); }

            Self&   Rotate (Rad_t horizontal, Rad_t vertical)               __NE___;
            Self&   Rotate (const Rad2_t &v)                                __NE___ { return Rotate( v.x, v.y ); }

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
    void  OrbitalCameraTempl<T>::SetPerspective (Rad fovY, T aspect, T zNear, T zFar) __NE___
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
    OrbitalCameraTempl<T>&  OrbitalCameraTempl<T>::Rotate (Rad_t horizontal, Rad_t vertical) __NE___
    {
        if ( IsZero( vertical ) & IsZero( horizontal ))
            return *this;

        Quat_t&     q = _camera.transform.orientation;

        q = Quat_t::RotateY( horizontal ) * q;
        q = Quat_t::Rotate( vertical, q * Vec3_t{ T{1}, T{0}, T{0} }) * q;
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
*
    template <typename T>
    OrbitalCameraTempl<T>&  OrbitalCameraTempl<T>::Move (const Vec3_t &delta) __NE___
    {
        _camera.transform.position += Transform( delta );
        return *this;
    }

    template <typename T>
    typename OrbitalCameraTempl<T>::Vec3_t
        OrbitalCameraTempl<T>::Transform (const Vec3_t &delta) C_NE___
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
    SetPosition
=================================================
*/
    template <typename T>
    OrbitalCameraTempl<T>&  OrbitalCameraTempl<T>::SetPosition (const Vec3_t &pos) __NE___
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
    OrbitalCameraTempl<T>&  OrbitalCameraTempl<T>::SetRotation (const Quat_t &q) __NE___
    {
        _camera.transform.orientation = q;
        return *this;
    }


} // AE::Math
