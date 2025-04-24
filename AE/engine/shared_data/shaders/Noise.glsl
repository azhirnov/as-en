// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Noise functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Hash.glsl"


struct NoiseParams
{
	float4	custom;			// custom params specific for each noise type
	float3	seedScale;		// scale position before applying hash
	float3	seedBias;		// offset position before applying hash
};
ND_ NoiseParams  CreateNoiseParams ();


struct TileableNoiseParams
{
	float4	custom;			// custom params specific for each noise type
	float3	seedScale;		// scale position before applying hash
	float3	seedBias;		// offset position before applying hash
	float3	tileSize;
};
ND_ TileableNoiseParams  CreateTileableNoiseParams (float2 tileSize);
ND_ TileableNoiseParams  CreateTileableNoiseParams (float3 tileSize);


struct FBMParams
{
	float	lacunarity;
	float	persistence;
	int		octaveCount;
};
ND_ FBMParams  CreateFBMParams (float lacunarity, float persistence, int octaveCount);
//-----------------------------------------------------------------------------


// range [-1..1],	MIT
ND_ float  GradientNoise (gl::CombinedTex2D<float> rgbaNoise,	const float3 pos,	const NoiseParams params);
ND_ float  GradientNoise (gl::CombinedTex2D<float> rgbaNoise,	const float3 pos);
ND_ float  GradientNoise (const float3 pos,		const NoiseParams params);
ND_ float  GradientNoise (const float3 pos);

// range [0..1],	MIT
// custom:
//		float	u;	// in range [-0.5, 0.5]
//		float	v;	// in range [0.4, 1.0]
ND_ float  IQNoise (gl::CombinedTex2D<float> rgbaNoise,		const float3 pos,	const NoiseParams params);
ND_ float  IQNoise (gl::CombinedTex2D<float> rgbaNoise,		const float3 pos);
ND_ float  IQNoise (const float3 pos,	const NoiseParams params);
ND_ float  IQNoise (const float3 pos);

// range [-1..1],	CC BY-NC-SA 3.0
ND_ float  ValueNoise (gl::CombinedTex2D<float> greyNoise,	const float3 pos,	const NoiseParams params);
ND_ float  ValueNoise (gl::CombinedTex2D<float> greyNoise,	const float3 pos);
ND_ float  ValueNoise (const float3 pos,	const NoiseParams params);
ND_ float  ValueNoise (const float3 pos);

// range [-1..1],	CC BY-NC-SA 3.0
ND_ float  PerlinNoise (gl::CombinedTex2D<float> rgbaNoise,		const float3 pos,	const NoiseParams params);
ND_ float  PerlinNoise (gl::CombinedTex2D<float> rgbaNoise,		const float3 pos);
ND_ float  PerlinNoise (const float3 pos,	const NoiseParams params);
ND_ float  PerlinNoise (const float3 pos);

// range [-1..1],	MIT
ND_ float  SimplexNoise (gl::CombinedTex2D<float> rgbaNoise,	const float3 pos,	const NoiseParams params);
ND_ float  SimplexNoise (gl::CombinedTex2D<float> rgbaNoise,	const float3 pos);
ND_ float  SimplexNoise (const float3 pos,	const NoiseParams params);
ND_ float  SimplexNoise (const float3 pos);

// range [0..1],	MIT
// custom:
//		float	z;	// ???
//		float	k;	// ???
ND_ float  WaveletNoise (float2 pos,	const NoiseParams params);
ND_ float  WaveletNoise (float2 pos);
//-----------------------------------------------------------------------------


// Voronoi 2D //

struct VoronoiResult2
{
	float2	icenter;	// range: floor(pos) +-1
	float2	offset;		// ceil center = icenter + offset
	float	minDist;	// squared (?) distance in range [0..inf]
};

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ VoronoiResult2  VoronoiR (const float2 pos,	const NoiseParams params);
ND_ VoronoiResult2  VoronoiR (const float2 pos);

