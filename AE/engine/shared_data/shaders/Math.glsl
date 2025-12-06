// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Default math types and functions
*/

#ifdef __cplusplus
# pragma once

# ifdef __INTELLISENSE__
#	include <aestyle.glsl.h>
# endif
#endif

#ifndef AE_ENABLE_BYTE_TYPE
# define AE_ENABLE_BYTE_TYPE	0
#endif

#ifndef AE_ENABLE_SHORT_TYPE
# define AE_ENABLE_SHORT_TYPE	0	// suffix 'S', 'US'
#endif

#ifndef AE_ENABLE_LONG_TYPE
# define AE_ENABLE_LONG_TYPE	0	// suffix 'L', 'UL'
#endif

#ifndef AE_ENABLE_HALF_TYPE
# define AE_ENABLE_HALF_TYPE	0	// suffix 'HF'
#endif

#ifndef AE_ENABLE_DOUBLE_TYPE
# define AE_ENABLE_DOUBLE_TYPE	0	// suffix 'LF'
#endif

// helper
#define UNITE( x, y )	x##y
//-----------------------------------------------------------------------------


#define Any						any				// (bool)
#define All						all				// (bool)
#define Abs						abs				// (any signed)
#define ACos					acos			// (half, float)	result in range [0 .. Pi],			result is undefined if abs(x) > 1
#define ASin					asin			// (half, float)	result in range [-Pi/2 ... Pi/2],	result is undefined if abs(x) > 1
#define ASinH					asinh			// (half, float)
#define ACosH					acosh			// (half, float)
#define ATan					atan			// (half, float)	for 2 arg overload: result in range [-Pi...+Pi]
												//					for 1 arg overload: result in range [-Pi/2 ... Pi/2], result is undefined if x=0
#define BitScanReverse			findMSB			// (any int)
#define BitScanForward			findLSB			// (any int)
#define ATanH					atanh			// (half, float)
#define Clamp					clamp			// (any except bool)
#define Ceil					ceil			// (any fp)
#define Cos						cos				// (half, float)
#define CosH					cosh			// (half, float)
#define Cross					cross			// (any fp)
#define Distance				distance		// (any fp)
#define Dot						dot				// (any fp)			x[0]*y[0] + ...
#define Exp						exp				// (half, float)	Pow( float_Euler, x )
#define Exp2					exp2			// (half, float)	Pow( 2, x )
#define Exp10( _a_ )			pow(10.0,(_a_))	// (half, float)
#define Fract					fract			// (any fp)			x - Floor( x )
#define Floor					floor			// (any fp)
#define IsNaN					isnan			// (any fp)
#define IsInfinity				isinf			// (any fp)
#define InvSqrt					inversesqrt		// (any fp)							result is undefined if x <= 0
#define IntLog2					BitScanReverse	// (any int)
#define Length					length			// (any fp)
#define Lerp					mix				// (any fp)
#define Ln						log				// (half, float)					result is undefined if x <= 0
#define Log2					log2			// (half, float)					result is undefined if x <= 0
#define Log( _a_, _base_ )		(Ln(_a_) / Ln(_base_))			// (half, float)	result is undefined if x <= 0
#define Log10( _a_ )			(Ln(_a_) * 0.4342944819032518)	// (half, float)
#define Min						min				// (any except bool)
#define Max						max				// (any except bool)
#define Mod						mod				// (any fp)
#define Normalize				normalize		// (any fp)
#define Pow						pow				// (half, float)					result is undefined if x<0 or if x=0 and y<=0
#define Round					round			// (any fp)
#define Reflect					reflect			// (any fp)
#define Refract					refract			// (any fp)
#define Step					step			// (any fp)			x < edge ? 0 : 1
#define Sqrt					sqrt			// (any fp)							result is undefined if x < 0
#define Sin						sin				// (half, float)
#define SinH					sinh			// (half, float)
#define SignOrZero				sign			// (int, any fp)	-1, 0, +1,	x=nan: 0 (NV)
#define Tan						tan				// (half, float)
#define TanH					tanh			// (half, float)
#define Trunc					trunc			// (any fp)
#define BitCount				bitCount		// (any int)
#define ToDeg					degrees			// (half, float)
#define ToRad					radians			// (half, float)

#define MulAdd(_a_,_b_,_c_)		((_a_)*(_b_)+(_c_))

#define FusedMulAdd				fma				// (any fp)				(a * b) + c
//#define FusedMulAdd			MulAdd			// different precision

#define MatInverse				inverse
#define MatTranspose			transpose
#define MatDeterminant			determinant

// range [0, 1], pattern __/'''
#define SmoothStep( _x_, _edge0_, _edge1_ )	smoothstep( (_edge0_), (_edge1_), (_x_) )


ND_ float2  SinCos (const float x)		{ return float2(sin(x), cos(x)); }

//-----------------------------------------------------------------------------


// to mark 'out' and 'inout' argument in function call
// in function argument list use defined by GLSL qualificators: in, out, inout
#define OUT
#define INOUT


//-----------------------------------------------------------------------------
// Constants

#if AE_ENABLE_BYTE_TYPE
#	define sbyte_min		(-128)
#	define sbyte_max		(127)
#	define ubyte_max		(0xFF)
#endif
#if AE_ENABLE_SHORT_TYPE
#	define sshort_min		(-32768s)
#	define sshort_max		(32767s)
#	define ushort_max		(0xFFFFus)
#endif
#if 1
#	define sint_min			(-2147483648)
#	define sint_max			(2147483647)
#	define uint_max			(~0u)		// 0xFFFFFFFFu
#endif
#if AE_ENABLE_LONG_TYPE
#	define slong_min		(-9223372036854775808l)
#	define slong_max		(9223372036854775807l)
#	define ulong_max		(~0ul)		// 0xFFFFFFFFFFFFFFFFul
#endif
#if AE_ENABLE_HALF_TYPE
#	define half_zero		(0.0hf)
#	define half_min			(0.00006103515625hf)		// smallest positive normal number
#	define half_max			(65504.0hf)
#  if AE_ENABLE_SHORT_TYPE
#	define half_inf			(uint16BitsToHalf( 0x7c00us ))
#	define half_inf_neg		(uint16BitsToHalf( 0xfc00us ))
#	define half_nan			(uint16BitsToHalf( 0x7fffus ))	// or 0xffff
#	define half_nan2		(uint16BitsToHalf( 0x7f80us ))	// or 0xff80
#  else
#	define half_inf			(1.hf / 0.hf)
#	define half_inf_neg		(-1.hf / 0.hf)
#	define half_nan			(0.hf / 0.hf)
#	define half_nan2		(-0.hf / 0.hf)
#  endif
#	define half_Pi			(3.14159265358979323846hf)
#	define half_Pi2			(6.28318530717958647692hf)
#	define half_HalfPi		(1.57079632679489661923hf)
#	define half_InvPi		(0.31830988618379067153hf)
#	define half_SqrtOf2		(1.41421356237309504880hf)
#	define half_Euler		(2.71828182845904523536hf)
#endif
#if 1
#	define float_zero		(0.0f)
#	define float_min		(1.1754943508e-38f)			// smallest positive normal number
#	define float_max		(3.4028234664e+38f)
#	define float_inf		(uintBitsToFloat( 0x7F800000u ))
#	define float_inf_neg	(uintBitsToFloat( 0xFF800000u ))
#	define float_qnan		(uintBitsToFloat( 0xFFC00001u ))	// quiet nan
#	define float_snan		(uintBitsToFloat( 0xFF800001u ))	// signaling nan
#	define float_nan		(uintBitsToFloat( 0xFFFFFFFFu ))
#	define float_Pi			(3.14159265358979323846f)
#	define float_Pi2		(6.28318530717958647692f)
#	define float_HalfPi		(1.57079632679489661923f)
#	define float_InvPi		(0.31830988618379067153f)
#	define float_SqrtOf2	(1.41421356237309504880f)			// Sqrt(2.0f)
#	define float_Euler		(2.71828182845904523536f)
#	define float_epsilon	(2.0e-5f)
#endif
#if AE_ENABLE_DOUBLE_TYPE
#	define double_zero		(0.0lf)
#	define double_min		(2.2250738585072014e-308lf)	 // smallest positive normal number
#	define double_max		(1.7976931348623157e+308lf)
#  if AE_ENABLE_LONG_TYPE
#	define double_inf		(uint64BitsToDouble( 0x7FF0000000000000ul ))
#	define double_inf_neg	(uint64BitsToDouble( 0xFFF0000000000000ul ))
#	define double_qnan		(uint64BitsToDouble( 0x7FF8000000000001ul ))	// quiet nan
#	define double_snan		(uint64BitsToDouble( 0x7FF0000000000001ul ))	// signaling nan
#	define double_nan		(uint64BitsToDouble( 0x7FFFFFFFFFFFFFFFul ))
#  else
#	define double_inf		(1.lf / 0.lf)
#	define double_inf_neg	(-1.lf / 0.lf)
#	define double_qnan		(0.lf / 0.lf)
#	define double_snan		(-0.lf / 0.lf)
#	define double_nan		(0.lf / -0.lf)
#  endif
#	define double_Pi		(3.14159265358979323846lf)
#	define double_Pi2		(6.28318530717958647692lf)
#	define double_HalfPi	(1.57079632679489661923lf)
#	define double_InvPi		(0.31830988618379067153lf)
#	define double_SqrtOf2	(1.41421356237309504880lf)
#	define double_Euler		(2.71828182845904523536lf)
#endif
//-----------------------------------------------------------------------------


/*
=================================================
	Equal / NotEqual /
	Less / Greater / LessEqual / GreaterEqual
----
	boolType  Equal (T lhs, T rhs)
	boolType  NotEqual (T lhs, T rhs)
	boolType  Less (T lhs, T rhs)
	boolType  Greater (T lhs, T rhs)
	boolType  LessEqual (T lhs, T rhs)
	boolType  GreaterEqual (T lhs, T rhs)
----
	per component comparator
=================================================
*/
#define Gen_CMP1( _stype_, _vtype_, _name_, _opS_, _opV_ )														\
	ND_ bool   _name_ (const _stype_          lhs, const _stype_          rhs)	{ return lhs _opS_ rhs; }		\
	ND_ bool2  _name_ (const UNITE(_vtype_,2) lhs, const UNITE(_vtype_,2) rhs)	{ return _opV_( lhs, rhs ); }	\
	ND_ bool3  _name_ (const UNITE(_vtype_,3) lhs, const UNITE(_vtype_,3) rhs)	{ return _opV_( lhs, rhs ); }	\
	ND_ bool4  _name_ (const UNITE(_vtype_,4) lhs, const UNITE(_vtype_,4) rhs)	{ return _opV_( lhs, rhs ); }	\
	ND_ bool2  _name_ (const UNITE(_vtype_,2) lhs, const _stype_          rhs)	{ return _opV_( lhs, UNITE(_vtype_,2)(rhs) ); }	\
	ND_ bool3  _name_ (const UNITE(_vtype_,3) lhs, const _stype_          rhs)	{ return _opV_( lhs, UNITE(_vtype_,3)(rhs) ); }	\
	ND_ bool4  _name_ (const UNITE(_vtype_,4) lhs, const _stype_          rhs)	{ return _opV_( lhs, UNITE(_vtype_,4)(rhs) ); }

