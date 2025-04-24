/*
	3party noise functions.
*/
#ifdef __cplusplus
# pragma once
#endif

#define _PosTranf2D( _p_ )		((_p_) * params.seedScale.xy + params.seedBias.xy)
#define _PosTranf3D( _p_ )		((_p_) * params.seedScale + params.seedBias)

#define _UHash22( _p_ )			DHash22( _PosTranf2D( _p_ ))
#define _UHash33( _p_ )			DHash33( _PosTranf3D( _p_ ))
#define _SHash33( _p_ )			ToSNorm( _UHash33( _p_ ))
#define _UHash13( _p_ )			DHash13( _PosTranf3D( _p_ ))
#define _SHash13( _p_ )			ToSNorm( _UHash13( _p_ ))

#define _TileUHash22( _p_ )		_UHash22( Mod( (_p_), params.tileSize.xy ))
#define _TileUHash33( _p_ )		_UHash33( Mod( (_p_), params.tileSize ))
#define _TileUHash13( _p_ )		_UHash13( Mod( (_p_), params.tileSize ))
#define _TileSHash33( _p_ )		ToSNorm( _TileUHash33( _p_ ))
#define _TileSHash13( _p_ )		ToSNorm( _TileUHash13( _p_ ))

#define _RGBTexUHash33( _p_ )	gl.texture.SampleLod( rgbaNoise, (_p_).xy * 0.01 + (_p_).z * float2(0.01723059, 0.053092949), 0.0 ).rgb
#define _RGBTexSHash33( _p_ )	ToSNorm( _RGBTexUHash33( _p_ ))
#define _GreyTexUHash13( _p_ )	gl.texture.SampleLod( greyNoise, (_p_).xy * 0.01 + (_p_).z * float2(0.01723059, 0.053092949), 0.0 ).r
#define _GreyTexSHash13( _p_ )	ToSNorm( _GreyTexUHash13( _p_ ))
//-----------------------------------------------------------------------------


/*
=================================================
	GradientNoise
	TileableGradientNoise
----
	range [-1..1]

	from https://www.shadertoy.com/view/4dffRH
	The MIT License
	Copyright (c) 2017 Inigo Quilez
=================================================
*/
#ifdef AE_LICENSE_MIT

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

	float  GradientNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const NoiseParams params)	{ _GRADIENT_NOISE( _RGBTexSHash33 )}
	float  GradientNoise (const float3 pos, const NoiseParams params)										{ _GRADIENT_NOISE( _SHash33 )}

	float  GradientNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)								{ return GradientNoise( rgbaNoise, pos, CreateNoiseParams() ); }
	float  GradientNoise (const float3 pos)																	{ return GradientNoise( pos, CreateNoiseParams() ); }

	float  TileableGradientNoise (const float3 pos, const TileableNoiseParams params)						{ _GRADIENT_NOISE( _TileSHash33 )}
	float  TileableGradientNoise (const float3 pos, const float3 tileSize)									{ return TileableGradientNoise( pos, CreateTileableNoiseParams(tileSize) ); }

	#undef _GRADIENT_NOISE
#endif // AE_LICENSE_MIT

