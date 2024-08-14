/*
	3party noise functions.
*/
#ifdef __cplusplus
# pragma once
#endif

#define _UHash22( _a_ )			DHash22( _a_ )
#define _UHash33( _a_ )			DHash33( _a_ )
#define _SHash33( _a_ )			ToSNorm( DHash33( _a_ ))
#define _RGBTexSHash33( _p_ )	ToSNorm( gl.texture.SampleLod( rgbaNoise, (_p_).xy * 0.01 + (_p_).z * float2(0.01723059, 0.053092949), 0.0 ).rgb )
//-----------------------------------------------------------------------------


/*
=================================================
	GradientNoise
----
	range [-1..1]

	from https://www.shadertoy.com/view/4dffRH
	The MIT License
	Copyright (c) 2017 Inigo Quilez
=================================================
*/
#define _GRADIENT_NOISE( hash )																		\
{																									\
	/* grid */																						\
	float3 i = Floor(pos);																			\
	float3 w = Fract(pos);																			\
																									\
	/* quintic interpolant */																		\
	float3 u = w*w*w*(w*(w*6.0-15.0)+10.0);															\
	float3 du = 30.0*w*w*(w*(w-2.0)+1.0);															\
																									\
	/* cubic interpolant																			\
	float3 u = w*w*(3.0-2.0*w);																		\
	float3 du = 6.0*w*(1.0-w);																		\
	*/																								\
																									\
	/* gradients */																					\
	float3 ga = hash( i+float3(0.0,0.0,0.0) );														\
	float3 gb = hash( i+float3(1.0,0.0,0.0) );														\
	float3 gc = hash( i+float3(0.0,1.0,0.0) );														\
	float3 gd = hash( i+float3(1.0,1.0,0.0) );														\
	float3 ge = hash( i+float3(0.0,0.0,1.0) );														\
	float3 gf = hash( i+float3(1.0,0.0,1.0) );														\
	float3 gg = hash( i+float3(0.0,1.0,1.0) );														\
	float3 gh = hash( i+float3(1.0,1.0,1.0) );														\
																									\
	/* projections */																				\
	float va = Dot( ga, w-float3(0.0,0.0,0.0) );													\
	float vb = Dot( gb, w-float3(1.0,0.0,0.0) );													\
	float vc = Dot( gc, w-float3(0.0,1.0,0.0) );													\
	float vd = Dot( gd, w-float3(1.0,1.0,0.0) );													\
	float ve = Dot( ge, w-float3(0.0,0.0,1.0) );													\
	float vf = Dot( gf, w-float3(1.0,0.0,1.0) );													\
	float vg = Dot( gg, w-float3(0.0,1.0,1.0) );													\
	float vh = Dot( gh, w-float3(1.0,1.0,1.0) );													\
																									\
	/* interpolations */																			\
	return 	va + u.x*(vb-va) + u.y*(vc-va) + u.z*(ve-va) + u.x*u.y*(va-vb-vc+vd) +					\
			u.y*u.z*(va-vc-ve+vg) + u.z*u.x*(va-vb-ve+vf) + (-va+vb+vc-vd+ve-vf-vg+vh)*u.x*u.y*u.z;	\
}

float  GradientNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)	{ _GRADIENT_NOISE( _RGBTexSHash33 )}
float  GradientNoise (const float3 pos)										{ _GRADIENT_NOISE( _SHash33 )}
#undef _GRADIENT_NOISE

/*
=================================================
	IQNoise
----
	range [-1..1]

	from https://www.shadertoy.com/view/Xd23Dh
	The MIT License
	Copyright (c) 2014 Inigo Quilez
=================================================
*/
#define _IQNOISE( hash )													\
{																			\
	uv = Clamp( uv, float2(-0.5,0.4), float2(0.5,1.0) );					\
																			\
	const float3	p = Floor(pos);											\
	const float3	f = Fract(pos);											\
	const float		k = 1.0 + 63.0 * Pow( 1.0 - uv.y, 6.0 );				\
																			\
	float va = 0.0;															\
	float wt = 0.0;															\
	for (int z = -2; z <= 2; ++z)											\
	for (int y = -2; y <= 2; ++y)											\
	for (int x = -2; x <= 2; ++x)											\
	{																		\
		float3	g	= float3( float(x),float(y), float(z) );				\
		float3	o	= _SHash33( p + g ) * float3(uv.x,uv.x,1.0);			\
		float3	r	= g - f + o;											\
		float	d	= LengthSq( r );										\
		float	ww	= Pow( 1.0 - SmoothStep( Sqrt(d), 0.0, 1.414 ), k );	\
		va += o.z*ww;														\
		wt += ww;															\
	}																		\
	return va / wt;															\
}

