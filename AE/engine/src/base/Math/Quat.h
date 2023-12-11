// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Radian.h"

namespace AE::Math
{

    //
    // Quaternion
    //

    template <typename T, glm::qualifier Q>
    struct TQuat final
    {
        StaticAssert( IsScalar<T> and IsFloatPoint<T> );

    // types
    public:
        using Value_t       = T;
        using Self          = TQuat< T, Q >;
        using _GLM_Quat_t   = glm::qua< T, Q >;
        using Vec2_t        = TVec< T, 2, Q >;
        using Vec3_t        = TVec< T, 3, Q >;
        using Vec4_t        = TVec< T, 4, Q >;
        using Rad_t         = TRadian< T >;
        using Rad3_t        = RadianVec< T, 3 >;
        using Mat3_t        = TMatrix< T, 3, 3, Q >;
        using Mat4_t        = TMatrix< T, 4, 4, Q >;


    // variables
    public:
        _GLM_Quat_t     _value;


    // methods
    public:
        TQuat ()                                                    __NE___ : _value{} {}
        TQuat (const Self &other)                                   __NE___ : _value{other._value} {}

        explicit TQuat (const _GLM_Quat_t &val)                     __NE___ : _value{val} {}

        template <typename B>
        explicit TQuat (const TQuat<B,Q> &other)                    __NE___ : _value{other._value} {}

        TQuat (Self &&other)                                        __NE___ : _value{other._value} {}

        TQuat (T w, T x, T y, T z)                                  __NE___ : _value{ w, x, y, z } {}

        explicit TQuat (const Rad3_t &eulerAngles)                  __NE___ : _value{ Vec3_t{ T(eulerAngles.x), T(eulerAngles.y), T(eulerAngles.z) }} {}

        explicit TQuat (const Mat3_t &m)                            __NE___;
        explicit TQuat (const Mat4_t &m)                            __NE___;

            Self&   Inverse ()                                      __NE___ { _value = glm::inverse( _value );  return *this; }
        ND_ Self    Inversed ()                                     C_NE___ { return Self{ glm::inverse( _value )}; }

            Self&   Normalize ()                                    __NE___ { _value = glm::normalize( _value );  return *this; }
        ND_ Self    Normalized ()                                   C_NE___ { return Self{ glm::normalize( _value )}; }

            Self&   operator = (const Self &rhs)                    __NE___ { _value = rhs._value;  return *this; }

        ND_ Value_t& operator [] (usize index)                      __NE___ { return _value[index]; }
        ND_ Value_t  operator [] (usize index)                      C_NE___ { return _value[index]; }

            Self&   operator += (const Self &rhs)                   __NE___ { _value += rhs._value;  return *this; }
            Self&   operator -= (const Self &rhs)                   __NE___ { _value -= rhs._value;  return *this; }
            Self&   operator *= (const Self &rhs)                   __NE___ { _value *= rhs._value;  return *this; }

            Self&   operator *= (T rhs)                             __NE___ { _value *= rhs;  return *this; }
            Self&   operator /= (T rhs)                             __NE___ { _value /= rhs;  return *this; }

        ND_ Self    operator + ()                                   C_NE___ { return *this; }
        ND_ Self    operator - ()                                   C_NE___ { return Self{ -_value }; }

        ND_ Self    operator + (const Self &rhs)                    C_NE___ { return Self{ _value + rhs._value }; }
        ND_ Self    operator - (const Self &rhs)                    C_NE___ { return Self{ _value - rhs._value }; }
        ND_ Self    operator * (const Self &rhs)                    C_NE___ { return Self{ _value * rhs._value }; }

        ND_ Vec3_t  operator * (const Vec3_t &rhs)                  C_NE___ { return _value * rhs; }
        ND_ Vec4_t  operator * (const Vec4_t &rhs)                  C_NE___ { return _value * rhs; }

        ND_ Self    operator * (T rhs)                              C_NE___ { return Self{ _value * rhs }; }
        ND_ Self    operator / (T rhs)                              C_NE___ { return Self{ _value / rhs }; }