// range [0..inf],	MIT
ND_ float  Voronoi (const float2 pos,	const NoiseParams params);
ND_ float  Voronoi (const float2 pos);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ VoronoiResult2  VoronoiContourR (const float2 pos,	const NoiseParams params);
ND_ VoronoiResult2  VoronoiContourR (const float2 pos);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
//		float3	hashScaleBiasOff
ND_ float  VoronoiContourSparse (const float2 pos,	const NoiseParams params);
ND_ float  VoronoiContourSparse (const float2 pos);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ float  VoronoiContour (const float2 pos,	const NoiseParams params);
ND_ float  VoronoiContour (const float2 pos);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
//		float	radiusScale;
ND_ float  VoronoiCircles (const float2 pos,	const NoiseParams params);
ND_ float  VoronoiCircles (const float2 pos,	const float radiusScale);

// range [-inf..1],	MIT
// custom:
//		float	maxCeilOffset;
ND_ float  WarleyNoise (const float2 pos,	const NoiseParams params);
ND_ float  WarleyNoise (const float2 pos);
//-----------------------------------------------------------------------------


// Voronoi 3D //

struct VoronoiResult3
{
	float3	icenter;	// range: floor(pos) +-1
	float3	offset;		// ceil center = icenter + offset
	float	minDist;	// squared (?) distance in range [0..inf]
};

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ VoronoiResult3  VoronoiR (const float3 pos,	const NoiseParams params);
ND_ VoronoiResult3  VoronoiR (const float3 pos);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ float  Voronoi (const float3 pos,	const NoiseParams params);
ND_ float  Voronoi (const float3 pos);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ VoronoiResult3  VoronoiContourR (const float3 pos,	const NoiseParams params);
ND_ VoronoiResult3  VoronoiContourR (const float3 pos);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
//		float3	hashScaleBiasOff;
ND_ float  VoronoiContourSparse (const float3 pos,	const NoiseParams params);
ND_ float  VoronoiContourSparse (const float3 pos);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ float  VoronoiContour (const float3 pos,	const NoiseParams params);
ND_ float  VoronoiContour (const float3 pos);

// range [-inf..1],	MIT
// custom:
//		float	maxCeilOffset;
ND_ float  WarleyNoise (const float3 pos,	const NoiseParams params);
ND_ float  WarleyNoise (const float3 pos);
//-----------------------------------------------------------------------------


// Tileable Voronoi 2D //

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ VoronoiResult2  TileableVoronoiNoiseR (const float2 pos,	const TileableNoiseParams params);
ND_ VoronoiResult2  TileableVoronoiNoiseR (const float2 pos,	const float2 tileSize);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ float  TileableVoronoiNoise (const float2 pos,	const TileableNoiseParams params);
ND_ float  TileableVoronoiNoise (const float2 pos,	const float2 tileSize);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ VoronoiResult2  TileableVoronoiContourR (const float2 pos,	const TileableNoiseParams params);
ND_ VoronoiResult2  TileableVoronoiContourR (const float2 pos,	const float2 tileSize);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ float  TileableVoronoiContour (const float2 pos,	const TileableNoiseParams params);
ND_ float  TileableVoronoiContour (const float2 pos,	const float2 tileSize);

// range [-inf..1],	MIT
// custom:
//		float	maxCeilOffset;
ND_ float  TileableWarleyNoise (const float2 pos,	const TileableNoiseParams params);
ND_ float  TileableWarleyNoise (const float2 pos,	const float2 tileSize);
//---------------------------


// Tileable Voronoi 3D //

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ VoronoiResult3  TileableVoronoiNoiseR (const float3 pos,	const TileableNoiseParams params);
ND_ VoronoiResult3  TileableVoronoiNoiseR (const float3 pos,	const float3 tileSize);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ float  TileableVoronoiNoise (const float3 pos,	const TileableNoiseParams params);
ND_ float  TileableVoronoiNoise (const float3 pos,	const float3 tileSize);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ VoronoiResult3  TileableVoronoiContourR (const float3 pos,	const TileableNoiseParams params);
ND_ VoronoiResult3  TileableVoronoiContourR (const float3 pos,	const float3 tileSize);

// range [0..inf],	MIT
// custom:
//		float	maxCeilOffset;
ND_ float  TileableVoronoiContour (const float3 pos,	const TileableNoiseParams params);
ND_ float  TileableVoronoiContour (const float3 pos,	const float3 tileSize);

