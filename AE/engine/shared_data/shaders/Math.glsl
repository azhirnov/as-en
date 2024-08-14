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
//-----------------------------------------------------------------------------


#define and						&&
#define or						||

#define Any						any				// (bool)
#define All						all				// (bool)
#define Abs						abs				// (any int, any fp)
#define ACos					acos			// (half, float)	result in range [0 .. Pi]
#define ASin					asin			// (half, float)	result in range [-Pi/2 ... Pi/2]
#define ASinH					asinh			// (half, float)
#define ACosH					acosh			// (half, float)
#define ATan					atan			// (half, float)	for 2 arg overload: result in range [-Pi...+Pi], for 1 arg overload result in range [-Pi/2 ... Pi/2]
#define BitScanReverse			findMSB			// (any int)
#define BitScanForward			findLSB			// (any int)
#define ATanH					atanh			// (half, float)
#define Clamp					clamp			// (any except bool)
#define Ceil					ceil			// (any fp)
#define Cos						cos				// (half, float)
#define CosH					cosh			// (half, float)
#define Cross					cross			// (any fp)
#define Distance				distance		// (any fp)
#define Dot						dot				// (any fp)
#define Exp						exp				// (half, float)	Pow( float_Euler, x )
#define Exp2					exp2			// (half, float)	Pow( 2, x )
#define Exp10( _a_ )			pow(10.0,(_a_))	// (half, float)
#define Fract					fract			// (any fp)			x - Floor( x )
#define Floor					floor			// (any fp)
#define IsNaN					isnan			// (any fp)
#define IsInfinity				isinf			// (any fp)
#define InvSqrt					inversesqrt		// (any fp)
#define IntLog2					BitScanReverse	// (any int)
#define Length					length			// (any fp)
#define Lerp					mix				// (any fp)
#define Ln						log				// (half, float)
#define Log2					log2			// (half, float)
#define Log( _a_, _base_ )		(Ln(_a_) / Ln(_base_))			// (half, float)
#define Log10( _a_ )			(Ln(_a_) * 0.4342944819032518)	// (half, float)
#define Min						min				// (any except bool)
#define Max						max				// (any except bool)
#define Mod						mod				// (any fp)
#define Normalize				normalize		// (any fp)
#define Pow						pow				// (half, float)
#define Round					round			// (any fp)
#define Reflect					reflect			// (any fp)
#define Refract					refract			// (any fp)
#define Step					step			// (any fp)			x < edge ? 0 : 1
#define GreaterEqualFp(_a_,_b_)	step((_b_),(_a_))//(any fp)			a >= b ? 1 : 0
#define LessFp(_a_,_b_)			step((_a_),(_b_))//(any fp)			a <  b ? 1 : 0
#define Sqrt					sqrt			// (any fp)
#define Sin						sin				// (half, float)
#define SinH					sinh			// (half, float)
#define SignOrZero				sign			// (int, any fp)	-1, 0, +1, nan = 0
#define Tan						tan				// (half, float)
#define TanH					tanh			// (half, float)
#define Trunc					trunc			// (any fp)
#define BitCount				bitCount		// (any int)
#define ToDeg					degrees			// (half, float)
#define ToRad					radians			// (half, float)