float  IQNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, float2 uv)	{ _IQNOISE( _RGBTexSHash33 )}
float  IQNoise (const float3 pos, float2 uv)										{ _IQNOISE( _SHash33 )}
float  IQNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)				{ return IQNoise( rgbaNoise, pos, float2(0.5, 0.9) ); }
float  IQNoise (const float3 pos)													{ return IQNoise( pos, float2(0.5, 0.9) ); }
#undef _IQNOISE

/*
=================================================
	ValueNoise
----
	range [-1..1]

	from https://www.shadertoy.com/view/4sc3z2
	license CC BY-NC-SA 3.0 (shadertoy default)
=================================================
*/
#define _VALUENOISE																		\
{																						\
	float3 pi = Floor(pos);																\
	float3 pf = pos - pi;																\
																						\
	float3 w = pf * pf * (3.0 - 2.0 * pf);												\
																						\
	return 	Lerp(																		\
				Lerp(																	\
					Lerp(hash(pi + float3(0, 0, 0)), hash(pi + float3(1, 0, 0)), w.x),	\
					Lerp(hash(pi + float3(0, 0, 1)), hash(pi + float3(1, 0, 1)), w.x),	\
					w.z),																\
				Lerp(																	\
					Lerp(hash(pi + float3(0, 1, 0)), hash(pi + float3(1, 1, 0)), w.x),	\
					Lerp(hash(pi + float3(0, 1, 1)), hash(pi + float3(1, 1, 1)), w.x),	\
					w.z),																\
				w.y);																	\
}

float  ValueNoise (gl::CombinedTex2D<float> greyNoise, const float3 pos)
{
	#define hash( _p_ )	ToSNorm( gl.texture.SampleLod( greyNoise, (_p_).xy * 0.01 + (_p_).z * float2(0.01723059, 0.053092949), 0.0 ).r )
	_VALUENOISE
	#undef hash
}

float  ValueNoise (const float3 pos)
{
	#define hash( _p_ )	ToSNorm( DHash13( _p_ ))
	_VALUENOISE
	#undef hash
}

#undef _VALUENOISE

/*
=================================================
	PerlinNoise
----
	range [-1..1]

	from https://www.shadertoy.com/view/4sc3z2
	license CC BY-NC-SA 3.0 (shadertoy default)
=================================================
*/
#define _PERLINNOISE( hash )													\
{																				\
	float3 pi = Floor(pos);														\
	float3 pf = pos - pi;														\
																				\
	float3 w = pf * pf * (3.0 - 2.0 * pf);										\
																				\
	return 	Lerp(																\
				Lerp(															\
					Lerp(Dot(pf - float3(0, 0, 0), hash(pi + float3(0, 0, 0))),	\
						 Dot(pf - float3(1, 0, 0), hash(pi + float3(1, 0, 0))),	\
						 w.x),													\
					Lerp(Dot(pf - float3(0, 0, 1), hash(pi + float3(0, 0, 1))),	\
						 Dot(pf - float3(1, 0, 1), hash(pi + float3(1, 0, 1))),	\
						 w.x),													\
					w.z),														\
				Lerp(															\
					Lerp(Dot(pf - float3(0, 1, 0), hash(pi + float3(0, 1, 0))),	\
						 Dot(pf - float3(1, 1, 0), hash(pi + float3(1, 1, 0))),	\
						 w.x),													\
					Lerp(Dot(pf - float3(0, 1, 1), hash(pi + float3(0, 1, 1))),	\
						 Dot(pf - float3(1, 1, 1), hash(pi + float3(1, 1, 1))),	\
						 w.x),													\
					w.z),														\
				w.y) * 2.0;														\
}

float  PerlinNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)	{ _PERLINNOISE( _RGBTexSHash33 )}
float  PerlinNoise (const float3 pos)										{ _PERLINNOISE( _SHash33 )}
#undef _PERLINNOISE

