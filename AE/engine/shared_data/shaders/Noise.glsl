// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Noise functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Hash.glsl"


ND_ float  GradientNoise (gl::CombinedTex2D<float> rgbaNoise,
						  const float3 pos);							// range [-1..1],	MIT
ND_ float  GradientNoise (const float3 pos);							// range [-1..1],	MIT

ND_ float  IQNoise (gl::CombinedTex2D<float> rgbaNoise,
					const float3 pos,
					float2		 uv);									// range [-1..1],	MIT
ND_ float  IQNoise (const float3 pos,
					float2		 uv);									// range [-1..1],	MIT

ND_ float  IQNoise (gl::CombinedTex2D<float> rgbaNoise,
					const float3 pos);									// range [-1..1],	MIT
ND_ float  IQNoise (const float3 pos);									// range [-1..1],	MIT

ND_ float  ValueNoise (gl::CombinedTex2D<float> greyNoise,
					   const float3 pos);								// range [-1..1],	CC BY-NC-SA 3.0
ND_ float  ValueNoise (const float3 pos);								// range [-1..1],	CC BY-NC-SA 3.0

ND_ float  PerlinNoise (gl::CombinedTex2D<float> rgbaNoise,
						const float3 pos);								// range [-1..1],	CC BY-NC-SA 3.0
ND_ float  PerlinNoise (const float3 pos);								// range [-1..1],	CC BY-NC-SA 3.0

ND_ float  SimplexNoise (gl::CombinedTex2D<float> rgbaNoise,
						 const float3 pos);								// range [-1..1],	CC BY-NC-SA 3.0
ND_ float  SimplexNoise (const float3 pos);								// range [-1..1],	CC BY-NC-SA 3.0

ND_ float  WaveletNoise (float2 coord, const float2 zk);				// range [0..1],	MIT
ND_ float  WaveletNoise (float2 coord);									// range [0..1],	MIT
//-----------------------------------------------------------------------------


// 2D
struct VoronoiResult2
{
	float2	icenter;	// range: floor(coord) +-1
	float2	offset;		// ceil center = icenter + offset
	float	minDist;	// squared (?) distance in range [0..inf]
};

ND_ VoronoiResult2  Voronoi (const float2 coord,
							 const float3 seedScaleBias_offsetScale);
ND_ VoronoiResult2  Voronoi (const float2 coord);

ND_ VoronoiResult2  VoronoiContour2 (const float2 coord,
									 const float3 seedScaleBias_offsetScale);	// MIT
ND_ VoronoiResult2  VoronoiContour2 (const float2 coord);						// MIT

ND_ float  VoronoiContour3 (const float2 coord,
							const float3 seedScaleBias_offsetScale,
							const float3 hashScaleBiasOff);						// MIT
ND_ float  VoronoiContour3 (const float2 coord);								// MIT

ND_ float  VoronoiContour (const float2 coord,
						   const float3 seedScaleBias_offsetScale);				// range [0..inf],	MIT
ND_ float  VoronoiContour (const float2 coord);									// range [0..inf],	MIT

ND_ float  VoronoiCircles (const float2 coord,
						   const float  radiusScale,
						   const float3 seedScaleBias_offsetScale);				// range [0..inf],	MIT
ND_ float  VoronoiCircles (const float2 coord,
						   const float  radiusScale);							// range [0..inf],	MIT

ND_ float  WarleyNoise (const float2 pos,
						const float3 seedScaleBias_offsetScale);				// range [-inf..1]
ND_ float  WarleyNoise (const float2 pos);										// range [-inf..1]
//-----------------------------------------------------------------------------


// 3D
struct VoronoiResult3
{
	float3	icenter;	// range: floor(coord) +-1
	float3	offset;		// ceil center = icenter + offset
	float	minDist;	// squared (?) distance in range [0..inf]
};

ND_ VoronoiResult3  Voronoi (const float3 coord,
							 const float3 seedScaleBias_offsetScale);
ND_ VoronoiResult3  Voronoi (const float3 coord);

ND_ VoronoiResult3  VoronoiContour2 (const float3 coord,
									 const float3 seedScaleBias_offsetScale);	// MIT
ND_ VoronoiResult3  VoronoiContour2 (const float3 coord);						// MIT