// range [-inf..1],	MIT
// custom:
//		float	maxCeilOffset;
ND_ float  TileableWarleyNoise (const float3 pos,	const TileableNoiseParams params);
ND_ float  TileableWarleyNoise (const float3 pos,	const float3 tileSize);
//---------------------------


// range [-1..1],	MIT
ND_ float  TileableGradientNoise (const float3 pos,		const TileableNoiseParams params);
ND_ float  TileableGradientNoise (const float3 pos,		const float3 tileSize);

// range [0..1],	MIT
ND_ float  TileableIQNoise (const float3 pos,	const TileableNoiseParams params);
ND_ float  TileableIQNoise (const float3 pos,	const float3 tileSize);

// range [-1..1],	CC BY-NC-SA 3.0
ND_ float  TileableValueNoise (const float3 pos,	const TileableNoiseParams params);
ND_ float  TileableValueNoise (const float3 pos,	const float3 tileSize);

// range [-1..1],	CC BY-NC-SA 3.0
ND_ float  TileablePerlinNoise (const float3 pos,	const TileableNoiseParams params);
ND_ float  TileablePerlinNoise (const float3 pos,	const float3 tileSize);
//-----------------------------------------------------------------------------



// FBM
#if 0
ND_ float  ***FBM (in float3 pos, const FBMParams fbm);
ND_ float  ***FBM (gl::CombinedTex2D<float> noiseTex, in float3 pos, const FBMParams fbm);

ND_ float  ***FBM (in float3 pos, const NoiseParams params, const FBMParams fbm);
ND_ float  ***FBM (gl::CombinedTex2D<float> noiseTex, in float3 pos, const NoiseParams params, const FBMParams fbm);

// templates:
//	FBM_NOISE
//	FBM_NOISE_Hash
//	FBM_NOISE_Tex

// example:
//	FBM_NOISE( Perlin ) --> PerlinFBM
#endif
//-----------------------------------------------------------------------------


// Tileable FBM
#if 0
ND_ float  Tileable***FBM (in float3 pos, const TileableNoiseParams params, const FBMParams fbm);

// templates:
//	FBM_TILE_NOISE
//	FBM_TILE_NOISE_Hash
#endif
//-----------------------------------------------------------------------------


// returns position distortion
#if 0
ND_ float3 Turbulence_*** (const float3 pos);
ND_ float3 Turbulence_*** (gl::CombinedTex2D<float> noiseTex, const float3 pos);

ND_ float3 Turbulence_***FBM (const float3 pos, const FBMParams fbm);
ND_ float3 Turbulence_***FBM (gl::CombinedTex2D<float> noiseTex, const float3 pos, const FBMParams fbm);

// templates:
//	TURBULENCE
//	TURBULENCE_Hash
//	TURBULENCE_Tex
//	TURBULENCE_FBM
//	TURBULENCE_FBM_Hash
//	TURBULENCE_FBM_Tex
//	TURBULENCE2D_Hash
//	TURBULENCE2D_FBM_Hash

// example:
//	Turbulence_GradientNoise
//	Turbulence_PerlinFBM
#endif
//-----------------------------------------------------------------------------

#ifndef UNROLL
#	define UNROLL	// [[unroll]]
#endif


#include "../3party_shaders/Noise-1.glsl"
#include "../3party_shaders/Noise-2.glsl"
//-----------------------------------------------------------------------------


void  _TurbulenceTransform (const float3 pos, out float3 p0, out float3 p1, out float3 p2)
{
	p0 = pos + float3( 0.189422, 0.993713, 0.478164 );
	p1 = pos + float3( 0.404647, 0.276611, 0.923049 );
	p2 = pos + float3( 0.821228, 0.171096, 0.684280 );
}

void  _TurbulenceTransform2D (const float2 pos, out float2 p0, out float2 p1)
{
	p0 = pos + float2( 0.189422, 0.993713 );
	p1 = pos + float2( 0.404647, 0.276611 );
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
		const float2x2	rot = float2x2( 0.838,  0.544,		// 33 deg
									   -0.544,  0.838 );
		return rot * pos;
	#else
		const float3x2	rot = float3x2( 0.838,  0.544,		// 33 deg
									   -0.544,  0.838,
										0.276, -0.404 );	// offset
		return rot * float3(pos, 0.0);
	#endif
}
//-----------------------------------------------------------------------------


