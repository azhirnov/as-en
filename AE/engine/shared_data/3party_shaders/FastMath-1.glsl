
#ifdef AE_ENABLE_UNKNOWN_LICENSE

/*
=================================================
	FastACos
----
	from https://developer.download.nvidia.com/cg/acos.html
----
	Handbook of Mathematical Functions
	M. Abramowitz and I.A. Stegun, Ed.
----
	Absolute error <= 6.7e-5
=================================================
*/
#define Gen_FAST_ACOS1( _stype_, _vtype_ )\
	ND_ _vtype_  FastACos (_vtype_ x)							\
	{															\
		_vtype_	negate	= LessFp( x, _vtype_(_stype_(0.0)) );	\
				x		= Abs( x );								\
		_vtype_	ret		= _vtype_(_stype_(-0.0187293));			\
		ret *= x;												\
		ret += _stype_(0.0742610);								\
		ret *= x;												\
		ret -= _stype_(0.2121144);								\
		ret *= x;												\
		ret += _stype_(float_HalfPi);							\
		ret *= Sqrt( _stype_(1.0) - x );						\
		ret -= _stype_(2.0) * negate * ret;						\
		return negate * _stype_(float_Pi) + ret;				\
	}

#define Gen_FAST_ACOS( _stype_, _vtype_ )\
	Gen_FAST_ACOS1( _stype_, _stype_ )\
	Gen_FAST_ACOS1( _stype_, UNITE( _vtype_, 2 ))\
	Gen_FAST_ACOS1( _stype_, UNITE( _vtype_, 3 ))\
	Gen_FAST_ACOS1( _stype_, UNITE( _vtype_, 4 ))

Gen_FAST_ACOS( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_FAST_ACOS( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_FAST_ACOS( double, double_vec_t )
#endif

#undef Gen_FAST_ACOS1
#undef Gen_FAST_ACOS

/*
=================================================
	FastASin
----
	from https://developer.download.nvidia.com/cg/asin.html
----
	Handbook of Mathematical Functions
	M. Abramowitz and I.A. Stegun, Ed.
=================================================
*/
#define Gen_FAST_ASIN1( _stype_, _vtype_ )\
	ND_ _vtype_  FastASin (_vtype_ x)									\
	{																	\
		_vtype_	negate	= LessFp( x, _vtype_(_stype_(0.0)) );			\
				x		= Abs( x );										\
		_vtype_	ret		= _vtype_(_stype_(-0.0187293));					\
		ret *= x;														\
		ret += _stype_(0.0742610);										\
		ret *= x;														\
		ret -= _stype_(0.2121144);										\
		ret *= x;														\
		ret += _stype_(float_HalfPi);									\
		ret = _stype_(float_HalfPi) - Sqrt( _stype_(1.0) - x ) * ret;	\
		return ret - _stype_(2.0) * negate * ret;						\
	}

#define Gen_FAST_ASIN( _stype_, _vtype_ )\
	Gen_FAST_ASIN1( _stype_, _stype_ )\
	Gen_FAST_ASIN1( _stype_, UNITE( _vtype_, 2 ))\
	Gen_FAST_ASIN1( _stype_, UNITE( _vtype_, 3 ))\
	Gen_FAST_ASIN1( _stype_, UNITE( _vtype_, 4 ))

Gen_FAST_ASIN( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_FAST_ASIN( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_FAST_ASIN( double, double_vec_t )
#endif

#undef Gen_FAST_ASIN1
#undef Gen_FAST_ASIN

/*
=================================================
	FastATan, FastATan2
----
	from https://developer.download.nvidia.com/cg/atan2.html
=================================================
*/
#define Gen_FAST_ATAN1( _stype_, _vtype_ )\
	ND_ _vtype_  FastATan2 (const _vtype_ y, const _vtype_ x)							\
	{																					\
		_vtype_	t0, t1, t2, t3, t4;														\
																						\
		t3 = Abs( x );																	\
		t1 = Abs( y );																	\
		t0 = Max( t3, t1 );																\
		t1 = Min( t3, t1 );																\
		t3 = _stype_(1.0) / t0;															\
		t3 = t1 * t3;																	\
																						\
		t4 = t3 * t3;																	\
		t0 = _vtype_( - _stype_(0.013480470));											\
		t0 = t0 * t4 + _stype_(0.057477314);											\
		t0 = t0 * t4 - _stype_(0.121239071);											\
		t0 = t0 * t4 + _stype_(0.195635925);											\
		t0 = t0 * t4 - _stype_(0.332994597);											\
		t0 = t0 * t4 + _stype_(0.999995630);											\
		t3 = t0 * t3;																	\
																						\
		t3 = SelectFp( Abs(x),  Abs(y),			  _stype_(float_HalfPi) - t3,	t3 );	\
		t3 = SelectFp( x, _vtype_(_stype_(0.0)),  _stype_(float_Pi) - t3,		t3 );	\
		t3 = SelectFp( y, _vtype_(_stype_(0.0)),  -t3,							t3 );	\
																						\
		return t3;																		\
	}																					\
																						\
	ND_ _vtype_  FastATan (_vtype_ x) {													\
		return FastATan2( x, _vtype_(_stype_(1.0)) );									\
	}

#define Gen_FAST_ATAN( _stype_, _vtype_ )\
	Gen_FAST_ATAN1( _stype_, _stype_ )\
	Gen_FAST_ATAN1( _stype_, UNITE( _vtype_, 2 ))\
	Gen_FAST_ATAN1( _stype_, UNITE( _vtype_, 3 ))\
	Gen_FAST_ATAN1( _stype_, UNITE( _vtype_, 4 ))

Gen_FAST_ATAN( float, float_vec_t )

#if AE_ENABLE_HALF_TYPE
	Gen_FAST_ATAN( half, half_vec_t )
#endif
#if AE_ENABLE_DOUBLE_TYPE
	Gen_FAST_ATAN( double, double_vec_t )
#endif

#undef Gen_FAST_ATAN1
#undef Gen_FAST_ATAN

#endif // AE_ENABLE_UNKNOWN_LICENSE
//-----------------------------------------------------------------------------


#ifdef AE_LICENSE_MIT

/*
=================================================
	FastSqrt
----
	warning: native Sqrt is faster on most devices
----
	from https://www.shadertoy.com/view/ssyyDh
	The MIT License
	Copyright © 2022 Inigo Quilez
=================================================
*/
	ND_ float  FastSqrt_v1 (const float x)
	{
		float y = uintBitsToFloat( 532545536u + (floatBitsToUint(x) >> 1) );
		y = 0.5 * (y + x/y);
		y = 0.5 * (y + x/y);
		return y;
	}

	ND_ float  FastSqrt_v2 (const float x)
	{
		float	y	= uintBitsToFloat( 532545536u + (floatBitsToUint(x) >> 1) );
		float	y2	= y*y;
		float	x2	= x*x;
		return (y2 * (y2 + 6.0 * x) + x2) / (4.0 * y * (y2 + x));
	}

/*
=================================================
	FastCbrt
----
	warning: native Pow is faster on most devices
----
	from https://www.shadertoy.com/view/ssyyDh
	The MIT License
	Copyright © 2022 Inigo Quilez
=================================================
*/
	ND_ float  FastCbrt (const float x)
	{
		float y = uintBitsToFloat( 709973695u + floatBitsToUint(x) / 3u );
		y = y * (2.0/3.0) + (1.0/3.0) * x / (y*y);
		y = y * (2.0/3.0) + (1.0/3.0) * x / (y*y);
		return y;
	}

#endif // AE_LICENSE_MIT
//-----------------------------------------------------------------------------
