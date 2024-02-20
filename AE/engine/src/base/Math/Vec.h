// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/GLM.h"
#include "base/Math/Byte.h"
#include "base/Math/Percent.h"
#include "base/Math/BitMath.h"
#include "base/Math/Bool32.h"
#include "base/Math/Float8.h"
#include "base/Math/Float16.h"
#include "base/Math/Range.h"
#include "base/Algorithms/Cast.h"

namespace glm
{
    using namespace AE::Math;

/*
=================================================
    operator !
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<T,I,Q>>  operator ! (const TVec<T,I,Q> &value) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = !value[i]; // TODO: optimize
        }
        return res;
    }

/*
=================================================
    operator << , operator >>
=================================================
*/
    template <typename T, int I, typename S, glm::qualifier Q>
    ND_ EnableIf<IsScalar<S> and not IsSameTypes<T,S>, TVec<T,I,Q>>  operator << (const TVec<T,I,Q> &lhs, const S rhs) __NE___
    {
        return lhs << static_cast<T>(rhs);
    }

    template <typename T, int I, typename S, glm::qualifier Q>
    ND_ EnableIf<IsScalar<S> and not IsSameTypes<T,S>, TVec<T,I,Q>>  operator >> (const TVec<T,I,Q> &lhs, const S rhs) __NE___
    {
        return lhs >> static_cast<T>(rhs);
    }

/*
=================================================
    operator *, operator /
=================================================
*/
    template <typename T, int I, typename S, glm::qualifier Q>
    ND_ EnableIf<IsScalar<S> and not IsSameTypes<T,S>, TVec<T,I,Q>>  operator * (const TVec<T,I,Q> &lhs, const S rhs) __NE___
    {
        return lhs * static_cast<T>(rhs);
    }

    template <typename T, int I, typename S, glm::qualifier Q>
    ND_ EnableIf<IsScalar<S> and not IsSameTypes<T,S>, TVec<T,I,Q>>  operator / (const TVec<T,I,Q> &lhs, const S rhs) __NE___
    {
        return lhs / static_cast<T>(rhs);
    }

/*
=================================================
    operator %
=================================================
*/
    /*template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  operator % (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return glm::mod( lhs, rhs );
    }*/

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>&>  operator %= (TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return (lhs = glm::mod( lhs, rhs ));
    }

/*
=================================================
    operator == UMax
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator == (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_UMax) __NE___
    {
        return glm::equal( lhs, TVec<T,I,Q>{MaxValue<T>()} );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator != (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_UMax) __NE___
    {
        return glm::notEqual( lhs, TVec<T,I,Q>{MaxValue<T>()} );
    }

/*
=================================================
    operator == Zero
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator == (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
    {
        return glm::equal( lhs, TVec<T,I,Q>{} );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator != (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
    {
        return glm::notEqual( lhs, TVec<T,I,Q>{} );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator >= (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
    {
        return glm::greaterThanEqual( lhs, TVec<T,I,Q>{} );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator <= (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
    {
        return glm::lessThanEqual( lhs, TVec<T,I,Q>{} );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator > (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
    {
        return glm::greaterThan( lhs, TVec<T,I,Q>{} );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator < (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
    {
        return glm::lessThan( lhs, TVec<T,I,Q>{} );
    }

/*
=================================================
    operator <, >, ==, !=
=================================================
*/
    template <typename T, int I, typename S, glm::qualifier Q>
    ND_ EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator == (const TVec<T,I,Q> &lhs, const S rhs) __NE___
    {
        return glm::equal( lhs, TVec<T,I,Q>{rhs} );
    }

    template <typename T, int I, typename S, glm::qualifier Q>
    ND_ EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator != (const TVec<T,I,Q> &lhs, const S rhs) __NE___
    {
        return glm::notEqual( lhs, TVec<T,I,Q>{rhs} );
    }

    template <typename T, int I, typename S, glm::qualifier Q>
    ND_ EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator >= (const TVec<T,I,Q> &lhs, const S rhs) __NE___
    {
        return glm::greaterThanEqual( lhs, TVec<T,I,Q>{rhs} );
    }

    template <typename T, int I, typename S, glm::qualifier Q>
    ND_ EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator <= (const TVec<T,I,Q> &lhs, const S rhs) __NE___
    {
        return glm::lessThanEqual( lhs, TVec<T,I,Q>{rhs} );
    }

    template <typename T, int I, typename S, glm::qualifier Q>
    ND_ EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator > (const TVec<T,I,Q> &lhs, const S rhs) __NE___
    {
        return glm::greaterThan( lhs, TVec<T,I,Q>{rhs} );
    }