ND_ float  VoronoiContour3 (const float3 coord,
							const float3 seedScaleBias_offsetScale,
							const float3 hashScaleBiasOff);						// MIT
ND_ float  VoronoiContour3 (const float3 coord);								// MIT

ND_ float  VoronoiContour (const float3 coord,
						   const float3 seedScaleBias_offsetScale);				// range [0..inf],	MIT
ND_ float  VoronoiContour (const float3 coord);									// range [0..inf],	MIT

ND_ float  WarleyNoise (const float3 pos,
						const float3 seedScaleBias_offsetScale);				// range [-inf..1]
ND_ float  WarleyNoise (const float3 pos);										// range [-inf..1]
//-----------------------------------------------------------------------------



// FBM
#if 0
ND_ float  ***FBM (in float3 pos, const float lacunarity, const float persistence, const int octaveCount);
ND_ float  ***FBM (gl::CombinedTex2D<float> noiseTex, in float3 pos, const float lacunarity, const float persistence, const int octaveCount);

// templates:
//	FBM_NOISE
//	FBM_NOISE_A1
//	FBM_NOISE_A2

// example:
//	FBM_NOISE( Perlin ) --> PerlinFBM
#endif
//-----------------------------------------------------------------------------


// returns position distortion
#if 0
ND_ float3 Turbulence_*** (const float3 pos);
ND_ float3 Turbulence_*** (gl::CombinedTex2D<float> noiseTex, const float3 pos);

ND_ float3 Turbulence_***FBM (const float3 pos, const float lacunarity, const float persistence, const int octaveCount);
ND_ float3 Turbulence_***FBM (gl::CombinedTex2D<float> noiseTex, const float3 pos, const float lacunarity, const float persistence, const int octaveCount);

// templates:
//	TURBULENCE
//	TURBULENCE_A1
//	TURBULENCE_FBM

// example:
//	Turbulence_GradientNoise
//	Turbulence_PerlinFBM
#endif
//-----------------------------------------------------------------------------


#include "../3party_shaders/Noise-1.glsl"
#include "../3party_shaders/Noise-2.glsl"
//-----------------------------------------------------------------------------


void  _TurbulenceTransform (const float3 pos, out float3 p0, out float3 p1, out float3 p2)
{
	p0 = pos + float3( 0.189422, 0.993713, 0.478164 );
	p1 = pos + float3( 0.404647, 0.276611, 0.923049 );
	p2 = pos + float3( 0.821228, 0.171096, 0.684280 );
}

ND_ float3  _FBMTransform (const float3 pos)
{
	#if 0
		const float3x3	rot = float3x3( 0.9017,  0.4321, -0.00746,
									   -0.3461,  0.7324,  0.5862,
										0.2588, -0.5260,  0.8100 );
		return rot * pos;
	#elif 0
		const float3x3	rot = float3x3(  0.00,  1.60,  1.20,
										-1.60,  0.72, -0.96,
										-1.20, -0.96,  1.28 );
		return rot * pos;
	#else
		const float4x3	rot = float4x3( 0.9017,  0.4321, -0.00746,
									   -0.3461,  0.7324,  0.5862,
										0.2588, -0.5260,  0.8100,
										0.4781, -0.8212,  0.1710 );
		return rot * float4(pos, 0.0);
	#endif
}

ND_ float2  _FBMTransform (const float2 pos)
{
	#if 0
		const float2x2	rot = float2x2( 0.838,  0.544,	// 33 deg
									   -0.544,  0.838 );
		return rot * pos;
	#else
		const float3x2	rot = float3x2( 0.838,  0.544,	// 33 deg
									   -0.544,  0.838,
										0.276, -0.404 );
		return rot * float3(pos, 0.0);
	#endif
}
//-----------------------------------------------------------------------------


#define _FBM_NOISE2( _noise_ )									\
	{															\
		float	value	= 0.0;									\
		float	pers	= 1.0;									\
		float	scale	= 1.0;									\
																\
		for (int octave = 0; octave < octaveCount; ++octave)	\
		{														\
			value += (_noise_) * pers;							\
			scale += pers;										\
			pos    = _FBMTransform( pos * lacunarity );			\
			pers  *= persistence;								\
		}														\
		return value / scale;									\
	}