#define _FBM_NOISE2( _noise_ )											\
	{																	\
		float	value	= 0.0;											\
		float	pers	= 1.0;											\
		float	scale	= fbm.octaveCount < 1 ? 1.0 : 0.0;				\
																		\
		UNROLL for (int octave = 0; octave < fbm.octaveCount; ++octave)	\
		{																\
			value += (_noise_) * pers;									\
			scale += pers;												\
			pos    = _FBMTransform( pos * fbm.lacunarity );				\
			pers  *= fbm.persistence;									\
		}																\
		return value / scale;											\
	}

#define FBM_NOISE_Hash( _noise_ )																\
	ND_ float  _noise_##FBM (in float3 pos, const NoiseParams params, const FBMParams fbm) {	\
		_FBM_NOISE2( _noise_( pos, params ))													\
	}																							\
																								\
	ND_ float  _noise_##FBM (in float3 pos, const FBMParams fbm) {								\
		_FBM_NOISE2( _noise_( pos ))															\
	}

#define FBM_NOISE2D_Hash( _noise_ )																\
	ND_ float  _noise_##FBM (in float2 pos, const NoiseParams params, const FBMParams fbm) {	\
		_FBM_NOISE2( _noise_( pos, params ))													\
	}																							\
																								\
	ND_ float  _noise_##FBM (in float2 pos, const FBMParams fbm) {								\
		_FBM_NOISE2( _noise_( pos ))															\
	}

#define FBM_NOISE_Tex( _noise_ )																	\
	ND_ float  _noise_##FBM (gl::CombinedTex2D<float> noiseTex, in float3 pos,						\
							 const NoiseParams params, const FBMParams fbm) {						\
		_FBM_NOISE2( _noise_( noiseTex, pos, params ))												\
	}																								\
																									\
	ND_ float  _noise_##FBM (gl::CombinedTex2D<float> noiseTex, in float3 pos, const FBMParams fbm){\
		_FBM_NOISE2( _noise_( noiseTex, pos ))														\
	}

#define FBM_NOISE( _noise_ )\
	FBM_NOISE_Hash( _noise_ )\
	FBM_NOISE_Tex( _noise_ )
//-----------------------------------------------------------------------------


#define _FBM_TILE_NOISE2( _noise_ )													\
	{																				\
		float	value	= 0.0;														\
		float	pers	= 1.0;														\
		float	scale	= fbm.octaveCount < 1 ? 1.0 : 0.0;							\
																					\
		fbm.lacunarity = Max( 1.0, Round( fbm.lacunarity ));						\
																					\
		UNROLL for (int octave = 0; octave < fbm.octaveCount; ++octave)				\
		{																			\
			value			 += (_noise_) * pers;									\
			scale			 += pers;												\
			params.tileSize	 *= fbm.lacunarity;										\
			pos				  = pos * fbm.lacunarity +								\
								params.tileSize * float3(-0.1710, 0.4781, 0.8212);	\
			params.seedScale += float3( 0.3461, 0.7324, 0.5862 );					\
			params.seedBias	  = _FBMTransform( params.seedBias * fbm.lacunarity );	\
			pers			 *= fbm.persistence;									\
		}																			\
		return value / scale;														\
	}


#define FBM_TILE_NOISE_Hash( _noise_ )														\
	ND_ float  _noise_##FBM (in float3 pos, in TileableNoiseParams params, FBMParams fbm) {	\
		_FBM_TILE_NOISE2( _noise_( pos, params ))											\
	}

#define FBM_TILE_NOISE( _noise_ )\
	FBM_TILE_NOISE_Hash( _noise_ )
	// TODO: FBM_TILE_NOISE_Tex
//-----------------------------------------------------------------------------