/*
=================================================
	IQNoise
	TileableIQNoise
----
	result in range [0..1]
	'uv.x' in range [-0.5, 0.5]
	'uv.y' in range [0.4, 1.0]

	based on https://www.shadertoy.com/view/Xd23Dh
	The MIT License
	Copyright (c) 2014 Inigo Quilez
=================================================
*/
#ifdef AE_LICENSE_MIT

	#define _IQNOISE( hash, hash2 )										\
	{																	\
		const float		u = Clamp( params.custom.x, -0.5, 0.5 );		\
		const float		v = Clamp( params.custom.y, 0.4, 1.0 );			\
		const float3	p = Floor(pos);									\
		const float3	f = Fract(pos);									\
		const float		k = 1.0 + 63.0 * Pow( 1.0 - v, 6.0 );			\
																		\
		float va = 0.0;													\
		float wt = 0.0;													\
		UNROLL for (int z = -2; z <= 2; ++z)							\
		UNROLL for (int y = -2; y <= 2; ++y)							\
		UNROLL for (int x = -2; x <= 2; ++x)							\
		{																\
			float3	g	= float3(x,y,z);								\
			float3	o	= hash( p + g ) * u;							\
			float3	r	= g - f + o;									\
			float	d	= Length( r );									\
			float	ww	= Pow( 1.0 - SmoothStep( d, 0.0, 1.414 ), k );	\
			va += hash2( p + g ) * ww;									\
			wt += ww;													\
		}																\
		return va / wt;													\
	}

	float  IQNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const NoiseParams params)	{ _IQNOISE( _RGBTexUHash33, _UHash13 )}
	float  IQNoise (const float3 pos, const NoiseParams params)										{ _IQNOISE( _UHash33, _UHash13 )}

	float  IQNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)
	{
		NoiseParams	p = CreateNoiseParams();
		p.custom.xy = float2( 0.5, 0.9 );
		return IQNoise( rgbaNoise, pos, p );
	}

	float  IQNoise (const float3 pos)
	{
		NoiseParams	p = CreateNoiseParams();
		p.custom.xy = float2( 0.5, 0.9 );
		return IQNoise( pos, p );
	}

	float  TileableIQNoise (const float3 pos, const TileableNoiseParams params)		{ _IQNOISE( _TileUHash33, _TileUHash13 )}

	float  TileableIQNoise (const float3 pos, const float3 tileSize)
	{
		TileableNoiseParams	p = CreateTileableNoiseParams( tileSize );
		p.custom.xy = float2( 0.5, 0.9 );
		return TileableIQNoise( pos, p );
	}

	#undef _IQNOISE
#endif // AE_LICENSE_MIT

/*
=================================================
	ValueNoise
	TileableValueNoise
----
	range [-1..1]

	from https://www.shadertoy.com/view/4sc3z2
	license CC BY-NC-SA 3.0 (shadertoy default)
=================================================
*/
#ifdef AE_LICENSE_CC_BY_NC_SA_3

	#define _VALUENOISE( hash )																\
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

	float  ValueNoise (gl::CombinedTex2D<float> greyNoise, const float3 pos, const NoiseParams params)	{ _VALUENOISE( _GreyTexSHash13 )}
	float  ValueNoise (const float3 pos, const NoiseParams params)										{ _VALUENOISE( _SHash13 )}

	float  ValueNoise (gl::CombinedTex2D<float> greyNoise, const float3 pos)							{ return ValueNoise( greyNoise, pos, CreateNoiseParams() ); }
	float  ValueNoise (const float3 pos)																{ return ValueNoise( pos, CreateNoiseParams() ); }

	float  TileableValueNoise (const float3 pos, const TileableNoiseParams params)						{ _VALUENOISE( _TileSHash13 )}
	float  TileableValueNoise (const float3 pos, const float3 tileSize)									{ return TileableValueNoise( pos, CreateTileableNoiseParams(tileSize) ); }

	#undef _VALUENOISE
#endif // AE_LICENSE_CC_BY_NC_SA_3

/*
=================================================
	PerlinNoise
	TileablePerlinNoise
----
	range [-1..1]

	from https://www.shadertoy.com/view/4sc3z2
	license CC BY-NC-SA 3.0 (shadertoy default)
=================================================
*/
#ifdef AE_LICENSE_CC_BY_NC_SA_3

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

	float  PerlinNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const NoiseParams params)	{ _PERLINNOISE( _RGBTexSHash33 )}
	float  PerlinNoise (const float3 pos, const NoiseParams params)										{ _PERLINNOISE( _SHash33 )}

	float  PerlinNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)							{ return PerlinNoise( rgbaNoise, pos, CreateNoiseParams() ); }
	float  PerlinNoise (const float3 pos)																{ return PerlinNoise( pos, CreateNoiseParams() ); }

	float  TileablePerlinNoise (const float3 pos, const TileableNoiseParams params)						{ _PERLINNOISE( _TileSHash33 )}
	float  TileablePerlinNoise (const float3 pos, const float3 tileSize)								{ return TileablePerlinNoise( pos, CreateTileableNoiseParams(tileSize) ); }

	#undef _PERLINNOISE