//-----------------------------------------------------------------------------


#define FBM_NOISE_A1_Hash( _noise_, _argType_, _argName_ )													\
	ND_ float  _noise_##FBM (in float3 pos, const _argType_ _argName_,										\
							 const float lacunarity, const float persistence, const int octaveCount) {		\
		_FBM_NOISE2( _noise_( pos, _argName_ ))																\
	}

#define FBM_NOISE_A1_Tex( _noise_, _argType_, _argName_ )													\
	ND_ float  _noise_##FBM (gl::CombinedTex2D<float> noiseTex, in float3 pos, const _argType_ _argName_,	\
							  const float lacunarity, const float persistence, const int octaveCount) {		\
		_FBM_NOISE2( _noise_( noiseTex, pos, _argName_ ))													\
	}

#define FBM_NOISE_A1( _noise_, _argType_, _argName_ )\
	FBM_NOISE_A1_Hash( _noise_, _argType_, _argName_ )\
	FBM_NOISE_A1_Tex( _noise_, _argType_, _argName_ )
//-----------------------------------------------------------------------------


#define FBM_NOISE_A2_Hash( _noise_, _arg1Type_, _arg1Name_, _arg2Type_, _arg2Name_ )						\
	ND_ float  _noise_##FBM (in float3 pos, const _arg1Type_ _arg1Name_, const _arg2Type_ _arg2Name_,		\
							 const float lacunarity, const float persistence, const int octaveCount) {		\
		_FBM_NOISE2( _noise_( pos, _arg1Name_, _arg2Name_ ))												\
	}

#define FBM_NOISE_A2_Tex( _noise_, _arg1Type_, _arg1Name_, _arg2Type_, _arg2Name_ )							\
	ND_ float  _noise_##FBM (gl::CombinedTex2D<float> noiseTex, in float3 pos,								\
							 const _arg1Type_ _arg1Name_, const _arg2Type_ _arg2Name_,						\
							 const float lacunarity, const float persistence, const int octaveCount) {		\
		_FBM_NOISE2( _noise_( noiseTex, pos, _arg1Name_, _arg2Name_ ))										\
	}

#define FBM_NOISE_A2( _noise_, _arg1Type_, _arg1Name_, _arg2Type_, _arg2Name_ )\
	FBM_NOISE_A2_Hash( _noise_, _arg1Type_, _arg1Name_, _arg2Type_, _arg2Name_ )\
	FBM_NOISE_A2_Tex( _noise_, _arg1Type_, _arg1Name_, _arg2Type_, _arg2Name_ )
//-----------------------------------------------------------------------------


#define FBM_NOISE_Hash( _noise_ )																						\
	ND_ float  _noise_##FBM (in float3 pos, const float lacunarity, const float persistence, const int octaveCount)	{	\
		_FBM_NOISE2( _noise_( pos ))																					\
	}

#define FBM_NOISE_Tex( _noise_ )																						\
	ND_ float  _noise_##FBM (gl::CombinedTex2D<float> noiseTex, in float3 pos,											\
							 const float lacunarity, const float persistence, const int octaveCount) {					\
		_FBM_NOISE2( _noise_( pos ))																					\
	}

#define FBM_NOISE( _noise_ )\
	FBM_NOISE_Hash( _noise_ )\
	FBM_NOISE_Tex( _noise_ )
//-----------------------------------------------------------------------------


// returns position distortion
#define TURBULENCE_Hash( _noise_ )																										\
	ND_ float3  Turbulence_##_noise_ (const float3 pos)																					\
	{																																	\
		float3 p0, p1, p2;																												\
		_TurbulenceTransform( pos, p0, p1, p2 );																						\
		const float3 distort = float3(_noise_( p0 ),																					\
									  _noise_( p1 ),																					\
									  _noise_( p2 ));																					\
		return distort;																													\
	}																																	\

#define TURBULENCE_Tex( _noise_ )																										\
	ND_ float3  Turbulence_##_noise_ (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)												\
	{																																	\
		float3 p0, p1, p2;																												\
		_TurbulenceTransform( pos, p0, p1, p2 );																						\
		const float3 distort = float3(_noise_( rgbaNoise, p0 ),																			\
									  _noise_( rgbaNoise, p1 ),																			\
									  _noise_( rgbaNoise, p2 ));																		\
		return distort;																													\
	}

