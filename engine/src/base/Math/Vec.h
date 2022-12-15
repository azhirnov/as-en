// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Math.h"
#include "base/Math/BitMath.h"
#include "base/Math/GLM.h"
#include "base/Math/Bool32.h"
#include "base/Math/Float8.h"
#include "base/Math/Float16.h"
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
	ND_ forceinline EnableIf<IsScalar<T>, TVec<T,I,Q>>  operator ! (const TVec<T,I,Q> &value) __NE___
	{
		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = !value[i];	// TODO: optimize
		}
		return res;
	}
	
/*
=================================================
	operator << , operator >>
=================================================
*/
	template <typename T, int I, typename S, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<S> and not IsSameTypes<T,S>, TVec<T,I,Q>>  operator << (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return lhs << static_cast<T>(rhs);
	}

	template <typename T, int I, typename S, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<S> and not IsSameTypes<T,S>, TVec<T,I,Q>>  operator >> (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return lhs >> static_cast<T>(rhs);
	}
	
/*
=================================================
	operator *, oeprator /
=================================================
*/
	template <typename T, int I, typename S, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<S> and not IsSameTypes<T,S>, TVec<T,I,Q>>  operator * (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return lhs * static_cast<T>(rhs);
	}

	template <typename T, int I, typename S, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<S> and not IsSameTypes<T,S>, TVec<T,I,Q>>  operator / (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return lhs / static_cast<T>(rhs);
	}

/*
=================================================
	operator %
=================================================
*/
	/*template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  operator % (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return glm::mod( lhs, rhs );
	}*/
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T> and IsFloatPoint<T>, TVec<T,I,Q>&>  operator %= (TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return (lhs = glm::mod( lhs, rhs ));
	}