#endif // AE_LICENSE_CC_BY_NC_SA_3

/*
=================================================
	SimplexNoise
----
	range [-1..1]

	from https://www.shadertoy.com/view/4sc3z2
	license CC BY-NC-SA 3.0 (shadertoy default)
=================================================
*
#ifdef AE_LICENSE_CC_BY_NC_SA_3

	#define _SIMPLEXNOISE( hash )																		\
	{																									\
		const float K1 = 0.333333333;																	\
		const float K2 = 0.166666667;																	\
																										\
		float3 i = Floor(pos + (pos.x + pos.y + pos.z) * K1);											\
		float3 d0 = pos - (i - (i.x + i.y + i.z) * K2);													\
																										\
		float3 e = Step(float3(0.0), d0 - d0.yzx);														\
		float3 i1 = e * (1.0 - e.zxy);																	\
		float3 i2 = 1.0 - e.zxy * (1.0 - e);															\
																										\
		float3 d1 = d0 - (i1 - 1.0 * K2);																\
		float3 d2 = d0 - (i2 - 2.0 * K2);																\
		float3 d3 = d0 - (1.0 - 3.0 * K2);																\
																										\
		float4 h =	Max(0.6 - float4(LengthSq(d0), LengthSq(d1), LengthSq(d2), LengthSq(d3)), 0.0);		\
		float4 n =	h * h * h * h *																		\
					float4( Dot(d0, hash(i)), Dot(d1, hash(i + i1)),									\
							Dot(d2, hash(i + i2)), Dot(d3, hash(i + 1.0)) );							\
																										\
		return Dot(float4(31.316), n);																	\
	}

	float  SimplexNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)	{ _SIMPLEXNOISE( _RGBTexSHash33 )}
	float  SimplexNoise (const float3 pos)										{ _SIMPLEXNOISE( _SHash33 )}

	#undef _SIMPLEXNOISE
#endif // AE_LICENSE_CC_BY_NC_SA_3

/*
=================================================
	SimplexNoise
----
	range [-1..1]

	from https://www.shadertoy.com/view/XsX3zB
	The MIT License
	Copyright © 2013 Nikita Miropolskiy
=================================================
*/
#ifdef AE_LICENSE_MIT

	#define _SIMPLEXNOISE( hash )															\
	{																						\
		const float F3 =  0.3333333;														\
		const float G3 =  0.1666667;														\
																							\
		/* 1. find current tetrahedron T and it's four vertices */							\
		/* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */	\
		/* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/		\
																							\
		/* calculate s and x */																\
		float3 s = Floor(pos + Dot(pos, float3(F3)));										\
		float3 x = pos - s + Dot(s, float3(G3));											\
																							\
		/* calculate i1 and i2 */															\
		float3 e = Step(float3(0.0), x - x.yzx);											\
		float3 i1 = e*(1.0 - e.zxy);														\
		float3 i2 = 1.0 - e.zxy*(1.0 - e);													\
	 																						\
		/* x1, x2, x3 */																	\
		float3 x1 = x - i1 + G3;															\
		float3 x2 = x - i2 + 2.0*G3;														\
		float3 x3 = x - 1.0 + 3.0*G3;														\
																							\
		/* 2. find four surflets and store them in d */										\
		float4 w, d;																		\
																							\
		/* calculate surflet weights */														\
		w.x = Dot(x, x);																	\
		w.y = Dot(x1, x1);																	\
		w.z = Dot(x2, x2);																	\
		w.w = Dot(x3, x3);																	\
																							\
		/* w fades from 0.6 at the center of the surflet to 0.0 at the margin */			\
		w = Max(0.6 - w, 0.0);																\
																							\
		/* calculate surflet components */													\
		d.x = Dot(hash(s)-0.5, x);															\
		d.y = Dot(hash(s + i1)-0.5, x1);													\
		d.z = Dot(hash(s + i2)-0.5, x2);													\
		d.w = Dot(hash(s + 1.0)-0.5, x3);													\
																							\
		/* multiply d by w^4 */																\
		w *= w;																				\
		w *= w;																				\
		d *= w;																				\
																							\
		/* 3. return the sum of the four surflets */										\
		return Dot(d, float4(52.0));														\
	}

	float  SimplexNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const NoiseParams params)	{ _SIMPLEXNOISE( _RGBTexUHash33 )}
	float  SimplexNoise (const float3 pos, const NoiseParams params)										{ _SIMPLEXNOISE( _UHash33 )}

	float  SimplexNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)								{ return SimplexNoise( rgbaNoise, pos, CreateNoiseParams() ); }
	float  SimplexNoise (const float3 pos)																	{ return SimplexNoise( pos, CreateNoiseParams() ); }

	#undef _SIMPLEXNOISE
