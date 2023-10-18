// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Quat.h"
#include "base/Math/Radians.h"
#include "base/Math/Rectangle.h"

namespace AE::Math
{
    using half2x2   = Matrix< half, 2, 2 >;
    using half2x3   = Matrix< half, 2, 3 >;
    using half2x4   = Matrix< half, 2, 4 >;
    using half3x2   = Matrix< half, 3, 2 >;
    using half3x3   = Matrix< half, 3, 3 >;
    using half3x4   = Matrix< half, 3, 4 >;
    using half4x2   = Matrix< half, 4, 2 >;
    using half4x3   = Matrix< half, 4, 3 >;
    using half4x4   = Matrix< half, 4, 4 >;

    using float2x2  = Matrix< float, 2, 2 >;
    using float2x3  = Matrix< float, 2, 3 >;
    using float2x4  = Matrix< float, 2, 4 >;
    using float3x2  = Matrix< float, 3, 2 >;
    using float3x3  = Matrix< float, 3, 3 >;
    using float3x4  = Matrix< float, 3, 4 >;
    using float4x2  = Matrix< float, 4, 2 >;
    using float4x3  = Matrix< float, 4, 3 >;
    using float4x4  = Matrix< float, 4, 4 >;

    using double2x2 = Matrix< double, 2, 2 >;
    using double2x3 = Matrix< double, 2, 3 >;
    using double2x4 = Matrix< double, 2, 4 >;
    using double3x2 = Matrix< double, 3, 2 >;
    using double3x3 = Matrix< double, 3, 3 >;
    using double3x4 = Matrix< double, 3, 4 >;
    using double4x2 = Matrix< double, 4, 2 >;
    using double4x3 = Matrix< double, 4, 3 >;
    using double4x4 = Matrix< double, 4, 4 >;


    using packed_half2x2    = PackedMatrix< half, 2, 2 >;
    using packed_half2x3    = PackedMatrix< half, 2, 3 >;
    using packed_half2x4    = PackedMatrix< half, 2, 4 >;
    using packed_half3x2    = PackedMatrix< half, 3, 2 >;
    using packed_half3x3    = PackedMatrix< half, 3, 3 >;
    using packed_half3x4    = PackedMatrix< half, 3, 4 >;
    using packed_half4x2    = PackedMatrix< half, 4, 2 >;
    using packed_half4x3    = PackedMatrix< half, 4, 3 >;
    using packed_half4x4    = PackedMatrix< half, 4, 4 >;

    using packed_float2x2   = PackedMatrix< float, 2, 2 >;
    using packed_float2x3   = PackedMatrix< float, 2, 3 >;
    using packed_float2x4   = PackedMatrix< float, 2, 4 >;
    using packed_float3x2   = PackedMatrix< float, 3, 2 >;
    using packed_float3x3   = PackedMatrix< float, 3, 3 >;
    using packed_float3x4   = PackedMatrix< float, 3, 4 >;
    using packed_float4x2   = PackedMatrix< float, 4, 2 >;
    using packed_float4x3   = PackedMatrix< float, 4, 3 >;
    using packed_float4x4   = PackedMatrix< float, 4, 4 >;

    using packed_double2x2  = PackedMatrix< double, 2, 2 >;
    using packed_double2x3  = PackedMatrix< double, 2, 3 >;
    using packed_double2x4  = PackedMatrix< double, 2, 4 >;
    using packed_double3x2  = PackedMatrix< double, 3, 2 >;
    using packed_double3x3  = PackedMatrix< double, 3, 3 >;
    using packed_double3x4  = PackedMatrix< double, 3, 4 >;
    using packed_double4x2  = PackedMatrix< double, 4, 2 >;
    using packed_double4x3  = PackedMatrix< double, 4, 3 >;
    using packed_double4x4  = PackedMatrix< double, 4, 4 >;

namespace _hidden_
{
    struct _MatrixDim
    {
        ubyte       columns;
        ubyte       rows;

        constexpr _MatrixDim (ubyte c, ubyte r)                 __NE___ : columns{c}, rows{r} {}