        ND_ bool4   operator == (const Self &rhs)                   C_NE___;
        ND_ bool4   operator != (const Self &rhs)                   C_NE___ { return not (*this == rhs); }

        ND_ friend Self operator * (T lhs, const Self &rhs)         __NE___ { return Self{ lhs * rhs._value }; }

        ND_ Rad3_t  ToEuler ()                                      C_NE___ { return Rad3_t{glm::eulerAngles( _value )}; }
        ND_ Rad_t   ToEulerX ()                                     C_NE___ { return Rad_t{glm::pitch( _value )}; }
        ND_ Rad_t   ToEulerY ()                                     C_NE___ { return Rad_t{glm::yaw( _value )}; }
        ND_ Rad_t   ToEulerZ ()                                     C_NE___ { return Rad_t{glm::roll( _value )}; }

        ND_ Self    Conjugate ()                                    C_NE___ { return Self{glm::conjugate( _value )}; }
        ND_ T       Length ()                                       C_NE___ { return glm::length( _value ); }

        ND_ Vec3_t  ToDirection ()                                  C_NE___;
        ND_ Vec3_t  AxisX ()                                        C_NE___;
        ND_ Vec3_t  AxisY ()                                        C_NE___;
        ND_ Vec3_t  AxisZ ()                                        C_NE___;

        ND_ static Self  Identity ()                                __NE___ { return Self{glm::quat_identity<T,Q>()}; }

        ND_ static Self  Rotate (Rad_t angle, const Vec3_t &axis)   __NE___ { return Self{glm::rotate( Identity()._value, T(angle), axis )}; }
        ND_ static Self  RotateX (Rad_t angle)                      __NE___ { return Rotate( angle, Vec3_t{T{1}, T{0}, T{0}} ); }
        ND_ static Self  RotateY (Rad_t angle)                      __NE___ { return Rotate( angle, Vec3_t{T{0}, T{1}, T{0}} ); }
        ND_ static Self  RotateZ (Rad_t angle)                      __NE___ { return Rotate( angle, Vec3_t{T{0}, T{0}, T{1}} ); }
        ND_ static Self  Rotate (const Rad3_t &angle)               __NE___ { return RotateX( angle.x ) * RotateY( angle.y ) * RotateZ( angle.z ); }
        ND_ static Self  Rotate2 (const Rad3_t &angle)              __NE___;

        ND_ static Self  FromDirection (const Vec3_t &dir, const Vec3_t &up) __NE___;
    };