#define Gen_CMP( _stype_, _vtype_ )										\
	Gen_CMP1( _stype_, _vtype_, Equal,			==, equal )				\
	Gen_CMP1( _stype_, _vtype_, NotEqual,		!=, notEqual )			\
	Gen_CMP1( _stype_, _vtype_, Less,			<,  lessThan )			\
	Gen_CMP1( _stype_, _vtype_, Greater,		>,  greaterThan )		\
	Gen_CMP1( _stype_, _vtype_, LessEqual,		<=, lessThanEqual )		\
	Gen_CMP1( _stype_, _vtype_, GreaterEqual,	>=, greaterThanEqual )

Gen_CMP( float,	float_vec_t )
Gen_CMP( int,	int_vec_t )
Gen_CMP( uint,	uint_vec_t )

#if AE_ENABLE_BYTE_TYPE
	Gen_CMP( sbyte,		sbyte_vec_t )
	Gen_CMP( ubyte,		ubyte_vec_t )
#endif
#if AE_ENABLE_SHORT_TYPE
	Gen_CMP( sshort,	sshort_vec_t )
	Gen_CMP( ushort,	ushort_vec_t )
#endif
#if AE_ENABLE_LONG_TYPE
	Gen_CMP( slong,		slong_vec_t )
	Gen_CMP( ulong,		ulong_vec_t )