/*
=================================================
	SimplexNoise
----
	range [-1..1]

	from https://www.shadertoy.com/view/4sc3z2
	license CC BY-NC-SA 3.0 (shadertoy default)
=================================================
*/
#define _SIMPLEXNOISE( hash )																									\
{																																\
	const float K1 = 0.333333333;																								\
	const float K2 = 0.166666667;																								\
																																\
	float3 i = Floor(pos + (pos.x + pos.y + pos.z) * K1);																		\
	float3 d0 = pos - (i - (i.x + i.y + i.z) * K2);																				\
																																\
	float3 e = Step(float3(0.0), d0 - d0.yzx);																					\
	float3 i1 = e * (1.0 - e.zxy);																								\
	float3 i2 = 1.0 - e.zxy * (1.0 - e);																						\
																																\
	float3 d1 = d0 - (i1 - 1.0 * K2);																							\
	float3 d2 = d0 - (i2 - 2.0 * K2);																							\
	float3 d3 = d0 - (1.0 - 3.0 * K2);																							\
																																\
	float4 h = Max(0.6 - float4(LengthSq(d0), LengthSq(d1), LengthSq(d2), LengthSq(d3)), 0.0);									\
	float4 n = h * h * h * h * float4(Dot(d0, hash(i)), Dot(d1, hash(i + i1)), Dot(d2, hash(i + i2)), Dot(d3, hash(i + 1.0)));	\
																																\
	return Dot(float4(31.316), n);																								\
}

float  SimplexNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)	{ _SIMPLEXNOISE( _RGBTexSHash33 )}
float  SimplexNoise (const float3 pos)										{ _SIMPLEXNOISE( _SHash33 )}
#undef _SIMPLEXNOISE

/*
=================================================
	VoronoiContour
----
	range [0..inf]

	based on code from https://www.shadertoy.com/view/ldl3W8
	The MIT License
	Copyright (c) 2013 Inigo Quilez
=================================================
*/
VoronoiResult2  VoronoiContour2 (const float2 coord, const float3 seedScaleBias_offsetScale)
{
	float2	ipoint	= Floor( coord );
	float2	fpoint	= Fract( coord );
	float2	mr;

	VoronoiResult2	result;
	result.minDist = float_max;

	for (int y = -1; y <= 1; ++y)
	for (int x = -1; x <= 1; ++x)
	{
		float2	ioffset	= float2( x, y );
		float2	offset	= _UHash22( (ipoint + ioffset) * seedScaleBias_offsetScale.x + seedScaleBias_offsetScale.y ) * seedScaleBias_offsetScale.z;
		float2	vec		= offset + ioffset - fpoint;
		float	d		= LengthSq( vec );

		if ( d < result.minDist )
		{
			result.minDist = d;
			result.icenter	 = ioffset;
			result.offset	 = offset;
			mr				 = vec;
		}
	}

	result.minDist = float_max;

	for (int y = -2; y <= 2; ++y)
	for (int x = -2; x <= 2; ++x)
	{
		float2	ioffset	= result.icenter + float2( x, y );
		float2	offset	= _UHash22( (ipoint + ioffset) * seedScaleBias_offsetScale.x + seedScaleBias_offsetScale.y ) * seedScaleBias_offsetScale.z;
		float2	vec		= offset + ioffset - fpoint;
		float	d		= LengthSq( mr - vec );

		if ( d > 0.00001 )
			result.minDist = Min( result.minDist, Dot( 0.5*(mr + vec), Normalize(vec - mr) ));
	}

	result.icenter += ipoint;
	return result;
}

