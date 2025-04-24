// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define TILE
#	define TILE_3D
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	RC<Postprocess>  CreateRenderTileableNoisePass (RC<Image> rt)
	{
		const array<float>	params = {
			10, 									// iNoise
			3, 										// iOctaves
			5, 										// iLacunarity
			4, 										// iPersistence
			1.0000f, 								// iPScale
			0.0000f, 0.0000f, 0.0000f, 0.0000f, 	// iPBias
			1.0000f, 0.0000f, 1.0000f, 				// iSeed
			1.0000f, 0.0000f, 						// iVScale
		};
		RC<Postprocess>		pass = Postprocess( "", "TILE" );
		int					i = 0;
		pass.Output( "out_Noise",	rt );
		pass.Slider( "iNoise",		0,					16,					int(params[i]) );	++i;
		pass.Slider( "iOctaves",	1,					8,					int(params[i]) );	++i;
		pass.Slider( "iLacunarity",	1,					12,					int(params[i]) );	++i;
		pass.Slider( "iPersistence",1,					12,					int(params[i]) );	++i;
		pass.Slider( "iPScale",		1.f,				100.f,				params[i] );		++i;
		pass.Slider( "iPBias",		float4(-4.f),		float4(4.f),		float4( params[i], params[i+1], params[i+2], params[i+3] ));	i += 4;
		pass.Slider( "iSeed",		float3(-0.5f),		float3(1.f),		float3( params[i], params[i+1], params[i+2] ));					i += 3;
		pass.Slider( "iVScale",		float2(0.1f, -2.f),	float2(4.f, 2.f),	float2( params[i], params[i+1] ));								i += 2;
		return pass;
	}

	RC<ComputePass>  CreateRenderTileableNoisePass3D (RC<Image> rt)
	{
		const array<float>	params = {
			0, 								// iNoise
			2, 								// iOctaves
			4, 								// iLacunarity
			1, 								// iPersistence
			10.0000f, 						// iPScale
			0.0000f, 0.0000f, 0.0000f, 		// iPBias
			1.0000f, 0.0000f, 1.0000f, 		// iSeed
			1.0000f, 0.0000f, 				// iVScale
		};
		RC<ComputePass>		pass = ComputePass( "", "TILE_3D" );
		int					i = 0;
		pass.ArgOut( "un_Noise",	rt );
		pass.Slider( "iNoise",		0,					12,					int(params[i]) );	++i;
		pass.Slider( "iOctaves",	1,					8,					int(params[i]) );	++i;
		pass.Slider( "iLacunarity",	1,					12,					int(params[i]) );	++i;
		pass.Slider( "iPersistence",1,					12,					int(params[i]) );	++i;
		pass.Slider( "iPScale",		1.f,				100.f,				params[i] );		++i;
		pass.Slider( "iPBias",		float3(-4.f),		float3(4.f),		float3( params[i], params[i+1], params[i+2] ));	i += 3;
		pass.Slider( "iSeed",		float3(-0.5f),		float3(1.f),		float3( params[i], params[i+1], params[i+2] ));	i += 3;
		pass.Slider( "iVScale",		float2(0.1f, -2.f),	float2(4.f, 2.f),	float2( params[i], params[i+1] ));				i += 2;
		pass.LocalSize( 8, 8, 1 );
		pass.DispatchThreads( rt.Dimension() );
		return pass;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef TILE
	#define GEN_NOISE_2D
	#include "tools/TileableNoise.glsl"
	#include "InvocationID.glsl"


	float  Noise (float posScale)
	{
		TileableNoiseParams		params;
		params.seedScale	= float3(1.0);
		params.custom		= float4(-float_max);

		float	ratio		= float(GetGlobalSize().x) / float(GetGlobalSize().y);
				posScale	= Max( 1.0, RoundToBase( posScale, 2.0 ));
		float3	pos			= (GetGlobalCoordUNorm() + float3(0.f, 0.f, iPBias.w)) * posScale;	pos.x *= ratio;
		float	lac			= iLacunarity;
		float	pers		= iPersistence * 0.5;

		params.seedBias		= iPBias.xyz * posScale;
		params.tileSize		= float3(posScale);
		params.tileSize.x	*= ratio;

		return TileableNoise2D( pos, params, CreateFBMParams( lac, pers, iOctaves ));
	}

	void  Main ()
	{
		out_Noise.r = Noise( iPScale );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef TILE_3D
	#define GEN_NOISE_3D
	#include "tools/TileableNoise.glsl"
	#include "InvocationID.glsl"


	float  Noise (float posScale)
	{
		TileableNoiseParams		params;
		params.seedScale	= float3(1.0);
		params.custom		= float4(-float_max);

				posScale	= Max( 1.0, RoundToBase( posScale, 2.0 ));
		float3	pos			= GetGlobalCoordUNorm() * posScale;
		float	lac			= iLacunarity;
		float	pers		= iPersistence * 0.5;

		params.seedBias		= iPBias.xyz * posScale;
		params.tileSize		= float3(posScale);

		return TileableNoise3D( pos, params, CreateFBMParams( lac, pers, iOctaves ));
	}

	void  Main ()
	{
		gl.image.Store( un_Noise, GetGlobalCoord(), float4(Noise( iPScale )) );
	}

#endif
//-----------------------------------------------------------------------------