#endif // AE_LICENSE_MIT

/*
=================================================
	Voronoi 2D
=================================================
*/
#ifdef AE_LICENSE_MIT

	#define _VORONOI_NOISE										\
		{														\
			float2	ipoint	= Floor( pos );						\
			float2	fpoint	= Fract( pos );						\
																\
			VoronoiResult2	result;								\
			result.minDist = float_max;							\
																\
			UNROLL for (int y = -1; y <= 1; ++y)				\
			UNROLL for (int x = -1; x <= 1; ++x)				\
			{													\
				float2	ioffset	= float2( x, y );				\
				float2	offset	= hash( ipoint + ioffset );		\
				float2	vec		= offset + ioffset - fpoint;	\
				float	d		= LengthSq( vec );				\
																\
				if ( d < result.minDist )						\
				{												\
					result.minDist	= d;						\
					result.icenter	= ipoint + ioffset;			\
					result.offset	= offset;					\
				}												\
			}													\
			return result;										\
		}

	VoronoiResult2  VoronoiR (const float2 pos, const NoiseParams params)
	{
		#define hash( _p_ )		(_UHash22(_p_) * /*maxCeilOffset*/params.custom.x)
		_VORONOI_NOISE
		#undef hash
	}

	VoronoiResult2  VoronoiR (const float2 pos)
	{
		NoiseParams	p = CreateNoiseParams();
		p.custom.x = 1.0;	// maxCeilOffset
		return VoronoiR( pos, p );
	}

	VoronoiResult2  TileableVoronoiNoiseR (const float2 pos, const TileableNoiseParams params)
	{
		#define hash( _p_ )		(_TileUHash22(_p_) * /*maxCeilOffset*/params.custom.x)
		_VORONOI_NOISE
		#undef hash
	}

	VoronoiResult2  TileableVoronoiNoiseR (const float2 pos, const float2 tileSize)
	{
		TileableNoiseParams	p = CreateTileableNoiseParams( tileSize );
		p.custom.x = 1.0;	// maxCeilOffset
		return TileableVoronoiNoiseR( pos, p );
	}

	#undef _VORONOI_NOISE
#endif // AE_LICENSE_MIT

