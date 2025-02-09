// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Spline interpolation.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

#if 0
	T  CatmullRom (T p0, T p1, T p2, T p3, Scalar t)
	T  QLerp (T p0, T p1, Scalar t)
	T  Bezier3 (T p0, T p1, T p2, Scalar t)
	T  Bezier4 (T p0, T p1, T p2, T p3, Scalar t)
	T  BSpline (T p0, T p1, T p2, T p3, Scalar t)
#endif
//-----------------------------------------------------------------------------


/*
=================================================
	CatmullRom
----
	T  CatmullRom (T p0, T p1, T p2, T p3, Scalar t)
----
	Result will be in range [p1, p2],
	points p0, p3 is a control points.
----
	Multiple calls:
		CatmullRom( p4, p5, p6, p7, a );	result in range: [p5, p6]
		CatmullRom( p5, p6, p7, p8, b );	result in range: [p6, p7]
=================================================
*/
#define Gen_CATMULLROM1( _stype_, _type_ )											\
	ND_ _type_  CatmullRom (_type_ p0, _type_ p1, _type_ p2, _type_ p3, _stype_ t)	\
	{																				\
		_type_	a0	= Lerp( p0, p1, t + _stype_(1.0) );								\
		_type_	a1	= Lerp( p1, p2, t );											\
		_type_	a2	= Lerp( p2, p3, t - _stype_(1.0) );								\
		_type_	b0	= Lerp( a0, a1, ToUNorm(t) );									\
		_type_	b1	= Lerp( a1, a2, t * _stype_(0.5) );								\
		return Lerp( b0, b1, t );													\
	}

#define Gen_CATMULLROM( _stype_, _vtype_ )			\
	Gen_CATMULLROM1( _stype_, _stype_ )				\
	Gen_CATMULLROM1( _stype_, UNITE( _vtype_, 2 ))	\
	Gen_CATMULLROM1( _stype_, UNITE( _vtype_, 3 ))	\
	Gen_CATMULLROM1( _stype_, UNITE( _vtype_, 4 ))