        ND_ constexpr bool  operator == (const _MatrixDim &rhs) C_NE___ { return columns == rhs.columns and rows == rhs.rows; }
    };
}
} // AE::Math


#define Columns 2
#define Rows    2
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 2
#define Rows    3
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 2
#define Rows    4
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 3
#define Rows    2
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 3
#define Rows    3
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 3
#define Rows    4
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 4
#define Rows    2
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 4
#define Rows    3
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows

#define Columns 4
#define Rows    4
#include "MatrixImpl.h"
#undef  Columns
#undef  Rows


namespace AE::Math
{
/*
=================================================
    operator *
=================================================
*/
    template <typename T, uint C, uint R, uint Q, glm::qualifier Qf>
    ND_ TMatrix<T,Q,R,Qf>  operator * (const TMatrix<T,C,R, Qf> &lhs, const TMatrix<T,Q,C,Qf> &rhs) __NE___
    {
        return TMatrix<T,Q,R,Qf>{ lhs._value * rhs._value };
    }

/*
=================================================
    Equals
=================================================
*/
    template <typename T, uint C, uint R, glm::qualifier Q>
    ND_ bool  Equals (const TMatrix<T,C,R,Q> &lhs, const TMatrix<T,C,R,Q> &rhs, const T err = Epsilon<T>()) __NE___
    {
        uint    eq = 1;
        for (uint i = 0; i < C; ++i) {
            eq &= All( Equals( lhs[i], rhs[i], err ));
        }
        return eq == 1;
    }

    template <typename T, uint C, uint R, glm::qualifier Q>
    ND_ bool  Equals (const TMatrix<T,C,R,Q> &lhs, const TMatrix<T,C,R,Q> &rhs, const Percent err) __NE___
    {
        uint    eq = 1;
        for (uint i = 0; i < C; ++i) {
            eq &= All( Equals( lhs[i], rhs[i], err ));
        }
        return eq == 1;
    }

/*
=================================================
    BitEqual
=================================================
*/
    template <typename T, uint C, uint R, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, bool>  BitEqual (const TMatrix<T,C,R,Q> &lhs, const TMatrix<T,C,R,Q> &rhs, const EnabledBitCount bitCount) __NE___
    {
        uint    eq = 1;
        for (uint i = 0; i < C; ++i) {
            eq &= All( BitEqual( lhs[i], rhs[i], bitCount ));
        }
        return eq == 1;
    }

    template <typename T, uint C, uint R, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, bool>  BitEqual (const TMatrix<T,C,R,Q> &lhs, const TMatrix<T,C,R,Q> &rhs) __NE___
    {
        uint    eq = 1;
        for (uint i = 0; i < C; ++i) {
            eq &= uint(All( BitEqual( lhs[i], rhs[i] )));
        }
        return eq == 1;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    TQuat()
=================================================
*/
    template <typename T, glm::qualifier Q>
    TQuat<T,Q>::TQuat (const TMatrix<T,3,3,Q> &m) __NE___ : _value{glm::quat_cast(m)} {}

    template <typename T, glm::qualifier Q>
    TQuat<T,Q>::TQuat (const TMatrix<T,4,4,Q> &m) __NE___ : _value{glm::quat_cast(m)} {}

/*
=================================================
    Axis*
=================================================
*/
    template <typename T, glm::qualifier Q>
    typename TQuat<T,Q>::Vec3_t  TQuat<T,Q>::AxisX () C_NE___
    {
        Mat3_t  mat {*this};    // TODO: optimize
        return Vec3_t( mat[0][0], mat[1][0], mat[2][0] );
    }

    template <typename T, glm::qualifier Q>
    typename TQuat<T,Q>::Vec3_t  TQuat<T,Q>::AxisY () C_NE___
    {
        Mat3_t  mat {*this};    // TODO: optimize
        return Vec3_t( mat[0][1], mat[1][1], mat[2][1] );
    }

    template <typename T, glm::qualifier Q>
    typename TQuat<T,Q>::Vec3_t  TQuat<T,Q>::AxisZ () C_NE___
    {
        Mat3_t  mat {*this};    // TODO: optimize
        return Vec3_t( mat[0][2], mat[1][2], mat[2][2] );
    }


} // AE::Math