#define TURBULENCE( _noise_ )\
	TURBULENCE_Hash( _noise_ )\
	TURBULENCE_Tex( _noise_ )


#define TURBULENCE_A1_Hash( _noise_, _argType_, _argName_ )																				\
	ND_ float3  Turbulence_##_noise_ (const float3 pos, const _argType_ _argName_)														\
	{																																	\
		float3 p0, p1, p2;																												\
		_TurbulenceTransform( pos, p0, p1, p2 );																						\
		const float3 distort = float3(_noise_( p0, _argName_ ),																			\
									  _noise_( p1, _argName_ ),																			\
									  _noise_( p2, _argName_ ));																		\
		return distort;																													\
	}																																	\

#define TURBULENCE_A1_Tex( _noise_, _argType_, _argName_ )																				\
	ND_ float3  Turbulence_##_noise_ (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const _argType_ _argName_)					\
	{																																	\
		float3 p0, p1, p2;																												\
		_TurbulenceTransform( pos, p0, p1, p2 );																						\
		const float3 distort = float3(_noise_( rgbaNoise, p0, _argName_ ),																\
									  _noise_( rgbaNoise, p1, _argName_ ),																\
									  _noise_( rgbaNoise, p2, _argName_ ));																\
		return distort;																													\
	}

#define TURBULENCE_A1( _noise_, _argType_, _argName_ )\
	TURBULENCE_A1_Hash( _noise_, _argType_, _argName_ )\
	TURBULENCE_A1_Tex( _noise_, _argType_, _argName_ )


#define TURBULENCE_FBM_Hash( _noise_ )																									\
	ND_ float3  Turbulence_##_noise_ (const float3 pos, const float lacunarity, const float persistence, const int octaveCount)			\
	{																																	\
		float3 p0, p1, p2;																												\
		_TurbulenceTransform( pos, p0, p1, p2 );																						\
		const float3 distort = float3(_noise_( p0, lacunarity, persistence, octaveCount ),												\
									  _noise_( p1, lacunarity, persistence, octaveCount ),												\
									  _noise_( p2, lacunarity, persistence, octaveCount ));												\
		return distort;																													\
	}																																	\

#define TURBULENCE_FBM_Tex( _noise_ )																									\
	ND_ float3  Turbulence_##_noise_ (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const float lacunarity,						\
										const float persistence, const int octaveCount)													\
	{																																	\
		float3 p0, p1, p2;																												\
		_TurbulenceTransform( pos, p0, p1, p2 );																						\
		const float3 distort = float3(_noise_( rgbaNoise, p0, lacunarity, persistence, octaveCount ),									\
									  _noise_( rgbaNoise, p1, lacunarity, persistence, octaveCount ),									\
									  _noise_( rgbaNoise, p2, lacunarity, persistence, octaveCount ));									\
		return distort;																													\
	}

#define TURBULENCE_FBM( _noise_ )\
	TURBULENCE_FBM_Hash( _noise_ )\
	TURBULENCE_FBM_Tex( _noise_ )
//-----------------------------------------------------------------------------


/*
=================================================
	Voronoi
=================================================
*/
VoronoiResult2  Voronoi (const float2 coord, const float3 seedScaleBias_offsetScale)
{
	float2	ipoint	= Floor( coord );
	float2	fpoint	= Fract( coord );

	VoronoiResult2	result;
	result.minDist = float_max;

	for (int y = -1; y <= 1; ++y)
	for (int x = -1; x <= 1; ++x)
	{
		float2	ioffset	= float2( x, y );
		float2	offset	= DHash22( (ipoint + ioffset) * seedScaleBias_offsetScale.x + seedScaleBias_offsetScale.y ) * seedScaleBias_offsetScale.z;
		float2	vec		= offset + ioffset - fpoint;
		float	d		= LengthSq( vec );

		if ( d < result.minDist )
		{
			result.minDist	= d;
			result.icenter		= ipoint + ioffset;
			result.offset		= offset;
		}
	}
	return result;
}

VoronoiResult2  Voronoi (const float2 coord)
{
	return Voronoi( coord, float3(1.0, 0.0, 1.0) );
}