/*
=================================================
	Voronoi 3D
=================================================
*/
#ifdef AE_LICENSE_MIT

	#define _VORONOI_NOISE										\
		{														\
			float3	ipoint	= Floor( pos );						\
			float3	fpoint	= Fract( pos );						\
																\
			VoronoiResult3	result;								\
			result.minDist = float_max;							\
																\
			UNROLL for (int z = -1; z <= 1; ++z)				\
			UNROLL for (int y = -1; y <= 1; ++y)				\
			UNROLL for (int x = -1; x <= 1; ++x)				\
			{													\
				float3	ioffset	= float3( x, y, z );			\
				float3	offset	= hash( ipoint + ioffset );		\
				float3	vec		= offset + ioffset - fpoint;	\
				float	d		= LengthSq( vec );				\
																\
				if ( d < result.minDist )						\
				{												\
					result.minDist	= d;						\
					result.icenter	= ipoint + ioffset;			\
					result.offset	= offset;					\
				}												\
			}													\
			return result;										\
		}

	VoronoiResult3  VoronoiR (const float3 pos, const NoiseParams params)
	{
		#define hash( _p_ )		(_UHash33(_p_) * /*maxCeilOffset*/params.custom.x)
		_VORONOI_NOISE
		#undef hash
	}

	VoronoiResult3  VoronoiR (const float3 pos)
	{
		NoiseParams	p = CreateNoiseParams();
		p.custom.x = 1.0;	// maxCeilOffset
		return VoronoiR( pos, p );
	}

	VoronoiResult3  TileableVoronoiNoiseR (const float3 pos, const TileableNoiseParams params)
	{
		#define hash( _p_ )		(_TileUHash33(_p_) * /*maxCeilOffset*/params.custom.x)
		_VORONOI_NOISE
		#undef hash
	}

	VoronoiResult3  TileableVoronoiNoiseR (const float3 pos, const float3 tileSize)
	{
		TileableNoiseParams	p = CreateTileableNoiseParams( tileSize );
		p.custom.x = 1.0;	// maxCeilOffset
		return TileableVoronoiNoiseR( pos, p );
	}

	#undef _VORONOI_NOISE