VoronoiResult3  VoronoiContour2 (const float3 coord, const float3 seedScaleBias_offsetScale)
{
	float3	ipoint	= Floor( coord );
	float3	fpoint	= Fract( coord );
	float3	mr;

	VoronoiResult3	result;
	result.minDist = float_max;

	for (int z = -1; z <= 1; ++z)
	for (int y = -1; y <= 1; ++y)
	for (int x = -1; x <= 1; ++x)
	{
		float3	ioffset	= float3( x, y, z );
		float3	offset	= _UHash33( (ipoint + ioffset) * seedScaleBias_offsetScale.x + seedScaleBias_offsetScale.y ) * seedScaleBias_offsetScale.z;
		float3	vec		= offset + ioffset - fpoint;
		float	d		= LengthSq( vec );

		if ( d < result.minDist )
		{
			result.minDist = d;
			result.icenter	 = ioffset;
			result.offset	 = offset;
			mr				 = vec;
		}
	}

	result.minDist = float_max;

	for (int z = -2; z <= 2; ++z)
	for (int y = -2; y <= 2; ++y)
	for (int x = -2; x <= 2; ++x)
	{
		float3	ioffset	= result.icenter + float3( x, y, z );
		float3	offset	= _UHash33( (ipoint + ioffset) * seedScaleBias_offsetScale.x + seedScaleBias_offsetScale.y ) * seedScaleBias_offsetScale.z;
		float3	vec		= offset + ioffset - fpoint;
		float	d		= LengthSq( mr - vec );

		if ( d > 0.00001 )
			result.minDist = Min( result.minDist, Dot( 0.5*(mr + vec), Normalize(vec - mr) ));
	}

	result.icenter += ipoint;
	return result;
}

/*
=================================================
	VoronoiCircles
----
	range [0..inf]

	based on VoronoiContour
=================================================
*/
float  VoronoiCircles (const float2 coord, const float radiusScale, const float3 seedScaleBias_offsetScale)
{
	float2	ipoint	= Floor( coord );
	float2	fpoint	= Fract( coord );

	float2	icenter	= float2(0.0);
	float	md		= float_max;
	float	mr		= float_max;

	// find nearest circle
	for (int y = -1; y <= 1; ++y)
	for (int x = -1; x <= 1; ++x)
	{
		float2	cur	= float2(x, y);
		float2	off	= _UHash22( (cur + ipoint) * seedScaleBias_offsetScale.x + seedScaleBias_offsetScale.y ) * seedScaleBias_offsetScale.z + cur - fpoint;
		float	d	= Dot( off, off );

		if ( d < md )
		{
			md = d;
			icenter = cur;
		}
	}

	// calc circle radius
	for (int y = -2; y <= 2; ++y)
	for (int x = -2; x <= 2; ++x)
	{
		if ( AllEqual( int2(x,y), int2(0) ))
			continue;

		float2	cur = icenter + float2(x, y);
		float2	off	= _UHash22( (cur + ipoint) * seedScaleBias_offsetScale.x + seedScaleBias_offsetScale.y ) * seedScaleBias_offsetScale.z + cur - fpoint;
		float	d	= LengthSq( off );

		if ( d < mr )
			mr = d;
	}

	md = Sqrt( md );
	mr = Sqrt( mr ) * 0.5 * radiusScale;

	if ( md < mr )
		return 1.0 / (Square( md / mr ) * 16.0) - 0.07;

	return 0.0;
}

float  VoronoiCircles (const float2 coord, const float radiusScale)
{
	return VoronoiCircles( coord, radiusScale, float3(1.0, 0.0, 0.75) );
}

/*
=================================================
	WaveletNoise
----
	from https://www.shadertoy.com/view/wsBfzK
	The MIT License
	Copyright (c) 2020 Martijn Steinrucken
=================================================
*/
float  WaveletNoise (float2 coord, const float2 zk)
{
	float d = 0.0f;
	float s = 1.0f;
	float m = 0.0f;

	for (float i = 0.0f; i < 4.0f; ++i)
	{
		float2 q = coord * s;
		float2 g = Fract(Floor(q) * float2(123.34f, 233.53f));
		g += Dot(g, g + 23.234f);

		float a = Fract(g.x * g.y) * 1.0e+3f; // +z*(mod(g.x+g.y, 2.)-1.); // add vorticity
		q = (Fract(q) - 0.5) * float2x2(Cos(a), -Sin(a), Sin(a), Cos(a));
		d += Sin(q.x * 10.0 + zk.x) * SmoothStep( LengthSq(q), 0.25f, 0.0f ) / s;

		coord = coord * float2x2(0.54f, -0.84f, 0.84f, 0.54f) + i;
		m += 1.0 / s;
		s *= zk.y;
	}
	return d / m;
}

float  WaveletNoise (float2 coord)
{
	return WaveletNoise( coord, float2(0.2, 0.0) );
}
//-----------------------------------------------------------------------------


#undef _UHash22
#undef _UHash33
#undef _SHash33
#undef _RGBTexSHash33