VoronoiResult3  Voronoi (const float3 coord, const float3 seedScaleBias_offsetScale)
{
	float3	ipoint	= Floor( coord );
	float3	fpoint	= Fract( coord );

	VoronoiResult3	result;
	result.minDist = float_max;

	for (int z = -1; z <= 1; ++z)
	for (int y = -1; y <= 1; ++y)
	for (int x = -1; x <= 1; ++x)
	{
		float3	ioffset	= float3( x, y, z );
		float3	offset	= DHash33( (ipoint + ioffset) * seedScaleBias_offsetScale.x + seedScaleBias_offsetScale.y ) * seedScaleBias_offsetScale.z;
		float3	vec		= offset + ioffset - fpoint;
		float	d		= LengthSq( vec );

		if ( d < result.minDist )
		{
			result.minDist	= d;
			result.icenter		= ipoint + ioffset;
			result.offset		= offset;
		}
	}
	return result;
}

VoronoiResult3  Voronoi (const float3 coord)
{
	return Voronoi( coord, float3(1.0, 0.0, 1.0) );
}

/*
=================================================
	WarleyNoise
----
	range [-inf..1]
=================================================
*/
float  WarleyNoise (const float3 pos, const float3 seedScaleBias_offsetScale)
{
	return 1.0 - Voronoi( pos, seedScaleBias_offsetScale ).minDist;
}

float  WarleyNoise (const float3 pos)
{
	return 1.0 - Voronoi( pos ).minDist;
}

float  WarleyNoise (const float2 pos, const float3 seedScaleBias_offsetScale)
{
	return 1.0 - Voronoi( pos, seedScaleBias_offsetScale ).minDist;
}

float  WarleyNoise (const float2 pos)
{
	return 1.0 - Voronoi( pos ).minDist;
}

/*
=================================================
	VoronoiContour2
=================================================
*/
VoronoiResult2  VoronoiContour2 (const float2 coord)
{
	return VoronoiContour2( coord, float3(1.0, 0.0, 1.0) );
}

float  VoronoiContour (const float2 coord, const float3 seedScaleBias_offsetScale)
{
	return VoronoiContour2( coord, seedScaleBias_offsetScale ).minDist;
}

float  VoronoiContour (const float2 coord)
{
	return VoronoiContour2( coord, float3(1.0, 0.0, 1.0) ).minDist;
}

VoronoiResult3  VoronoiContour2 (const float3 coord)
{
	return VoronoiContour2( coord, float3(1.0, 0.0, 1.0) );
}

float  VoronoiContour (const float3 coord, const float3 seedScaleBias_offsetScale)
{
	return VoronoiContour2( coord, seedScaleBias_offsetScale ).minDist;
}

float  VoronoiContour (const float3 coord)
{
	return VoronoiContour2( coord, float3(1.0, 0.0, 1.0) ).minDist;
}

/*
=================================================
	VoronoiContour3
=================================================
*/
float  VoronoiContour3 (const float2 coord, const float3 seedScaleBias_offsetScale, const float3 hashScaleBiasOff)
{
	VoronoiResult2	r = VoronoiContour2( coord, seedScaleBias_offsetScale );
	float			d = r.minDist;

	d *= Sign( DHash12( (r.icenter + r.offset) * hashScaleBiasOff.x + hashScaleBiasOff.y ) - hashScaleBiasOff.z );
	return d;
}

float  VoronoiContour3 (const float2 coord)
{
	return VoronoiContour3( coord, float3(1.0, 0.0, 1.0), float3(1.0, 0.0, 0.5) );
}

float  VoronoiContour3 (const float3 coord, const float3 seedScaleBias_offsetScale, const float3 hashScaleBiasOff)
{
	VoronoiResult3	r = VoronoiContour2( coord, seedScaleBias_offsetScale );
	float			d = r.minDist;

	d *= Sign( DHash13( (r.icenter + r.offset) * hashScaleBiasOff.x + hashScaleBiasOff.y ) - hashScaleBiasOff.z );
	return d;
}

float  VoronoiContour3 (const float3 coord)
{
	return VoronoiContour3( coord, float3(1.0, 0.0, 1.0), float3(1.0, 0.0, 0.5) );
}