    using Quat          = TQuat< float, GLMSimdQualifier >;
    using PackedQuat    = TQuat< float, GLMPackedQualifier >;


/*
=================================================
    Dot
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ T  Dot (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs) __NE___
    {
        return glm::dot( lhs._value, rhs._value );
    }

/*
=================================================
    Cross
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ TQuat<T,Q>  Cross (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs) __NE___
    {
        return TQuat<T,Q>{ glm::cross( lhs._value, rhs._value )};
    }

/*
=================================================
    operator ==
=================================================
*/
    template <typename T, glm::qualifier Q>
    bool4  TQuat<T,Q>::operator == (const Self &rhs) C_NE___
    {
        return bool4{ _value.x == rhs._value.x,
                      _value.y == rhs._value.y,
                      _value.z == rhs._value.z,
                      _value.w == rhs._value.w };
    }

/*
=================================================
    Equal
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ bool4  Equal (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const T err = Epsilon<T>()) __NE___
    {
        return bool4{
                Math::Equal( lhs._value.x, rhs._value.x, err ),
                Math::Equal( lhs._value.y, rhs._value.y, err ),
                Math::Equal( lhs._value.z, rhs._value.z, err ),
                Math::Equal( lhs._value.w, rhs._value.w, err )};
    }

    template <typename T, glm::qualifier Q>
    ND_ bool4  Equal (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const Percent err) __NE___
    {
        return bool4{
                Math::Equal( lhs._value.x, rhs._value.x, err ),
                Math::Equal( lhs._value.y, rhs._value.y, err ),
                Math::Equal( lhs._value.z, rhs._value.z, err ),
                Math::Equal( lhs._value.w, rhs._value.w, err )};
    }

/*
=================================================
    BitEqual
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ bool4  BitEqual (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const EnabledBitCount bitCount) __NE___
    {
        return bool4{
                Math::BitEqual( lhs._value.x, rhs._value.x, bitCount ),
                Math::BitEqual( lhs._value.y, rhs._value.y, bitCount ),
                Math::BitEqual( lhs._value.z, rhs._value.z, bitCount ),
                Math::BitEqual( lhs._value.w, rhs._value.w, bitCount )};
    }

    template <typename T, glm::qualifier Q>
    ND_ bool4  BitEqual (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs) __NE___
    {
        return bool4{
                Math::BitEqual( lhs._value.x, rhs._value.x ),
                Math::BitEqual( lhs._value.y, rhs._value.y ),
                Math::BitEqual( lhs._value.z, rhs._value.z ),
                Math::BitEqual( lhs._value.w, rhs._value.w )};
    }

/*
=================================================
    IsInfinity / IsNaN / IsFinite
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ bool4  IsInfinity (const TQuat<T,Q> &q) __NE___
    {
        return glm::isinf( q );
    }

    template <typename T, glm::qualifier Q>
    ND_ bool4  IsNaN (const TQuat<T,Q> &q) __NE___
    {
        return glm::isnan( q );
    }

    template <typename T, glm::qualifier Q>
    ND_ bool4  IsFinite (const TQuat<T,Q> &q) __NE___
    {
        return (q == q);
    }

/*
=================================================
    Lerp / SLerp
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ TQuat<T,Q>  Lerp (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const T factor) __NE___
    {
        return TQuat<T,Q>{glm::lerp( lhs._value, rhs._value, factor )};
    }

    template <typename T, glm::qualifier Q>
    ND_ TQuat<T,Q>  SLerp (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const T a) __NE___
    {
        return TQuat<T,Q>{glm::slerp( lhs._value, rhs._value, a )};
    }

    template <typename T, glm::qualifier Q>
    ND_ TQuat<T,Q>  SLerp (const TQuat<T,Q> &lhs, const TQuat<T,Q> &rhs, const T a, const T k) __NE___
    {
        return TQuat<T,Q>{glm::slerp( lhs._value, rhs._value, a, k )};
    }

/*
=================================================
    ToDirection / FromDirection
=================================================
*/
    template <typename T, glm::qualifier Q>
    typename TQuat<T,Q>::Vec3_t  TQuat<T,Q>::ToDirection () C_NE___
    {
        return Vec3_t{  T{2} * _value.x * _value.z + T{2} * _value.y * _value.w,
                        T{2} * _value.z * _value.y - T{2} * _value.x * _value.w,
                        T{1} - T{2} * _value.x * _value.x - T{2} * _value.y * _value.y };
    }

    template <typename T, glm::qualifier Q>
    TQuat<T,Q>  TQuat<T,Q>::FromDirection (const Vec3_t &dir, const Vec3_t &up) __NE___
    {
        return Self{glm::quatLookAt( dir, up )};
    }

/*
=================================================
    Rotate2
=================================================
*/
    template <typename T, glm::qualifier Q>
    TQuat<T,Q>  TQuat<T,Q>::Rotate2 (const Rad3_t &angle) __NE___
    {
        const Vec2_t    scr  = SinCos( angle.x * T(0.5) );
        const Vec2_t    scp  = SinCos( angle.y * T(0.5) );
        const Vec2_t    scy  = SinCos( angle.z * T(0.5) );

        const T         cpcy = scp[1] * scy[1];
        const T         spcy = scp[0] * scy[1];
        const T         cpsy = scp[1] * scy[0];
        const T         spsy = scp[0] * scy[0];

        Self            result;

        result._value.x = scr[0] * cpcy - scr[1] * spsy;
        result._value.y = scr[1] * spcy + scr[0] * cpsy;
        result._value.z = scr[1] * cpsy - scr[0] * spcy;
        result._value.w = scr[1] * cpcy + scr[0] * spsy;

        return result.Normalize();
    }


} // AE::Math
