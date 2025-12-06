// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Math functions will return zero instead of NaN.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

/*
=================================================
	SafeSqrt
----
	T  SafeSqrt (T x)
----
	doesn't return undefined result
=================================================
*/
#define Gen_SafeSqrt1( _type_, _zero_ )			\
	ND_ _type_  SafeSqrt (const _type_ x) {		\
		return sqrt( max( x, _type_(_zero_) ));	\
	}

#define Gen_SafeSqrt( _stype_, _vtype_, _zero_ )\
	Gen_SafeSqrt1( _stype_,				_zero_ )\
	Gen_SafeSqrt1( UNITE( _vtype_, 2 ), _zero_ )\
	Gen_SafeSqrt1( UNITE( _vtype_, 3 ), _zero_ )\
	Gen_SafeSqrt1( UNITE( _vtype_, 4 ), _zero_ )

Gen_SafeSqrt( float, float_vec_t, float_zero )

#if AE_ENABLE_HALF_TYPE
	Gen_SafeSqrt( half, half_vec_t, half_zero )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_SafeSqrt( double, double_vec_t, double_zero )
#endif

#undef Gen_SafeSqrt1
#undef Gen_SafeSqrt

/*
=================================================
	SafeInvSqrt
----
	T  SafeInvSqrt (T x)
----
	doesn't return undefined result
=================================================
*/
#define Gen_SafeInvSqrt1( _type_, _zero_ )				\
	ND_ _type_  SafeInvSqrt (const _type_ x) {			\
		return inversesqrt( max( x, _type_(_zero_) ));	\
	}

#define Gen_SafeInvSqrt( _stype_, _vtype_, _zero_ )\
	Gen_SafeInvSqrt1( _stype_,				_zero_ )\
	Gen_SafeInvSqrt1( UNITE( _vtype_, 2 ),	_zero_ )\
	Gen_SafeInvSqrt1( UNITE( _vtype_, 3 ),	_zero_ )\
	Gen_SafeInvSqrt1( UNITE( _vtype_, 4 ),	_zero_ )

Gen_SafeInvSqrt( float, float_vec_t, float_min )

#if AE_ENABLE_HALF_TYPE
	Gen_SafeInvSqrt( half, half_vec_t, half_min )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_SafeInvSqrt( double, double_vec_t, double_min )
#endif

#undef Gen_SafeInvSqrt1
#undef Gen_SafeInvSqrt

/*
=================================================
	SafeLn
----
	T  SafeLn (T x)
----
	doesn't return undefined result
=================================================
*/
#define Gen_SafeLn1( _type_, _zero_ )			\
	ND_ _type_  SafeLn (const _type_ x) {		\
		return log( max( x, _type_(_zero_) ));	\
	}

#define Gen_SafeLn( _stype_, _vtype_, _zero_ )\
	Gen_SafeLn1( _stype_,				_zero_ )\
	Gen_SafeLn1( UNITE( _vtype_, 2 ),	_zero_ )\
	Gen_SafeLn1( UNITE( _vtype_, 3 ),	_zero_ )\
	Gen_SafeLn1( UNITE( _vtype_, 4 ),	_zero_ )

Gen_SafeLn( float, float_vec_t, float_min )

#if AE_ENABLE_HALF_TYPE
	Gen_SafeLn( half, half_vec_t, half_min )
#endif

#undef Gen_SafeLn1
#undef Gen_SafeLn

/*
=================================================
	SafeLog2
----
	T  SafeLog2 (T x)
----
	doesn't return undefined result
=================================================
*/
#define Gen_SafeLog21( _type_, _zero_ )			\
	ND_ _type_  SafeLog2 (const _type_ x) {		\
		return log2( max( x, _type_(_zero_) ));	\
	}

#define Gen_SafeLog2( _stype_, _vtype_, _zero_ )\
	Gen_SafeLog21( _stype_,				_zero_ )\
	Gen_SafeLog21( UNITE( _vtype_, 2 ), _zero_ )\
	Gen_SafeLog21( UNITE( _vtype_, 3 ),	_zero_ )\
	Gen_SafeLog21( UNITE( _vtype_, 4 ),	_zero_ )

Gen_SafeLog2( float, float_vec_t, float_min )

#if AE_ENABLE_HALF_TYPE
	Gen_SafeLog2( half, half_vec_t, half_min )
#endif

#undef Gen_SafeLog21
#undef Gen_SafeLog2

/*
=================================================
	SafePow
----
	T  SafePow (T x, T y)
----
	doesn't return undefined result
	supports only positive X, supports negative Y.
=================================================
*/
#define Gen_SafePow1( _type_, _zero_ )						\
	ND_ _type_  SafePow (const _type_ x, const _type_ y) {	\
		return pow( max( abs(x), _type_(_zero_) ), y );		\
	}