// returns position distortion
#define TURBULENCE_Hash( _noise_ )																						\
	ND_ float3  Turbulence_##_noise_ (const float3 pos, const NoiseParams params)										\
	{																													\
		float3 p0, p1, p2;																								\
		_TurbulenceTransform( pos, OUT p0, OUT p1, OUT p2 );															\
		return float3(_noise_( p0, params ), _noise_( p1, params ), _noise_( p2, params ));								\
	}																													\
																														\
	ND_ float3  Turbulence_##_noise_ (const float3 pos)																	\
	{																													\
		float3 p0, p1, p2;																								\
		_TurbulenceTransform( pos, OUT p0, OUT p1, OUT p2 );															\
		return float3(_noise_( p0 ), _noise_( p1 ), _noise_( p2 ));														\
	}																													\

#define TURBULENCE2D_Hash( _noise_ )																					\
	ND_ float2  Turbulence_##_noise_ (const float2 pos, const NoiseParams params)										\
	{																													\
		float2 p0, p1;																									\
		_TurbulenceTransform2D( pos, OUT p0, OUT p1 );																	\
		const float2 distort = float2( _noise_( p0, params ), _noise_( p1, params ));									\
		return distort;																									\
	}																													\
																														\
	ND_ float2  Turbulence_##_noise_ (const float2 pos)																	\
	{																													\
		float2 p0, p1;																									\
		_TurbulenceTransform2D( pos, OUT p0, OUT p1 );																	\
		const float2 distort = float2(_noise_( p0 ), _noise_( p1 ));													\
		return distort;																									\
	}																													\

#define TURBULENCE_Tex( _noise_ )																						\
	ND_ float3  Turbulence_##_noise_ (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const NoiseParams params)	\
	{																													\
		float3 p0, p1, p2;																								\
		_TurbulenceTransform( pos, OUT p0, OUT p1, OUT p2 );															\
		const float3 distort = float3(_noise_( rgbaNoise, p0, params ),													\
									  _noise_( rgbaNoise, p1, params ),													\
									  _noise_( rgbaNoise, p2, params ));												\
		return distort;																									\
	}																													\
																														\
	ND_ float3  Turbulence_##_noise_ (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)								\
	{																													\
		float3 p0, p1, p2;																								\
		_TurbulenceTransform( pos, OUT p0, OUT p1, OUT p2 );															\
		const float3 distort = float3(_noise_( rgbaNoise, p0 ),															\
									  _noise_( rgbaNoise, p1 ),															\
									  _noise_( rgbaNoise, p2 ));														\
		return distort;																									\
	}

#define TURBULENCE( _noise_ )\
	TURBULENCE_Hash( _noise_ )\
	TURBULENCE_Tex( _noise_ )


#define TURBULENCE_FBM_Hash( _noise_ )																			\
	ND_ float3  Turbulence_##_noise_ (const float3 pos, const NoiseParams params, const FBMParams fbm)			\
	{																											\
		float3 p0, p1, p2;																						\
		_TurbulenceTransform( pos, OUT p0, OUT p1, OUT p2 );													\
		const float3 distort = float3(_noise_( p0, params, fbm ),												\
									  _noise_( p1, params, fbm ),												\
									  _noise_( p2, params, fbm ));												\
		return distort;																							\
	}																											\
																												\
	ND_ float3  Turbulence_##_noise_ (const float3 pos, const FBMParams fbm)									\
	{																											\
		float3 p0, p1, p2;																						\
		_TurbulenceTransform( pos, OUT p0, OUT p1, OUT p2 );													\
		const float3 distort = float3(_noise_( p0, fbm ),														\
									  _noise_( p1, fbm ),														\
									  _noise_( p2, fbm ));														\
		return distort;																							\
	}																											\

#define TURBULENCE2D_FBM_Hash( _noise_ )																		\
	ND_ float2  Turbulence_##_noise_ (const float2 pos, const NoiseParams params, const FBMParams fbm)			\
	{																											\
		float2 p0, p1;																							\
		_TurbulenceTransform2D( pos, OUT p0, OUT p1 );															\
		return float2(_noise_( p0, params, fbm ), _noise_( p1, params, fbm ));									\
	}																											\
																												\
	ND_ float2  Turbulence_##_noise_ (const float2 pos, const FBMParams fbm)									\
	{																											\
		float2 p0, p1;																							\
		_TurbulenceTransform2D( pos, OUT p0, OUT p1 );															\
		return float2(_noise_( p0, fbm ), _noise_( p1, fbm ));													\
	}																											\