#endif // AE_LICENSE_MIT

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
#ifdef AE_LICENSE_MIT

	#define _VORONOI_CONTOUR2																			\
		{																								\
			float2	ipoint	= Floor( pos );																\
			float2	fpoint	= Fract( pos );																\
			float2	mr;																					\
																										\
			VoronoiResult2	result;																		\
			result.minDist = float_max;																	\
																										\
			UNROLL for (int y = -1; y <= 1; ++y)														\
			UNROLL for (int x = -1; x <= 1; ++x)														\
			{																							\
				float2	ioffset	= float2( x, y );														\
				float2	offset	= hash( ipoint + ioffset );												\
				float2	vec		= offset + ioffset - fpoint;											\
				float	d		= LengthSq( vec );														\
																										\
				if ( d < result.minDist )																\
				{																						\
					result.minDist	= d;																\
					result.icenter	= ioffset;															\
					result.offset	= offset;															\
					mr				= vec;																\
				}																						\
			}																							\
																										\
			result.minDist = float_max;																	\
																										\
			UNROLL for (int y = -2; y <= 2; ++y)														\
			UNROLL for (int x = -2; x <= 2; ++x)														\
			{																							\
				float2	ioffset	= result.icenter + float2( x, y );										\
				float2	offset	= hash( ipoint + ioffset );												\
				float2	vec		= offset + ioffset - fpoint;											\
				float	d		= LengthSq( mr - vec );													\
																										\
				if ( d > 0.00001 )																		\
					result.minDist = Min( result.minDist, Dot( 0.5*(mr + vec), Normalize(vec - mr) ));	\
			}																							\
																										\
			result.icenter += ipoint;																	\
			return result;																				\
		}

	VoronoiResult2  VoronoiContourR (const float2 pos, const NoiseParams params)
	{
		#define hash( _p_ )		(_UHash22(_p_) * /*maxCeilOffset*/params.custom.x)
		_VORONOI_CONTOUR2
		#undef hash
	}

	VoronoiResult2  VoronoiContourR (const float2 pos)
	{
		NoiseParams	p = CreateNoiseParams();
		p.custom.x	= 1.0;	// maxCeilOffset
		return VoronoiContourR( pos, p );
	}

	VoronoiResult2  TileableVoronoiContourR (const float2 pos, const TileableNoiseParams params)
	{
		#define hash( _p_ )		(_TileUHash22(_p_) * /*maxCeilOffset*/params.custom.x)
		_VORONOI_CONTOUR2
		#undef hash
	}

	VoronoiResult2  TileableVoronoiContourR (const float2 pos, const float2 tileSize)
	{
		TileableNoiseParams	p = CreateTileableNoiseParams( tileSize );
		p.custom.x	= 1.0;	// maxCeilOffset
		return TileableVoronoiContourR( pos, p );
	}

	#define _VORONOI_CONTOUR3																			\
		{																								\
			float3	ipoint	= Floor( pos );																\
			float3	fpoint	= Fract( pos );																\
			float3	mr;																					\
																										\
			VoronoiResult3	result;																		\
			result.minDist = float_max;																	\
																										\
			UNROLL for (int z = -1; z <= 1; ++z)														\
			UNROLL for (int y = -1; y <= 1; ++y)														\
			UNROLL for (int x = -1; x <= 1; ++x)														\
			{																							\
				float3	ioffset	= float3( x, y, z );													\
				float3	offset	= hash( ipoint + ioffset );												\
				float3	vec		= offset + ioffset - fpoint;											\
				float	d		= LengthSq( vec );														\
																										\
				if ( d < result.minDist )																\
				{																						\
					result.minDist	= d;																\
					result.icenter	= ioffset;															\
					result.offset	= offset;															\
					mr				= vec;																\
				}																						\
			}																							\
																										\
			result.minDist = float_max;																	\
																										\
			UNROLL for (int z = -2; z <= 2; ++z)														\
			UNROLL for (int y = -2; y <= 2; ++y)														\
			UNROLL for (int x = -2; x <= 2; ++x)														\
			{																							\
				float3	ioffset	= result.icenter + float3( x, y, z );									\
				float3	offset	= hash( ipoint + ioffset );												\
				float3	vec		= offset + ioffset - fpoint;											\
				float	d		= LengthSq( mr - vec );													\
																										\
				if ( d > 0.00001 )																		\
					result.minDist = Min( result.minDist, Dot( 0.5*(mr + vec), Normalize(vec - mr) ));	\
			}																							\
																										\
			result.icenter += ipoint;																	\
			return result;																				\
		}

	VoronoiResult3  VoronoiContourR (const float3 pos, const NoiseParams params)
	{
		#define hash( _p_ )		(_UHash33(_p_) * /*maxCeilOffset*/params.custom.x)
		_VORONOI_CONTOUR3
		#undef hash
	}

	VoronoiResult3  VoronoiContourR (const float3 pos)
	{
		NoiseParams	p = CreateNoiseParams();
		p.custom.x	= 1.0;	// maxCeilOffset
		return VoronoiContourR( pos, p );
	}

	VoronoiResult3  TileableVoronoiContourR (const float3 pos, const TileableNoiseParams params)
	{
		#define hash( _p_ )		(_TileUHash33(_p_) * /*maxCeilOffset*/params.custom.x)
		_VORONOI_CONTOUR3
		#undef hash
	}

	VoronoiResult3  TileableVoronoiContourR (const float3 pos, const float3 tileSize)
	{
		TileableNoiseParams	p = CreateTileableNoiseParams( tileSize );
		p.custom.x	= 1.0;	// maxCeilOffset
		return TileableVoronoiContourR( pos, p );
	}

	#undef _VORONOI_CONTOUR2
	#undef _VORONOI_CONTOUR3
#endif // AE_LICENSE_MIT