/*
=================================================
	operator == UMax
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator == (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_UMax) __NE___
	{
		return glm::equal( lhs, TVec<T,I,Q>{MaxValue<T>()} );
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator != (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_UMax) __NE___
	{
		return glm::notEqual( lhs, TVec<T,I,Q>{MaxValue<T>()} );
	}

/*
=================================================
	operator == Zero
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator == (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
	{
		return glm::equal( lhs, TVec<T,I,Q>{} );
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator != (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
	{
		return glm::notEqual( lhs, TVec<T,I,Q>{} );
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator >= (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
	{
		return glm::greaterThanEqual( lhs, TVec<T,I,Q>{} );
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator <= (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
	{
		return glm::lessThanEqual( lhs, TVec<T,I,Q>{} );
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator > (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
	{
		return glm::greaterThan( lhs, TVec<T,I,Q>{} );
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator < (const TVec<T,I,Q> &lhs, AE::Base::_hidden_::_Zero) __NE___
	{
		return glm::lessThan( lhs, TVec<T,I,Q>{} );
	}

/*
=================================================
	operator <, >, ==, !=
=================================================
*/
	template <typename T, int I, typename S, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator == (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return glm::equal( lhs, TVec<T,I,Q>{rhs} );
	}
	
	template <typename T, int I, typename S, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator != (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return glm::notEqual( lhs, TVec<T,I,Q>{rhs} );
	}

	template <typename T, int I, typename S, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator >= (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return glm::greaterThanEqual( lhs, TVec<T,I,Q>{rhs} );
	}

	template <typename T, int I, typename S, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator <= (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return glm::lessThanEqual( lhs, TVec<T,I,Q>{rhs} );
	}
	
	template <typename T, int I, typename S, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator > (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return glm::greaterThan( lhs, TVec<T,I,Q>{rhs} );
	}
	
	template <typename T, int I, typename S, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<S>, TVec<bool,I,Q>>  operator < (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return glm::lessThan( lhs, TVec<T,I,Q>{rhs} );
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator >= (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return glm::greaterThanEqual( lhs, rhs );
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator <= (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return glm::lessThanEqual( lhs, rhs );
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator > (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return glm::greaterThan( lhs, rhs );
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<bool,I,Q>  operator < (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return glm::lessThan( lhs, rhs );
	}

} // glm


namespace AE::Math
{

	using bool2				= Vec< bool, 2 >;
	using bool3				= Vec< bool, 3 >;
	using bool4				= Vec< bool, 4 >;
	
	using packed_bool2		= PackedVec< bool, 2 >;
	using packed_bool3		= PackedVec< bool, 3 >;
	using packed_bool4		= PackedVec< bool, 4 >;
	
	// large bool
	using lbool				= Bool32;
	using lbool2			= Vec< lbool, 2 >;
	using lbool3			= Vec< lbool, 3 >;
	using lbool4			= Vec< lbool, 4 >;
	
	using packed_lbool2		= PackedVec< lbool, 2 >;
	using packed_lbool3		= PackedVec< lbool, 3 >;
	using packed_lbool4		= PackedVec< lbool, 4 >;

	using sbyte2			= Vec< sbyte, 2 >;
	using sbyte3			= Vec< sbyte, 3 >;
	using sbyte4			= Vec< sbyte, 4 >;
	
	using packed_sbyte2		= PackedVec< sbyte, 2 >;
	using packed_sbyte3		= PackedVec< sbyte, 3 >;
	using packed_sbyte4		= PackedVec< sbyte, 4 >;

	using ubyte2			= Vec< ubyte, 2 >;
	using ubyte3			= Vec< ubyte, 3 >;
	using ubyte4			= Vec< ubyte, 4 >;
	
	using packed_ubyte2		= PackedVec< ubyte, 2 >;
	using packed_ubyte3		= PackedVec< ubyte, 3 >;
	using packed_ubyte4		= PackedVec< ubyte, 4 >;

	using short2			= Vec< short, 2 >;
	using short3			= Vec< short, 3 >;
	using short4			= Vec< short, 4 >;
	
	using packed_short2		= PackedVec< short, 2 >;
	using packed_short3		= PackedVec< short, 3 >;
	using packed_short4		= PackedVec< short, 4 >;

	using ushort2			= Vec< ushort, 2 >;
	using ushort3			= Vec< ushort, 3 >;
	using ushort4			= Vec< ushort, 4 >;
	
	using packed_ushort2	= PackedVec< ushort, 2 >;
	using packed_ushort3	= PackedVec< ushort, 3 >;
	using packed_ushort4	= PackedVec< ushort, 4 >;

	using uint2				= Vec< uint, 2 >;
	using uint3				= Vec< uint, 3 >;
	using uint4				= Vec< uint, 4 >;
	
	using packed_uint2		= PackedVec< uint, 2 >;
	using packed_uint3		= PackedVec< uint, 3 >;
	using packed_uint4		= PackedVec< uint, 4 >;

	using int2				= Vec< int, 2 >;
	using int3				= Vec< int, 3 >;
	using int4				= Vec< int, 4 >;
	
	using packed_int2		= PackedVec< int, 2 >;
	using packed_int3		= PackedVec< int, 3 >;
	using packed_int4		= PackedVec< int, 4 >;

	using usize2			= Vec< usize, 2 >;
	using usize3			= Vec< usize, 3 >;
	using usize4			= Vec< usize, 4 >;
	
	using packed_usize2		= PackedVec< usize, 2 >;
	using packed_usize3		= PackedVec< usize, 3 >;
	using packed_usize4		= PackedVec< usize, 4 >;

	using slong2			= Vec< slong, 2 >;
	using slong3			= Vec< slong, 3 >;
	using slong4			= Vec< slong, 4 >;
	
	using packed_slong2		= PackedVec< slong, 2 >;
	using packed_slong3		= PackedVec< slong, 3 >;
	using packed_slong4		= PackedVec< slong, 4 >;

	using ulong2			= Vec< ulong, 2 >;
	using ulong3			= Vec< ulong, 3 >;
	using ulong4			= Vec< ulong, 4 >;
	
	using packed_ulong2		= PackedVec< ulong, 2 >;
	using packed_ulong3		= PackedVec< ulong, 3 >;
	using packed_ulong4		= PackedVec< ulong, 4 >;

	using float2			= Vec< float, 2 >;
	using float3			= Vec< float, 3 >;
	using float4			= Vec< float, 4 >;
	
	using packed_float2		= PackedVec< float, 2 >;
	using packed_float3		= PackedVec< float, 3 >;
	using packed_float4		= PackedVec< float, 4 >;

	using double2			= Vec< double, 2 >;
	using double3			= Vec< double, 3 >;
	using double4			= Vec< double, 4 >;
	
	using packed_double2	= PackedVec< double, 2 >;
	using packed_double3	= PackedVec< double, 3 >;
	using packed_double4	= PackedVec< double, 4 >;

	using half2				= Vec< half, 2 >;
	using half3				= Vec< half, 3 >;
	using half4				= Vec< half, 4 >;
	
	using packed_half2		= PackedVec< half, 2 >;
	using packed_half3		= PackedVec< half, 3 >;
	using packed_half4		= PackedVec< half, 4 >;
	
	using uhalf2			= Vec< uhalf, 2 >;
	using uhalf3			= Vec< uhalf, 3 >;
	using uhalf4			= Vec< uhalf, 4 >;
	
	using packed_uhalf2		= PackedVec< uhalf, 2 >;
	using packed_uhalf3		= PackedVec< uhalf, 3 >;
	using packed_uhalf4		= PackedVec< uhalf, 4 >;

/*
=================================================
	IsVec
=================================================
*/
	namespace _hidden_
	{
		template <typename T>
		struct _IsVec {
			static constexpr bool	value = false;
		};

		template <typename T, int I, glm::qualifier Q>
		struct _IsVec< TVec<T,I,Q> > {
			static constexpr bool	value = true;
		};

	} // _hidden_

	template <typename T>
	static constexpr bool  IsVec = Math::_hidden_::_IsVec<T>::value;
	
/*
=================================================
	VecSize
=================================================
*/
	namespace _hidden_
	{
		template <typename T>
		struct _VecSize {
		};

		template <typename T, int I, glm::qualifier Q>
		struct _VecSize< TVec<T,I,Q> > {
			static constexpr int	value = I;
		};

	} // _hidden_

	template <typename T>
	static constexpr uint  VecSize = Math::_hidden_::_VecSize<T>::value;
	
	namespace _hidden_
	{
		template <typename T>
		static constexpr bool  IsBool = IsSameTypes<T, bool> or IsSameTypes<T, Bool32>;
	}

/*
=================================================
	All
----
	should be faster than glm implementation
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ forceinline EnableIf<Math::_hidden_::IsBool<T>, bool>  All (const TVec<T,2,Q> &v) __NE___
	{
		return v.x & v.y;
	}
	
	template <typename T, glm::qualifier Q>
	ND_ forceinline EnableIf<Math::_hidden_::IsBool<T>, bool>  All (const TVec<T,3,Q> &v) __NE___
	{
		return v.x & v.y & v.z;
	}
	
	template <typename T, glm::qualifier Q>
	ND_ forceinline EnableIf<Math::_hidden_::IsBool<T>, bool>  All (const TVec<T,4,Q> &v) __NE___
	{
		return v.x & v.y & v.z & v.w;
	}
	
/*
=================================================
	Any
----
	should be faster than glm implementation
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ forceinline EnableIf<Math::_hidden_::IsBool<T>, bool>  Any (const TVec<T,2,Q> &v) __NE___
	{
		return v.x | v.y;
	}
	
	template <typename T, glm::qualifier Q>
	ND_ forceinline EnableIf<Math::_hidden_::IsBool<T>, bool>  Any (const TVec<T,3,Q> &v) __NE___
	{
		return v.x | v.y | v.z;
	}
	
	template <typename T, glm::qualifier Q>
	ND_ forceinline EnableIf<Math::_hidden_::IsBool<T>, bool>  Any (const TVec<T,4,Q> &v) __NE___
	{
		return v.x | v.y | v.z | v.w;
	}

/*
=================================================
	Sum
=================================================
*/
	template <typename R, typename T, int I, glm::qualifier Q>
	ND_ forceinline R  Sum (const TVec<T,I,Q> &v) __NE___
	{
		R	res = R(0);
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
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T>, TVec<T,I,Q>>  Abs (const TVec<T,I,Q> &value) __NE___
	{
		return glm::abs( value );
	}

/*
=================================================
	Floor/Ceil/Fract
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T> and IsFloatPoint<T>, TVec<T,I,Q>>  Floor (const TVec<T,I,Q> &value) __NE___
	{
		return glm::floor( value );
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T> and IsFloatPoint<T>, TVec<T,I,Q>>  Ceil (const TVec<T,I,Q> &value) __NE___
	{
		return glm::ceil( value );
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T> and IsFloatPoint<T>, TVec<T,I,Q>>  Fract (const TVec<T,I,Q> &value) __NE___
	{
		return glm::fract( value );
	}

/*
=================================================
	Lerp
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T>, TVec<T,I,Q>>  Lerp (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs, const T factor) __NE___
	{
		return glm::mix( lhs, rhs, factor );
	}

/*
=================================================
	Area
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ forceinline T  Area (const TVec<T,2,Q> &v) __NE___
	{
		return v.x * v.y;
	}
	
	template <typename T, glm::qualifier Q>
	ND_ forceinline T  Area (const TVec<T,3,Q> &v) __NE___
	{
		return v.x * v.y * v.z;
	}
	
	template <typename T, glm::qualifier Q>
	ND_ forceinline T  Area (const TVec<T,4,Q> &v) __NE___
	{
		return v.x * v.y * v.z * v.w;
	}
	
/*
=================================================
	Dot
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T>, T>  Dot (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return glm::dot( lhs, rhs );
	}

/*
=================================================
	Cross
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T>, TVec<T,3,Q>>  Cross (const TVec<T,3,Q> &lhs, const TVec<T,3,Q> &rhs) __NE___
	{
		return glm::cross( lhs, rhs );
	}

/*
=================================================
	Equals
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T>, TVec<bool,I,Q>>  Equals (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs, const T &err = Epsilon<T>()) __NE___
	{
		TVec<bool,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = Equals( lhs[i], rhs[i], err );
		}
		return res;
	}
	
/*
=================================================
	IsZero / IsNotZero
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T>, TVec<bool,I,Q>>  IsZero (const TVec<T,I,Q> &x) __NE___
	{
		return Equals( x, TVec<T,I,Q>{T{0}}, Epsilon<T>() );
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T>, TVec<bool,I,Q>>  IsNotZero (const TVec<T,I,Q> &x) __NE___
	{
		return not IsZero( x );
	}

/*
=================================================
	Min
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<T,I,Q>  Min (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return glm::min( lhs, rhs );
	}
	
	template <typename T, int I, typename S, glm::qualifier Q,
			  typename = EnableIf<IsScalar<S>>
			 >
	ND_ forceinline TVec<T,I,Q>  Min (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return Min( lhs, TVec<T,I,Q>{rhs} );
	}

	template <typename T, int I, typename S, glm::qualifier Q,
			  typename = EnableIf<IsScalar<S>>
			 >
	ND_ forceinline TVec<T,I,Q>  Min (const S &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return Min( TVec<T,I,Q>{lhs}, rhs );
	}

	template <typename LT, typename RT,
			  typename = EnableIf<not IsVec<LT> and not IsVec<RT>>
			 >
	ND_ forceinline constexpr auto  Min (const LT &lhs, const RT &rhs) __NE___
	{
		if constexpr( IsSameTypes<LT, RT> )
		{
			return lhs > rhs ? rhs : lhs;
		}
		else
		{
			using T = decltype(lhs + rhs);
			return T(lhs) > T(rhs) ? T(rhs) : T(lhs);
		}
	}
	
	template <typename T0, typename T1, typename T2, typename ...Types>
	ND_ forceinline constexpr auto  Min (const T0 &arg0, const T1 &arg1, const T2 &arg2, const Types& ...args) __NE___
	{
		return Min( arg0, Min( arg1, arg2, args... ));
	}
	
/*
=================================================
	Max
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<T,I,Q>  Max (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return glm::max( lhs, rhs );
	}
	
	template <typename T, int I, typename S, glm::qualifier Q,
			  typename = EnableIf<IsScalar<S>>
			 >
	ND_ forceinline TVec<T,I,Q>  Max (const TVec<T,I,Q> &lhs, const S &rhs) __NE___
	{
		return Max( lhs, TVec<T,I,Q>{rhs} );
	}

	template <typename T, int I, typename S, glm::qualifier Q,
			  typename = EnableIf<IsScalar<S>>
			 >
	ND_ forceinline auto  Max (const S &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return Max( TVec<T,I,Q>{lhs}, rhs );
	}
	
	template <typename LT, typename RT,
			  typename = EnableIf<not IsVec<LT> and not IsVec<RT>>
			 >
	ND_ forceinline constexpr auto  Max (const LT &lhs, const RT &rhs) __NE___
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

	template <typename T0, typename T1, typename T2, typename ...Types>
	ND_ forceinline constexpr auto  Max (const T0 &arg0, const T1 &arg1, const T2 &arg2, const Types& ...args) __NE___
	{
		return Max( arg0, Max( arg1, arg2, args... ));
	}
	
/*
=================================================
	AssignMin / AssignMax
=================================================
*/
	template <typename T>
	forceinline void  AssignMin (INOUT T& lhs, const T &rhs) __NE___
	{
		lhs = Min( lhs, rhs );
	}
	
	template <typename T>
	forceinline void  AssignMax (INOUT T& lhs, const T &rhs) __NE___
	{
		lhs = Max( lhs, rhs );
	}

/*
=================================================
	Clamp
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<T,I,Q>  Clamp (const TVec<T,I,Q> &value, const TVec<T,I,Q> &minVal, const TVec<T,I,Q> &maxVal) __NE___
	{
		return glm::clamp( value, minVal, maxVal );
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<T,I,Q>  Clamp (const TVec<T,I,Q> &value, const T &minVal, const T &maxVal) __NE___
	{
		return glm::clamp( value, TVec<T,I,Q>{minVal}, TVec<T,I,Q>{maxVal} );
	}
	
	template <typename ValT, typename MinT, typename MaxT>
	ND_ forceinline constexpr auto  Clamp (const ValT &value, const MinT &minVal, const MaxT &maxVal) __NE___
	{
		ASSERT(All( minVal <= maxVal ));
		return Min( maxVal, Max( value, minVal ));
	}

/*
=================================================
	Wrap (float)
=================================================
*/
	template <typename T>
	forceinline constexpr EnableIf<IsFloatPoint<T>, T>  Wrap (const T& value, const T& minValue, const T& maxValue) __NE___
	{
		// check for NaN
		if_unlikely( minValue >= maxValue )
			return minValue;

		T	result = T( minValue + std::fmod( value - minValue, maxValue - minValue ));
		
		if ( result < minValue )
			result += (maxValue - minValue);

		return result;
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  Wrap (const TVec<T,I,Q>& v, const T& minValue, const T& maxValue) __NE___
	{
		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = Wrap( v[i], minValue, maxValue );
		}
		return res;
	}

/*
=================================================
	Wrap (int)
=================================================
*/
	template <typename T>
	forceinline constexpr EnableIf<IsInteger<T>, T>  Wrap (const T& value, const T& minValue, const T& maxValue) __NE___
	{
		// check for div by zero
		if_unlikely( minValue > maxValue )
			return minValue;

		T	result = T( minValue + ((value - minValue) % (maxValue - minValue + 1)) );
		
		if ( result < minValue )
			result += (maxValue - minValue + 1);

		return result;
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsInteger<T>, TVec<T,I,Q>>  Wrap (const TVec<T,I,Q>& v, const T& minValue, const T& maxValue) __NE___
	{
		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = Wrap( v[i], minValue, maxValue );
		}
		return res;
	}

/*
=================================================
	MirroredWrap (float)
=================================================
*/
	template <typename T>
	forceinline constexpr EnableIf<IsFloatPoint<T>, T>  MirroredWrap (const T& value, const T& minValue, const T& maxValue) __NE___
	{
		// check for NaN
		if_unlikely( minValue >= maxValue )
			return minValue;
			
		const T	 size	= (maxValue - minValue) * T(2);
		const T	 val	= Fract( (value - minValue) / size );

		return Min( val, T(1) - val ) * size + minValue;
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsFloatPoint<T>, TVec<T,I,Q>>  MirroredWrap (const TVec<T,I,Q>& v, const T& minValue, const T& maxValue) __NE___
	{
		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = MirroredWrap( v[i], minValue, maxValue );
		}
		return res;
	}

/*
=================================================
	MirroredWrap (int)
=================================================
*/
	template <typename T>
	forceinline constexpr EnableIf<IsInteger<T>, T>  MirroredWrap (const T& value, const T& minValue, const T& maxValue) __NE___
	{
		// check for division by zero
		if_unlikely( minValue >= maxValue )
			return minValue;
			
		const T	 size	= (maxValue - minValue) * T(2);
		const T	 val	= Abs( value - minValue ) % size;

		return Min( val, size - val ) + minValue;
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsInteger<T>, TVec<T,I,Q>>  MirroredWrap (const TVec<T,I,Q>& v, const T& minValue, const T& maxValue) __NE___
	{
		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = MirroredWrap( v[i], minValue, maxValue );
		}
		return res;
	}

/*
=================================================
	Round / RoundToInt / RoundToUint
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T> and IsFloatPoint<T>, T>  Round (const TVec<T,I,Q>& v) __NE___
	{
		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = Round( v[i] );
		}
		return res;
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline auto  RoundToInt (const TVec<T,I,Q>& v) __NE___
	{
		using R = decltype(RoundToInt(T()));

		TVec<R,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = RoundToInt( v[i] );
		}
		return res;
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline auto  RoundToUint (const TVec<T,I,Q>& v) __NE___
	{
		using R = decltype(RoundToUint(T()));

		TVec<R,I,Q>	res;
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
	ND_ forceinline EnableIf<IsScalar<T> and IsFloatPoint<T>, T>  Length (const TVec<T,I,Q> &v) __NE___
	{
		return glm::length( v );
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T> and IsFloatPoint<T>, T>  LengthSqr (const TVec<T,I,Q> &v) __NE___
	{
		return glm::length2( v );
	}
	
/*
=================================================
	Distance
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T> and IsFloatPoint<T>, T>  Distance (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return glm::distance( lhs, rhs );
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T> and IsFloatPoint<T>, T>  DistanceSqr (const TVec<T,I,Q> &lhs, const TVec<T,I,Q> &rhs) __NE___
	{
		return glm::distance2( lhs, rhs );
	}
	
/*
=================================================
	Normalize
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline EnableIf<IsScalar<T> and IsFloatPoint<T>, TVec<T,I,Q>>  Normalize (const TVec<T,I,Q> &v) __NE___
	{
		return glm::normalize( v );
	}
	
/*
=================================================
	SafeDiv
=================================================
*/
	template <typename T1, typename T2, typename T3,
			  typename = EnableIf<IsScalarOrEnum<T1> and IsScalarOrEnum<T2> and IsScalarOrEnum<T3>>
			 >
	ND_ forceinline constexpr auto  SafeDiv (const T1& lhs, const T2& rhs, const T3& defVal) __NE___
	{
		using T = decltype( lhs + rhs + defVal );

		return IsNotZero( rhs ) ? (T(lhs) / T(rhs)) : T(defVal);
	}
	
	template <typename T1, typename T2,
			  typename = EnableIf<IsScalarOrEnum<T1> and IsScalarOrEnum<T2>>
			 >
	ND_ forceinline constexpr auto  SafeDiv (const T1& lhs, const T2& rhs) __NE___
	{
		return SafeDiv( lhs, rhs, T1{0} );
	}

	template <typename T, int I, typename S, glm::qualifier Q>
	ND_ forceinline TVec<T,I,Q>  SafeDiv (const TVec<T,I,Q>& lhs, const TVec<T,I,Q>& rhs, const S& defVal) __NE___
	{
		TVec<T,I,Q>	res;
		for (int i = 0; i < I; ++i) {
			res[i] = SafeDiv( lhs[i], rhs[i], defVal );
		}
		return res;
	}
	
	template <typename T, int I, typename S1, typename S2, glm::qualifier Q,
			  typename = EnableIf<IsScalar<S1> and IsScalar<S2>>
			 >
	ND_ forceinline TVec<T,I,Q>  SafeDiv (const TVec<T,I,Q>& lhs, const S1& rhs, const S2& defVal) __NE___
	{
		return SafeDiv( lhs, TVec<T,I,Q>{rhs}, T(defVal) );
	}
	
	template <typename T, int I, typename S1, glm::qualifier Q,
			  typename = EnableIf<IsScalar<S1>>
			 >
	ND_ forceinline TVec<T,I,Q>  SafeDiv (const TVec<T,I,Q>& lhs, const S1& rhs) __NE___
	{
		return SafeDiv( lhs, TVec<T,I,Q>{rhs}, T{0} );
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<T,I,Q>  SafeDiv (const TVec<T,I,Q>& lhs, const TVec<T,I,Q>& rhs) __NE___
	{
		return SafeDiv( lhs, rhs, T{0} );
	}
	
/*
=================================================
	BaryLerp
----
	barycentric interpolation
=================================================
*/
	template <typename T, typename A, typename B, typename C, glm::qualifier Q>
	ND_ forceinline auto  BaryLerp (const A& a, const B& b, const C& c, const TVec<T,3,Q> &barycentrics) __NE___
	{
		STATIC_ASSERT( IsFloatPoint<T> );
		return a * barycentrics.x + b * barycentrics.y + c * barycentrics.z;
	}
	
/*
=================================================
	BiLerp
----
	bilinear interpolation
=================================================
*/
	template <typename T, typename Coord, glm::qualifier Q>
	ND_ forceinline auto  BiLerp (const Coord& x1y1, const Coord& x2y1, const Coord& x1y2, const Coord& x2y2, const TVec<T,2,Q> &factor) __NE___
	{
		STATIC_ASSERT( IsFloatPoint<T> );
		return Lerp( Lerp( x1y1, x2y1, factor.x ),
					 Lerp( x1y2, x2y2, factor.x ), factor.y );
	}
	
/*
=================================================
	Remap
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ forceinline auto  Remap (const TVec<T,2,Q> &src, const TVec<T,2,Q> &dst, const T& x) __NE___
	{
		STATIC_ASSERT( IsFloatPoint<T> );
		return (x - src[0]) / (src[1] - src[0]) * (dst[1] - dst[0]) + dst[0];
	}
	
/*
=================================================
	RemapClamped
=================================================
*/
	template <typename T, glm::qualifier Q>
	ND_ forceinline auto  RemapClamped (const TVec<T,2,Q> &src, const TVec<T,2,Q> &dst, const T& x) __NE___
	{
		return Clamp( Remap( src, dst, x ), dst[0], dst[1] );
	}
	
/*
=================================================
	ToUNorm
----
	snorm -> unorm
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline auto  ToUNorm (const TVec<T,I,Q> &snorm) __NE___
	{
		STATIC_ASSERT( IsFloatPoint<T> );
		return snorm * T(0.5) + T(0.5);
	}

	template <typename T>
	ND_ forceinline constexpr EnableIf<IsScalar<T>, T >  ToUNorm (T snorm) __NE___
	{
		return snorm * T(0.5) + T(0.5);
	}

/*
=================================================
	ToSNorm
----
	unorm -> snorm
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline auto  ToSNorm (const TVec<T,I,Q> &unorm) __NE___
	{
		STATIC_ASSERT( IsFloatPoint<T> );
		return unorm * T(2.0) - T(1.0);
	}

	template <typename T>
	ND_ forceinline constexpr EnableIf<IsScalar<T>, T >  ToSNorm (T unorm) __NE___
	{
		return unorm * T(2.0) - T(1.0);
	}
	
/*
=================================================
	FloorPOT / CeilPOT
=================================================
*/
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<T,I,Q>  FloorPOT (const TVec<T,I,Q> &v) __NE___
	{
		TVec<T,I,Q>		result;
		for (int i = 0; i < I; ++i) {
			result[i] = FloorPOT( v[i] );
		}
		return result;
	}
	
	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<T,I,Q>  CeilPOT (const TVec<T,I,Q> &v) __NE___
	{
		TVec<T,I,Q>		result;
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
	ND_ forceinline TVec<int,I,Q>  IntLog2 (const TVec<T,I,Q> &v) __NE___
	{
		TVec<int,I,Q>		result;
		for (int i = 0; i < I; ++i) {
			result[i] = IntLog2( v[i] );
		}
		return result;
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<int,I,Q>  CeilIntLog2 (const TVec<T,I,Q> &v) __NE___
	{
		TVec<int,I,Q>		result;
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
	ND_ forceinline TVec<int,I,Q>  BitScanForward (const TVec<T,I,Q> &v) __NE___
	{
		TVec<int,I,Q>		result;
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
	ND_ forceinline TVec<usize,I,Q>  BitCount (const TVec<T,I,Q> &v) __NE___
	{
		TVec<usize,I,Q>		result;
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
	ND_ forceinline TVec<bool,I,Q>  IsPowerOfTwo (const TVec<T,I,Q> &v) __NE___
	{
		TVec<bool,I,Q>	result;
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
	ND_ forceinline TVec<T,I,Q>  SafeLeftBitShift (const TVec<T,I,Q> &v) __NE___
	{
		TVec<T,I,Q>		result;
		for (int i = 0; i < I; ++i) {
			result[i] = SafeLeftBitShift( v[i] );
		}
		return result;
	}

	template <typename T, int I, glm::qualifier Q>
	ND_ forceinline TVec<T,I,Q>  SafeRightBitShift (const TVec<T,I,Q> &v) __NE___
	{
		TVec<T,I,Q>		result;
		for (int i = 0; i < I; ++i) {
			result[i] = SafeRightBitShift( v[i] );
		}
		return result;
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

		To	res;
		for (int i = 0; i < I; ++i) {
			res[i] = CheckCast<T>( src[i] );
		}
		return res;
	}

} // AE::Base


namespace std
{
#if AE_FAST_HASH
	template <typename T, uint32_t I, glm::qualifier Q>
	struct hash< AE::Math::TVec<T,I,Q> > {
		ND_ size_t  operator () (const AE::Math::TVec<T,I,Q> &value) C_NE___ {
			return size_t(AE::Base::HashOf( value.data(), value.size() * sizeof(T) ));
		}
	};

#else
	template <typename T, glm::qualifier Q>
	struct hash< AE::Math::TVec<T,2,Q> > {
		ND_ size_t  operator () (const AE::Math::TVec<T,2,Q> &value) C_NE___ {
			return size_t(AE::Base::HashOf( value.x ) + AE::Base::HashOf( value.y ));
		}
	};
	
	template <typename T, glm::qualifier Q>
	struct hash< AE::Math::TVec<T,3,Q> > {
		ND_ size_t  operator () (const AE::Math::TVec<T,3,Q> &value) C_NE___ {
			return size_t(AE::Base::HashOf( value.x ) + AE::Base::HashOf( value.y ) + AE::Base::HashOf( value.z ));
		}
	};
	
	template <typename T, glm::qualifier Q>
	struct hash< AE::Math::TVec<T,4,Q> > {
		ND_ size_t  operator () (const AE::Math::TVec<T,4,Q> &value) C_NE___ {
			return size_t(AE::Base::HashOf( value.x ) + AE::Base::HashOf( value.y ) + AE::Base::HashOf( value.z ) + AE::Base::HashOf( value.w ));
		}
	};
#endif

} // std
