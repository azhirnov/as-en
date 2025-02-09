// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE_2D
	#include "Noise.glsl"

	FBM_TILE_NOISE_Hash( TileableGradientNoise )
	FBM_TILE_NOISE_Hash( TileableIQNoise )
	FBM_TILE_NOISE_Hash( TileableValueNoise )
	FBM_TILE_NOISE_Hash( TileablePerlinNoise )
	FBM_TILE_NOISE_Hash( TileableWarleyNoise )
	FBM_TILE_NOISE_Hash( TileableVoronoiContour )
	FBM_TILE_NOISE_Hash( TileableVoronoiNoise )

	#define CastUNorm	n = n * iVScale.x + iVScale.y;
	#define CastSNorm	CastUNorm;	n = ToUNorm( n );


	void  SetupIQ (inout TileableNoiseParams params)
	{
		params.custom.xy = iSeed.xy;
	}

	void  SetupVoronoi (inout TileableNoiseParams params)
	{
		params.seedScale	= float3(iSeed.x);
		params.seedBias		*= iSeed.y;
		params.custom.x		= iSeed.z;
	}

	float  TileableNoise2D (float3 pos, TileableNoiseParams params, const FBMParams fbm)
	{
		#if iNoise_max != 16
		#	error iNoise max value must be 16
		#endif

		float	n = 0.f;
		switch ( iNoise )
		{
			case 0 :							n = TileableGradientNoise( pos, params );		CastSNorm;	break;
			case 1 :	SetupIQ( params );		n = TileableIQNoise( pos, params );				CastUNorm;	break;
			case 2 :							n = TileableValueNoise( pos, params );			CastSNorm;	break;
			case 3 :							n = TileablePerlinNoise( pos, params );			CastSNorm;	break;

			case 4 :	SetupVoronoi( params );	n = TileableVoronoiNoise( pos.xy, params );		CastUNorm;	break;
			case 5 :	SetupVoronoi( params );	n = TileableVoronoiNoise( pos, params );		CastUNorm;	break;
			case 6 :	SetupVoronoi( params );	n = TileableWarleyNoise( pos.xy, params );		CastUNorm;	break;
			case 7 :	SetupVoronoi( params );	n = TileableWarleyNoise( pos, params );			CastUNorm;	break;
			case 8 :	SetupVoronoi( params );	n = TileableVoronoiContour( pos.xy, params );	CastUNorm;	break;
			case 9 :	SetupVoronoi( params );	n = TileableVoronoiContour( pos, params );		CastUNorm;	break;

			case 10 :							n = TileableGradientNoiseFBM( pos, params, fbm );	CastSNorm;	break;
			case 11 :	SetupIQ( params );		n = TileableIQNoiseFBM( pos, params, fbm );			CastUNorm;	break;
			case 12 :							n = TileableValueNoiseFBM( pos, params, fbm );		CastSNorm;	break;
			case 13 :							n = TileablePerlinNoiseFBM( pos, params, fbm );		CastSNorm;	break;
			case 14 :	SetupVoronoi( params );	n = TileableVoronoiNoiseFBM( pos, params, fbm );	CastUNorm;	break;
			case 15 :	SetupVoronoi( params );	n = TileableWarleyNoiseFBM( pos, params, fbm );		CastUNorm;	break;
			case 16 :	SetupVoronoi( params );	n = TileableVoronoiContourFBM( pos, params, fbm );	CastUNorm;	break;
		}
		return Saturate( n );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE_3D
	#include "Noise.glsl"

	FBM_TILE_NOISE_Hash( TileableGradientNoise )
	FBM_TILE_NOISE_Hash( TileableIQNoise )
	FBM_TILE_NOISE_Hash( TileableValueNoise )
	FBM_TILE_NOISE_Hash( TileablePerlinNoise )
	FBM_TILE_NOISE_Hash( TileableWarleyNoise )
	FBM_TILE_NOISE_Hash( TileableVoronoiContour )

	#define CastUNorm	n = n * iVScale.x + iVScale.y;
	#define CastSNorm	CastUNorm;	n = ToUNorm( n );


	void  SetupIQ (inout TileableNoiseParams params)
	{
		params.custom.xy = iSeed.xy;
	}

	void  SetupVoronoi (inout TileableNoiseParams params)
	{
		params.seedScale	= float3(iSeed.x);
		params.seedBias		*= iSeed.y;
		params.custom.x		= iSeed.z;
	}


	float  TileableNoise3D (float3 pos, TileableNoiseParams params, const FBMParams fbm)
	{
		#if iNoise_max != 12
		#	error iNoise max value must be 12
		#endif

		float	n = 0.f;
		switch ( iNoise )
		{
			case 0 :							n = TileableGradientNoise( pos, params );	CastSNorm;	break;
			case 1 :	SetupIQ( params );		n = TileableIQNoise( pos, params );			CastUNorm;	break;
			case 2 :							n = TileableValueNoise( pos, params );		CastSNorm;	break;
			case 3 :							n = TileablePerlinNoise( pos, params );		CastSNorm;	break;

			case 4 :	SetupVoronoi( params );	n = TileableVoronoiNoise( pos, params );	CastUNorm;	break;
			case 5 :	SetupVoronoi( params );	n = TileableWarleyNoise( pos, params );		CastUNorm;	break;
			case 6 :	SetupVoronoi( params );	n = TileableVoronoiContour( pos, params );	CastUNorm;	break;

			case 7 :							n = TileableGradientNoiseFBM( pos, params, fbm );	CastSNorm;	break;
			case 8 :	SetupIQ( params );		n = TileableIQNoiseFBM( pos, params, fbm );			CastUNorm;	break;
			case 9 :							n = TileableValueNoiseFBM( pos, params, fbm );		CastSNorm;	break;
			case 10 :							n = TileablePerlinNoiseFBM( pos, params, fbm );		CastSNorm;	break;
			case 11 :	SetupVoronoi( params );	n = TileableWarleyNoiseFBM( pos, params, fbm );		CastUNorm;	break;
			case 12 :	SetupVoronoi( params );	n = TileableVoronoiContourFBM( pos, params, fbm );	CastUNorm;	break;
		}

		return Saturate( n );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_CIRCLE_NOISE
	#include "Noise.glsl"

	#define _FBM_CIRCLE_NOISE2( _noise_ )												\
	{																					\
		float	value	= 0.0;															\
		float	pers	= 1.0;															\
		float	scale	= 0.0;															\
																						\
		fbm.lacunarity = Max( 0.5, RoundToBase( fbm.lacunarity, 0.5 ));					\
																						\
		for (int i = 0; i < fbm.octaveCount; ++i)										\
		{																				\
			params.tileSize	 = Max( float3(1.0), RoundToBase( params.tileSize, 2.0 ));	\
			value			+= _noise_( pos * params.tileSize, params ) * pers;			\
			scale			+= pers;													\
			params.tileSize	*= fbm.lacunarity;											\
			params.seedScale += float3( 0.3461, 0.7324, 0.5862 );						\
			params.seedBias	 = _FBMTransform( params.seedBias * fbm.lacunarity );		\
			pers			*= fbm.persistence;											\
		}																				\
		return value / scale;															\
	}

	#define FBM_CIRCLE_NOISE_Hash( _noise_ )													\
		ND_ float  _noise_##FBM (in float3 pos, TileableNoiseParams params, FBMParams fbm) {	\
			_FBM_CIRCLE_NOISE2( _noise_ )														\
		}

	FBM_CIRCLE_NOISE_Hash( TileableGradientNoise )
	FBM_CIRCLE_NOISE_Hash( TileableIQNoise )
	FBM_CIRCLE_NOISE_Hash( TileableValueNoise )
	FBM_CIRCLE_NOISE_Hash( TileablePerlinNoise )
	FBM_CIRCLE_NOISE_Hash( TileableVoronoiNoise )
	FBM_CIRCLE_NOISE_Hash( TileableWarleyNoise )
	FBM_CIRCLE_NOISE_Hash( TileableVoronoiContour )

	#define CastUNorm	n = n * iVScaleBias.x + iVScaleBias.y;
	#define CastSNorm	CastUNorm	n = ToUNorm( n );


	void  SetupIQ (inout TileableNoiseParams params)
	{
		params.custom.xy = Fract( float2(iPBias.yz) * 0.4 );
	}

	void  SetupVoronoi (inout TileableNoiseParams params)
	{
		params.custom.x  = 0.9;
	}


	float  CircleNoise (float3 pos, TileableNoiseParams params, const FBMParams fbm)
	{
		#if iNoise_max != 16
		#	error iNoise max value must be 16
		#endif

		float3	pos2	= pos;
		float	n		= 0.f;

		pos *= params.tileSize;

		switch ( iNoise )
		{
			case 0 :							n = TileableGradientNoise( pos, params );		CastSNorm;	break;
			case 1 :	SetupIQ( params );		n = TileableIQNoise( pos, params );				CastUNorm;	break;
			case 2 :							n = TileableValueNoise( pos, params );			CastSNorm;	break;
			case 3 :							n = TileablePerlinNoise( pos, params );			CastSNorm;	break;

			case 4 :	SetupVoronoi( params );	n = TileableVoronoiNoise( pos.xy, params );		CastUNorm;	break;
			case 5 :	SetupVoronoi( params );	n = TileableVoronoiNoise( pos, params );		CastUNorm;	break;
			case 6 :	SetupVoronoi( params );	n = TileableWarleyNoise( pos.xy, params );		CastUNorm;	break;
			case 7 :	SetupVoronoi( params );	n = TileableWarleyNoise( pos, params );			CastUNorm;	break;
			case 8 :	SetupVoronoi( params );	n = TileableVoronoiContour( pos.xy, params );	CastUNorm;	break;
			case 9 :	SetupVoronoi( params );	n = TileableVoronoiContour( pos, params );		CastUNorm;	break;

			case 10 :							n = TileableGradientNoiseFBM( pos2, params, fbm );	CastSNorm;	break;
			case 11 :	SetupIQ( params );		n = TileableIQNoiseFBM( pos2, params, fbm );		CastUNorm;	break;
			case 12 :							n = TileableValueNoiseFBM( pos2, params, fbm );		CastSNorm;	break;
			case 13 :							n = TileablePerlinNoiseFBM( pos2, params, fbm );	CastSNorm;	break;
			case 14 :	SetupVoronoi( params );	n = TileableVoronoiNoiseFBM( pos2, params, fbm );	CastUNorm;	break;
			case 15 :	SetupVoronoi( params );	n = TileableWarleyNoiseFBM( pos2, params, fbm );	CastUNorm;	break;
			case 16 :	SetupVoronoi( params );	n = TileableVoronoiContourFBM( pos2, params, fbm );	CastUNorm;	break;
		}
		return n;
	}

#endif
//-----------------------------------------------------------------------------