#define TURBULENCE_FBM_Tex( _noise_ )																			\
	ND_ float3  Turbulence_##_noise_ (gl::CombinedTex2D<float> rgbaNoise, const float3 pos,						\
									  const NoiseParams params,	const FBMParams fbm)							\
	{																											\
		float3 p0, p1, p2;																						\
		_TurbulenceTransform( pos, OUT p0, OUT p1, OUT p2 );													\
		const float3 distort = float3(_noise_( rgbaNoise, p0, params, fbm ),									\
									  _noise_( rgbaNoise, p1, params, fbm ),									\
									  _noise_( rgbaNoise, p2, params, fbm ));									\
		return distort;																							\
	}																											\
																												\
	ND_ float3  Turbulence_##_noise_ (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const FBMParams fbm)\
	{																											\
		float3 p0, p1, p2;																						\
		_TurbulenceTransform( pos, OUT p0, OUT p1, OUT p2 );													\
		const float3 distort = float3(_noise_( rgbaNoise, p0, fbm ),											\
									  _noise_( rgbaNoise, p1, fbm ),											\
									  _noise_( rgbaNoise, p2, fbm ));											\
		return distort;																							\
	}

#define TURBULENCE_FBM( _noise_ )\
	TURBULENCE_FBM_Hash( _noise_ )\
	TURBULENCE_FBM_Tex( _noise_ )
//-----------------------------------------------------------------------------


/*
=================================================
	CreateNoiseParams
=================================================
*/
NoiseParams  CreateNoiseParams ()
{
	NoiseParams	p;
	p.custom	= float4(-float_max);	// user must override it
	p.seedScale	= float3(1.0);
	p.seedBias	= float3(0.0);
	return p;
}

/*
=================================================
	CreateFBMParams
=================================================
*/
FBMParams  CreateFBMParams (float lacunarity, float persistence, int octaveCount)
{
	FBMParams	p;
	p.lacunarity	= lacunarity;
	p.persistence	= persistence;
	p.octaveCount	= octaveCount;
	return p;
}

#ifdef AE_LICENSE_MIT
/*
=================================================
	Voronoi
----
	range [0..inf]
=================================================
*/
float  Voronoi (const float2 pos, const NoiseParams params)
{
	return VoronoiR( pos, params ).minDist;
}

float  Voronoi (const float2 pos)
{
	return VoronoiR( pos ).minDist;
}

float  Voronoi (const float3 pos, const NoiseParams params)
{
	return VoronoiR( pos, params ).minDist;
}

float  Voronoi (const float3 pos)
{
	return VoronoiR( pos ).minDist;
}

/*
=================================================
	WarleyNoise
----
	range [-inf..1]
=================================================
*/
float  WarleyNoise (const float3 pos, const NoiseParams params)
{
	return 1.0 - Voronoi( pos, params );
}

float  WarleyNoise (const float3 pos)
{
	return 1.0 - Voronoi( pos );
}

float  WarleyNoise (const float2 pos, const NoiseParams params)
{
	return 1.0 - Voronoi( pos, params );
}

float  WarleyNoise (const float2 pos)
{
	return 1.0 - Voronoi( pos );
}

/*
=================================================
	VoronoiContour
=================================================
*/
float  VoronoiContour (const float2 pos, const NoiseParams params)
{
	return VoronoiContourR( pos, params ).minDist;
}

float  VoronoiContour (const float2 pos)
{
	return VoronoiContourR( pos ).minDist;
}

float  VoronoiContour (const float3 pos, const NoiseParams params)
{
	return VoronoiContourR( pos, params ).minDist;
}

float  VoronoiContour (const float3 pos)
{
	return VoronoiContourR( pos ).minDist;
}

/*
=================================================
	VoronoiContourSparse
=================================================
*/
float  VoronoiContourSparse (const float2 pos, const NoiseParams params)
{
	float3			hashScaleBiasOff = params.custom.yzw;
	VoronoiResult2	r = VoronoiContourR( pos, params );
	float			d = r.minDist;

	d *= Sign( DHash12( (r.icenter + r.offset) * hashScaleBiasOff.x + hashScaleBiasOff.y ) - hashScaleBiasOff.z );
	return d;
}