Gen_CATMULLROM( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_CATMULLROM( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_CATMULLROM( double, double_vec_t )
#endif

#undef Gen_CATMULLROM1
#undef Gen_CATMULLROM

/*
=================================================
	QLerp
----
	T  QLerp (T p0, T p1, Scalar t)
=================================================
*/
#define Gen_QLERP1( _stype_, _type_ )										\
	ND_ _type_  QLerp (_type_ p0, _type_ p1, _stype_ t)						\
	{																		\
		/* SmoothStep() - Hermite interpolation */							\
		return Lerp( p0, p1, SmoothStep( t, _stype_(0.0), _stype_(1.0) ));	\
	}

#define Gen_QLERP( _stype_, _vtype_ )			\
	Gen_QLERP1( _stype_, _stype_ )				\
	Gen_QLERP1( _stype_, UNITE( _vtype_, 2 ))	\
	Gen_QLERP1( _stype_, UNITE( _vtype_, 3 ))	\
	Gen_QLERP1( _stype_, UNITE( _vtype_, 4 ))

Gen_QLERP( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_QLERP( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_QLERP( double, double_vec_t )
#endif

#undef Gen_QLERP1
#undef Gen_QLERP

/*
=================================================
	Bezier3
----
	T  Bezier3 (T p0, T p1, T p2, Scalar t)
----
	Result is interpolation between all 3 points (p0, p1, p2), but:
	* may not intersects with specified points.
	* may lay outside of points bounding box.
----
	Multiple calls:
		Bezier3( p0, p1, p2, a );
		Bezier3( p2, p3, p4, b );
=================================================
*/
#define Gen_BEZIER3A( _stype_, _type_ )									\
	ND_ _type_  Bezier3 (_type_ p0, _type_ p1, _type_ p2, _stype_ t)	\
	{																	\
		_type_	a = Lerp( p0, p1, t );									\
		_type_	b = Lerp( p1, p2, t );									\
		return Lerp( a, b, t );											\
	}

#define Gen_BEZIER3( _stype_, _vtype_ )			\
	Gen_BEZIER3A( _stype_, _stype_ )			\
	Gen_BEZIER3A( _stype_, UNITE( _vtype_, 2 ))	\
	Gen_BEZIER3A( _stype_, UNITE( _vtype_, 3 ))	\
	Gen_BEZIER3A( _stype_, UNITE( _vtype_, 4 ))

Gen_BEZIER3( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_BEZIER3( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_BEZIER3( double, double_vec_t )
#endif

#undef Gen_BEZIER3A
#undef Gen_BEZIER3

/*
=================================================
	Bezier4
----
	T  Bezier4 (T p0, T p1, T p2, T p3, Scalar t)
----
	Result is interpolation between all 4 points (p0, p1, p2, p3), but:
	* may not intersects with specified points.
	* may lay outside of points bounding box.
----
	Multiple calls:
		Bezier4( p0, p1, p2, p3, a );
		Bezier4( p3, p4, p5, p6, b );
=================================================
*/
#define Gen_BEZIER4A( _stype_, _type_ )											\
	ND_ _type_  Bezier4 (_type_ p0, _type_ p1, _type_ p2, _type_ p3, _stype_ t)	\
	{																			\
		_type_	a = Lerp( p0, p1, t );											\
		_type_	b = Lerp( p1, p2, t );											\
		_type_	c = Lerp( p2, p3, t );											\
		_type_	m = Lerp( a,  b,  t );											\
		_type_	n = Lerp( b,  c,  t );											\
		return Lerp( m, n, t );													\
	}

#define Gen_BEZIER4( _stype_, _vtype_ )			\
	Gen_BEZIER4A( _stype_, _stype_ )			\
	Gen_BEZIER4A( _stype_, UNITE( _vtype_, 2 ))	\
	Gen_BEZIER4A( _stype_, UNITE( _vtype_, 3 ))	\
	Gen_BEZIER4A( _stype_, UNITE( _vtype_, 4 ))

Gen_BEZIER4( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_BEZIER4( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_BEZIER4( double, double_vec_t )
#endif

#undef Gen_BEZIER4A
#undef Gen_BEZIER4

/*
=================================================
	BSpline
----
	T  BSpline (T p0, T p1, T p2, T p3, Scalar t)
----
	Result will be in range [p1, p2],
	points p0, p3 is a control points.
----
	Multiple calls:
		BSpline( p4, p5, p6, p7, a );	result in range: [p5, p6]
		BSpline( p5, p6, p7, p8, b );	result in range: [p6, p7]
=================================================
*/
#define Gen_BSPLINE1( _stype_, _type_ )											\
	ND_ _type_  BSpline (_type_ p0, _type_ p1, _type_ p2, _type_ p3, _stype_ t)	\
	{																			\
		_type_	c0 = (p0 + _stype_(4.0) * p1 + p2) / _stype_(6.0);				\
		_type_	c1 = (p2 - p0) * _stype_(0.5);									\
		_type_	c2 = (p0 + p2) * _stype_(0.5) - p1;								\
		_type_	c3 = (p3 - p0) / _stype_(6.0) + (p1 - p2) * _stype_(0.5);		\
		return c0 + (c1 * t) + (c2 * t*t) + (c3 * t*t*t);						\
	}

#define Gen_BSPLINE( _stype_, _vtype_ )			\
	Gen_BSPLINE1( _stype_, _stype_ )			\
	Gen_BSPLINE1( _stype_, UNITE( _vtype_, 2 ))	\
	Gen_BSPLINE1( _stype_, UNITE( _vtype_, 3 ))	\
	Gen_BSPLINE1( _stype_, UNITE( _vtype_, 4 ))

Gen_BSPLINE( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_BSPLINE( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_BSPLINE( double, double_vec_t )
#endif

#undef Gen_BSPLINE1
#undef Gen_BSPLINE