#define Gen_SafePow( _stype_, _vtype_, _zero_ )\
	Gen_SafePow1( _stype_,				_zero_ )\
	Gen_SafePow1( UNITE( _vtype_, 2 ),	_zero_ )\
	Gen_SafePow1( UNITE( _vtype_, 3 ),	_zero_ )\
	Gen_SafePow1( UNITE( _vtype_, 4 ),	_zero_ )

Gen_SafePow( float, float_vec_t, float_min )

#if AE_ENABLE_HALF_TYPE
	Gen_SafePow( half, half_vec_t, half_min )
#endif

#undef Gen_SafePow1
#undef Gen_SafePow

/*
=================================================
	SafeDiv
----
	T  SafeDiv (T x, T y)
----
	doesn't return undefined result
=================================================
*/
#define Gen_SafeDiv1( _type_, _zero_ )							\
	ND_ _type_  SafeDiv (const _type_ x, const _type_ y) {		\
		return (x * sign(y)) / max( abs(y), _type_(_zero_) );	\
	}

#define Gen_SafeDiv( _stype_, _vtype_, _zero_ )\
	Gen_SafeDiv1( _stype_,				_zero_ )\
	Gen_SafeDiv1( UNITE( _vtype_, 2 ),	_zero_ )\
	Gen_SafeDiv1( UNITE( _vtype_, 3 ),	_zero_ )\
	Gen_SafeDiv1( UNITE( _vtype_, 4 ),	_zero_ )

Gen_SafeDiv( float, float_vec_t, float_min )

#if AE_ENABLE_HALF_TYPE
	Gen_SafeDiv( half, half_vec_t, half_min )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_SafeDiv( double, double_vec_t, double_min )
#endif

#undef Gen_SafeDiv1
#undef Gen_SafeDiv

/*
=================================================
	SafeCbrt
----
	T  SafeCbrt (T x)
=================================================
*/
#define Gen_CBRT1( _stype_, _type_, _zero_ )										\
	ND_ _type_  SafeCbrt (const _type_ x) {											\
		return pow( max( x, _type_(_zero_) ), _type_(_stype_(1.0)/_stype_(3.0)) );	\
	}

#define Gen_CBRT( _stype_, _vtype_, _zero_ )\
	Gen_CBRT1( _stype_, _stype_,			 _zero_ )\
	Gen_CBRT1( _stype_, UNITE( _vtype_, 2 ), _zero_ )\
	Gen_CBRT1( _stype_, UNITE( _vtype_, 3 ), _zero_ )\
	Gen_CBRT1( _stype_, UNITE( _vtype_, 4 ), _zero_ )

Gen_CBRT( float, float_vec_t, float_zero )

#if AE_ENABLE_HALF_TYPE
	Gen_CBRT( half, half_vec_t, half_zero )
#endif

#undef Gen_CBRT1
#undef Gen_CBRT

/*
=================================================
	SafeRcp
----
	will return zero instead of NaN
=================================================
*/
ND_ float	SafeRcp (const float  x)	{ return Sign(x) * Max( 1.0f / Abs(x), 0.f ); }
ND_ float2	SafeRcp (const float2 v)	{ return Sign(v) * Max( 1.0f / Abs(v), float2(0.f) ); }
ND_ float3	SafeRcp (const float3 v)	{ return Sign(v) * Max( 1.0f / Abs(v), float3(0.f) ); }
ND_ float4	SafeRcp (const float4 v)	{ return Sign(v) * Max( 1.0f / Abs(v), float4(0.f) ); }

/*
=================================================
	SafeLength / SafeNormalize
=================================================
*/
#define Gen_SAFELEN1( _stype_, _vtype_, _zero_ )	\
	ND_ _stype_  SafeLength (const _vtype_ v)		\
	{												\
		_stype_	sq = LengthSq( v );					\
		return Max( Sqrt( sq ), _zero_ );			\
	}												\
	ND_ _vtype_  SafeNormalize (const _vtype_ v)	\
	{												\
		_stype_	sq  = LengthSq( v );				\
		_stype_	inv = Max( InvSqrt( sq ), _zero_ );	\
		return v * inv;								\
	}

#define Gen_SAFELEN( _stype_, _vtype_, _zero_ )\
	Gen_SAFELEN1( _stype_, UNITE( _vtype_, 2 ), _zero_ )\
	Gen_SAFELEN1( _stype_, UNITE( _vtype_, 3 ), _zero_ )\
	Gen_SAFELEN1( _stype_, UNITE( _vtype_, 4 ), _zero_ )

Gen_SAFELEN( float, float_vec_t, float_zero )

#if AE_ENABLE_HALF_TYPE
	Gen_SAFELEN( half, half_vec_t, half_zero )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_SAFELEN( double, double_vec_t, double_min )
#endif

#undef Gen_SAFELEN1
#undef Gen_SAFELEN