float  VoronoiContourSparse (const float2 pos)
{
	NoiseParams	p = CreateNoiseParams();
	p.custom.x		= 1.0;	// maxCeilOffset
	p.custom.yzw	= float3(1.0, 0.0, 0.5);
	return VoronoiContourSparse( pos, p );
}

float  VoronoiContourSparse (const float3 pos, const NoiseParams params)
{
	float3			hashScaleBiasOff = params.custom.yzw;
	VoronoiResult3	r = VoronoiContourR( pos, params );
	float			d = r.minDist;

	d *= Sign( DHash13( (r.icenter + r.offset) * hashScaleBiasOff.x + hashScaleBiasOff.y ) - hashScaleBiasOff.z );
	return d;
}

float  VoronoiContourSparse (const float3 pos)
{
	NoiseParams	p = CreateNoiseParams();
	p.custom.x		= 1.0;	// maxCeilOffset
	p.custom.yzw	= float3(1.0, 0.0, 0.5);
	return VoronoiContourSparse( pos, p );
}

#endif // AE_LICENSE_MIT
//-----------------------------------------------------------------------------



/*
=================================================
	CreateTileableNoiseParams
=================================================
*/
TileableNoiseParams  CreateTileableNoiseParams (float2 tileSize)
{
	return CreateTileableNoiseParams( float3(tileSize, 0.f) );
}

TileableNoiseParams  CreateTileableNoiseParams (float3 tileSize)
{
	TileableNoiseParams	p;
	p.custom	= float4(-float_max);	// user must override it
	p.seedScale	= float3(1.0);
	p.seedBias	= float3(0.0);
	p.tileSize	= tileSize;
	return p;
}


#ifdef AE_LICENSE_MIT
/*
=================================================
	TileableVoronoiNoise
----
	range [0..inf]
=================================================
*/
float  TileableVoronoiNoise (const float2 pos, const TileableNoiseParams params)
{
	return TileableVoronoiNoiseR( pos, params ).minDist;
}

float  TileableVoronoiNoise (const float2 pos, const float2 tileSize)
{
	return TileableVoronoiNoiseR( pos, tileSize ).minDist;
}

float  TileableVoronoiNoise (const float3 pos, const TileableNoiseParams params)
{
	return TileableVoronoiNoiseR( pos, params ).minDist;
}

float  TileableVoronoiNoise (const float3 pos, const float3 tileSize)
{
	return TileableVoronoiNoiseR( pos, tileSize ).minDist;
}

/*
=================================================
	TileableWarleyNoise
----
	range [-inf..1]
=================================================
*/
float  TileableWarleyNoise (const float3 pos, const TileableNoiseParams params)
{
	return 1.0 - TileableVoronoiNoise( pos, params );
}

float  TileableWarleyNoise (const float3 pos, const float3 tileSize)
{
	return 1.0 - TileableVoronoiNoise( pos, tileSize );
}

float  TileableWarleyNoise (const float2 pos, const TileableNoiseParams params)
{
	return 1.0 - TileableVoronoiNoise( pos, params );
}

float  TileableWarleyNoise (const float2 pos, const float2 tileSize)
{
	return 1.0 - TileableVoronoiNoise( pos, tileSize );
}

/*
=================================================
	TileableVoronoiContour
=================================================
*/
float  TileableVoronoiContour (const float2 pos, const TileableNoiseParams params)
{
	return TileableVoronoiContourR( pos, params ).minDist;
}

float  TileableVoronoiContour (const float2 pos, const float2 tileSize)
{
	TileableNoiseParams	p = CreateTileableNoiseParams( tileSize );
	p.custom.x	= 1.0;	// maxCeilOffset
	return TileableVoronoiContourR( pos, p ).minDist;
}

float  TileableVoronoiContour (const float3 pos, const TileableNoiseParams params)
{
	return TileableVoronoiContourR( pos, params ).minDist;
}

float  TileableVoronoiContour (const float3 pos, const float3 tileSize)
{
	TileableNoiseParams	p = CreateTileableNoiseParams( tileSize );
	p.custom.x	= 1.0;	// maxCeilOffset
	return TileableVoronoiContourR( pos, p ).minDist;
}

#endif // AE_LICENSE_MIT
//-----------------------------------------------------------------------------