#endif
#if AE_ENABLE_HALF_TYPE
	Gen_CMP( half,		half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_CMP( double,	double_vec_t )
#endif

#undef Gen_CMP1
#undef Gen_CMP

ND_ bool   Not (const bool  value)						{ return !value; }
ND_ bool2  Not (const bool2 value)						{ return not(value); }
ND_ bool3  Not (const bool3 value)						{ return not(value); }
ND_ bool4  Not (const bool4 value)						{ return not(value); }

ND_ bool   BoolAnd (const bool  lhs, const bool  rhs)	{ return lhs and rhs; }
ND_ bool2  BoolAnd (const bool2 lhs, const bool2 rhs)	{ return bool2( lhs.x and rhs.x, lhs.y and rhs.y ); }
ND_ bool3  BoolAnd (const bool3 lhs, const bool3 rhs)	{ return bool3( lhs.x and rhs.x, lhs.y and rhs.y, lhs.z and rhs.z ); }
ND_ bool4  BoolAnd (const bool4 lhs, const bool4 rhs)	{ return bool4( lhs.x and rhs.x, lhs.y and rhs.y, lhs.z and rhs.z, lhs.w and rhs.w ); }

ND_ bool   BoolOr (const bool  lhs, const bool  rhs)	{ return lhs or rhs; }
ND_ bool2  BoolOr (const bool2 lhs, const bool2 rhs)	{ return bool2( lhs.x or rhs.x, lhs.y or rhs.y ); }
ND_ bool3  BoolOr (const bool3 lhs, const bool3 rhs)	{ return bool3( lhs.x or rhs.x, lhs.y or rhs.y, lhs.z or rhs.z ); }
ND_ bool4  BoolOr (const bool4 lhs, const bool4 rhs)	{ return bool4( lhs.x or rhs.x, lhs.y or rhs.y, lhs.z or rhs.z, lhs.w or rhs.w ); }


#define AllLess( a, b )			All( Less( (a), (b) ))
#define AllLessEqual( a, b )	All( LessEqual( (a), (b) ))

#define AllGreater( a, b )		All( Greater( (a), (b) ))
#define AllGreaterEqual( a, b )	All( GreaterEqual( (a), (b) ))

#define AnyLess( a, b )			Any( Less( (a), (b) ))
#define AnyLessEqual( a, b )	Any( LessEqual( (a), (b) ))

#define AnyGreater( a, b )		Any( Greater( (a), (b) ))
#define AnyGreaterEqual( a, b )	Any( GreaterEqual( (a), (b) ))

#define AllEqual( a, b )		All( Equal( (a), (b) ))
#define AnyEqual( a, b )		Any( Equal( (a), (b) ))

#define AllNotEqual( a, b )		All( Not( Equal( (a), (b) )))
#define AnyNotEqual( a, b )		Any( Not( Equal( (a), (b) )))

#define NotAllEqual( a, b )		Not( All( Equal( (a), (b) )))
#define NotAnyEqual( a, b )		Not( Any( Equal( (a), (b) )))

#define All2( a, b )			All(bool2( (a), (b) ))
#define All3( a, b, c )			All(bool3( (a), (b), (c) ))
#define All4( a, b, c, d )		All(bool4( (a), (b), (c), (d) ))

#define Any2( a, b )			Any(bool2( (a), (b) ))
#define Any3( a, b, c )			Any(bool3( (a), (b), (c) ))
#define Any4( a, b, c, d )		Any(bool4( (a), (b), (c), (d) ))


/*
=================================================
	Saturate
----
	T  Saturate (T x)
=================================================
*/
#define Gen_SATURATE1( _stype_, _type_ )				\
	ND_ _type_  Saturate (const _type_ x) {				\
		return clamp( x, _stype_(0.0), _stype_(1.0) );	\
	}

#define Gen_SATURATE( _stype_, _vtype_ )\
	Gen_SATURATE1( _stype_, _stype_	)	\
	Gen_SATURATE1( _stype_, UNITE( _vtype_, 2 ))\
	Gen_SATURATE1( _stype_, UNITE( _vtype_, 3 ))\
	Gen_SATURATE1( _stype_, UNITE( _vtype_, 4 ))

Gen_SATURATE( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_SATURATE( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_SATURATE( double, double_vec_t )
#endif

#undef Gen_SATURATE1
#undef Gen_SATURATE

/*
=================================================
	float point boolean operations
----
	1.0 - true
	0.0 - false
----
	LessF, GreaterF - may return NaN if one of operands contains NaN, use Saturate() to convert NaN to 0.0
=================================================
*/
#define Gen_FPBOOL1( _stype_, _vtype_ )\
	ND_ _vtype_  LessF (const _vtype_ lhs, const _stype_ rhs)	{ return step( lhs,			 _vtype_(rhs) ); }	\
	ND_ _vtype_  LessF (const _stype_ lhs, const _vtype_ rhs)	{ return step( _vtype_(lhs), rhs ); }			\
	ND_ _vtype_  LessF (const _vtype_ lhs, const _vtype_ rhs)	{ return step( lhs,			 rhs ); }			\
	ND_ _vtype_  NotF  (const _vtype_ rhs)						{ return _stype_(1.0) - rhs; }

#define Gen_FPBOOL( _stype_, _vtype_ )\
	ND_ _stype_  LessF (const _stype_ lhs, const _stype_ rhs)	{ return step( lhs, rhs ); }\
	Gen_FPBOOL1( _stype_,	UNITE( _vtype_, 2 ))\
	Gen_FPBOOL1( _stype_,	UNITE( _vtype_, 3 ))\
	Gen_FPBOOL1( _stype_,	UNITE( _vtype_, 4 ))

Gen_FPBOOL( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_FPBOOL( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_FPBOOL( double, double_vec_t )
#endif

#undef Gen_FPBOOL1
#undef Gen_FPBOOL

#define GreaterF( _a_, _b_ )		LessF( (_b_), (_a_) )
//#define AndF( _a_, _b_ )			Saturate( (_a_) * (_b_) )
//#define OrF( _a_, _b_ )			Saturate( (_a_) + (_b_) )

/*
=================================================
	SelectF
	SelectFSat
----
	T  SelectF (T x, T y, T ifLess, T ifNot)
	T  SelectFSat (T x, T y, T ifLess, T ifNot)
----
	Same as per component 'x < y ? ifLess : ifNot', both branches are always executed (branchless technique).
	May return NaN if one of branches returns NaN, even if it is inactive branch.
	'SelectFSat()' convert 'ifLess' and 'ifNot' to unorm to avoid NaNs.
	GLSL specs: both 'if' and '?' are branches.
	Note: 'Select()' or 'Branchless()' may be faster on modern GPUs.
=================================================
*/
#define SelectF( _x_, _y_, _ifLess_, _ifNot_ )		Lerp( (_ifNot_), (_ifLess_), LessF( _x_, _y_ ))
#define SelectFSat( _x_, _y_, _ifLess_, _ifNot_ )	Lerp( Saturate(_ifNot_), Saturate(_ifLess_), Saturate(LessF( _x_, _y_ )) )

/*
=================================================
	Diagonal
	InvDiagonal
----
	T  Diagonal (T x, T y)
	T  InvDiagonal (T x, T y)
=================================================
*/
#define Gen_DIAGONAL1( _type_ )									\
	ND_ _type_  Diagonal (const _type_ x, const _type_ y) {		\
		return Sqrt( x*x + y*y );								\
	}															\
	ND_ _type_  InvDiagonal (const _type_ x, const _type_ y) {	\
		return InvSqrt( x*x + y*y );							\
	}

#define Gen_DIAGONAL( _stype_, _vtype_ )\
	Gen_DIAGONAL1( _stype_ )			\
	Gen_DIAGONAL1( UNITE( _vtype_, 2 ))	\
	Gen_DIAGONAL1( UNITE( _vtype_, 3 ))	\
	Gen_DIAGONAL1( UNITE( _vtype_, 4 ))

Gen_DIAGONAL( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_DIAGONAL( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_DIAGONAL( double, double_vec_t )
#endif

#undef Gen_DIAGONAL1
#undef Gen_DIAGONAL

/*
=================================================
	Cbrt
----
	T  Cbrt (T x)
=================================================
*/
#define Gen_CBRT1( _stype_, _type_ )						\
	ND_ _type_  Cbrt (const _type_ x) {						\
		return pow( x, _type_(_stype_(1.0)/_stype_(3.0)) );	\
	}

#define Gen_CBRT( _stype_, _vtype_ )\
	Gen_CBRT1( _stype_, _stype_			  )\
	Gen_CBRT1( _stype_, UNITE( _vtype_, 2 ))\
	Gen_CBRT1( _stype_, UNITE( _vtype_, 3 ))\
	Gen_CBRT1( _stype_, UNITE( _vtype_, 4 ))

Gen_CBRT( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_CBRT( half, half_vec_t )
#endif

#undef Gen_CBRT1
#undef Gen_CBRT

/*
=================================================
	ToUNorm / ToSNorm
----
	T  ToUNorm (T x)				[-1, +1] to [ 0,  1]
	T  ToSNorm (T x)				[ 0,  1] to [-1, +1]
	T  ToUNorm (T x, T min, T max)	remap from [min, max] to [ 0,  1]
	T  ToSNorm (T x, T min, T max)	remap from [min, max] to [-1, +1]
=================================================
*/
#define Gen_TOUSNORM1( _type_ )														\
	ND_ _type_  ToUNorm (const _type_ x) {											\
		return FusedMulAdd( x, _type_(0.5), _type_(0.5) );							\
	}																				\
	ND_ _type_  ToSNorm (const _type_ x) {											\
		return FusedMulAdd( x, _type_(2.0), _type_(-1.0) );							\
	}																				\
	ND_ _type_  ToUNorm (const _type_ x, const _type_ minVal, const _type_ maxVal) {\
		return (x - minVal) / (maxVal - minVal);									\
	}																				\
	ND_ _type_  ToSNorm (const _type_ x, const _type_ minVal, const _type_ maxVal) {\
		return ToSNorm( ToUNorm( x, minVal, maxVal ));								\
	}																				\

#define Gen_TOUSNORM( _stype_, _vtype_ )\
	Gen_TOUSNORM1( _stype_ )\
	Gen_TOUSNORM1( UNITE( _vtype_, 2 ))\
	Gen_TOUSNORM1( UNITE( _vtype_, 3 ))\
	Gen_TOUSNORM1( UNITE( _vtype_, 4 ))

Gen_TOUSNORM( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_TOUSNORM( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_TOUSNORM( double, double_vec_t )
#endif

#undef Gen_TOUSNORM1
#undef Gen_TOUSNORM

/*
=================================================
	Square
----
	T  Square (T x)
=================================================
*/
#define Gen_SQUARE( _stype_, _vtype_ )\
	ND_ _stype_				Square (const _stype_			x)	{ return x * x; }\
	ND_ UNITE(_vtype_, 2 )	Square (const UNITE(_vtype_, 2) x)	{ return x * x; }\
	ND_ UNITE(_vtype_, 3 )	Square (const UNITE(_vtype_, 3) x)	{ return x * x; }\
	ND_ UNITE(_vtype_, 4 )	Square (const UNITE(_vtype_, 4) x)	{ return x * x; }

Gen_SQUARE( float,	float_vec_t )
Gen_SQUARE( int,	int_vec_t )
Gen_SQUARE( uint,	uint_vec_t )

#if AE_ENABLE_BYTE_TYPE
	Gen_SQUARE( sbyte,	sbyte_vec_t )
	Gen_SQUARE( ubyte,	ubyte_vec_t )
#endif
#if AE_ENABLE_SHORT_TYPE
	Gen_SQUARE( sshort,	sshort_vec_t )
	Gen_SQUARE( ushort,	ushort_vec_t )
#endif
#if AE_ENABLE_LONG_TYPE
	Gen_SQUARE( slong,	slong_vec_t )
	Gen_SQUARE( ulong,	ulong_vec_t )
#endif
#if AE_ENABLE_HALF_TYPE
	Gen_SQUARE( half,	half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_SQUARE( double,	double_vec_t )
#endif

#undef Gen_SQUARE

/*
=================================================
	Select
----
	T  Select (boolType condition, T ifTrue, T ifFalse)
----
	Same as per component 'condition ? ifTrue : ifFalse', both branches are always executed.
	GLSL specs: 'if' and '?' are branches.
=================================================
*/
#define Gen_SELECT1( _t1_, _t2_, _t3_, _t4_ )\
	ND_ _t1_  Select (const bool  condition, const _t1_ ifTrue, const _t1_ ifFalse)	{ _t1_ tmp[2] = {ifFalse, ifTrue};  return tmp[int(condition)]; }\
	ND_ _t2_  Select (const bool2 condition, const _t2_ ifTrue, const _t2_ ifFalse)	{ _t2_ tmp[2] = {ifFalse, ifTrue};  return _t2_( tmp[int(condition.x)].x, tmp[int(condition.y)].y ); }\
	ND_ _t3_  Select (const bool3 condition, const _t3_ ifTrue, const _t3_ ifFalse)	{ _t3_ tmp[2] = {ifFalse, ifTrue};  return _t3_( tmp[int(condition.x)].x, tmp[int(condition.y)].y, tmp[int(condition.z)].z ); }\
	ND_ _t4_  Select (const bool4 condition, const _t4_ ifTrue, const _t4_ ifFalse)	{ _t4_ tmp[2] = {ifFalse, ifTrue};  return _t4_( tmp[int(condition.x)].x, tmp[int(condition.y)].y, tmp[int(condition.z)].w, tmp[int(condition.z)].w ); }

#define Gen_SELECT( _stype_, _vtype_ )\
	Gen_SELECT1( _stype_, UNITE( _vtype_, 2 ), UNITE( _vtype_, 3 ), UNITE( _vtype_, 4 ))

Gen_SELECT( float,	float_vec_t )
Gen_SELECT( int,	int_vec_t )
Gen_SELECT( uint,	uint_vec_t )

#if AE_ENABLE_BYTE_TYPE
	Gen_SELECT( sbyte,	sbyte_vec_t )
	Gen_SELECT( ubyte,	ubyte_vec_t )
#endif
#if AE_ENABLE_SHORT_TYPE
	Gen_SELECT( sshort,	sshort_vec_t )
	Gen_SELECT( ushort,	ushort_vec_t )
#endif
#if AE_ENABLE_LONG_TYPE
	Gen_SELECT( slong,	slong_vec_t )
	Gen_SELECT( ulong,	ulong_vec_t )
#endif
#if AE_ENABLE_HALF_TYPE
	Gen_SELECT( half,	half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_SELECT( double,	double_vec_t )
#endif

#undef Gen_SELECT1
#undef Gen_SELECT

/*
=================================================
	BranchLess
----
	T  BranchLess (bool condition, T ifTrue, T ifFalse)
----
	Same as 'condition ? ifTrue : ifFalse' but both branches are always executed.
	GLSL specs: 'if' and '?' are branches.
=================================================
*/
#define Gen_BRANCHLESS1( _vtype_ )\
	ND_ _vtype_  BranchLess (const bool condition, const _vtype_ ifTrue, const _vtype_ ifFalse)	{ _vtype_ tmp[2] = {ifFalse, ifTrue};  return tmp[int(condition)]; }

#define Gen_BRANCHLESS( _stype_, _vtype_ )\
	Gen_BRANCHLESS1( _stype_    )\
	Gen_BRANCHLESS1( UNITE( _vtype_, 2 ))\
	Gen_BRANCHLESS1( UNITE( _vtype_, 3 ))\
	Gen_BRANCHLESS1( UNITE( _vtype_, 4 ))

Gen_BRANCHLESS( float,	float_vec_t )
Gen_BRANCHLESS( int,	int_vec_t )
Gen_BRANCHLESS( uint,	uint_vec_t )

#if AE_ENABLE_BYTE_TYPE
	Gen_BRANCHLESS( sbyte,	sbyte_vec_t )
	Gen_BRANCHLESS( ubyte,	ubyte_vec_t )
#endif
#if AE_ENABLE_SHORT_TYPE
	Gen_BRANCHLESS( sshort,	sshort_vec_t )
	Gen_BRANCHLESS( ushort,	ushort_vec_t )
#endif
#if AE_ENABLE_LONG_TYPE
	Gen_BRANCHLESS( slong,	slong_vec_t )
	Gen_BRANCHLESS( ulong,	ulong_vec_t )
#endif
#if AE_ENABLE_HALF_TYPE
	Gen_BRANCHLESS( half,	half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_BRANCHLESS( double,	double_vec_t )
#endif

#undef Gen_BRANCHLESS1
#undef Gen_BRANCHLESS

/*
=================================================
	Min / Max
	MinAbs / MaxAbs
	MinOf / MaxOf
	MinAbsOf / MaxAbsOf
=================================================
*/
#define Min3( a, b, c )			Min( Min( (a), (b) ), (c) )
#define Min4( a, b, c, d )		Min( Min( (a), (b) ), Min( (c), (d) ))
#define Max3( a, b, c )			Max( Max( (a), (b) ), (c) )
#define Max4( a, b, c, d )		Max( Max( (a), (b) ), Max( (c), (d) ))

#define MinAbs( _a_, _b_ )		Select( Less(Abs(_a_), Abs(_b_)), (_a_), (_b_) )
#define MaxAbs( _a_, _b_ )		Select( Greater(Abs(_a_), Abs(_b_)), (_a_), (_b_) )

#define MaxAbsOf( _vec_ )		MaxOf( Abs( _vec_ ))
#define MinAbsOf( _vec_ )		MinOf( Abs( _vec_ ))

#define Gen_MINMAX( _stype_, _vtype_ )													\
	ND_ _stype_  MinOf (UNITE( _vtype_, 2)  a)	{ return Min( a.x, a.y ); }				\
	ND_ _stype_  MinOf (UNITE( _vtype_, 3)  a)	{ return Min3( a.x, a.y, a.z ); }		\
	ND_ _stype_  MinOf (UNITE( _vtype_, 4)  a)	{ return Min4( a.x, a.y, a.z, a.w ); }	\
	ND_ _stype_  MaxOf (UNITE( _vtype_, 2)  a)	{ return Max( a.x, a.y ); }				\
	ND_ _stype_  MaxOf (UNITE( _vtype_, 3)  a)	{ return Max3( a.x, a.y, a.z ); }		\
	ND_ _stype_  MaxOf (UNITE( _vtype_, 4)  a)	{ return Max4( a.x, a.y, a.z, a.w ); }

Gen_MINMAX( float,	float_vec_t )
Gen_MINMAX( int,	int_vec_t )
Gen_MINMAX( uint,	uint_vec_t )

#if AE_ENABLE_BYTE_TYPE
	Gen_MINMAX( sbyte,	sbyte_vec_t )
	Gen_MINMAX( ubyte,	ubyte_vec_t )
#endif
#if AE_ENABLE_SHORT_TYPE
	Gen_MINMAX( sshort,	sshort_vec_t )
	Gen_MINMAX( ushort,	ushort_vec_t )
#endif
#if AE_ENABLE_LONG_TYPE
	Gen_MINMAX( slong,	slong_vec_t )
	Gen_MINMAX( ulong,	ulong_vec_t )
#endif
#if AE_ENABLE_HALF_TYPE
	Gen_MINMAX( half,	half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_MINMAX( double,	double_vec_t )
#endif

#undef Gen_MINMAX

/*
=================================================
	LengthSq / InvLength
	DistanceSq / InvDistance
----
	Scalar  LengthSq (Vec x)
	Scalar  InvLength (Vec x)
	Scalar  DistanceSq (Vec x, Vec y)
	Scalar  InvDistance (Vec x, Vec y)
=================================================
*/
#define Gen_LENGTHSQ_DISTANCESQ1( _stype_, _vtype_ )																\
	ND_ _stype_  LengthSq (const _vtype_ x)						{ return Dot( x, x ); }								\
	ND_ _stype_  InvLength (const _vtype_ x)					{ return InvSqrt( Dot( x, x )); }					\
	ND_ _stype_  DistanceSq (const _vtype_ x, const _vtype_ y)  { _vtype_ r = x - y;  return Dot( r, r ); }			\
	ND_ _stype_  InvDistance (const _vtype_ x, const _vtype_ y) { _vtype_ r = x - y;  return InvSqrt( Dot( r, r )); }

#define Gen_LENGTHSQ_DISTANCESQ( _stype_, _vtype_ )\
	Gen_LENGTHSQ_DISTANCESQ1( _stype_, UNITE( _vtype_, 2 ))\
	Gen_LENGTHSQ_DISTANCESQ1( _stype_, UNITE( _vtype_, 3 ))\
	Gen_LENGTHSQ_DISTANCESQ1( _stype_, UNITE( _vtype_, 4 ))

Gen_LENGTHSQ_DISTANCESQ( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_LENGTHSQ_DISTANCESQ( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_LENGTHSQ_DISTANCESQ( double, double_vec_t )
#endif

#undef Gen_LENGTHSQ_DISTANCESQ1
#undef Gen_LENGTHSQ_DISTANCESQ

/*
=================================================
	fast Sign
----
	T  Sign (T x)
----
	returns -1 or +1
	see [GPU Benchmarks: Shader instructions performance groups line:Shader-instructions-performance-groups](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/GPU_Benchmarks.md) for fast Sign performance.
	see [GPU Benchmarks: NaN line:NaN](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/GPU_Benchmarks.md) for 'Step()' results.
=================================================
*/
#define Gen_SIGN1( _vtype_ )\
	ND_ _vtype_  Sign (_vtype_ v)	{ return ToSNorm( GreaterF( v, _vtype_(0.0) )); }

#define Gen_SIGN( _stype_, _vtype_ )\
	Gen_SIGN1( _stype_ )\
	Gen_SIGN1( UNITE( _vtype_, 2 ))\
	Gen_SIGN1( UNITE( _vtype_, 3 ))\
	Gen_SIGN1( UNITE( _vtype_, 4 ))

Gen_SIGN( float,	float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_SIGN( half,		half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_SIGN( double,	double_vec_t )
#endif
#undef Gen_SIGN1
#undef Gen_SIGN

#define Gen_SIGN1( _vtype_ )\
	ND_ _vtype_  Sign (const _vtype_ v)	{ return Select( Less( v, _vtype_(0) ), _vtype_(-1), _vtype_(1) ); }

#define Gen_SIGN( _stype_, _vtype_ )\
	ND_ _stype_  Sign (const _stype_ x)  { return x < _stype_(0) ? _stype_(-1) : _stype_(1); }\
	Gen_SIGN1( UNITE( _vtype_, 2 ))\
	Gen_SIGN1( UNITE( _vtype_, 3 ))\
	Gen_SIGN1( UNITE( _vtype_, 4 ))

Gen_SIGN( int,		int_vec_t )

#if AE_ENABLE_BYTE_TYPE
	Gen_SIGN( sbyte,	sbyte_vec_t )
#endif
#if AE_ENABLE_SHORT_TYPE
	Gen_SIGN( sshort,	sshort_vec_t )
#endif
#if AE_ENABLE_LONG_TYPE
	Gen_SIGN( slong,	slong_vec_t )
#endif
#undef Gen_SIGN1
#undef Gen_SIGN

/*
=================================================
	SignBit
----
	bool  SignBit (T x)
=================================================
*/
ND_ bool	SignBit (float  x)	{ return (floatBitsToUint( x ) & 0x80000000u) != 0; }
ND_ bool2	SignBit (float2 v)	{ return bool2( SignBit( v.x ), SignBit( v.y )); }
ND_ bool3	SignBit (float3 v)	{ return bool3( SignBit( v.x ), SignBit( v.y ), SignBit( v.z )); }
ND_ bool4	SignBit (float4 v)	{ return bool4( SignBit( v.x ), SignBit( v.y ), SignBit( v.z ), SignBit( v.w )); }

#if AE_ENABLE_HALF_TYPE and AE_ENABLE_SHORT_TYPE
	ND_ bool	SignBit (half  x)	{ return (halfBitsToUint16( x ) & 0x8000us) != 0; }
	ND_ bool2	SignBit (half2 v)	{ return bool2( SignBit( v.x ), SignBit( v.y )); }
	ND_ bool3	SignBit (half3 v)	{ return bool3( SignBit( v.x ), SignBit( v.y ), SignBit( v.z )); }
	ND_ bool4	SignBit (half4 v)	{ return bool4( SignBit( v.x ), SignBit( v.y ), SignBit( v.z ), SignBit( v.w )); }
#endif
#if AE_ENABLE_DOUBLE_TYPE and AE_ENABLE_LONG_TYPE
	ND_ bool	SignBit (double  x)	{ return (doubleBitsToUint64( x ) & 0x8000000000000000ul) != 0; }
	ND_ bool2	SignBit (double2 v)	{ return bool2( SignBit( v.x ), SignBit( v.y )); }
	ND_ bool3	SignBit (double3 v)	{ return bool3( SignBit( v.x ), SignBit( v.y ), SignBit( v.z )); }
	ND_ bool4	SignBit (double4 v)	{ return bool4( SignBit( v.x ), SignBit( v.y ), SignBit( v.z ), SignBit( v.w )); }
#endif

/*
=================================================
	Rcp
----
	Reciprocal - fast division with less accuracy
=================================================
*/
ND_ float	Rcp (const float  x)		{ return 1.0f / x; }
ND_ float2	Rcp (const float2 x)		{ return 1.0f / x; }
ND_ float3	Rcp (const float3 x)		{ return 1.0f / x; }
ND_ float4	Rcp (const float4 x)		{ return 1.0f / x; }

#if AE_ENABLE_HALF_TYPE
	ND_ half	Rcp (const half  x)		{ return 1.0hf / x; }
	ND_ half2	Rcp (const half2 x)		{ return 1.0hf / x; }
	ND_ half3	Rcp (const half3 x)		{ return 1.0hf / x; }
	ND_ half4	Rcp (const half4 x)		{ return 1.0hf / x; }
#endif
#if AE_ENABLE_DOUBLE_TYPE
	ND_ double	Rcp (const double  x)	{ return 1.0lf / x; }
	ND_ double2	Rcp (const double2 x)	{ return 1.0lf / x; }
	ND_ double3	Rcp (const double3 x)	{ return 1.0lf / x; }
	ND_ double4	Rcp (const double4 x)	{ return 1.0lf / x; }
#endif

/*
=================================================
	LinearStep
----
	T  LinearStep (T x, T edge0, T edge1)
----                                          ___
	returns value in range [0, 1], pattern __/
=================================================
*/
#define Gen_LINEARSTEP1( _vtype_, _stype_ )													\
	ND_ _vtype_  LinearStep (const _vtype_ x, const _vtype_ edge0, const _vtype_ edge1) {	\
		return Saturate( (x - edge0) / (edge1 - edge0) );									\
	}																						\
																							\
	ND_ _vtype_  LinearStep (const _vtype_ x, const _stype_ edge0, const _stype_ edge1) {	\
		return Saturate( (x - edge0) / (edge1 - edge0) );									\
	}

#define Gen_LINEARSTEP( _stype_, _vtype_ )													\
	ND_ _stype_  LinearStep (const _stype_ x, const _stype_ edge0, const _stype_ edge1) {	\
		return Saturate( (x - edge0) / (edge1 - edge0) );									\
	}																						\
	Gen_LINEARSTEP1( UNITE( _vtype_, 2 ), _stype_ )											\
	Gen_LINEARSTEP1( UNITE( _vtype_, 3 ), _stype_ )											\
	Gen_LINEARSTEP1( UNITE( _vtype_, 4 ), _stype_ )

Gen_LINEARSTEP( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_LINEARSTEP( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_LINEARSTEP( double, double_vec_t )
#endif

#undef Gen_LINEARSTEP1
#undef Gen_LINEARSTEP

/*
=================================================
	BumpStep
----
	T  BumpStep (T x, T edge0, T edge1)
----
	returns value in range [0, 1], pattern __/\__
=================================================
*/
#define Gen_BUMPSTEP1( _vtype_, _stype_ )																	\
	ND_ _vtype_  BumpStep (const _vtype_ x, const _vtype_ edge0, const _vtype_ edge1) {						\
		return _stype_(1) - Abs( Saturate( (x - edge0) / (edge1 - edge0) ) - _stype_(0.5) ) * _stype_(2.0);	\
	}																										\
																											\
	ND_ _vtype_  BumpStep (const _vtype_ x, const _stype_ edge0, const _stype_ edge1) {						\
		return _stype_(1) - Abs( Saturate( (x - edge0) / (edge1 - edge0) ) - _stype_(0.5) ) * _stype_(2.0);	\
	}

#define Gen_BUMPSTEP( _stype_, _vtype_ )																	\
	ND_ _stype_  BumpStep (const _stype_ x, const _stype_ edge0, const _stype_ edge1) {						\
		return _stype_(1) - Abs( Saturate( (x - edge0) / (edge1 - edge0) ) - _stype_(0.5) ) * _stype_(2.0);	\
	}																										\
	Gen_BUMPSTEP1( UNITE( _vtype_, 2 ), _stype_ )															\
	Gen_BUMPSTEP1( UNITE( _vtype_, 3 ), _stype_ )															\
	Gen_BUMPSTEP1( UNITE( _vtype_, 4 ), _stype_ )

Gen_BUMPSTEP( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_BUMPSTEP( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_BUMPSTEP( double, double_vec_t )
#endif

#undef Gen_BUMPSTEP1
#undef Gen_BUMPSTEP

/*
=================================================
	SmoothBumpStep
----
	T  SmoothBumpStep (T x, T edge0, T edge1)
----
	returns value in range [0, 1], pattern __/\__
=================================================
*/
#define Gen_SMOOTHBUMPSTEP1( _vtype_, _stype_ )											\
	ND_ _vtype_  SmoothBumpStep (_vtype_ x, const _vtype_ edge0, const _vtype_ edge1) {	\
		x = BumpStep( x, edge0, edge1 );												\
		return x * x * (_stype_(3.0) - _stype_(2.0) * x);								\
	}																					\
																						\
	ND_ _vtype_  SmoothBumpStep (_vtype_ x, const _stype_ edge0, const _stype_ edge1) {	\
		x = BumpStep( x, edge0, edge1 );												\
		return x * x * (_stype_(3.0) - _stype_(2.0) * x);								\
	}

#define Gen_SMOOTHBUMPSTEP( _stype_, _vtype_ )											\
	ND_ _stype_  SmoothBumpStep (_stype_ x, const _stype_ edge0, const _stype_ edge1) {	\
		x = BumpStep( x, edge0, edge1 );												\
		return x * x * (_stype_(3.0) - _stype_(2.0) * x);								\
	}																					\
	Gen_SMOOTHBUMPSTEP1( UNITE( _vtype_, 2 ), _stype_ )									\
	Gen_SMOOTHBUMPSTEP1( UNITE( _vtype_, 3 ), _stype_ )									\
	Gen_SMOOTHBUMPSTEP1( UNITE( _vtype_, 4 ), _stype_ )

Gen_SMOOTHBUMPSTEP( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_SMOOTHBUMPSTEP( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_SMOOTHBUMPSTEP( double, double_vec_t )
#endif

#undef Gen_SMOOTHBUMPSTEP1
#undef Gen_SMOOTHBUMPSTEP

/*
=================================================
	TriangleWave
----
	T  TriangleWave (T x)
----
	returns value in range [0, 1], pattern /\/\/\
=================================================
*/
#define Gen_TRIANGLEWAVE1( _vtype_, _stype_ )							\
	ND_ _vtype_  TriangleWave (_vtype_ x) {								\
		x = Fract( x );  return Min( x, _stype_(1) - x ) * _stype_(2);	\
	}

#define Gen_TRIANGLEWAVE( _stype_, _vtype_ )\
	Gen_TRIANGLEWAVE1( _stype_,				_stype_ )\
	Gen_TRIANGLEWAVE1( UNITE( _vtype_, 2 ), _stype_ )\
	Gen_TRIANGLEWAVE1( UNITE( _vtype_, 3 ), _stype_ )\
	Gen_TRIANGLEWAVE1( UNITE( _vtype_, 4 ), _stype_ )

Gen_TRIANGLEWAVE( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_TRIANGLEWAVE( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_TRIANGLEWAVE( double, double_vec_t )
#endif

#undef Gen_TRIANGLEWAVE1
#undef Gen_TRIANGLEWAVE

/*
=================================================
	Steps
----
	T  Steps (T x, T stepCount)
----
	returns X in range [0, 1], with steps pattern _-
	returns Y in range [0, 1], with linear interpolation per step /
=================================================
*/
ND_ float2  Steps (float x, const float stepCount)
{
	x = x * stepCount;
	float i = Floor( x );
	return float2( i / stepCount, x - i );
}

/*
=================================================
	FloorToBase, RoundToBase
=================================================
*/
#define	FloorToBase( _val_, _base_ )		(Floor( (_val_) / (_base_) ) * (_base_))
#define	RoundToBase( _val_, _base_ )		(Round( (_val_) / (_base_) ) * (_base_))

/*
=================================================
	FloorToPOT, RoundToPOT
----
	round to power of 2
=================================================
*/
#define FloorToPOT( _val_ )					Exp2( Floor( Log2( _val_ )))
#define RoundToPOT( _val_ )					Exp2( Round( Log2( _val_ )))

/*
=================================================
	ClampOut (float)
=================================================
*/
#define Gen_CLAMPOUT1( _stype_, _vtype_ )												\
	ND_ _vtype_  ClampOut (const _vtype_ x, const _vtype_ minVal, const _vtype_ maxVal)	\
	{																					\
		_vtype_	mid = (minVal * _stype_(0.5)) + (maxVal * _stype_(0.5));				\
		return Lerp( Max( x, maxVal ), Min( x, minVal ), LessF( x, mid ));				\
	}
#define Gen_CLAMPOUT2( _stype_, _vtype_ )												\
	Gen_CLAMPOUT1( _stype_, _vtype_ )													\
	ND_ _vtype_  ClampOut (const _vtype_ x, const _stype_ minVal, const _stype_ maxVal)	\
	{																					\
		_stype_	mid = (minVal * _stype_(0.5)) + (maxVal * _stype_(0.5));				\
		return Lerp( Max( x, maxVal ), Min( x, minVal ), LessF( x, _vtype_(mid) ));		\
	}

#define Gen_CLAMPOUT( _stype_, _vtype_ )\
	Gen_CLAMPOUT1( _stype_, _stype_ )\
	Gen_CLAMPOUT2( _stype_, UNITE( _vtype_, 2 ))\
	Gen_CLAMPOUT2( _stype_, UNITE( _vtype_, 3 ))\
	Gen_CLAMPOUT2( _stype_, UNITE( _vtype_, 4 ))

Gen_CLAMPOUT( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_CLAMPOUT( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_CLAMPOUT( double, double_vec_t )
#endif

#undef Gen_CLAMPOUT1
#undef Gen_CLAMPOUT

/*
=================================================
	ClampOut (int)
=================================================
*/
#define Gen_CLAMPOUT1( _type_ )																	\
	ND_ _type_  ClampOut (const _type_ x, const _type_ minVal, const _type_ maxVal)				\
	{																							\
		_type_	mid = (minVal + _type_(1)) / _type_(2) + (maxVal + _type_(1)) / _type_(2);		\
		return x < mid ? Min( x, minVal ) : Max( x, maxVal );									\
	}
#define Gen_CLAMPOUT( _stype_, _vtype_ )																	\
	Gen_CLAMPOUT1( _stype_ )																				\
	ND_ UNITE(_vtype_,2)  ClampOut (const UNITE(_vtype_,2) v, const _stype_ minVal, const _stype_ maxVal) {	\
		return UNITE(_vtype_,2)( ClampOut( v.x, minVal, maxVal ),											\
								 ClampOut( v.y, minVal, maxVal ));											\
	}																										\
	ND_ UNITE(_vtype_,3)  ClampOut (const UNITE(_vtype_,3) v, const _stype_ minVal, const _stype_ maxVal) {	\
		return UNITE(_vtype_,3)( ClampOut( v.x, minVal, maxVal ),											\
								 ClampOut( v.y, minVal, maxVal ),											\
								 ClampOut( v.z, minVal, maxVal ));											\
	}																										\
	ND_ UNITE(_vtype_,4)  ClampOut (const UNITE(_vtype_,4) v, const _stype_ minVal, const _stype_ maxVal) {	\
		return UNITE(_vtype_,4)( ClampOut( v.x, minVal, maxVal ),											\
								 ClampOut( v.y, minVal, maxVal ),											\
								 ClampOut( v.z, minVal, maxVal ),											\
								 ClampOut( v.w, minVal, maxVal ));											\
	}

Gen_CLAMPOUT( int,	int_vec_t )
Gen_CLAMPOUT( uint,	uint_vec_t )

#if AE_ENABLE_BYTE_TYPE
	Gen_CLAMPOUT( sbyte,	sbyte_vec_t )
	Gen_CLAMPOUT( ubyte,	ubyte_vec_t )
#endif
#if AE_ENABLE_SHORT_TYPE
	Gen_CLAMPOUT( sshort,	sshort_vec_t )
	Gen_CLAMPOUT( ushort,	ushort_vec_t )
#endif
#if AE_ENABLE_LONG_TYPE
	Gen_CLAMPOUT( slong,	slong_vec_t )
	Gen_CLAMPOUT( ulong,	ulong_vec_t )
#endif

#undef Gen_CLAMPOUT1
#undef Gen_CLAMPOUT

/*
=================================================
	Wrap (float)
=================================================
*/
#define Gen_WRAP1( _stype_, _vtype_ )												\
	ND_ _vtype_  Wrap (const _vtype_ x, const _vtype_ minVal, const _vtype_ maxVal)	\
	{																				\
		_vtype_  size = maxVal - minVal;											\
		_vtype_  res  = minVal + Mod( x - minVal, size );							\
		return res + size * LessF( res, minVal );									\
	}
#define Gen_WRAP2( _stype_, _vtype_ )												\
	Gen_WRAP1( _stype_, _vtype_ )													\
	ND_ _vtype_  Wrap (const _vtype_ x, const _stype_ minVal, const _stype_ maxVal){\
		return Wrap( x, _vtype_(minVal), _vtype_(maxVal) );							\
	}

#define Gen_WRAP( _stype_, _vtype_ )\
	Gen_WRAP1( _stype_, _stype_ )\
	Gen_WRAP2( _stype_, UNITE( _vtype_, 2 ))\
	Gen_WRAP2( _stype_, UNITE( _vtype_, 3 ))\
	Gen_WRAP2( _stype_, UNITE( _vtype_, 4 ))

Gen_WRAP( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_WRAP( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_WRAP( double, double_vec_t )
#endif

#undef Gen_WRAP1
#undef Gen_WRAP

/*
=================================================
	Wrap (int)
=================================================
*/
#define Gen_WRAP1( _type_ )														\
	ND_ _type_  Wrap (const _type_ x, const _type_ minVal, const _type_ maxVal)	\
	{																			\
		_type_  size = maxVal + _type_(1) - minVal;								\
		_type_  res  = minVal + ((x - minVal) % size);							\
		if ( res < minVal ) res += size;										\
		return res;																\
	}
#define Gen_WRAP( _stype_, _vtype_ )																	\
	Gen_WRAP1( _stype_ )																				\
	ND_ UNITE(_vtype_,2)  Wrap (const UNITE(_vtype_,2) v, const _stype_ minVal, const _stype_ maxVal) {	\
		return UNITE(_vtype_,2)( Wrap( v.x, minVal, maxVal ),											\
								 Wrap( v.y, minVal, maxVal ));											\
	}																									\
	ND_ UNITE(_vtype_,3)  Wrap (const UNITE(_vtype_,3) v, const _stype_ minVal, const _stype_ maxVal) {	\
		return UNITE(_vtype_,3)( Wrap( v.x, minVal, maxVal ),											\
								 Wrap( v.y, minVal, maxVal ),											\
								 Wrap( v.z, minVal, maxVal ));											\
	}																									\
	ND_ UNITE(_vtype_,4)  Wrap (const UNITE(_vtype_,4) v, const _stype_ minVal, const _stype_ maxVal) {	\
		return UNITE(_vtype_,4)( Wrap( v.x, minVal, maxVal ),											\
								 Wrap( v.y, minVal, maxVal ),											\
								 Wrap( v.z, minVal, maxVal ),											\
								 Wrap( v.w, minVal, maxVal ));											\
	}																									\
	ND_ UNITE(_vtype_,2)  Wrap (const UNITE(_vtype_,2) v, const UNITE(_vtype_,2) minVal, const UNITE(_vtype_,2) maxVal) {	\
		return UNITE(_vtype_,2)( Wrap( v.x, minVal.x, maxVal.x ),  Wrap( v.y, minVal.y, maxVal.y ));						\
	}																														\
	ND_ UNITE(_vtype_,3)  Wrap (const UNITE(_vtype_,3) v, const UNITE(_vtype_,3) minVal, const UNITE(_vtype_,3) maxVal) {	\
		return UNITE(_vtype_,3)( Wrap( v.x, minVal.x, maxVal.x ),															\
								 Wrap( v.y, minVal.y, maxVal.y ),															\
								 Wrap( v.z, minVal.z, maxVal.z ));															\
	}																														\
	ND_ UNITE(_vtype_,4)  Wrap (const UNITE(_vtype_,4) v, const UNITE(_vtype_,4) minVal, const UNITE(_vtype_,4) maxVal) {	\
		return UNITE(_vtype_,4)( Wrap( v.x, minVal.x, maxVal.x ),  Wrap( v.y, minVal.y, maxVal.y ),							\
								 Wrap( v.z, minVal.z, maxVal.z ),  Wrap( v.w, minVal.w, maxVal.w ));						\
	}

Gen_WRAP( int,	int_vec_t )
Gen_WRAP( uint,	uint_vec_t )

#if AE_ENABLE_BYTE_TYPE
	Gen_WRAP( sbyte,	sbyte_vec_t )
	Gen_WRAP( ubyte,	ubyte_vec_t )
#endif
#if AE_ENABLE_SHORT_TYPE
	Gen_WRAP( sshort,	sshort_vec_t )
	Gen_WRAP( ushort,	ushort_vec_t )
#endif
#if AE_ENABLE_LONG_TYPE
	Gen_WRAP( slong,	slong_vec_t )
	Gen_WRAP( ulong,	ulong_vec_t )
#endif

#undef Gen_WRAP1
#undef Gen_WRAP

/*
=================================================
	MirroredWrap (float)
=================================================
*/
#define Gen_MIRWRAP1( _stype_, _vtype_ )													\
	ND_ _vtype_  MirroredWrap (const _vtype_ x, const _vtype_ minVal, const _vtype_ maxVal)	\
	{																						\
		_vtype_  size = (maxVal - minVal) * _stype_(2.0);									\
		_vtype_  res  = Fract( (x - minVal) / size );										\
		return Min( res, _stype_(1.0) - res ) * size + minVal;								\
	}
#define Gen_MIRWRAP2( _stype_, _vtype_ )													\
	Gen_MIRWRAP1( _stype_, _vtype_ )														\
	ND_ _vtype_  MirroredWrap (const _vtype_ x, const _stype_ minVal, const _stype_ maxVal){\
		return MirroredWrap( x, _vtype_(minVal), _vtype_(maxVal) );							\
	}

#define Gen_MIRWRAP( _stype_, _vtype_ )\
	Gen_MIRWRAP1( _stype_, _stype_ )\
	Gen_MIRWRAP2( _stype_, UNITE( _vtype_, 2 ))\
	Gen_MIRWRAP2( _stype_, UNITE( _vtype_, 3 ))\
	Gen_MIRWRAP2( _stype_, UNITE( _vtype_, 4 ))

Gen_MIRWRAP( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_MIRWRAP( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_MIRWRAP( double, double_vec_t )
#endif

#undef Gen_MIRWRAP1
#undef Gen_MIRWRAP

/*
=================================================
	MirroredWrap (int)
=================================================
*/
#define Gen_MIRWRAP1( _stype_, _vtype_ )													\
	ND_ _vtype_  MirroredWrap (const _vtype_ x, const _vtype_ minVal, const _vtype_ maxVal)	\
	{																						\
		_vtype_  size = (maxVal - minVal) * _stype_(2);										\
		_vtype_  res  = Abs( x - minVal ) % size;											\
		return Min( res, size - res ) + minVal;												\
	}
#define Gen_MIRWRAP2( _stype_, _vtype_ )													\
	Gen_MIRWRAP1( _stype_, _vtype_ )														\
	ND_ _vtype_  MirroredWrap (const _vtype_ x, const _stype_ minVal, const _stype_ maxVal){\
		return MirroredWrap( x, _vtype_(minVal), _vtype_(maxVal) );							\
	}

#define Gen_MIRWRAP( _stype_, _vtype_ )\
	Gen_MIRWRAP1( _stype_, _stype_ )\
	Gen_MIRWRAP2( _stype_, UNITE( _vtype_, 2 ))\
	Gen_MIRWRAP2( _stype_, UNITE( _vtype_, 3 ))\
	Gen_MIRWRAP2( _stype_, UNITE( _vtype_, 4 ))

Gen_MIRWRAP( int,	int_vec_t )

#if AE_ENABLE_BYTE_TYPE
	Gen_MIRWRAP( sbyte,	sbyte_vec_t )
#endif
#if AE_ENABLE_SHORT_TYPE
	Gen_MIRWRAP( sshort, sshort_vec_t )
#endif
#if AE_ENABLE_LONG_TYPE
	Gen_MIRWRAP( slong,	slong_vec_t )
#endif

#undef Gen_MIRWRAP1
#undef Gen_MIRWRAP

//-----------------------------------------------------------------------------
// bit operations

ND_ int  BitRotateLeft (const int x, uint shift)
{
	const uint mask = 31u;
	shift = shift & mask;
	return (x << shift) | (x >> ( ~(shift-1u) & mask ));
}

ND_ uint  BitRotateLeft (const uint x, uint shift)
{
	const uint mask = 31u;
	shift = shift & mask;
	return (x << shift) | (x >> ( ~(shift-1u) & mask ));
}


ND_ int  BitRotateRight (const int x, uint shift)
{
	const uint mask = 31u;
	shift = shift & mask;
	return (x >> shift) | (x << ( ~(shift-1u) & mask ));
}

ND_ uint  BitRotateRight (const uint x, uint shift)
{
	const uint mask = 31u;
	shift = shift & mask;
	return (x >> shift) | (x << ( ~(shift-1u) & mask ));
}

ND_ bool  HasBit (const uint value, const uint index)
{
	return (value & (1u << index)) != 0;
}

//	extract lowest non-zero bit.
//	returns zero if 'bits' is zero.
ND_ uint  ExtractBit (inout uint bits)
{
	uint	result = bits & ~(bits - 1u);
	bits = bits & ~result;
	return result;
}

//	extract highest non-zero bit.
//	returns zero if 'value' is zero.
ND_ uint  ExtractHighBit (inout uint bits)
{
	uint	result = bits & ~(bits >> 1);
	bits = bits & ~result;
	return result;
}

//  return '~0u' if 'bits' is zero.
ND_ uint  ExtractBitIndex (inout uint bits)
{
	return uint(IntLog2( ExtractBit( INOUT bits )));
}

/*
=================================================
	BaryLerp
----
	T  BaryLerp (T v0, T v1, T v2, Vec3 barycentrics)  -- barycentric interpolation
	T  BaryLerp (T v0, T v1, T v2, Vec2 barycentrics)  -- barycentric interpolation with much better precision
=================================================
*/
// TODO: use Dot ?
#define Gen_BARYLERP1( _type_, _bary3_, _bary2_ )\
	ND_ _type_  BaryLerp (const _type_ v0, const _type_ v1, const _type_ v2, const _bary3_ barycentrics)  { return v0 * barycentrics.x + v1 * barycentrics.y + v2 * barycentrics.z; }\
	ND_ _type_  BaryLerp (const _type_ v0, const _type_ v1, const _type_ v2, const _bary2_ barycentrics)  { return v0 + FusedMulAdd( _type_(barycentrics.x), (v1 - v0), barycentrics.y * (v2 - v0) ); }

#define Gen_BARYLERP( _stype_, _vtype_ )\
	Gen_BARYLERP1( _stype_,				UNITE( _vtype_, 3 ), UNITE( _vtype_, 2 ))\
	Gen_BARYLERP1( UNITE( _vtype_, 2 ), UNITE( _vtype_, 3 ), UNITE( _vtype_, 2 ))\
	Gen_BARYLERP1( UNITE( _vtype_, 3 ), UNITE( _vtype_, 3 ), UNITE( _vtype_, 2 ))\
	Gen_BARYLERP1( UNITE( _vtype_, 4 ), UNITE( _vtype_, 3 ), UNITE( _vtype_, 2 ))

Gen_BARYLERP( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_BARYLERP( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_BARYLERP( double, double_vec_t )
#endif

#undef Gen_BARYLERP1
#undef Gen_BARYLERP

/*
=================================================
	BiLerp, BiCubic
----
	T  BiLerp (T x1y1, T x2y1, T x1y2, T x2y2, Vec2 factor)
	T  BiCubic (T x1y1, T x2y1, T x1y2, T x2y2, Vec2 factor)
----
	bilinear and bicubic interpolation
=================================================
*/
#define Gen_BILERP1( _type_, _factor_, _stype_ )\
	ND_ _type_  BiLerp (const _type_ x1y1, const _type_ x2y1, const _type_ x1y2, const _type_ x2y2, const _factor_ factor)	{ return Lerp( Lerp( x1y1, x2y1, factor.x ), Lerp( x1y2, x2y2, factor.x ), factor.y ); }\
	ND_ _type_  BiCubic (const _type_ x1y1, const _type_ x2y1, const _type_ x1y2, const _type_ x2y2, const _factor_ factor)	{ return BiLerp( x1y1, x2y1, x1y2, x2y2,  factor * factor * (_stype_(3.0) - _stype_(2.0) * factor) ); }

#define Gen_BILERP( _stype_, _vtype_ )\
	Gen_BILERP1( _stype_,				UNITE( _vtype_, 2 ), _stype_ )\
	Gen_BILERP1( UNITE( _vtype_, 2 ),	UNITE( _vtype_, 2 ), _stype_ )\
	Gen_BILERP1( UNITE( _vtype_, 3 ),	UNITE( _vtype_, 2 ), _stype_ )\
	Gen_BILERP1( UNITE( _vtype_, 4 ),	UNITE( _vtype_, 2 ), _stype_ )

Gen_BILERP( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_BILERP( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_BILERP( double, double_vec_t )
#endif

#undef Gen_BILERP1
#undef Gen_BILERP

/*
=================================================
	Remap
----
	T  Remap (Vec2 src, Vec2 dst, T v)
	T  RemapDst (Vec2 dst, T v)
	T  RemapSrc (Vec2 src, T v)
----
	Remap - map 'v' in 'src' interval to 'dst' interval.
	RemapDst - map 'v' in [0,1] interval to 'dst' interval.
	RemapSrc - map 'v' in 'src' interval to [0,1] interval.
	Interval is a scalar range which specified for all components in vector.
=================================================
*/
#define Gen_REMAP1( _type_, _range_ )\
	ND_ _type_  RemapDst (const _type_ unormVal, const _range_ dstRange)		{ return unormVal * (dstRange.y - dstRange.x) + dstRange.x; }\
	ND_ _type_  RemapSrc (const _range_ srcRange, const _type_ valInSrc)		{ return (valInSrc - srcRange.x) / (srcRange.y - srcRange.x); }\
	ND_ _type_  Remap (const _range_ src, const _range_ dst, const _type_ v)	{ return RemapDst( RemapSrc( src, v ), dst ); }

#define Gen_REMAP2( _type_ )\
	ND_ _type_  RemapDst (const _type_ unormVal, const _type_ dst0, const _type_ dst1)	{ return unormVal * (dst1 - dst0) + dst0; }\
	ND_ _type_  RemapSrc (const _type_ src0, const _type_ src1, const _type_ valInSrc)	{ return (valInSrc - src0) / (src1 - src0); }\
	ND_ _type_  Remap (const _type_ src0, const _type_ src1, const _type_ dst0, const _type_ dst1, const _type_ v)	{ return RemapDst( RemapSrc( src0, src1, v ), dst0, dst1 ); }

#define Gen_REMAP( _stype_, _vtype_ )\
	Gen_REMAP1( _stype_,				UNITE( _vtype_, 2 ))\
	Gen_REMAP1( UNITE( _vtype_, 2 ),	UNITE( _vtype_, 2 ))\
	Gen_REMAP1( UNITE( _vtype_, 3 ),	UNITE( _vtype_, 2 ))\
	Gen_REMAP1( UNITE( _vtype_, 4 ),	UNITE( _vtype_, 2 ))\
	Gen_REMAP2( UNITE( _vtype_, 2 ))\
	Gen_REMAP2( UNITE( _vtype_, 3 ))\
	Gen_REMAP2( UNITE( _vtype_, 4 ))

Gen_REMAP( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_REMAP( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_REMAP( double, double_vec_t )
#endif

#undef Gen_REMAP2
#undef Gen_REMAP1
#undef Gen_REMAP

// remap + easing
#define RemapEase( _easeFn_, _src_, _dst_, _val_ )						RemapDst( _easeFn_( RemapSrc( (_src_), (_val_) )), (_dst_) )
#define RemapEaseV( _easeFn_, _src0_, _src1_, _dst0_, _dst1_, _val_ )	RemapDst( _easeFn_( RemapSrc( (_src0_), (_src1_), (_val_) )), (_dst0_), (_dst1_) )

#define RemapClamp( _src_, _dst_, _val_ )								RemapEase( Saturate, (_src_), (_dst_), (_val_) )
#define RemapWrap( _src_, _dst_, _val_ )								RemapEase( Fract, (_src_), (_dst_), (_val_) )
#define RemapWrapMirror( _src_, _dst_, _val_ )							RemapEase( TriangleWave, (_src_), (_dst_), (_val_) )

/*
=================================================
	UIndexToUNormFloor / UIndexToSNormFloor
----
	map coordinate 'index' in N dimension with size 'count'
	to unorm value with rounding downwards
=================================================
*/
ND_ float   UIndexToUNormFloor (const int  index, const int  count)			{ return float( index) / float( count - 1); }
ND_ float2  UIndexToUNormFloor (const int2 index, const int2 count)			{ return float2(index) / float2(count - 1); }
ND_ float3  UIndexToUNormFloor (const int3 index, const int3 count)			{ return float3(index) / float3(count - 1); }
ND_ float4  UIndexToUNormFloor (const int4 index, const int4 count)			{ return float4(index) / float4(count - 1); }

ND_ float   UIndexToUNormFloor (const uint  index, const uint  count)		{ return float( index) / float( count - 1u); }
ND_ float2  UIndexToUNormFloor (const uint2 index, const uint2 count)		{ return float2(index) / float2(count - 1u); }
ND_ float3  UIndexToUNormFloor (const uint3 index, const uint3 count)		{ return float3(index) / float3(count - 1u); }
ND_ float4  UIndexToUNormFloor (const uint4 index, const uint4 count)		{ return float4(index) / float4(count - 1u); }

ND_ float   UIndexToUNormFloor (const float  index, const float  count)		{ return index / (count - 1.f); }
ND_ float2  UIndexToUNormFloor (const float2 index, const float2 count)		{ return index / (count - 1.f); }
ND_ float3  UIndexToUNormFloor (const float3 index, const float3 count)		{ return index / (count - 1.f); }
ND_ float4  UIndexToUNormFloor (const float4 index, const float4 count)		{ return index / (count - 1.f); }

#define UIndexToSNormFloor( _index_, _count_ )								ToSNorm( UIndexToUNormFloor( (_index_), (_count_) ))

/*
=================================================
	UIndexToUNormRound / UIndexToSNormRound
----
	map coordinate 'index' in N dimension with size 'count'
	to unorm value with rounding
=================================================
*/
ND_ float   UIndexToUNormRound (const int  index, const int  count)			{ return (float( index) + 0.5f) / float( count); }
ND_ float2  UIndexToUNormRound (const int2 index, const int2 count)			{ return (float2(index) + 0.5f) / float2(count); }
ND_ float3  UIndexToUNormRound (const int3 index, const int3 count)			{ return (float3(index) + 0.5f) / float3(count); }
ND_ float4  UIndexToUNormRound (const int4 index, const int4 count)			{ return (float4(index) + 0.5f) / float4(count); }

ND_ float   UIndexToUNormRound (const uint  index, const uint  count)		{ return (float( index) + 0.5f) / float( count); }
ND_ float2  UIndexToUNormRound (const uint2 index, const uint2 count)		{ return (float2(index) + 0.5f) / float2(count); }
ND_ float3  UIndexToUNormRound (const uint3 index, const uint3 count)		{ return (float3(index) + 0.5f) / float3(count); }
ND_ float4  UIndexToUNormRound (const uint4 index, const uint4 count)		{ return (float4(index) + 0.5f) / float4(count); }

ND_ float   UIndexToUNormRound (const float  index, const float  count)		{ return (index + 0.5f) / count; }
ND_ float2  UIndexToUNormRound (const float2 index, const float2 count)		{ return (index + 0.5f) / count; }
ND_ float3  UIndexToUNormRound (const float3 index, const float3 count)		{ return (index + 0.5f) / count; }
ND_ float4  UIndexToUNormRound (const float4 index, const float4 count)		{ return (index + 0.5f) / count; }

#define UIndexToSNormRound( _index_, _count_ )								ToSNorm( UIndexToUNormRound( (_index_), (_count_) ))

/*
=================================================
	IndexToVec2 / IndexToVec3
=================================================
*/
ND_ int2   IndexToVec2 (const int  index, const int2  tile)					{ return int2(  index % tile.x, (index / tile.x) % tile.y ); }
ND_ uint2  IndexToVec2 (const uint index, const uint2 tile)					{ return uint2( index % tile.x, (index / tile.x) % tile.y ); }
ND_ int3   IndexToVec3 (const int  index, const int3  tile)					{ return int3(  index % tile.x, (index / tile.x) % tile.y, (index / (tile.x * tile.y)) % tile.z ); }
ND_ uint3  IndexToVec3 (const uint index, const uint3 tile)					{ return uint3( index % tile.x, (index / tile.x) % tile.y, (index / (tile.x * tile.y)) % tile.z ); }

ND_ int2   IndexToVec2 (const int  index, const int   tile)					{ return IndexToVec2( index,  int2(tile) ); }
ND_ uint2  IndexToVec2 (const uint index, const uint  tile)					{ return IndexToVec2( index, uint2(tile) ); }
ND_ int3   IndexToVec3 (const int  index, const int   tile)					{ return IndexToVec3( index,  int3(tile) ); }
ND_ uint3  IndexToVec3 (const uint index, const uint  tile)					{ return IndexToVec3( index, uint3(tile) ); }

/*
=================================================
	VecToIndex
=================================================
*/
ND_ int		VecToIndex (const int2  coord, const int   dimX)				{ return coord.x + coord.y * dimX; }
ND_ uint	VecToIndex (const uint2 coord, const uint  dimX)				{ return coord.x + coord.y * dimX; }

ND_ int		VecToIndex (const int2  coord, const int2  dim)					{ return VecToIndex( coord, dim.x ); }
ND_ uint	VecToIndex (const uint2 coord, const uint2 dim)					{ return VecToIndex( coord, dim.x ); }

ND_ int		VecToIndex (const int3  coord, const int2  dimXY)				{ return coord.x + coord.y * dimXY.x + coord.z * dimXY.x * dimXY.y; }
ND_ uint	VecToIndex (const uint3 coord, const uint2 dimXY)				{ return coord.x + coord.y * dimXY.x + coord.z * dimXY.x * dimXY.y; }

ND_ int		VecToIndex (const int3  coord, const int3  dim)					{ return VecToIndex( coord, dim.xy ); }
ND_ uint	VecToIndex (const uint3 coord, const uint3 dim)					{ return VecToIndex( coord, dim.xy ); }

/*
=================================================
	SLerp / BiSLerp
----
	T  SLerp (T x, T y, float factor)
	T  BiSLerp (T x1y1, T x2y1, T x1y2, T x2y2, float2 factor)
		where 'T' is float2 or float3
=================================================
*/
#define Gen_SLERP1( _stype_, _vtype_, _vtype2_ )\
	ND_ _vtype_  SLerp (const _vtype_ x, const _vtype_ y, const _stype_ factor)	\
	{																			\
		/* from GLM (MIT license) https://github.com/g-truc/glm	*/				\
		_stype_	cos_a	= Dot( x, y );											\
		_stype_	alpha	= ACos( cos_a );										\
		_stype_	sin_a	= Sin( alpha );											\
		_stype_	t1		= Sin( (_stype_(1) - factor) * alpha ) / sin_a;			\
		_stype_	t2		= Sin( factor * alpha ) / sin_a;						\
		return x * t1 + y * t2;													\
	}																			\
																				\
	ND_ _vtype_  BiSLerp (const _vtype_ x1y1, const _vtype_ x2y1,				\
						  const _vtype_ x1y2, const _vtype_ x2y2,				\
						  const _vtype2_ factor)								\
	{																			\
		return SLerp( SLerp( x1y1, x2y1, factor.x ),							\
					  SLerp( x1y2, x2y2, factor.x ), factor.y );				\
	}

#define Gen_SLERP( _stype_, _vtype_ )\
	Gen_SLERP1( _stype_, UNITE( _vtype_, 2 ), UNITE( _vtype_, 2 ))\
	Gen_SLERP1( _stype_, UNITE( _vtype_, 3 ), UNITE( _vtype_, 2 ))

Gen_SLERP( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_SLERP( half, half_vec_t )
#endif

#undef Gen_SLERP

/*
=================================================
	NearestSampleArray / LinearSampleArray
----
	used to get array element from unorm float
=================================================
*/
#define NearestSampleArray( _result_, _array_, _factor_ )							\
	{																				\
		int		lll = (_array_).length() - 1;										\
		float	aaa = RemapClamp( float2(0.0, 1.0), float2(0, lll), (_factor_) );	\
		int		iii = int(aaa + 0.5f);												\
		_result_ = (_array_)[iii];													\
	}

#define LinearSampleArray2( _result_, _array_, _factor_, _lerp_ )					\
	{																				\
		int		lll = (_array_).length() - 1;										\
		float	aaa = RemapClamp( float2(0.0, 1.0), float2(0, lll), (_factor_) );	\
		int		iii = int(aaa);														\
		int		jjj	= Min( int(aaa) + 1, lll );										\
		_result_ = _lerp_( (_array_)[iii], (_array_)[jjj], Fract(aaa) );			\
	}

#define LinearSampleArray( _result_, _array_, _factor_ )\
	LinearSampleArray2( (_result_), (_array_), (_factor_), Lerp )

/*
=================================================
	InterpolateQuad / InterpolateTriangle
----
	can be used in TessEval shader
=================================================
*/
#define InterpolateQuad( _arr_, _field_, _factor2_ )\
	BiLerp( _arr_[0] _field_, _arr_[1] _field_, _arr_[3] _field_, _arr_[2] _field_, _factor2_.xy )

#define InterpolateTriangle( _arr_, _field_, _factor3_ )\
	( _factor3_.x * _arr_[0] _field_ +					\
	  _factor3_.y * _arr_[1] _field_ +					\
	  _factor3_.z * _arr_[2] _field_ )

/*
=================================================
	IsZero / IsNotZero / IsNormalized
=================================================
*/
ND_ bool   IsZero (const float  x)		{ return Abs(x) <= float_epsilon; }
ND_ bool2  IsZero (const float2 v)		{ return LessEqual( Abs(v), float2(float_epsilon) ); }
ND_ bool3  IsZero (const float3 v)		{ return LessEqual( Abs(v), float3(float_epsilon) ); }
ND_ bool4  IsZero (const float4 v)		{ return LessEqual( Abs(v), float4(float_epsilon) ); }

ND_ bool   IsNotZero (const float  x)	{ return Abs(x) > float_epsilon; }
ND_ bool2  IsNotZero (const float2 v)	{ return Greater( Abs(v), float2(float_epsilon) ); }
ND_ bool3  IsNotZero (const float3 v)	{ return Greater( Abs(v), float3(float_epsilon) ); }
ND_ bool4  IsNotZero (const float4 v)	{ return Greater( Abs(v), float4(float_epsilon) ); }

#define AllZeros( v )					All( IsZero( v ))
#define AnyNotZero( v )					Any( IsNotZero( v ))

#define IsFinite( v )					Not( BoolOr( IsNaN( v ), IsInfinity( v )))
#define AllFinite( v )					(All(IsNaN( v )) and All(IsInfinity( v )))

ND_ bool  IsNormalized (const float2 v, const float err)	{ float d = Dot( v, v ) - 1.f;  return Abs(d) < err; }
ND_ bool  IsNormalized (const float3 v, const float err)	{ float d = Dot( v, v ) - 1.f;  return Abs(d) < err; }
ND_ bool  IsNormalized (const float2 v)						{ return IsNormalized( v, float_epsilon ); }
ND_ bool  IsNormalized (const float3 v)						{ return IsNormalized( v, float_epsilon ); }

/*
=================================================
	IsUNorm / IsSNorm
=================================================
*/
#define Gen_IS_UNORM_SNORM( _stype_, _vtype_ )																						\
	ND_ bool  IsUNorm (const _stype_			x)		{ return Abs( x - _stype_(0.5) ) <= _stype_(0.5); }							\
	ND_ bool  IsUNorm (const UNITE(_vtype_,2)	x)		{ return AllLessEqual( Abs( x - _stype_(0.5) ), UNITE(_vtype_,2)(0.5) ); }	\
	ND_ bool  IsUNorm (const UNITE(_vtype_,3)	x)		{ return AllLessEqual( Abs( x - _stype_(0.5) ), UNITE(_vtype_,3)(0.5) ); }	\
	ND_ bool  IsUNorm (const UNITE(_vtype_,4)	x)		{ return AllLessEqual( Abs( x - _stype_(0.5) ), UNITE(_vtype_,4)(0.5) ); }	\
																																	\
	ND_ bool  IsSNorm (const _stype_			x)		{ return Abs(x) <= _stype_(1.0); }											\
	ND_ bool  IsSNorm (const UNITE(_vtype_,2)	x)		{ return AllLessEqual( Abs(x), UNITE(_vtype_,2)(1.0) ); }					\
	ND_ bool  IsSNorm (const UNITE(_vtype_,3)	x)		{ return AllLessEqual( Abs(x), UNITE(_vtype_,3)(1.0) ); }					\
	ND_ bool  IsSNorm (const UNITE(_vtype_,4)	x)		{ return AllLessEqual( Abs(x), UNITE(_vtype_,4)(1.0) ); }					\
																																	\
	ND_ bool  IsNotUNorm (const _stype_				x)	{ return Abs( x - _stype_(0.5) ) > _stype_(0.5); }							\
	ND_ bool  IsNotUNorm (const UNITE(_vtype_,2)	x)	{ return AnyGreater( Abs( x - _stype_(0.5) ), UNITE(_vtype_,2)(0.5) ); }	\
	ND_ bool  IsNotUNorm (const UNITE(_vtype_,3)	x)	{ return AnyGreater( Abs( x - _stype_(0.5) ), UNITE(_vtype_,3)(0.5) ); }	\
	ND_ bool  IsNotUNorm (const UNITE(_vtype_,4)	x)	{ return AnyGreater( Abs( x - _stype_(0.5) ), UNITE(_vtype_,4)(0.5) ); }	\
																																	\
	ND_ bool  IsNotSNorm (const _stype_				x)	{ return Abs(x) > _stype_(1.0); }											\
	ND_ bool  IsNotSNorm (const UNITE(_vtype_,2)	x)	{ return AnyGreater( Abs(x), UNITE(_vtype_,2)(1.0) ); }						\
	ND_ bool  IsNotSNorm (const UNITE(_vtype_,3)	x)	{ return AnyGreater( Abs(x), UNITE(_vtype_,3)(1.0) ); }						\
	ND_ bool  IsNotSNorm (const UNITE(_vtype_,4)	x)	{ return AnyGreater( Abs(x), UNITE(_vtype_,4)(1.0) ); }

Gen_IS_UNORM_SNORM( float,	float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_IS_UNORM_SNORM( half,	half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_IS_UNORM_SNORM( double,	double_vec_t )
#endif

#undef Gen_IS_UNORM_SNORM

/*
=================================================
	FpEqual / FpNotEqual
----
	boolType  FEqual (T lhs, T rhs, T err)
	boolType  FNotEqual (T lhs, T rhs, T err)
----
	per component comparator
=================================================
*/
#define Gen_FPEQUAL( _stype_, _vtype_ )																															\
	ND_ bool   FpEqual (const _stype_          lhs, const _stype_          rhs, const _stype_ err)	{ return Abs( lhs - rhs ) < err; }							\
	ND_ bool2  FpEqual (const UNITE(_vtype_,2) lhs, const UNITE(_vtype_,2) rhs, const _stype_ err)	{ return Less( Abs( lhs - rhs ), UNITE(_vtype_,2)(err) ); }	\
	ND_ bool3  FpEqual (const UNITE(_vtype_,3) lhs, const UNITE(_vtype_,3) rhs, const _stype_ err)	{ return Less( Abs( lhs - rhs ), UNITE(_vtype_,3)(err) ); }	\
	ND_ bool4  FpEqual (const UNITE(_vtype_,4) lhs, const UNITE(_vtype_,4) rhs, const _stype_ err)	{ return Less( Abs( lhs - rhs ), UNITE(_vtype_,4)(err) ); }

Gen_FPEQUAL( float,	float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_FPEQUAL( half,		half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_FPEQUAL( double,	double_vec_t )
#endif

#undef Gen_FPEQUAL

#define AllFpEqual( a, b, err )			All( FpEqual( (a), (b), (err) ))
#define AnyFpEqual( a, b, err )			Any( FpEqual( (a), (b), (err) ))

#define FpNotEqual( a, b, err )			Not( FpEqual( (a), (b), (err) ))
#define AllFpNotEqual( a, b, err )		All( FpNotEqual( (a), (b), (err) ))
#define AnyFpNotEqual( a, b, err )		Any( FpNotEqual( (a), (b), (err) ))

/*
=================================================
	Swap
----
	void  Swap (T& lhs, T& rhs)
=================================================
*/
#define Gen_SWAP1( _type_ )\
	void  Swap (inout _type_ lhs, inout _type_ rhs)	{ _type_ tmp = lhs;  lhs = rhs;  rhs = tmp; }

#define Gen_SWAP( _stype_, _vtype_ )\
	Gen_SWAP1( _stype_ )\
	Gen_SWAP1( UNITE(_vtype_,2) )\
	Gen_SWAP1( UNITE(_vtype_,3) )\
	Gen_SWAP1( UNITE(_vtype_,4) )

Gen_SWAP( float,	float_vec_t )
Gen_SWAP( int,		int_vec_t )
Gen_SWAP( uint,		uint_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_SWAP( half,		half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_SWAP( double,	double_vec_t )
#endif
#if AE_ENABLE_BYTE_TYPE
	Gen_SWAP( sbyte,	sbyte_vec_t )
	Gen_SWAP( ubyte,	ubyte_vec_t )
#endif
#if AE_ENABLE_SHORT_TYPE
	Gen_SWAP( sshort,	sshort_vec_t )
	Gen_SWAP( ushort,	ushort_vec_t )
#endif
#if AE_ENABLE_LONG_TYPE
	Gen_SWAP( slong,	slong_vec_t )
	Gen_SWAP( ulong,	ulong_vec_t )
#endif

#undef Gen_SWAP1
#undef Gen_SWAP

/*
=================================================
	DivCeil
----
	T  DivCeil (T lhs, T rhs)
=================================================
*/
#define Gen_DIVCEIL1( _stype_, _vtype_ )																		\
	ND_ _vtype_  DivCeil (const _vtype_ lhs, const _vtype_ rhs)		{ return (lhs + rhs - _stype_(1)) / rhs; }	\
	ND_ _vtype_  DivCeil (const _vtype_ lhs, const _stype_ rhs)		{ return (lhs + rhs - _stype_(1)) / rhs; }

#define Gen_DIVCEIL( _stype_, _vtype_ )																			\
	ND_ _stype_  DivCeil (const _stype_ lhs, const _stype_ rhs)		{ return (lhs + rhs - _stype_(1)) / rhs; }	\
	Gen_DIVCEIL1( _stype_, UNITE( _vtype_, 2))																	\
	Gen_DIVCEIL1( _stype_, UNITE( _vtype_, 3))																	\
	Gen_DIVCEIL1( _stype_, UNITE( _vtype_, 4))

Gen_DIVCEIL( int,	int_vec_t )
Gen_DIVCEIL( uint,	uint_vec_t )

#if AE_ENABLE_LONG_TYPE
	Gen_DIVCEIL( slong,	slong_vec_t )
	Gen_DIVCEIL( ulong,	ulong_vec_t )
#endif

#undef Gen_DIVCEIL1
#undef Gen_DIVCEIL

/*
=================================================
	Cross2
=================================================
*/
#define Gen_CROSS2( _stype_, _vtype_ )\
	ND_ _stype_  Cross2 (const _vtype_ v0, const _vtype_ v1)	{ return (v0.x * v1.y) - (v0.y * v1.x); }

Gen_CROSS2( float, float2 )

#if AE_ENABLE_HALF_TYPE
	Gen_CROSS2( half, half2 )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_CROSS2( double, double2 )
#endif
#undef Gen_CROSS2

/*
=================================================
	MaxIndexOf
----
	uint  MaxIndexOf (T vec)
----
	return index of max element in vector
=================================================
*/
//ND_ uint	MaxIndexOf (const float2 v)		{ bool2 m = Less( v, v.yx );  return Dot( uint2(m), uint2(0,1) ) & 1; }		// TODO: check

ND_ uint	MaxIndexOf (const float3 v)
{
	float	max_val = v.x;
	uint	idx		= 0;
	if ( v.y > max_val ) { max_val = v.y;  idx = 1; }
	if ( v.z > max_val ) { max_val = v.z;  idx = 2; }
	return idx;
}

ND_ uint	MaxIndexOf (const float4 v)
{
#if 1
	float	max_val = v.x;
	uint	idx		= 0;
	if ( v.y > max_val ) { max_val = v.y;  idx = 1; }
	if ( v.z > max_val ) { max_val = v.z;  idx = 2; }
	if ( v.w > max_val ) { max_val = v.w;  idx = 3; }
	return idx;
#else
	float maxVal = v.x;
	uint  idx    = 0;

	bool b1 = v.y > maxVal;
	maxVal  = b1 ? v.y : maxVal;
	idx     = b1 ? 1u  : idx;

	bool b2 = v.z > maxVal;
	maxVal  = b2 ? v.z : maxVal;
	idx     = b2 ? 2u  : idx;

	bool b3 = v.w > maxVal;
	maxVal  = b3 ? v.w : maxVal;
	idx     = b3 ? 3u  : idx;

	return idx;
#endif
}

/*
=================================================
	QuadGroup_dFdx
	QuadGroup_dFdy
	QuadGroup_fwidth
----
	warning:
		in CS order of indices is not defined,
		use 'GetLocalCoordQuadCorrected()' or 'GetGlobalCoordQuadCorrected()'
		to get the same order as in FS.
=================================================
*/
#if defined(AE_shader_subgroup_quad) and defined(AE_shader_subgroup_basic) and defined(AE_subgroupBroadcastDynamicId)
#	define QuadGroup_dFdxFine( _a_ )		(gl.quadGroup.Broadcast( (_a_), (gl.subgroup.Index&2)|1 ) - gl.quadGroup.Broadcast( (_a_), gl.subgroup.Index&2 ))
#	define QuadGroup_dFdyFine( _a_ )		(gl.quadGroup.Broadcast( (_a_), (gl.subgroup.Index&1)|2 ) - gl.quadGroup.Broadcast( (_a_), gl.subgroup.Index&1 ))
#	define QuadGroup_fwidthFine( _a_ )		Abs(QuadGroup_dFdxFine(_a_)) + Abs(QuadGroup_dFdyFine(_a_))
#endif
#if defined(AE_shader_subgroup_quad) and defined(AE_shader_subgroup_basic)
#	define QuadGroup_dFdxCoarse( _a_ )		(gl.quadGroup.Broadcast( (_a_), 1 ) - gl.quadGroup.Broadcast( (_a_), 0 ))
#	define QuadGroup_dFdyCoarse( _a_ )		(gl.quadGroup.Broadcast( (_a_), 2 ) - gl.quadGroup.Broadcast( (_a_), 0 ))
#	define QuadGroup_fwidthCoarse( _a_ )	Abs(QuadGroup_dFdxCoarse(_a_)) + Abs(QuadGroup_dFdyCoarse(_a_))
#endif
//-----------------------------------------------------------------------------


// Dummy function, used in [ShaderTrace](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/res_pack/shader_trace/Readme.md)
//
// empty functions will be replaced during shader compilation
void dbg_EnableTraceRecording (bool b) {}
void dbg_PauseTraceRecording (bool b) {}
void dbg_EnableProfiling (bool b) {}