    template <typename T, int I, typename S, glm::qualifier Q>
    ND_ EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator < (const TVec<T,I,Q> &lhs, const S rhs) __NE___
    {
        return glm::lessThan( lhs, TVec<T,I,Q>{rhs} );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator >= (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return glm::greaterThanEqual( lhs, rhs );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator <= (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return glm::lessThanEqual( lhs, rhs );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator > (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return glm::greaterThan( lhs, rhs );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<bool,I,Q>  operator < (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return glm::lessThan( lhs, rhs );
    }

} // glm


namespace AE::Math
{

    using bool2             = Vec< bool, 2 >;
    using bool3             = Vec< bool, 3 >;
    using bool4             = Vec< bool, 4 >;

    using packed_bool2      = PackedVec< bool, 2 >;
    using packed_bool3      = PackedVec< bool, 3 >;
    using packed_bool4      = PackedVec< bool, 4 >;

    // large bool
    using lbool             = Bool32;
    using lbool2            = Vec< lbool, 2 >;
    using lbool3            = Vec< lbool, 3 >;
    using lbool4            = Vec< lbool, 4 >;

    using packed_lbool2     = PackedVec< lbool, 2 >;
    using packed_lbool3     = PackedVec< lbool, 3 >;
    using packed_lbool4     = PackedVec< lbool, 4 >;

    using sbyte2            = Vec< sbyte, 2 >;
    using sbyte3            = Vec< sbyte, 3 >;
    using sbyte4            = Vec< sbyte, 4 >;

    using packed_sbyte2     = PackedVec< sbyte, 2 >;
    using packed_sbyte3     = PackedVec< sbyte, 3 >;
    using packed_sbyte4     = PackedVec< sbyte, 4 >;

    using ubyte2            = Vec< ubyte, 2 >;
    using ubyte3            = Vec< ubyte, 3 >;
    using ubyte4            = Vec< ubyte, 4 >;

    using packed_ubyte2     = PackedVec< ubyte, 2 >;
    using packed_ubyte3     = PackedVec< ubyte, 3 >;
    using packed_ubyte4     = PackedVec< ubyte, 4 >;

    using short2            = Vec< short, 2 >;
    using short3            = Vec< short, 3 >;
    using short4            = Vec< short, 4 >;

    using packed_short2     = PackedVec< short, 2 >;
    using packed_short3     = PackedVec< short, 3 >;
    using packed_short4     = PackedVec< short, 4 >;

    using ushort2           = Vec< ushort, 2 >;
    using ushort3           = Vec< ushort, 3 >;
    using ushort4           = Vec< ushort, 4 >;

    using packed_ushort2    = PackedVec< ushort, 2 >;
    using packed_ushort3    = PackedVec< ushort, 3 >;
    using packed_ushort4    = PackedVec< ushort, 4 >;

    using uint2             = Vec< uint, 2 >;
    using uint3             = Vec< uint, 3 >;
    using uint4             = Vec< uint, 4 >;

    using packed_uint2      = PackedVec< uint, 2 >;
    using packed_uint3      = PackedVec< uint, 3 >;
    using packed_uint4      = PackedVec< uint, 4 >;

    using int2              = Vec< int, 2 >;
    using int3              = Vec< int, 3 >;
    using int4              = Vec< int, 4 >;

    using packed_int2       = PackedVec< int, 2 >;
    using packed_int3       = PackedVec< int, 3 >;
    using packed_int4       = PackedVec< int, 4 >;

    using usize2            = Vec< usize, 2 >;
    using usize3            = Vec< usize, 3 >;
    using usize4            = Vec< usize, 4 >;

    using packed_usize2     = PackedVec< usize, 2 >;
    using packed_usize3     = PackedVec< usize, 3 >;
    using packed_usize4     = PackedVec< usize, 4 >;

    using slong2            = Vec< slong, 2 >;
    using slong3            = Vec< slong, 3 >;
    using slong4            = Vec< slong, 4 >;

    using packed_slong2     = PackedVec< slong, 2 >;
    using packed_slong3     = PackedVec< slong, 3 >;
    using packed_slong4     = PackedVec< slong, 4 >;

    using ulong2            = Vec< ulong, 2 >;
    using ulong3            = Vec< ulong, 3 >;
    using ulong4            = Vec< ulong, 4 >;

    using packed_ulong2     = PackedVec< ulong, 2 >;
    using packed_ulong3     = PackedVec< ulong, 3 >;
    using packed_ulong4     = PackedVec< ulong, 4 >;

    using float2            = Vec< float, 2 >;
    using float3            = Vec< float, 3 >;
    using float4            = Vec< float, 4 >;

    using packed_float2     = PackedVec< float, 2 >;
    using packed_float3     = PackedVec< float, 3 >;
    using packed_float4     = PackedVec< float, 4 >;

    using double2           = Vec< double, 2 >;
    using double3           = Vec< double, 3 >;
    using double4           = Vec< double, 4 >;

    using packed_double2    = PackedVec< double, 2 >;
    using packed_double3    = PackedVec< double, 3 >;
    using packed_double4    = PackedVec< double, 4 >;

    using half2             = Vec< half, 2 >;
    using half3             = Vec< half, 3 >;
    using half4             = Vec< half, 4 >;

    using packed_half2      = PackedVec< half, 2 >;
    using packed_half3      = PackedVec< half, 3 >;
    using packed_half4      = PackedVec< half, 4 >;

    using uhalf2            = Vec< uhalf, 2 >;
    using uhalf3            = Vec< uhalf, 3 >;
    using uhalf4            = Vec< uhalf, 4 >;

    using packed_uhalf2     = PackedVec< uhalf, 2 >;
    using packed_uhalf3     = PackedVec< uhalf, 3 >;
    using packed_uhalf4     = PackedVec< uhalf, 4 >;

/*
=================================================
    _VecInfo
=================================================
*/
namespace _hidden_
{
    template <typename T>
    struct _VecInfo {
        using                   type    = void;
        static constexpr bool   is_vec  = false;
        static constexpr bool   is_simd = false;
        static constexpr int    size    = 0;
    };

    template <typename T, int I, glm::qualifier Q>
    struct _VecInfo< TVec<T,I,Q> >
    {
        using                   type    = T;
        static constexpr bool   is_vec  = true;
        static constexpr bool   is_simd = false;
        static constexpr int    size    = I;
    };

    template <typename T, int I>
    struct _VecInfo< TVec<T,I,GLMSimdQualifier> >
    {
        using                   type    = T;
        static constexpr bool   is_vec  = true;
        static constexpr bool   is_simd = true;
        static constexpr int    size    = I;
    };

}
/*
=================================================
    IsVec / IsSimdVec
=================================================
*/
    template <typename T>
    static constexpr bool   IsVec = Math::_hidden_::_VecInfo<T>::is_vec;

    template <typename T>
    static constexpr bool   IsSimdVec = Math::_hidden_::_VecInfo<T>::is_simd;

/*
=================================================
    VecToScalarType
=================================================
*/
    template <typename T>
    using VecToScalarType = typename Math::_hidden_::_VecInfo<T>::type;

/*
=================================================
    IsIntegerVec / IsFloatPointVec
=================================================
*/
    template <typename T>
    static constexpr bool   IsIntegerVec = IsVec<T> and IsInteger< VecToScalarType<T> >;

    template <typename T>
    static constexpr bool   IsFloatPointVec = IsVec<T> and IsFloatPoint< VecToScalarType<T> >;

/*
=================================================
    VecSize
=================================================
*/
    template <typename T>
    static constexpr uint  VecSize = Math::_hidden_::_VecInfo<T>::size;

    namespace _hidden_
    {
        template <typename T>
        static constexpr bool  IsBool = IsSameTypes<T, bool> or IsSameTypes<T, Bool32>;
    }

    template <typename T>
    static constexpr T EulerNumber = T( 2.71828182845904523536 );

/*
=================================================
    AdditionIsSafe
=================================================
*/
    template <typename T1, typename T2>
    ND_ constexpr EnableIf<IsSignedInteger<T1> and IsSignedInteger<T2>, bool>  AdditionIsSafe (const T1 a, const T2 b) __NE___
    {
        StaticAssert( IsScalar<T1> and IsScalar<T2> );

        using T = decltype(a + b);

        const T x   = T(a);
        const T y   = T(b);
        const T min = MinValue<T>();
        const T max = MaxValue<T>();

        bool    overflow =  ((y > 0) and (x > max - y)) or
                            ((y < 0) and (x < min - y));
        return not overflow;
    }

/*
=================================================
    AdditionIsSafe
=================================================
*/
    template <typename T1, typename T2>
    ND_ constexpr EnableIf<IsUnsignedInteger<T1> and IsUnsignedInteger<T2>, bool>  AdditionIsSafe (const T1 a, const T2 b) __NE___
    {
        StaticAssert( IsScalar<T1> and IsScalar<T2> );

        using T = decltype(a + b);

        const T x   = T(a);
        const T y   = T(b);

        return (x + y) >= (x | y);
    }

/*
=================================================
    Square
=================================================
*/
    template <typename T>
    ND_ constexpr T  Square (const T &value) __NE___
    {
        return value * value;
    }

/*
=================================================
    Epsilon
=================================================
*/
    template <typename T>
    ND_ constexpr  EnableIf<IsScalar<T>, T>  Epsilon () __NE___
    {
        return std::numeric_limits<T>::epsilon() * T(2);
    }

/*
=================================================
    All
=================================================
*/
    ND_ forceinline constexpr bool  All (const bool &value) __NE___
    {
        return value;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<Math::_hidden_::IsBool<T>, bool>  All (const TVec<T,I,Q> &v) __NE___
    {
        return glm::all( v );
    }

/*
=================================================
    Any
=================================================
*/
    ND_ forceinline constexpr bool  Any (const bool &value) __NE___
    {
        return value;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<Math::_hidden_::IsBool<T>, bool>  Any (const TVec<T,I,Q> &v) __NE___
    {
        return glm::any( v );
    }

/*
=================================================
    All/Any
=================================================
*/
    template <typename T, typename ...Args>
    ND_ bool  All (const T &arg0, const Args& ...args) __NE___
    {
        if constexpr( sizeof...(Args) )
            return bool{arg0} and All( args... );
        else
            return arg0;
    }

    template <typename T, typename ...Args>
    ND_ bool  Any (const T &arg0, const Args& ...args) __NE___
    {
        if constexpr( sizeof...(Args) )
            return bool{arg0} or Any( args... );
        else
            return arg0;
    }

/*
=================================================
    Sum
=================================================
*/
    template <typename R, typename T, int I, glm::qualifier Q>
    ND_ R  Sum (const TVec<T,I,Q> &v) __NE___
    {
        R   res = R(0);
        for (int i = 0; i < I; ++i) {
            res += R{v[i]};
        }
        return res;
    }

/*
=================================================
    Abs
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, T>  Abs (const T x) __NE___
    {
        return std::abs( x );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<T,I,Q>>  Abs (const TVec<T,I,Q> &value) __NE___
    {
        return glm::abs( value );
    }

/*
=================================================
    Floor / Ceil / Trunc / Fract (scalar)
=================================================
*/
    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Floor (const T x) __NE___
    {
        return std::floor( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Ceil (const T x) __NE___
    {
        return std::ceil( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Trunc (const T x) __NE___
    {
    #if 1
        return std::trunc( x );
    #else
        return x > T{0} ? Floor(x) : Ceil(x);
    #endif
    }

    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  Fract (const T x) __NE___
    {
        return x - Floor( x );
    }

/*
=================================================
    Floor / Ceil / Fract (vec)
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Floor (const TVec<T,I,Q> &value) __NE___
    {
        return glm::floor( value );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Ceil (const TVec<T,I,Q> &value) __NE___
    {
        return glm::ceil( value );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Fract (const TVec<T,I,Q> &value) __NE___
    {
        return glm::fract( value );
    }

/*
=================================================
    Lerp
----
    linear interpolation
=================================================
*/
    template <typename T, typename B>
    ND_ constexpr EnableIf<IsScalar<T>, T>  Lerp (const T x, const T y, const B factor) __NE___
    {
        //return T(factor) * (y - x) + x;
        return x * (T{1} - T(factor)) + y * T(factor);
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Lerp (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs, const T factor) __NE___
    {
        return glm::mix( lhs, rhs, factor );
    }

/*
=================================================
    SLerp
----
    spherical interpolation
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,3,Q>>  SLerp (const TVec<T,3,Q> &lhs, const TVec<T,3,Q> &rhs, const T factor) __NE___
    {
        return glm::slerp( lhs, rhs, factor );
    }

/*
=================================================
    QLerp
----
    quadratic interpolation
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  QLerp (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs, const T factor) __NE___
    {
        return glm::mix( lhs, rhs, glm::smoothstep( T{0}, T{1}, factor ));
    }

/*
=================================================
    Area
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, T>  Area (const TVec<T,2,Q> &v) __NE___
    {
        return v.x * v.y;
    }

    template <typename T, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, T>  Area (const TVec<T,3,Q> &v) __NE___
    {
        return v.x * v.y * v.z;
    }

    template <typename T, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, T>  Area (const TVec<T,4,Q> &v) __NE___
    {
        return v.x * v.y * v.z * v.w;
    }

/*
=================================================
    Dot
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, T>  Dot (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return glm::dot( lhs, rhs );
    }

/*
=================================================
    Cross
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<T,3,Q>>  Cross (const TVec<T,3,Q> &lhs, const TVec<T,3,Q> &rhs) __NE___
    {
        return glm::cross( lhs, rhs );
    }

/*
=================================================
    Equal (Optional)
=================================================
*/
    template <typename T>
    ND_ constexpr bool  Equal (const Optional<T> &lhs, const Optional<T> &rhs) __NE___
    {
        return  lhs.has_value() == rhs.has_value()  and
                (lhs.has_value() ? All( *lhs == *rhs ) : false);
    }

/*
=================================================
    Equal (with absolute error)
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, bool>  Equal (const T lhs, const T rhs, const T err = Epsilon<T>()) __NE___
    {
        if constexpr( IsUnsignedInteger<T> )
        {
            return lhs < rhs ? ((rhs - lhs) <= err) : ((lhs - rhs) <= err);
        }else
            return Abs( lhs - rhs ) <= err;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<bool,I,Q>>  Equal (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs, const T err = Epsilon<T>()) __NE___
    {
        TVec<bool,I,Q>  res;
        for (int i = 0; i < I; ++i) {
            res[i] = Equal( lhs[i], rhs[i], err );
        }
        return res;
    }

/*
=================================================
    Equal (with relative error)
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, bool>  Equal (const T lhs, const T rhs, const Percent err) __NE___
    {
        T   pct = std::abs( std::min( lhs, rhs ) / std::max( lhs, rhs ) - T{1});

        return  std::abs( lhs - rhs ) <= Epsilon<T>() or    // for zero
                pct <= T(err.GetFraction());
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<bool,I,Q>>  Equal (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs, const Percent err) __NE___
    {
        TVec<bool,I,Q>  res;
        for (int i = 0; i < I; ++i) {
            res[i] = Equal( lhs[i], rhs[i], err );
        }
        return res;
    }

/*
=================================================
    BitEqual
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<bool,I,Q>>  BitEqual (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs, const EnabledBitCount bitCount) __NE___
    {
        TVec<bool,I,Q>  res;
        for (int i = 0; i < I; ++i) {
            res[i] = BitEqual( lhs[i], rhs[i], bitCount );
        }
        return res;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<bool,I,Q>>  BitEqual (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        TVec<bool,I,Q>  res;
        for (int i = 0; i < I; ++i) {
            res[i] = BitEqual( lhs[i], rhs[i] );
        }
        return res;
    }

/*
=================================================
    IsZero / IsNotZero
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, bool>  IsZero (const T x) __NE___
    {
        return Equal( x, T{0}, Epsilon<T>() );
    }

    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, bool>  IsNotZero (const T x) __NE___
    {
        return not IsZero( x );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<bool,I,Q>>  IsZero (const TVec<T,I,Q> &x) __NE___
    {
        return Equal( x, TVec<T,I,Q>{T{0}}, Epsilon<T>() );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<bool,I,Q>>  IsNotZero (const TVec<T,I,Q> &x) __NE___
    {
        return not IsZero( x );
    }

/*
=================================================
    Min
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<T,I,Q>  Min (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return glm::min( lhs, rhs );
    }

    template <typename T, int I, typename S, glm::qualifier Q, ENABLEIF( IsScalar<S> )
             >
    ND_ TVec<T,I,Q>  Min (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
    {
        return Min( lhs, TVec<T,I,Q>{rhs} );
    }

    template <typename T, int I, typename S, glm::qualifier Q, ENABLEIF( IsScalar<S> )
             >
    ND_ TVec<T,I,Q>  Min (const S &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return Min( TVec<T,I,Q>{lhs}, rhs );
    }

    template <typename LT, typename RT, ENABLEIF( not IsVec<LT> and not IsVec<RT> )>
    ND_ constexpr auto  Min (const LT &lhs, const RT &rhs) __NE___
    {
        if constexpr( IsSameTypes<LT, RT> )
        {
            return lhs > rhs ? rhs : lhs;
        }
        else
        {
            using T2 = decltype(lhs + rhs);
            return T2(lhs) > T2(rhs) ? T2(rhs) : T2(lhs);
        }
    }

namespace _hidden_
{
    template <typename T0>
    ND_ constexpr auto  _Min (const T0 &arg0) __NE___
    {
        return arg0;
    }

    template <typename T0, typename T1, typename ...Types>
    ND_ constexpr auto  _Min (const T0 &arg0, const T1 &arg1, const Types& ...args) __NE___
    {
        if constexpr( sizeof...(Types) == 0 )
            return Math::Min( arg0, arg1 );
        else
        if constexpr( sizeof...(Types) == 1 )
            return Math::Min( arg0, _Min( arg1, args... ));
        else
            return Math::Min( Math::Min( arg0, arg1 ), _Min( args... ));
    }

} // _hidden_

    template <typename T0, typename ...Types>
    ND_ constexpr auto  Min (const T0 &arg0, const Types& ...args) __NE___
    {
        return Math::_hidden_::_Min( arg0, args... );
    }

/*
=================================================
    Max
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<T,I,Q>  Max (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return glm::max( lhs, rhs );
    }

    template <typename T, int I, typename S, glm::qualifier Q, ENABLEIF( IsScalar<S> )
             >
    ND_ TVec<T,I,Q>  Max (const TVec<T,I,Q> &lhs, const S rhs) __NE___
    {
        return Max( lhs, TVec<T,I,Q>{rhs} );
    }

    template <typename T, int I, typename S, glm::qualifier Q, ENABLEIF( IsScalar<S> )
             >
    ND_ auto  Max (const S lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return Max( TVec<T,I,Q>{lhs}, rhs );
    }

    template <typename LT, typename RT, ENABLEIF( not IsVec<LT> and not IsVec<RT> )>
    ND_ constexpr auto  Max (const LT lhs, const RT rhs) __NE___
    {
        if constexpr( IsSameTypes<LT, RT> )
        {
            return lhs > rhs ? lhs : rhs;
        }
        else
        {
            using T = decltype(lhs + rhs);
            return lhs > rhs ? T(lhs) : T(rhs);
        }
    }

namespace _hidden_
{
    template <typename T0>
    ND_ constexpr auto  _Max (const T0 &arg0) __NE___
    {
        return arg0;
    }

    template <typename T0, typename T1, typename ...Types>
    ND_ constexpr auto  _Max (const T0 &arg0, const T1 &arg1, const Types& ...args) __NE___
    {
        if constexpr( sizeof...(Types) == 0 )
            return Math::Max( arg0, arg1 );
        else
        if constexpr( sizeof...(Types) == 1 )
            return Math::Max( arg0, _Max( arg1, args... ));
        else
            return Math::Max( Math::Max( arg0, arg1 ), _Max( args... ));
    }

} // _hidden_

    template <typename T0, typename ...Types>
    ND_ constexpr auto  Max (const T0 &arg0, const Types& ...args) __NE___
    {
        return Math::_hidden_::_Max( arg0, args... );
    }

/*
=================================================
    AssignMin / AssignMax
=================================================
*/
    template <typename T0, typename T1, typename ...Types>
    void  AssignMin (INOUT T0 &dst, const T1 &arg0, const Types& ...args) __NE___
    {
        dst = Min( dst, arg0, args... );
    }

    template <typename T0, typename T1, typename ...Types>
    void  AssignMax (INOUT T0 &dst, const T1 &arg0, const Types& ...args) __NE___
    {
        dst = Max( dst, arg0, args... );
    }

/*
=================================================
    Clamp
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<T,I,Q>>  Clamp (const TVec<T,I,Q> &value, const TVec<T,I,Q> &minVal, const TVec<T,I,Q> &maxVal) __NE___
    {
        return glm::clamp( value, minVal, maxVal );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<T,I,Q>>  Clamp (const TVec<T,I,Q> &value, const T minVal, const T maxVal) __NE___
    {
        return glm::clamp( value, TVec<T,I,Q>{minVal}, TVec<T,I,Q>{maxVal} );
    }

    template <typename ValT, typename MinT, typename MaxT>
    ND_ constexpr auto  Clamp (const ValT &value, const MinT &minVal, const MaxT &maxVal) __NE___
    {
        ASSERT( All( minVal <= maxVal ));
        return Min( maxVal, Max( value, minVal ));
    }

/*
=================================================
    Saturate
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  Saturate (const T value) __NE___
    {
        return Clamp( value, T{0}, T{1} );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Saturate (const TVec<T,I,Q> &value) __NE___
    {
        return Clamp( value, T{0}, T{1} );
    }

/*
=================================================
    Wrap (float)
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  Wrap (const T value, const T minValue, const T maxValue) __NE___
    {
        // check for NaN
        if_unlikely( minValue >= maxValue )
            return minValue;

        T   result = T( minValue + std::fmod( value - minValue, maxValue - minValue ));

        if ( result < minValue )
            result += (maxValue - minValue);

        return result;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Wrap (const TVec<T,I,Q>& v, const T minValue, const T maxValue) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = Wrap( v[i], minValue, maxValue );
        }
        return res;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Wrap (const TVec<T,I,Q>& v, const TVec<T,I,Q>& minValue, const TVec<T,I,Q>& maxValue) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = Wrap( v[i], minValue[i], maxValue[i] );
        }
        return res;
    }

/*
=================================================
    Wrap (int)
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsInteger<T>, T>  Wrap (const T value, const T minValue, const T maxValue) __NE___
    {
        // check for div by zero
        if_unlikely( minValue > maxValue )
            return minValue;

        T   result = T( minValue + ((value - minValue) % (maxValue - minValue + 1)) );

        if ( result < minValue )
            result += (maxValue - minValue + 1);

        return result;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<T,I,Q>>  Wrap (const TVec<T,I,Q>& v, const T minValue, const T maxValue) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = Wrap( v[i], minValue, maxValue );
        }
        return res;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<T,I,Q>>  Wrap (const TVec<T,I,Q>& v, const TVec<T,I,Q>& minValue, const TVec<T,I,Q>& maxValue) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = Wrap( v[i], minValue[i], maxValue[i] );
        }
        return res;
    }

/*
=================================================
    MirroredWrap (float)
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  MirroredWrap (const T value, const T minValue, const T maxValue) __NE___
    {
        // check for NaN
        if_unlikely( minValue >= maxValue )
            return minValue;

        const T  size   = (maxValue - minValue) * T(2);
        const T  val    = Fract( (value - minValue) / size );

        return Min( val, T(1) - val ) * size + minValue;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  MirroredWrap (const TVec<T,I,Q>& v, const T minValue, const T maxValue) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = MirroredWrap( v[i], minValue, maxValue );
        }
        return res;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  MirroredWrap (const TVec<T,I,Q>& v, const TVec<T,I,Q>& minValue, const TVec<T,I,Q>& maxValue) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = MirroredWrap( v[i], minValue[i], maxValue[i] );
        }
        return res;
    }

/*
=================================================
    MirroredWrap (int)
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsInteger<T>, T>  MirroredWrap (const T value, const T minValue, const T maxValue) __NE___
    {
        // check for division by zero
        if_unlikely( minValue >= maxValue )
            return minValue;

        const T  size   = (maxValue - minValue) * T(2);
        const T  val    = Abs( value - minValue ) % size;

        return Min( val, size - val ) + minValue;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<T,I,Q>>  MirroredWrap (const TVec<T,I,Q>& v, const T minValue, const T maxValue) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = MirroredWrap( v[i], minValue, maxValue );
        }
        return res;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<T,I,Q>>  MirroredWrap (const TVec<T,I,Q>& v, const TVec<T,I,Q>& minValue, const TVec<T,I,Q>& maxValue) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = MirroredWrap( v[i], minValue[i], maxValue[i] );
        }
        return res;
    }

/*
=================================================
    Round / RoundToInt / RoundToUint (scalar)
=================================================
*/
    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Round (const T x) __NE___
    {
        return std::round( x );
    }

    template <typename T>
    ND_ auto  RoundToInt (const T x) __NE___
    {
        StaticAssert( IsFloatPoint<T> );

        if constexpr( sizeof(T) >= sizeof(slong) )
            return slong(std::round( x ));

        if constexpr( sizeof(T) >= sizeof(int32_t) )
            return int32_t(std::round( x ));
    }

    template <typename T>
    ND_ auto  RoundToUint (const T x) __NE___
    {
        StaticAssert( IsFloatPoint<T> );

        if constexpr( sizeof(T) >= sizeof(ulong) )
            return ulong(std::round( x ));

        if constexpr( sizeof(T) >= sizeof(uint) )
            return uint(std::round( x ));
    }

/*
=================================================
    Round / RoundToInt / RoundToUint (vec)
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Round (const TVec<T,I,Q>& v) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = Round( v[i] );
        }
        return res;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ auto  RoundToInt (const TVec<T,I,Q>& v) __NE___
    {
        using R = decltype(RoundToInt(T()));

        TVec<R,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = RoundToInt( v[i] );
        }
        return res;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ auto  RoundToUint (const TVec<T,I,Q>& v) __NE___
    {
        using R = decltype(RoundToUint(T()));

        TVec<R,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = RoundToUint( v[i] );
        }
        return res;
    }

/*
=================================================
    Length
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, T>  Length (const TVec<T,I,Q> &v) __NE___
    {
        return glm::length( v );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, T>  LengthSq (const TVec<T,I,Q> &v) __NE___
    {
        return glm::length2( v );
    }

/*
=================================================
    Distance
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, T>  Distance (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return glm::distance( lhs, rhs );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, T>  DistanceSq (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
    {
        return glm::distance2( lhs, rhs );
    }

/*
=================================================
    Normalize
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Normalize (const TVec<T,I,Q> &v) __NE___
    {
        return glm::normalize( v );
    }

/*
=================================================
    Sign / SignOrZero
----
    extract sign and return same type as input
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, T>  Sign (const T value) __NE___
    {
        if constexpr( IsSigned<T> )
            return value < T{0} ? T{-1} : T{1};
        else
            return T{1};
    }

    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, T>  SignOrZero (const T value) __NE___
    {
        if constexpr( IsSigned<T> )
            return value < T{0} ? T{-1} : value > T{0} ? T{1} : T{0};
        else
            return value > T{0} ? T{1} : T{0};
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<T,I,Q>>  Sign (const TVec<T,I,Q> &value) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = Sign( value[i] );
        }
        return res;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<T,I,Q>>  SignOrZero (const TVec<T,I,Q> &value) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = SignOrZero( value[i] );
        }
        return res;
    }

/*
=================================================
    HasSign
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, bool>  HasSign (const T value) __NE___
    {
        if constexpr( IsSigned<T> )
            return value < T{0};
        else
            return false;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<bool,I,Q>>  HasSign (const TVec<T,I,Q> &value) __NE___
    {
        TVec<bool,I,Q>  res;
        for (int i = 0; i < I; ++i) {
            res[i] = HasSign( value[i] );
        }
        return res;
    }

/*
=================================================
    SafeDiv
=================================================
*/
    template <typename T1, typename T2, typename T3,
              ENABLEIF( IsScalarOrEnum<T1> and IsScalarOrEnum<T2> and IsScalarOrEnum<T3> )
             >
    ND_ constexpr auto  SafeDiv (const T1 lhs, const T2 rhs, const T3 defVal) __NE___
    {
        using T = decltype( lhs + rhs + defVal );

        return IsNotZero( rhs ) ? (T(lhs) / T(rhs)) : T(defVal);
    }

    template <typename T1, typename T2,
              ENABLEIF( IsScalarOrEnum<T1> and IsScalarOrEnum<T2> )
             >
    ND_ constexpr auto  SafeDiv (const T1 lhs, const T2 rhs) __NE___
    {
        return SafeDiv( lhs, rhs, T1{0} );
    }

    template <typename T, int I, typename S, glm::qualifier Q,
              ENABLEIF( IsScalar<S> )
             >
    ND_ TVec<T,I,Q>  SafeDiv (const TVec<T,I,Q>& lhs, const TVec<T,I,Q>& rhs, const S defVal) __NE___
    {
        TVec<T,I,Q> res;
        for (int i = 0; i < I; ++i) {
            res[i] = SafeDiv( lhs[i], rhs[i], defVal );
        }
        return res;
    }

    template <typename T, int I, typename S1, typename S2, glm::qualifier Q,
              ENABLEIF( IsScalar<S1> and IsScalar<S2> )
             >
    ND_ TVec<T,I,Q>  SafeDiv (const TVec<T,I,Q>& lhs, const S1 rhs, const S2 defVal) __NE___
    {
        return SafeDiv( lhs, TVec<T,I,Q>{rhs}, T(defVal) );
    }

    template <typename T, int I, typename S1, glm::qualifier Q,
              ENABLEIF( IsScalar<S1> )
             >
    ND_ TVec<T,I,Q>  SafeDiv (const TVec<T,I,Q>& lhs, const S1 rhs) __NE___
    {
        return SafeDiv( lhs, TVec<T,I,Q>{rhs}, T{0} );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<T,I,Q>  SafeDiv (const TVec<T,I,Q>& lhs, const TVec<T,I,Q>& rhs) __NE___
    {
        return SafeDiv( lhs, rhs, T{0} );
    }

/*
=================================================
    FusedMulAdd
----
    faster and more precise version of (a * b) + c
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  FusedMulAdd (const T a, const T b, const T c) __NE___
    {
        return std::fma( a, b, c );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  FusedMulAdd (const TVec<T,I,Q> &a, const TVec<T,I,Q> &b, const TVec<T,I,Q> &c) __NE___
    {
        return glm::fma( a, b, c );
    }

/*
=================================================
    BaryLerp
----
    barycentric interpolation
=================================================
*/
    template <typename T, typename A, typename B, typename C, glm::qualifier Q>
    ND_ auto  BaryLerp (const A& v0, const B& v1, const C& v2, const TVec<T,3,Q> &barycentrics) __NE___
    {
        StaticAssert( IsFloatPoint<A> and IsFloatPoint<B> and IsFloatPoint<C> and IsFloatPoint<T> );
        return v0 * barycentrics.x + v1 * barycentrics.y + v2 * barycentrics.z;
    }

    template <typename T, typename A, typename B, typename C, glm::qualifier Q>
    ND_ auto  BaryLerp (const A& v0, const B& v1, const C& v2, const TVec<T,2,Q> &barycentrics) __NE___
    {
        StaticAssert( IsFloatPoint<A> and IsFloatPoint<B> and IsFloatPoint<C> and IsFloatPoint<T> );
        return v0 + FusedMulAdd( barycentrics.x, (v1 - v0), barycentrics.y * (v2 - v0));
    }

/*
=================================================
    BiLerp / BiSLerp
----
    bilinear interpolation
=================================================
*/
    template <typename T, typename Coord, glm::qualifier Q>
    ND_ auto  BiLerp (const Coord& x1y1, const Coord& x2y1, const Coord& x1y2, const Coord& x2y2, const TVec<T,2,Q> &factor) __NE___
    {
        return Lerp( Lerp( x1y1, x2y1, factor.x ),
                     Lerp( x1y2, x2y2, factor.x ), factor.y );
    }

    template <typename T, typename Coord, glm::qualifier Q>
    ND_ auto  BiSLerp (const Coord& x1y1, const Coord& x2y1, const Coord& x1y2, const Coord& x2y2, const TVec<T,2,Q> &factor) __NE___
    {
        return SLerp( SLerp( x1y1, x2y1, factor.x ),
                      SLerp( x1y2, x2y2, factor.x ), factor.y );
    }

/*
=================================================
    Remap
=================================================
*/
    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Remap (const Range<T> &src, const Range<T> &dst, const T v) __NE___
    {
        return ((v - src.begin) / src.Size()) * dst.Size() + dst.begin;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Remap (const Range<TVec<T,I,Q>> &src, const Range<TVec<T,I,Q>> &dst, const TVec<T,I,Q>& v) __NE___
    {
        return ((v - src.begin) / src.Size()) * dst.Size() + dst.begin;
    }

/*
=================================================
    RemapClamp
=================================================
*/
    template <typename T>
    ND_ T  RemapClamp (const Range<T> &src, const Range<T> &dst, const T v) __NE___
    {
        return Clamp( Remap( src, dst, v ), dst.begin, dst.end );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  RemapClamp (const Range<TVec<T,I,Q>> &src, const Range<TVec<T,I,Q>> &dst, const TVec<T,I,Q>& v) __NE___
    {
        return Clamp( Remap( src, dst, v ), dst.begin, dst.end );
    }

/*
=================================================
    RemapWrap
=================================================
*/
    template <typename T>
    ND_ T  RemapWrap (const Range<T> &src, const Range<T> &dst, const T v) __NE___
    {
        return Wrap( Remap( src, dst, v ), dst.begin, dst.end );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  RemapWrap (const Range<TVec<T,I,Q>> &src, const Range<TVec<T,I,Q>> &dst, const TVec<T,I,Q>& v) __NE___
    {
        return Wrap( Remap( src, dst, v ), dst.begin, dst.end );
    }

/*
=================================================
    RemapMirroredWrap
=================================================
*/
    template <typename T>
    ND_ T  RemapMirroredWrap (const Range<T> &src, const Range<T> &dst, const T v) __NE___
    {
        return MirroredWrap( Remap( src, dst, v ), dst.begin, dst.end );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  RemapMirroredWrap (const Range<TVec<T,I,Q>> &src, const Range<TVec<T,I,Q>> &dst, const TVec<T,I,Q>& v) __NE___
    {
        return MirroredWrap( Remap( src, dst, v ), dst.begin, dst.end );
    }

/*
=================================================
    ToUNorm / ToSNorm
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  ToUNorm (const T snorm) __NE___
    {
        return snorm * T(0.5) + T(0.5);
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  ToUNorm (const TVec<T,I,Q> &snorm) __NE___
    {
        return snorm * T(0.5) + T(0.5);
    }

    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  ToSNorm (const T unorm) __NE___
    {
        return unorm * T(2.0) - T(1.0);
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  ToSNorm (const TVec<T,I,Q> &unorm) __NE___
    {
        return unorm * T(2.0) - T(1.0);
    }

/*
=================================================
    FloorPOT / CeilPOT
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<T,I,Q>>  FloorPOT (const TVec<T,I,Q> &v) __NE___
    {
        TVec<T,I,Q>     result;
        for (int i = 0; i < I; ++i) {
            result[i] = FloorPOT( v[i] );
        }
        return result;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<T,I,Q>>  CeilPOT (const TVec<T,I,Q> &v) __NE___
    {
        TVec<T,I,Q>     result;
        for (int i = 0; i < I; ++i) {
            result[i] = CeilPOT( v[i] );
        }
        return result;
    }

/*
=================================================
    IntLog2 / CeilIntLog2
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<int,I,Q>>  IntLog2 (const TVec<T,I,Q> &v) __NE___
    {
        TVec<int,I,Q>       result;
        for (int i = 0; i < I; ++i) {
            result[i] = IntLog2( v[i] );
        }
        return result;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<int,I,Q>>  CeilIntLog2 (const TVec<T,I,Q> &v) __NE___
    {
        TVec<int,I,Q>       result;
        for (int i = 0; i < I; ++i) {
            result[i] = CeilIntLog2( v[i] );
        }
        return result;
    }

/*
=================================================
    BitScanForward
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<int,I,Q>>  BitScanForward (const TVec<T,I,Q> &v) __NE___
    {
        TVec<int,I,Q>       result;
        for (int i = 0; i < I; ++i) {
            result[i] = BitScanForward( v[i] );
        }
        return result;
    }

/*
=================================================
    BitCount
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<usize,I,Q>>  BitCount (const TVec<T,I,Q> &v) __NE___
    {
        TVec<usize,I,Q>     result;
        for (int i = 0; i < I; ++i) {
            result[i] = BitCount( v[i] );
        }
        return result;
    }

/*
=================================================
    IsPowerOfTwo
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<bool,I,Q>>  IsPowerOfTwo (const TVec<T,I,Q> &v) __NE___
    {
        TVec<bool,I,Q>  result;
        for (int i = 0; i < I; ++i) {
            result[i] = IsPowerOfTwo( v[i] );
        }
        return result;
    }

/*
=================================================
    SafeLeftBitShift / SafeRightBitShift
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<T,I,Q>>  SafeLeftBitShift (const TVec<T,I,Q> &v) __NE___
    {
        TVec<T,I,Q>     result;
        for (int i = 0; i < I; ++i) {
            result[i] = SafeLeftBitShift( v[i] );
        }
        return result;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<T,I,Q>>  SafeRightBitShift (const TVec<T,I,Q> &v) __NE___
    {
        TVec<T,I,Q>     result;
        for (int i = 0; i < I; ++i) {
            result[i] = SafeRightBitShift( v[i] );
        }
        return result;
    }

/*
=================================================
    Average
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, T>  Average (T begin, T end) __NE___
    {
        if constexpr( IsFloatPoint<T> )
            return (begin * T{0.5}) + (end * T{0.5});
        else
        if constexpr( IsUnsignedInteger<T> )
            return (begin >> 1) + (end >> 1) + (((begin & 1) + (end & 1)) >> 1);
        else
        if constexpr( IsSignedInteger<T> )
            return (begin / 2) + (end / 2) + ((begin % 2) + (end % 2)) / 2;
    }

    template <typename T>
    ND_ constexpr TByte<T>  Average (TByte<T> begin, TByte<T> end) __NE___
    {
        return TByte<T>{ Average( T{begin}, T{end} )};
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ TVec<T,I,Q>  Average (const TVec<T,I,Q> &begin, const TVec<T,I,Q> &end) __NE___
    {
        TVec<T,I,Q>     result;
        for (int i = 0; i < I; ++i) {
            result[i] = Average( begin[i], end[i] );
        }
        return result;
    }

/*
=================================================
    Sqrt
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  Sqrt (const T value) __NE___
    {
        return std::sqrt( value );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Sqrt (const TVec<T,I,Q> &v)
    {
        return glm::sqrt( v );
    }

/*
=================================================
    InvSqrt
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  InvSqrt (const T value) __NE___
    {
        return glm::inversesqrt( value );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  InvSqrt (const TVec<T,I,Q> &v)
    {
        return glm::inversesqrt( v );
    }

/*
=================================================
    Ln / Log / Log2 / Log10 (scalar)
=================================================
*/
    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Ln (const T x) __NE___
    {
        ASSERT( x >= T{0} );
        return std::log( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Log2 (const T x) __NE___
    {
        ASSERT( x >= T{0} );
        return std::log2( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Log10 (const T x) __NE___
    {
        ASSERT( x >= T{0} );
        return std::log10( x );
    }

    template <auto Base, typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Log (const T x) __NE___
    {
        static constexpr auto log_base = std::log( Base );
        return Ln( x ) / log_base;
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Log (const T x, const T base) __NE___
    {
        return Ln( x ) / Ln( base );
    }

/*
=================================================
    Ln / Log / Log2 / Log10 (vec)
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Ln (const TVec<T,I,Q>& v) __NE___
    {
        ASSERT( All( v >= T{0} ));
        return glm::log( v );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Log2 (const TVec<T,I,Q>& v) __NE___
    {
        ASSERT( All( v >= T{0} ));
        return glm::log2( v );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Log (const TVec<T,I,Q>& v, const T base) __NE___
    {
        return Ln( v ) / Ln( base );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Log (const TVec<T,I,Q>& v, const TVec<T,I,Q>& base) __NE___
    {
        return Ln( v ) / Ln( base );
    }

/*
=================================================
    Pow / Exp / Exp2 / Exp10 / ExpMinus1 (scalar)
=================================================
*/
    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Pow (const T base, const T power) __NE___
    {
        ASSERT( base >= T{0} or power == Floor(power) );    // if 'base' < 0 and 'power' not integer then result is NaN
        return std::pow( base, power );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Exp (const T x) __NE___
    {
        return std::exp( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Exp2 (const T x) __NE___
    {
        return std::exp2( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  Exp10 (const T x) __NE___
    {
        return Pow( T(10), x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, T>  ExpMinus1 (const T x) __NE___
    {
        return std::expm1( x );
    }

/*
=================================================
    Pow / Exp / Exp2 / Exp10 (vec)
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Pow (const TVec<T,I,Q>& base, const TVec<T,I,Q>& power) __NE___
    {
        ASSERT( All( base >= T{0} ) or All( power == Floor(power) ));   // if 'base' < 0 and 'power' not integer then result is NaN
        return glm::pow( base, power );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Pow (const TVec<T,I,Q>& base, const T power) __NE___
    {
        ASSERT( All( base >= T{0} ) or power == Floor(power) ); // if 'base' < 0 and 'power' not integer then result is NaN
        return glm::pow( base, TVec<T,I,Q>{power} );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Exp (const TVec<T,I,Q>& v) __NE___
    {
        return glm::exp( v );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Exp2 (const TVec<T,I,Q>& v) __NE___
    {
        return glm::exp2( v );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Exp10 (const TVec<T,I,Q>& v) __NE___
    {
        return Pow( TVec<T,I,Q>{T{10}}, v );
    }

/*
=================================================
    LeftVector / RightVector
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<T,2,Q>>  LeftVector (const TVec<T,2,Q> &v) __NE___
    {
        return TVec<T,2,Q>{ -v.y, v.x };
    }

    template <typename T, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<T,2,Q>>  RightVector (const TVec<T,2,Q> &v) __NE___
    {
        return TVec<T,2,Q>{ v.y, -v.x };
    }

/*
=================================================
    LeftVectorXZ / RightVectorXZ
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<T,3,Q>>  LeftVectorXZ (const TVec<T,3,Q> &v) __NE___
    {
        return TVec<T,3,Q>{ -v.z, v.y, v.x };
    }

    template <typename T, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<T,3,Q>>  RightVectorXZ (const TVec<T,3,Q> &v) __NE___
    {
        return TVec<T,3,Q>{ v.z, v.y, -v.x };
    }

/*
=================================================
    VecToLinear
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, T>  VecToLinear (const TVec<T,I,Q> &pos, const TVec<T,I,Q> &dim) __NE___
    {
        if constexpr( I == 1 )
            return pos.x;

        if constexpr( I == 2 )
            return pos.x + pos.y * dim.x;

        if constexpr( I == 3 )
            return pos.x + (pos.y * dim.x) + (pos.z * dim.x * dim.y);

        if constexpr( I == 4 )
            return pos.x + (pos.y * dim.x) + (pos.z * dim.x * dim.y) + (pos.w * dim.x * dim.y * dim.z);
    }

/*
=================================================
    AlignDown
=================================================
*/
    template <typename T0, typename T1,
              ENABLEIF( IsScalar<T0> or IsBytes<T0> )
             >
    ND_ constexpr auto  AlignDown (const T0 value, const T1 align) __NE___
    {
        StaticAssert( (IsScalar<T0> or IsBytes<T0>) and (IsScalar<T1> or IsBytes<T1>) );
        StaticAssert( not IsFloatPoint<T0> and not IsFloatPoint<T1> );
        ASSERT( align > 0 );

        if constexpr( IsPointer<T0> )
        {
            Bytes   byte_align{ align };
            return BitCast<T0>(usize( (BitCast<usize>(value) / byte_align) * byte_align ));
        }
        else
            return (value / align) * align;
    }

    template <typename T0, typename T1, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T0>, TVec<T0,I,Q>>  AlignDown (const TVec<T0,I,Q> &value, const T1 align) __NE___
    {
        StaticAssert( IsEnum<T1> or IsInteger<T1> or IsIntegerVec<T1> );
        return (value / align) * align;
    }

/*
=================================================
    AlignUp
=================================================
*/
    template <typename T0, typename T1,
              ENABLEIF( IsScalar<T0> or IsBytes<T0> )
             >
    ND_ constexpr auto  AlignUp (const T0 value, const T1 align) __NE___
    {
        StaticAssert( (IsScalar<T0> or IsBytes<T0>) and (IsScalar<T1> or IsBytes<T1>) );
        StaticAssert( not IsFloatPoint<T0> and not IsFloatPoint<T1> );
        ASSERT( align > 0 );

        if constexpr( IsPointer<T0> )
        {
            Bytes   byte_align{ align };
            return BitCast<T0>(usize( ((BitCast<usize>(value) + byte_align-1) / byte_align) * byte_align ));
        }
        else
            return ((value + align-1) / align) * align;
    }

    template <typename T0, typename T1, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T0>, TVec<T0,I,Q>>  AlignUp (const TVec<T0,I,Q> &value, const T1 align) __NE___
    {
        StaticAssert( IsEnum<T1> or IsInteger<T1> or IsIntegerVec<T1> );
        return ((value + (align - T1{1})) / align) * align;
    }

/*
=================================================
    IsMultipleOf
=================================================
*/
    template <typename T0, typename T1>
    ND_ constexpr EnableIf<IsScalar<T0> or IsBytes<T0>, bool>  IsMultipleOf (const T0 value, const T1 align) __NE___
    {
        StaticAssert( IsScalar<T1> or IsBytes<T1> );
        StaticAssert( not IsFloatPoint<T0> and not IsFloatPoint<T1> );
        ASSERT( align > 0 );

        if constexpr( IsPointer<T0> )
        {
            return BitCast<usize>(value) % Bytes{align} == 0;
        }
        else
            return value % align == 0;
    }

    template <typename T0, typename T1, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T0>, TVec<bool,I,Q>>  IsMultipleOf (const TVec<T0,I,Q> &value, const T1 align) __NE___
    {
        StaticAssert( IsEnum<T1> or IsInteger<T1> or IsIntegerVec<T1> );
        return (value % align) == T0{0};
    }

/*
=================================================
    IsOdd / IsEven
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<bool,I,Q>>  IsOdd (const TVec<T,I,Q> &v) __NE___
    {
        TVec<bool,I,Q>      result;
        for (int i = 0; i < I; ++i) {
            result[i] = IsOdd( v[i] );
        }
        return result;
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsInteger<T>, TVec<bool,I,Q>>  IsEven (const TVec<T,I,Q> &v) __NE___
    {
        TVec<bool,I,Q>      result;
        for (int i = 0; i < I; ++i) {
            result[i] = IsEven( v[i] );
        }
        return result;
    }

/*
=================================================
    LinearStep / SmoothStep / BumpStep / SmoothBumpStep
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  LinearStep (const T x, const T edge0, const T edge1) __NE___
    {
        ASSERT( edge0 < edge1 );
        return Saturate( (x - edge0) / (edge1 - edge0) );
    }

    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  SmoothStep (const T x, const T edge0, const T edge1) __NE___
    {
        ASSERT( edge0 < edge1 );
        T t = Saturate( (x - edge0) / (edge1 - edge0) );
        return t * t * (T(3) - T(2) * t);
    }

    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  BumpStep (const T x, const T edge0, const T edge1) __NE___
    {
        ASSERT( edge0 < edge1 );
        return T(1) - Abs( Saturate( (x - edge0) / (edge1 - edge0) ) - T(0.5) ) * T(2);
    }

    template <typename T>
    ND_ constexpr EnableIf<IsFloatPoint<T>, T>  SmoothBumpStep (const T x, const T edge0, const T edge1) __NE___
    {
        T   t = BumpStep( x, edge0, edge1 );
        return t * t * (T(3) - T(2) * t);
    }

/*
=================================================
    LinearStep / SmoothStep / BumpStep / SmoothBumpStep
=================================================
*/
    template <typename T, int I, glm::qualifier Q, typename S>
    ND_ EnableIf<IsFloatPoint<T>, T>  LinearStep (const TVec<T,I,Q>& x, const S edge0, const S edge1) __NE___
    {
        StaticAssert( IsVec<S> or IsScalar<S> );
        ASSERT( All( edge0 < edge1 ));

        return Saturate( (x - edge0) / (edge1 - edge0) );
    }

    template <typename T, int I, glm::qualifier Q, typename S>
    ND_ EnableIf<IsFloatPoint<T>, T>  SmoothStep (const TVec<T,I,Q>& x, const S edge0, const S edge1) __NE___
    {
        StaticAssert( IsVec<S> or IsScalar<S> );
        ASSERT( All( edge0 < edge1 ));

        T t = Saturate( (x - edge0) / (edge1 - edge0) );
        return t * t * (T(3) - T(2) * t);
    }

    template <typename T, int I, glm::qualifier Q, typename S>
    ND_ EnableIf<IsFloatPoint<T>, T>  BumpStep (const TVec<T,I,Q>& x, const S edge0, const S edge1) __NE___
    {
        StaticAssert( IsVec<S> or IsScalar<S> );
        ASSERT( All( edge0 < edge1 ));

        return T(1) - Abs( Saturate( (x - edge0) / (edge1 - edge0) ) - T(0.5) ) * T(2);
    }

    template <typename T, int I, glm::qualifier Q, typename S>
    ND_ EnableIf<IsFloatPoint<T>, T>  SmoothBumpStep (const TVec<T,I,Q>& x, const S edge0, const S edge1) __NE___
    {
        TVec<T,I,Q>  t = BumpStep( x, edge0, edge1 );
        return t * t * (T(3) - T(2) * t);
    }

/*
=================================================
    IsNormalized
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, bool>  IsNormalized (const TVec<T,I,Q> &v) __NE___
    {
        return Abs( Dot( v, v ) - T{1} ) < T{1.0e-4};
    }

/*
=================================================
    Refract
----
    'eta' - relative index of refraction (etaI / etaT)
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Refract (const TVec<T,I,Q> &incident, const TVec<T,I,Q> &normal, const T eta) __NE___
    {
        ASSERT( IsNormalized( normal ));
        return glm::refract( incident, normal, eta );
    }

/*
=================================================
    Reflect
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Reflect (const TVec<T,I,Q> &incident, const TVec<T,I,Q> &normal) __NE___
    {
        ASSERT( IsNormalized( normal ));
        return glm::reflect( incident, normal );
    }

/*
=================================================
    UIndexToUNormFloor
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, ToFloatPoint<T>>  UIndexToUNormFloor (const T index, const T count) __NE___
    {
        // range [0, 1]
        using F = ToFloatPoint<T>;
        return F(index) / F(count - T(1));
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  UIndexToUNormFloor (const TVec<T,I,Q> &index, const T count) __NE___
    {
        // range [0, 1]
        using V = TVec<ToFloatPoint<T>,I,Q>;
        return V(index) / V(count - T(1));
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  UIndexToUNormFloor (const TVec<T,I,Q> &index, const TVec<T,I,Q> &count) __NE___
    {
        // range [0, 1]
        using V = TVec<ToFloatPoint<T>,I,Q>;
        return V(index) / V(count - T(1));
    }

/*
=================================================
    UIndexToUNormRound
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  UIndexToUNormRound (const TVec<T,I,Q> &index, const T count) __NE___
    {
        // range (0, 1)
        using F = ToFloatPoint<T>;
        using V = TVec<F,I,Q>;
        return (V(index) + F(0.5)) / V(count);
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  UIndexToUNormRound (const TVec<T,I,Q> &index, const TVec<T,I,Q> &count) __NE___
    {
        // range (0, 1)
        using F = ToFloatPoint<T>;
        using V = TVec<F,I,Q>;
        return (V(index) + F(0.5)) / V(count);
    }

    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, ToFloatPoint<T>>  UIndexToUNormRound (const T index, const T count) __NE___
    {
        // range (0, 1)
        using F = ToFloatPoint<T>;
        return (F(index) + F(0.5)) / F(count);
    }

/*
=================================================
    UIndexToSNormFloor
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, ToFloatPoint<T>>  UIndexToSNormFloor (const T index, const T count) __NE___
    {
        return ToSNorm( UIndexToUNormFloor( index, count ));
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  UIndexToSNormFloor (const TVec<T,I,Q> &index, const T count) __NE___
    {
        return ToSNorm( UIndexToUNormFloor( index, count ));
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  UIndexToSNormFloor (const TVec<T,I,Q> &index, const TVec<T,I,Q> &count) __NE___
    {
        return ToSNorm( UIndexToUNormFloor( index, count ));
    }

/*
=================================================
    UIndexToSNormRound
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  UIndexToSNormRound (const TVec<T,I,Q> &index, const T count) __NE___
    {
        return ToSNorm( UIndexToUNormRound( index, count ));
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  UIndexToSNormRound (const TVec<T,I,Q> &index, const TVec<T,I,Q> &count) __NE___
    {
        return ToSNorm( UIndexToUNormRound( index, count ));
    }

    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, ToFloatPoint<T>>  UIndexToSNormRound (const T index, const T count) __NE___
    {
        return ToSNorm( UIndexToUNormRound( index, count ));
    }

/*
=================================================
    SIndexToUNormFloor
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, ToFloatPoint<T>>  SIndexToUNormFloor (const T index, const T min, const T max) __NE___
    {
        // range [-1 .. +1]
        using F = ToFloatPoint<T>;
        return F(index - min) / F(max - min - T(1));
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  SIndexToUNormFloor (const TVec<T,I,Q> &index, const T min, const T max) __NE___
    {
        // range [-1 .. +1]
        using V = TVec<ToFloatPoint<T>,I,Q>;
        return V(index - min) / V(max - min - T(1));
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  SIndexToUNormFloor (const TVec<T,I,Q> &index, const TVec<T,I,Q> &min, const TVec<T,I,Q> &max) __NE___
    {
        // range [-1 .. +1]
        using V = TVec<ToFloatPoint<T>,I,Q>;
        return V(index - min) / V(max - min - T(1));
    }

/*
=================================================
    SIndexToUNormRound
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, ToFloatPoint<T>>  SIndexToUNormRound (const T index, const T min, const T max) __NE___
    {
        // range (-1 .. +1)
        using F = ToFloatPoint<T>;
        return (F(index - min) + F(0.5)) / F(max - min);
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  SIndexToUNormRound (const TVec<T,I,Q> &index, const T min, const T max) __NE___
    {
        // range (-1 .. +1)
        using F = ToFloatPoint<T>;
        using V = TVec<F,I,Q>;
        return (V(index - min) + F(0.5)) / V(max - min);
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  SIndexToUNormRound (const TVec<T,I,Q> &index, const TVec<T,I,Q> &min, const TVec<T,I,Q> &max) __NE___
    {
        // range (-1 .. +1)
        using F = ToFloatPoint<T>;
        using V = TVec<F,I,Q>;
        return (V(index - min) + F(0.5)) / V(max - min);
    }

/*
=================================================
    SIndexToSNormFloor
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, ToFloatPoint<T>>  SIndexToSNormFloor (const T index, const T min, const T max) __NE___
    {
        return ToSNorm( SIndexToUNormFloor( index, min, max ));
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  SIndexToSNormFloor (const TVec<T,I,Q> &index, const T min, const T max) __NE___
    {
        return ToSNorm( SIndexToUNormFloor( index, min, max ));
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  SIndexToSNormFloor (const TVec<T,I,Q> &index, const TVec<T,I,Q> &min, const TVec<T,I,Q> &max) __NE___
    {
        return ToSNorm( SIndexToUNormFloor( index, min, max ));
    }

/*
=================================================
    SIndexToSNormRound
=================================================
*/
    template <typename T>
    ND_ constexpr EnableIf<IsScalar<T>, ToFloatPoint<T>>  SIndexToSNormRound (const T index, const T min, const T max) __NE___
    {
        return ToSNorm( SIndexToUNormRound( index, min, max ));
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  SIndexToSNormRound (const TVec<T,I,Q> &index, const T min, const T max) __NE___
    {
        return ToSNorm( SIndexToUNormRound( index, min, max ));
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsScalar<T>, TVec<ToFloatPoint<T>,I,Q>>  SIndexToSNormRound (const TVec<T,I,Q> &index, const TVec<T,I,Q> &min, const TVec<T,I,Q> &max) __NE___
    {
        return ToSNorm( SIndexToUNormRound( index, min, max ));
    }

/*
=================================================
    IsInfinity / IsNaN / IsFinite (scalar)
=================================================
*/
    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, bool>  IsInfinity (const T x) __NE___
    {
        return std::isinf( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, bool>  IsNaN (const T x) __NE___
    {
        return std::isnan( x );
    }

    template <typename T>
    ND_ EnableIf<IsFloatPoint<T>, bool>  IsFinite (const T x) __NE___
    {
        return std::isfinite( x );
    }

/*
=================================================
    IsInfinity / IsNaN / IsFinite (vec)
=================================================
*/
    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<bool,I,Q>>  IsInfinity (const TVec<T,I,Q> &v) __NE___
    {
        return glm::isinf( v );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<bool,I,Q>>  IsNaN (const TVec<T,I,Q> &v) __NE___
    {
        return glm::isnan( v );
    }

    template <typename T, int I, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<bool,I,Q>>  IsFinite (const TVec<T,I,Q> &v) __NE___
    {
        return (v == v);
    }

/*
=================================================
    Exchange
=================================================
*/
    template <typename T1, typename T2>
    ND_ constexpr T1  Exchange (INOUT T1 &lhs, const T2 &rhs) __NE___
    {
        T1  tmp = lhs;
        lhs = rhs;
        return tmp;
    }

/*
=================================================
    DivCeil
=================================================
*/
    template <typename T1, typename T2>
    ND_ constexpr T1  DivCeil (const T1 &x, const T2 &divider) __NE___
    {
        return (x + divider - T1{1}) / divider;
    }

/*
=================================================
    AnyEqual
=================================================
*/
    template <typename Lhs, typename Rhs0, typename ...Rhs>
    ND_ constexpr bool  AnyEqual (const Lhs &lhs, const Rhs0 &rhs0, const Rhs& ...rhs) __NE___
    {
        if constexpr( sizeof... (Rhs) == 0 )
            return All( lhs == rhs0 );
        else
            return All( lhs == rhs0 ) or AnyEqual( lhs, rhs... );
    }

/*
=================================================
    IsIntersects
----
    1D intersection check
=================================================
*/
    template <typename T>
    ND_ constexpr bool  IsIntersects (const T begin1, const T end1,
                                      const T begin2, const T end2) __NE___
    {
        StaticAssert( IsScalar<T> or IsPointer<T> or IsBytes<T> );
        ASSERT( begin1 <= end1 );
        ASSERT( begin2 <= end2 );
        return (end1 > begin2) and (begin1 < end2);
    }

/*
=================================================
    IsCompletelyInside
=================================================
*/
    template <typename T>
    ND_ constexpr bool  IsCompletelyInside (const T largeBlockBegin, const T largeBlockEnd,
                                            const T smallBlockBegin, const T smallBlockEnd) __NE___
    {
        StaticAssert( IsScalar<T> or IsPointer<T> or IsBytes<T> );
        return (smallBlockBegin >= largeBlockBegin) and (smallBlockEnd <= largeBlockEnd);
    }

/*
=================================================
    GetIntersection
=================================================
*/
    template <typename T>
    ND_ constexpr bool  GetIntersection (const T begin1, const T end1,
                                         const T begin2, const T end2,
                                         OUT T& outBegin, OUT T& outEnd) __NE___
    {
        StaticAssert( IsScalar<T> or IsPointer<T> or IsBytes<T> );
        outBegin = Max( begin1, begin2 );
        outEnd   = Min( end1, end2 );
        return outBegin < outEnd;
    }

/*
=================================================
    GetSignificantAxis
=================================================
*/
    template <typename T, glm::qualifier Q>
    ND_ EnableIf<IsFloatPoint<T>, TVec<T,3,Q>>  GetSignificantAxis (const TVec<T,3,Q> &dir) __NE___
    {
        ASSERT( IsNormalized( dir ));
        const auto  a   = Abs( dir );
        const auto  max = Max( a.x, a.y, a.z );
        return  TVec<T,3,Q>{ float(a.x >= max) * Sign( dir.x ),
                             float(a.y >= max) * Sign( dir.y ),
                             float(a.z >= max) * Sign( dir.z ) };
    }

} // AE::Math


namespace AE::Base
{

/*
=================================================
    CheckCast
=================================================
*/
    template <typename To, typename From, int I, glm::qualifier Q>
    ND_ inline To  CheckCast (const TVec<From,I,Q>& src) __NE___
    {
        using T = typename To::value_type;

        To  res;
        for (int i = 0; i < I; ++i) {
            res[i] = CheckCast<T>( src[i] );
        }
        return res;
    }

} // AE::Base


#if AE_FAST_HASH
template <typename T, int I, glm::qualifier Q>
struct std::hash< AE::Math::TVec<T,I,Q> > {
    ND_ size_t  operator () (const AE::Math::TVec<T,I,Q> &value) C_NE___ {
        return size_t(AE::Base::HashOf( value.data(), value.size() * sizeof(T) ));
    }
};

#else
template <typename T, glm::qualifier Q>
struct std::hash< AE::Math::TVec<T,2,Q> > {
    ND_ size_t  operator () (const AE::Math::TVec<T,2,Q> &value) C_NE___ {
        return size_t(AE::Base::HashOf( value.x ) + AE::Base::HashOf( value.y ));
    }
};

template <typename T, glm::qualifier Q>
struct std::hash< AE::Math::TVec<T,3,Q> > {
    ND_ size_t  operator () (const AE::Math::TVec<T,3,Q> &value) C_NE___ {
        return size_t(AE::Base::HashOf( value.x ) + AE::Base::HashOf( value.y ) + AE::Base::HashOf( value.z ));
    }
};

template <typename T, glm::qualifier Q>
struct std::hash< AE::Math::TVec<T,4,Q> > {
    ND_ size_t  operator () (const AE::Math::TVec<T,4,Q> &value) C_NE___ {
        return size_t(AE::Base::HashOf( value.x ) + AE::Base::HashOf( value.y ) + AE::Base::HashOf( value.z ) + AE::Base::HashOf( value.w ));
    }
};
#endif