#define FusedMulAdd				fma				// (any fp)				(a * b) + c
//#define FusedMulAdd(a,b,c)	((a)*(b)+(c))	// different precision

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
#define Gen_CMP1( _stype_, _vtype_, _name_, _opS_, _opV_ )											\
	ND_ bool   _name_ (const _stype_    lhs, const _stype_    rhs)	{ return lhs _opS_ rhs; }		\
	ND_ bool2  _name_ (const _vtype_##2 lhs, const _vtype_##2 rhs)	{ return _opV_( lhs, rhs ); }	\
	ND_ bool3  _name_ (const _vtype_##3 lhs, const _vtype_##3 rhs)	{ return _opV_( lhs, rhs ); }	\
	ND_ bool4  _name_ (const _vtype_##4 lhs, const _vtype_##4 rhs)	{ return _opV_( lhs, rhs ); }

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
	Gen_DIAGONAL1( _vtype_##2 )			\
	Gen_DIAGONAL1( _vtype_##3 )			\
	Gen_DIAGONAL1( _vtype_##4 )

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
	Gen_SATURATE1( _stype_, _stype_ )	\
	Gen_SATURATE1( _stype_, _vtype_##2 )\
	Gen_SATURATE1( _stype_, _vtype_##3 )\
	Gen_SATURATE1( _stype_, _vtype_##4 )

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
	Cbrt
----
	T  Cbrt (T x)
=================================================
*/
#define Gen_CBRT1( _stype_, _type_ )		\
	ND_ _type_  Cbrt (const _type_ x) {		\
		return Pow( x, _type_(1.0/3.0) );	\
	}

#define Gen_CBRT( _stype_, _vtype_ )\
	Gen_CBRT1( _stype_, _stype_ )	\
	Gen_CBRT1( _stype_, _vtype_##2 )\
	Gen_CBRT1( _stype_, _vtype_##3 )\
	Gen_CBRT1( _stype_, _vtype_##4 )

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
	T  ToUNorm (T x)	[-1, +1] to [ 0,  1]
	T  ToSNorm (T x)	[ 0,  1] to [-1, +1]
=================================================
*/
#define Gen_TOUSNORM1( _type_ )								\
	ND_ _type_  ToUNorm (const _type_ x) {					\
		return FusedMulAdd( x, _type_(0.5), _type_(0.5) );	\
	}														\
	ND_ _type_  ToSNorm (const _type_ x) {					\
		return FusedMulAdd( x, _type_(2.0), _type_(-1.0) );	\
	}

#define Gen_TOUSNORM( _stype_, _vtype_ )\
	Gen_TOUSNORM1( _stype_ )			\
	Gen_TOUSNORM1( _vtype_##2 )			\
	Gen_TOUSNORM1( _vtype_##3 )			\
	Gen_TOUSNORM1( _vtype_##4 )

Gen_TOUSNORM( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_TOUSNORM( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_TOUSNORM( double, double_vec_t )
#endif

#undef Gen_TOUSNORM1
#undef Gen_TOUSNORM


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
#	define half_min			(0.00006103515625hf)		// smallest positive normal number
#	define half_max			(65504.0hf)
#	define half_inf			uint16BitsToHalf( 0x7c00us )
#	define half_inf_neg		uint16BitsToHalf( 0xfc00us )
#	define half_Pi			3.14159265358979323846hf
#	define half_Pi2			6.28318530717958647692hf
#	define half_HalfPi		1.57079632679489661923hf
#	define half_InvPi		0.31830988618379067153hf
#	define half_SqrtOf2		1.41421356237309504880hf
#	define half_Euler		2.71828182845904523536hf
#endif
#if 1
#	define float_min		(1.1754943508e-38f)			// smallest positive normal number
#	define float_max		(3.4028234664e+38f)
#	define float_inf		uintBitsToFloat( 0x7F800000u )
#	define float_inf_neg	uintBitsToFloat( 0xFF800000u )
#	define float_qnan		uintBitsToFloat( 0xFFC00001u )
#	define float_snan		uintBitsToFloat( 0xFF800001u )
#	define float_Pi			3.14159265358979323846f
#	define float_Pi2		6.28318530717958647692f
#	define float_HalfPi		1.57079632679489661923f
#	define float_InvPi		0.31830988618379067153f
#	define float_SqrtOf2	1.41421356237309504880f
#	define float_Euler		2.71828182845904523536f
#endif
#if AE_ENABLE_DOUBLE_TYPE
#	define double_min		(2.2250738585072014e-308lf)	 // smallest positive normal number
#	define double_max		(1.7976931348623157e+308lf)
#	define double_inf		uint64BitsToDouble( 0x7FF0000000000000ul )
#	define double_inf_neg	uint64BitsToDouble( 0xFFF0000000000000ul )
#	define double_qnan		uint64BitsToDouble( 0x7FF8000000000001ul )
#	define double_snan		uint64BitsToDouble( 0x7FF0000000000001ul )
#	define double_nan		uint64BitsToDouble( 0x7FFFFFFFFFFFFFFFul )
#	define double_Pi		3.14159265358979323846lf
#	define double_Pi2		6.28318530717958647692lf
#	define double_HalfPi	1.57079632679489661923lf
#	define double_InvPi		0.31830988618379067153lf
#	define double_SqrtOf2	1.41421356237309504880lf
#	define double_Euler		2.71828182845904523536lf
#endif


ND_ float  Epsilon ()				{ return 2.e-5f; }
ND_ float  Pi ()					{ return float_Pi; }
ND_ float  Pi2 ()					{ return float_Pi2; }
ND_ float  HalfPi ()				{ return float_HalfPi; }

ND_ float  ReciprocalPi ()			{ return float_InvPi; }
ND_ float  SqrtOf2 ()				{ return float_SqrtOf2; }
//-----------------------------------------------------------------------------


/*
=================================================
	Square
----
	T  Square (T x)
=================================================
*/
#define Gen_SQUARE( _stype_, _vtype_ )\
	ND_ _stype_		Square (const _stype_    x)		{ return x * x; }\
	ND_ _vtype_##2	Square (const _vtype_##2 x)		{ return x * x; }\
	ND_ _vtype_##3	Square (const _vtype_##3 x)		{ return x * x; }\
	ND_ _vtype_##4	Square (const _vtype_##4 x)		{ return x * x; }

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
	same as per component 'condition ? ifTrue : ifFalse'
=================================================
*/
#define Gen_SELECT1( _vtype_, _btype_ )\
	ND_ _vtype_  Select (const _btype_ condition, const _vtype_ ifTrue, const _vtype_ ifFalse)	{ return (ifFalse * _vtype_(Not(condition))) + (ifTrue * _vtype_(condition)); }

#define Gen_SELECT( _stype_, _vtype_ )\
	Gen_SELECT1( _stype_,		bool  )\
	Gen_SELECT1( _vtype_##2,	bool2 )\
	Gen_SELECT1( _vtype_##3,	bool3 )\
	Gen_SELECT1( _vtype_##4,	bool4 )

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
	SelectFp
----
	T  SelectFp (T x, T y, T ifLess, T ifNot)
----
	same as per component 'x < y ? ifLess : ifNot'
=================================================
*
#define Gen_SELECT1( _vtype_ )\
	ND_ _vtype_  SelectFp (_vtype_ x, const _vtype_ y, const _vtype_ ifLess, const _vtype_ ifNot)	{ x = LessFp( y, x );  return (ifLess * x) + ifNot * (x - _vtype_(1.0)); }

#define Gen_SELECT( _stype_, _vtype_ )\
	Gen_SELECT1( _stype_	)\
	Gen_SELECT1( _vtype_##2 )\
	Gen_SELECT1( _vtype_##3 )\
	Gen_SELECT1( _vtype_##4 )

Gen_SELECT( float,	float_vec_t )

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
	same as 'condition ? ifTrue : ifFalse' but both branches are always executed
=================================================
*/
#define Gen_BRANCHLESS1( _vtype_ )\
	ND_ _vtype_  BranchLess (const bool condition, const _vtype_ ifTrue, const _vtype_ ifFalse)	{ _vtype_ tmp[2] = {ifTrue, ifFalse};  return tmp[int(condition)]; }

#define Gen_BRANCHLESS( _stype_, _vtype_ )\
	Gen_BRANCHLESS1( _stype_    )\
	Gen_BRANCHLESS1( _vtype_##2 )\
	Gen_BRANCHLESS1( _vtype_##3 )\
	Gen_BRANCHLESS1( _vtype_##4 )

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
	Min* / Max*
=================================================
*/
#define Min3( a, b, c )			Min( Min( (a), (b) ), (c) )
#define Min4( a, b, c, d )		Min( Min( (a), (b) ), Min( (c), (d) ))
#define Max3( a, b, c )			Max( Max( (a), (b) ), (c) )
#define Max4( a, b, c, d )		Max( Max( (a), (b) ), Max( (c), (d) ))

#define MinAbs( _a_, _b_ )		Select( Less(Abs(_a_), Abs(_b_)), (_a_), (_b_) )
#define MaxAbs( _a_, _b_ )		Select( Greater(Abs(_a_), Abs(_b_)), (_a_), (_b_) )

#define Gen_MINMAX( _stype_, _vtype_ )											\
	ND_ _stype_  MinOf (_vtype_##2 a)	{ return Min( a.x, a.y ); }				\
	ND_ _stype_  MinOf (_vtype_##3 a)	{ return Min3( a.x, a.y, a.z ); }		\
	ND_ _stype_  MinOf (_vtype_##4 a)	{ return Min4( a.x, a.y, a.z, a.w ); }	\
	ND_ _stype_  MaxOf (_vtype_##2 a)	{ return Max( a.x, a.y ); }				\
	ND_ _stype_  MaxOf (_vtype_##3 a)	{ return Max3( a.x, a.y, a.z ); }		\
	ND_ _stype_  MaxOf (_vtype_##4 a)	{ return Max4( a.x, a.y, a.z, a.w ); }

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
	Gen_LENGTHSQ_DISTANCESQ1( _stype_, _vtype_##2 )\
	Gen_LENGTHSQ_DISTANCESQ1( _stype_, _vtype_##3 )

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
	Sign
----
	T  Sign (T x)
----
	returns -1 or +1, +1 on nan
	x4 faster than 'SignOrZero()' on NV
=================================================
*/
#define Gen_SIGN1( _vtype_ )\
	ND_ _vtype_  Sign (_vtype_ v)	{ return ToSNorm( GreaterEqualFp( v, _vtype_(0.0) )); }

#define Gen_SIGN( _stype_, _vtype_ )\
	Gen_SIGN1( _stype_ )\
	Gen_SIGN1( _vtype_##2 )\
	Gen_SIGN1( _vtype_##3 )\
	Gen_SIGN1( _vtype_##4 )

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
	Gen_SIGN1( _vtype_##2 )\
	Gen_SIGN1( _vtype_##3 )\
	Gen_SIGN1( _vtype_##4 )

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
	LinearStep
----
	T  LinearStep (T x, T edge0, T edge1)
----
	returns value in range [0, 1], pattern __/'''
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
	Gen_LINEARSTEP1( _vtype_##2, _stype_ )													\
	Gen_LINEARSTEP1( _vtype_##3, _stype_ )													\
	Gen_LINEARSTEP1( _vtype_##4, _stype_ )

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
	Gen_BUMPSTEP1( _vtype_##2, _stype_ )																	\
	Gen_BUMPSTEP1( _vtype_##3, _stype_ )																	\
	Gen_BUMPSTEP1( _vtype_##4, _stype_ )

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
	Gen_SMOOTHBUMPSTEP1( _vtype_##2, _stype_ )											\
	Gen_SMOOTHBUMPSTEP1( _vtype_##3, _stype_ )											\
	Gen_SMOOTHBUMPSTEP1( _vtype_##4, _stype_ )

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
	Gen_TRIANGLEWAVE1( _stype_,    _stype_ )\
	Gen_TRIANGLEWAVE1( _vtype_##2, _stype_ )\
	Gen_TRIANGLEWAVE1( _vtype_##3, _stype_ )\
	Gen_TRIANGLEWAVE1( _vtype_##4, _stype_ )

Gen_TRIANGLEWAVE( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_TRIANGLEWAVE( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_TRIANGLEWAVE( double, double_vec_t )
#endif

#undef Gen_TRIANGLEWAVE1
#undef Gen_TRIANGLEWAVE

//-----------------------------------------------------------------------------
// clamp / wrap

ND_ float  ClampOut (const float x, const float minVal, const float maxVal)
{
	float mid = (minVal + maxVal) * 0.5f;
	return x < mid ? (x < minVal ? x : minVal) : (x > maxVal ? x : maxVal);	// TODO: branchless
}

ND_ int  ClampOut (const int x, const int minVal, const int maxVal)
{
	int mid = (minVal+1)/2 + (maxVal+1)/2;
	return x < mid ? (x < minVal ? x : minVal) : (x > maxVal ? x : maxVal);	// TODO: branchless
}

ND_ float2  ClampOut (const float2 v, const float minVal, const float maxVal) {
	return float2( ClampOut( v.x, minVal, maxVal ),
				   ClampOut( v.y, minVal, maxVal ));
}

ND_ float3  ClampOut (const float3 v, const float minVal, const float maxVal) {
	return float3( ClampOut( v.x, minVal, maxVal ),
				   ClampOut( v.y, minVal, maxVal ),
				   ClampOut( v.z, minVal, maxVal ));
}

ND_ float4  ClampOut (const float4 v, const float minVal, const float maxVal) {
	return float4( ClampOut( v.x, minVal, maxVal ),
				   ClampOut( v.y, minVal, maxVal ),
				   ClampOut( v.z, minVal, maxVal ),
				   ClampOut( v.w, minVal, maxVal ));
}

ND_ int2  ClampOut (const int2 v, const int minVal, const int maxVal) {
	return int2( ClampOut( v.x, minVal, maxVal ),
				 ClampOut( v.y, minVal, maxVal ));
}

ND_ int3  ClampOut (const int3 v, const int minVal, const int maxVal) {
	return int3( ClampOut( v.x, minVal, maxVal ),
				 ClampOut( v.y, minVal, maxVal ),
				 ClampOut( v.z, minVal, maxVal ));
}

ND_ int4  ClampOut (const int4 v, const int minVal, const int maxVal) {
	return int4( ClampOut( v.x, minVal, maxVal ),
				 ClampOut( v.y, minVal, maxVal ),
				 ClampOut( v.z, minVal, maxVal ),
				 ClampOut( v.w, minVal, maxVal ));
}


ND_ float  Wrap (const float x, const float minVal, const float maxVal)
{
	if ( maxVal < minVal ) return minVal;	// TODO: branchless
	float size = maxVal - minVal;
	float res = minVal + Mod( x - minVal, size );
	if ( res < minVal ) return res + size;
	return res;
}

ND_ int  Wrap (const int x, const int minVal, const int maxVal)
{
	if ( maxVal < minVal ) return minVal;	// TODO: branchless
	int size = maxVal+1 - minVal;
	int res = minVal + ((x - minVal) % size);
	if ( res < minVal ) return res + size;
	return res;
}

ND_ float2  Wrap (const float2 v, const float minVal, const float maxVal) {
	return float2( Wrap( v.x, minVal, maxVal ),
				   Wrap( v.y, minVal, maxVal ));
}

ND_ float3  Wrap (const float3 v, const float minVal, const float maxVal) {
	return float3( Wrap( v.x, minVal, maxVal ),
				   Wrap( v.y, minVal, maxVal ),
				   Wrap( v.z, minVal, maxVal ));
}

ND_ float4  Wrap (const float4 v, const float minVal, const float maxVal) {
	return float4( Wrap( v.x, minVal, maxVal ),
				   Wrap( v.y, minVal, maxVal ),
				   Wrap( v.z, minVal, maxVal ),
				   Wrap( v.w, minVal, maxVal ));
}

ND_ float2 Wrap (const float2 v, const float2 minVal, const float2 maxVal) {
	return float2( Wrap( v.x, minVal.x, maxVal.y ),
				   Wrap( v.y, minVal.x, maxVal.y ));
}

ND_ float3 Wrap (const float3 v, const float3 minVal, const float3 maxVal) {
	return float3( Wrap( v.x, minVal.x, maxVal.x ),
				   Wrap( v.y, minVal.y, maxVal.y ),
				   Wrap( v.z, minVal.z, maxVal.z ));
}

ND_ float4 Wrap (const float4 v, const float4 minVal, const float4 maxVal) {
	return float4( Wrap( v.x, minVal.x, maxVal.x ),
				   Wrap( v.y, minVal.y, maxVal.y ),
				   Wrap( v.z, minVal.z, maxVal.z ),
				   Wrap( v.w, minVal.w, maxVal.w ));
}

ND_ int2 Wrap (const int2 v, const float minVal, const float maxVal) {
	return int2( Wrap( v.x, minVal, maxVal ),
				 Wrap( v.y, minVal, maxVal ));
}

ND_ int3  Wrap (const int3 v, const int minVal, const int maxVal) {
	return int3( Wrap( v.x, minVal, maxVal ),
				 Wrap( v.y, minVal, maxVal ),
				 Wrap( v.z, minVal, maxVal ));
}

ND_ int4  Wrap (const int4 v, const int minVal, const int maxVal) {
	return int4( Wrap( v.x, minVal, maxVal ),
				 Wrap( v.y, minVal, maxVal ),
				 Wrap( v.z, minVal, maxVal ),
				 Wrap( v.w, minVal, maxVal ));
}


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

ND_ uint  ExtractBit (inout uint bits)
{
	uint	result = bits & ~(bits - 1);
	bits = bits & ~result;
	return result;
}

ND_ uint  ExtractBitIndex (inout uint bits)
{
	return uint(IntLog2( ExtractBit( INOUT bits )));
}


//-----------------------------------------------------------------------------
// interpolation

/*
=================================================
	BaryLerp
----
	T  BaryLerp (T v0, T v1, T v2, Vec3 barycentrics)  -- barycentric interpolation
	T  BaryLerp (T v0, T v1, T v2, Vec2 barycentrics)  -- barycentric interpolation with much better precision
=================================================
*/
#define Gen_BARYLERP1( _type_, _bary3_, _bary2_ )\
	ND_ _type_  BaryLerp (const _type_ v0, const _type_ v1, const _type_ v2, const _bary3_ barycentrics)  { return v0 * barycentrics.x + v1 * barycentrics.y + v2 * barycentrics.z; }\
	ND_ _type_  BaryLerp (const _type_ v0, const _type_ v1, const _type_ v2, const _bary2_ barycentrics)  { return v0 + FusedMulAdd( _type_(barycentrics.x), (v1 - v0), barycentrics.y * (v2 - v0) ); }

#define Gen_BARYLERP( _stype_, _vtype_ )\
	Gen_BARYLERP1( _stype_,    _vtype_##3, _vtype_##2 )\
	Gen_BARYLERP1( _vtype_##2, _vtype_##3, _vtype_##2 )\
	Gen_BARYLERP1( _vtype_##3, _vtype_##3, _vtype_##2 )\
	Gen_BARYLERP1( _vtype_##4, _vtype_##3, _vtype_##2 )

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
	BiLerp
----
	T  BiLerp (T x1y1, T x2y1, T x1y2, T x2y2, Vec2 factor)
----
	bilinear interpolation
=================================================
*/
#define Gen_BILERP1( _type_, _factor_ )\
	ND_ _type_  BiLerp (const _type_ x1y1, const _type_ x2y1, const _type_ x1y2, const _type_ x2y2, const _factor_ factor)  { return Lerp( Lerp( x1y1, x2y1, factor.x ), Lerp( x1y2, x2y2, factor.x ), factor.y ); }

#define Gen_BILERP( _stype_, _vtype_ )\
	Gen_BILERP1( _stype_,    _vtype_##2 )\
	Gen_BILERP1( _vtype_##2, _vtype_##2 )\
	Gen_BILERP1( _vtype_##3, _vtype_##2 )\
	Gen_BILERP1( _vtype_##4, _vtype_##2 )

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
	Remap / RemapClamp / RemapWrap
----
	T  Remap (Vec2 src, Vec2 dst, T v)
	T  RemapClamp (Vec2 src, Vec2 dst, T v)
	T  RemapWrap (Vec2 src, Vec2 dst, T v)
----
	Map 'v' in 'src' interval to 'dst' interval.
	Map 'v' in 'src' interval to 'dst' interval and clamp.
	Interval is a scalar range which specified for all components.
=================================================
*/
ND_ float   Remap (const float2 src, const float2 dst, const float  v)  { return (v - src.x) / (src.y - src.x) * (dst.y - dst.x) + dst.x; }
ND_ float2  Remap (const float2 src, const float2 dst, const float2 v)  { return (v - src.x) / (src.y - src.x) * (dst.y - dst.x) + dst.x; }
ND_ float3  Remap (const float2 src, const float2 dst, const float3 v)  { return (v - src.x) / (src.y - src.x) * (dst.y - dst.x) + dst.x; }
ND_ float4  Remap (const float2 src, const float2 dst, const float4 v)  { return (v - src.x) / (src.y - src.x) * (dst.y - dst.x) + dst.x; }

ND_ float   RemapClamp (const float2 src, const float2 dst, const float  v)  { return Clamp( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float2  RemapClamp (const float2 src, const float2 dst, const float2 v)  { return Clamp( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float3  RemapClamp (const float2 src, const float2 dst, const float3 v)  { return Clamp( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float4  RemapClamp (const float2 src, const float2 dst, const float4 v)  { return Clamp( Remap( src, dst, v ), dst.x, dst.y ); }

ND_ float   RemapSmooth (const float2 src, const float2 dst, const float  v)  { return SmoothStep( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float2  RemapSmooth (const float2 src, const float2 dst, const float2 v)  { return SmoothStep( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float3  RemapSmooth (const float2 src, const float2 dst, const float3 v)  { return SmoothStep( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float4  RemapSmooth (const float2 src, const float2 dst, const float4 v)  { return SmoothStep( Remap( src, dst, v ), dst.x, dst.y ); }

ND_ float   RemapWrap (const float2 src, const float2 dst, const float  v)  { return Wrap( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float2  RemapWrap (const float2 src, const float2 dst, const float2 v)  { return Wrap( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float3  RemapWrap (const float2 src, const float2 dst, const float3 v)  { return Wrap( Remap( src, dst, v ), dst.x, dst.y ); }
ND_ float4  RemapWrap (const float2 src, const float2 dst, const float4 v)  { return Wrap( Remap( src, dst, v ), dst.x, dst.y ); }

/*
=================================================
	Remap / RemapClamp
----
	Map 'v' in 'src' interval to 'dst' interval.
	Map 'v' in 'src' interval to 'dst' interval and clamp.
	Interval is specified per-component.
=================================================
*/
ND_ float2  Remap (const float2 src0, const float2 src1, const float2 dst0, const float2 dst1, const float2 v)  { return (v - src0) / (src1 - src0) * (dst1 - dst0) + dst0; }
ND_ float3  Remap (const float3 src0, const float3 src1, const float3 dst0, const float3 dst1, const float3 v)  { return (v - src0) / (src1 - src0) * (dst1 - dst0) + dst0; }
ND_ float4  Remap (const float4 src0, const float4 src1, const float4 dst0, const float4 dst1, const float4 v)  { return (v - src0) / (src1 - src0) * (dst1 - dst0) + dst0; }

ND_ float2  RemapClamp (const float2 src0, const float2 src1, const float2 dst0, const float2 dst1, const float2 v)  { return Clamp( Remap( src0, src1, dst0, dst1, v ), dst0, dst1 ); }
ND_ float3  RemapClamp (const float3 src0, const float3 src1, const float3 dst0, const float3 dst1, const float3 v)  { return Clamp( Remap( src0, src1, dst0, dst1, v ), dst0, dst1 ); }
ND_ float4  RemapClamp (const float4 src0, const float4 src1, const float4 dst0, const float4 dst1, const float4 v)  { return Clamp( Remap( src0, src1, dst0, dst1, v ), dst0, dst1 ); }

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

ND_ float   UIndexToUNormFloor (const uint  index, const uint  count)		{ return float( index) / float( count - 1); }
ND_ float2  UIndexToUNormFloor (const uint2 index, const uint2 count)		{ return float2(index) / float2(count - 1); }
ND_ float3  UIndexToUNormFloor (const uint3 index, const uint3 count)		{ return float3(index) / float3(count - 1); }
ND_ float4  UIndexToUNormFloor (const uint4 index, const uint4 count)		{ return float4(index) / float4(count - 1); }

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
ND_ int2   IndexToVec2 (const int  index, const int2  tile)				{ return int2(  index % tile.x, (index / tile.x) % tile.y ); }
ND_ uint2  IndexToVec2 (const uint index, const uint2 tile)				{ return uint2( index % tile.x, (index / tile.x) % tile.y ); }
ND_ int3   IndexToVec3 (const int  index, const int3  tile)				{ return int3(  index % tile.x, (index / tile.x) % tile.y, (index / (tile.x * tile.y)) % tile.z ); }
ND_ uint3  IndexToVec3 (const uint index, const uint3 tile)				{ return uint3( index % tile.x, (index / tile.x) % tile.y, (index / (tile.x * tile.y)) % tile.z ); }

ND_ int2   IndexToVec2 (const int  index, const int   tile)				{ return IndexToVec2( index,  int2(tile) ); }
ND_ uint2  IndexToVec2 (const uint index, const uint  tile)				{ return IndexToVec2( index, uint2(tile) ); }
ND_ int3   IndexToVec3 (const int  index, const int   tile)				{ return IndexToVec3( index,  int3(tile) ); }
ND_ uint3  IndexToVec3 (const uint index, const uint  tile)				{ return IndexToVec3( index, uint3(tile) ); }

/*
=================================================
	SLerp / BiSLerp
----
	float3  SLerp (float3 x, float3 y, float factor)
	float3  BiSLerp (float3 x1y1, float3 x2y1, float3 x1y2, float3 x2y2, float2 factor)
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
	Gen_SLERP1( _stype_, _vtype_##2, _vtype_##2 )\
	Gen_SLERP1( _stype_, _vtype_##3, _vtype_##2 )

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
ND_ bool   IsZero (const float  x)		{ return Abs(x) <= Epsilon(); }
ND_ bool2  IsZero (const float2 v)		{ return LessEqual( Abs(v), float2(Epsilon()) ); }
ND_ bool3  IsZero (const float3 v)		{ return LessEqual( Abs(v), float3(Epsilon()) ); }
ND_ bool4  IsZero (const float4 v)		{ return LessEqual( Abs(v), float4(Epsilon()) ); }

ND_ bool   IsNotZero (const float  x)	{ return Abs(x) > Epsilon(); }
ND_ bool2  IsNotZero (const float2 v)	{ return Greater( Abs(v), float2(Epsilon()) ); }
ND_ bool3  IsNotZero (const float3 v)	{ return Greater( Abs(v), float3(Epsilon()) ); }
ND_ bool4  IsNotZero (const float4 v)	{ return Greater( Abs(v), float4(Epsilon()) ); }

#define AllZeros( v )					All( IsZero( v ))
#define AnyNotZero( v )					Any( IsNotZero( v ))

#define IsFinite( v )					Not( BoolOr( IsNaN( v ), IsInfinity( v )))
#define AllFinite( v )					(All(IsNaN( v )) and All(IsInfinity( v )))

ND_ bool  IsNormalized (const float2 v, const float err)	{ float d = Dot( v, v ) - 1.f;  return Abs(d) < err; }
ND_ bool  IsNormalized (const float3 v, const float err)	{ float d = Dot( v, v ) - 1.f;  return Abs(d) < err; }
ND_ bool  IsNormalized (const float2 v)						{ return IsNormalized( v, Epsilon() ); }
ND_ bool  IsNormalized (const float3 v)						{ return IsNormalized( v, Epsilon() ); }

/*
=================================================
	IsUNorm / IsSNorm
=================================================
*/
#define Gen_IS_UNORM_SNORM( _stype_, _vtype_ )																									\
	ND_ bool  IsUNorm (const _stype_ x)			{ return All2( x >= _stype_(0.0), x <= _stype_(1.0) ); }										\
	ND_ bool  IsUNorm (const _vtype_##2 x)		{ return All2( AllGreaterEqual( x, _vtype_##2(0.0) ), AllLessEqual( x, _vtype_##2(1.0) )); }	\
	ND_ bool  IsUNorm (const _vtype_##3 x)		{ return All2( AllGreaterEqual( x, _vtype_##3(0.0) ), AllLessEqual( x, _vtype_##3(1.0) )); }	\
	ND_ bool  IsUNorm (const _vtype_##4 x)		{ return All2( AllGreaterEqual( x, _vtype_##4(0.0) ), AllLessEqual( x, _vtype_##4(1.0) )); }	\
																																				\
	ND_ bool  IsSNorm (const _stype_ x)			{ return Abs(x) <= 1.0; }																		\
	ND_ bool  IsSNorm (const _vtype_##2 x)		{ return AllLessEqual( Abs(x), _vtype_##2(1.0) ); }												\
	ND_ bool  IsSNorm (const _vtype_##3 x)		{ return AllLessEqual( Abs(x), _vtype_##3(1.0) ); }												\
	ND_ bool  IsSNorm (const _vtype_##4 x)		{ return AllLessEqual( Abs(x), _vtype_##4(1.0) ); }

Gen_IS_UNORM_SNORM( float,	float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_IS_UNORM_SNORM( half,		half_vec_t )
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
#define Gen_FPEQUAL( _stype_, _vtype_ )																											\
	ND_ bool   FpEqual (const _stype_    lhs, const _stype_    rhs, const _stype_ err)	{ return Abs( lhs - rhs ) < err; }						\
	ND_ bool2  FpEqual (const _vtype_##2 lhs, const _vtype_##2 rhs, const _stype_ err)	{ return Less( Abs( lhs - rhs ), _vtype_##2(err) ); }	\
	ND_ bool3  FpEqual (const _vtype_##3 lhs, const _vtype_##3 rhs, const _stype_ err)	{ return Less( Abs( lhs - rhs ), _vtype_##3(err) ); }	\
	ND_ bool4  FpEqual (const _vtype_##4 lhs, const _vtype_##4 rhs, const _stype_ err)	{ return Less( Abs( lhs - rhs ), _vtype_##4(err) ); }

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
#define Gen_SWAP( _type_ )\
	void  Swap (inout _type_ lhs, inout _type_ rhs)	{ _type_ tmp = lhs;  lhs = rhs;  rhs = tmp; }

Gen_SWAP( float )

#undef Gen_SWAP

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
#if defined(AE_shader_subgroup_quad) and defined(AE_shader_subgroup_basic)
#	define QuadGroup_dFdxFine( _a_ )		(gl.quadGroup.Broadcast( (_a_), (gl.subgroup.Index&2)|1 ) - gl.quadGroup.Broadcast( (_a_), gl.subgroup.Index&2 ))
#	define QuadGroup_dFdyFine( _a_ )		(gl.quadGroup.Broadcast( (_a_), (gl.subgroup.Index&1)|2 ) - gl.quadGroup.Broadcast( (_a_), gl.subgroup.Index&1 ))
#	define QuadGroup_fwidthFine( _a_ )		Abs(QuadGroup_dFdxFine(_a_)) + Abs(QuadGroup_dFdyFine(_a_))

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


// TODO:
// Exponentiation  https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#exponentiation-with-small-fractional-arguments