/*
=================================================
	VoronoiCircles
----
	range [0..inf]

	based on VoronoiContour
=================================================
*/
#ifdef AE_LICENSE_MIT

	float  VoronoiCircles (const float2 pos, const NoiseParams params)
	{
		#define hash( _p_ )		(_UHash22(_p_) * /*maxCeilOffset*/params.custom.x)

		float2	ipoint	= Floor( pos );
		float2	fpoint	= Fract( pos );

		float2	icenter	= float2(0.0);
		float	md		= float_max;
		float	mr		= float_max;

		// find nearest circle
		UNROLL for (int y = -1; y <= 1; ++y)
		UNROLL for (int x = -1; x <= 1; ++x)
		{
			float2	cur	= float2(x, y);
			float2	off	= hash( cur + ipoint ) + cur - fpoint;
			float	d	= Dot( off, off );

			if ( d < md )
			{
				md = d;
				icenter = cur;
			}
		}

		// calc circle radius
		UNROLL for (int y = -2; y <= 2; ++y)
		UNROLL for (int x = -2; x <= 2; ++x)
		{
			if ( AllEqual( int2(x,y), int2(0) ))
				continue;

			float2	cur = icenter + float2(x, y);
			float2	off	= hash( cur + ipoint ) + cur - fpoint;
			float	d	= LengthSq( off );

			if ( d < mr )
				mr = d;
		}

		md = Sqrt( md );
		mr = Sqrt( mr ) * 0.5 * /*radiusScale*/params.custom.y;

		if ( md < mr )
			return 1.0 / (Square( md / mr ) * 16.0) - 0.07;

		return 0.0;
		#undef hash
	}

	float  VoronoiCircles (const float2 pos, const float radiusScale)
	{
		NoiseParams	p = CreateNoiseParams();
		p.custom.x	= 1.0;	// maxCeilOffset
		p.custom.y	= radiusScale;
		return VoronoiCircles( pos, p );
	}

#endif // AE_LICENSE_MIT

/*
=================================================
	WaveletNoise
----
	range [-1..1]

	from https://www.shadertoy.com/view/wsBfzK
	The MIT License
	Copyright (c) 2020 Martijn Steinrucken
=================================================
*/
#ifdef AE_LICENSE_MIT

	float  WaveletNoise (float2 pos, const NoiseParams params)
	{
		float d = 0.0f;
		float s = 1.0f;
		float m = 0.0f;
		float z = params.custom.x;
		float k = Max( 0.4, params.custom.y );

		UNROLL for (float i = 0.0f; i < 4.0f; ++i)
		{
			float2 q = pos * s;
			float2 g = Fract(Floor(q) * float2(123.34f, 233.53f));
			g += Dot(g, g + 23.234f);

			float a = Fract(g.x * g.y) * 1.0e+3f; // +z*(mod(g.x+g.y, 2.)-1.); // add vorticity
			q = (Fract(q) - 0.5) * float2x2(Cos(a), -Sin(a), Sin(a), Cos(a));
			d += Sin(q.x * 10.0 + z) * SmoothStep( LengthSq(q), 0.25f, 0.0f ) / s;

			pos = pos * float2x2(0.54f, -0.84f, 0.84f, 0.54f) + i;
			m += 1.0 / s;
			s *= k;
		}
		return d / m;
	}

	float  WaveletNoise (float2 pos)
	{
		NoiseParams	p = CreateNoiseParams();
		p.custom.xy = float2(0.2, 0.0);
		return WaveletNoise( pos, p );
	}

#endif // AE_LICENSE_MIT
//-----------------------------------------------------------------------------

#undef _PosTranf2D
#undef _PosTranf3D

#undef _UHash22
#undef _UHash33
#undef _SHash33
#undef _UHash13
#undef _SHash13

#undef _TileUHash22
#undef _TileUHash33
#undef _TileSHash33
#undef _TileUHash13
#undef _TileSHash13

#undef _RGBTexUHash33
#undef _RGBTexSHash33
#undef _GreyTexUHash13
#undef _GreyTexSHash13
