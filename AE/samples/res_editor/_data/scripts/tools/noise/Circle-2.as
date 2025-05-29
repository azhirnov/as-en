// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Use texture with tileable noise to generate circle with distortion.
	Can not be used for 2D space distortion!
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define TILE
#	define CIRCLE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	tex		= Image( EPixelFormat::R8_UNorm, uint2(128, 256), ImageLayer(3) );

		// render loop
		{
			const array<float>	params = {
				3, 										// iNoise
				3, 										// iOctaves
				5, 										// iLacunarity
				4, 										// iPersistence
				11.2720f, 								// iPScale
				0.0000f, 0.0000f, 0.0000f, 0.0000f, 	// iPBias
				1.0000f, 0.0000f, 1.0000f, 				// iSeed
				1.0000f, 0.0000f, 						// iVScale
			};
			int					i = 0;
			RC<Postprocess>		pass = Postprocess( "", "TILE" );
			pass.Output( "out_Noise1",	tex,	ImageLayer(0) );
			pass.Output( "out_Noise2",	tex,	ImageLayer(1) );
			pass.Output( "out_Noise3",	tex,	ImageLayer(2) );
			pass.Slider( "iNoise",		0,					16,					int(params[i]) );	++i;
			pass.Slider( "iOctaves",	1,					8,					int(params[i]) );	++i;
			pass.Slider( "iLacunarity",	1,					12,					int(params[i]) );	++i;
			pass.Slider( "iPersistence",1,					12,					int(params[i]) );	++i;
			pass.Slider( "iPScale",		1.f,				60.f,				params[i] );		++i;
			pass.Slider( "iPBias",		float4(-4.f),		float4(4.f),		float4( params[i], params[i+1], params[i+2], params[i+3] ));	i += 4;
			pass.Slider( "iSeed",		float3(-0.5f),		float3(1.f),		float3( params[i], params[i+1], params[i+2] ));					i += 3;
			pass.Slider( "iVScale",		float2(0.1f, -2.f),	float2(4.f, 2.f),	float2( params[i], params[i+1] ));								i += 2;
		}{
			const array<float>	params = {
				3, 						// iOctaves
				1.0000f, 				// iPScale
				0.1230f, 				// iPBias
				2.7640f, 				// iLacunarity
				0.6130f, 				// iPersistence
				1.1390f, 0.6970f, 		// iVScale
			};
			int					i = 0;
			RC<Postprocess>		pass = Postprocess( "", "CIRCLE" );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(  "un_Noise",		tex,	Sampler_LinearRepeat );
			pass.Slider( "iIsolines",		0,					1 );
			pass.Slider( "iCurve",			0,					2 );
			pass.Slider( "iOctaves",		1,					4,					int(params[i]) );	++i;
			pass.Slider( "iPScale",			1.f,				10.f,				params[i] );		++i;
			pass.Slider( "iPBias",			-0.1f,				1.1f,				params[i] );		++i;
			pass.Slider( "iLacunarity",		0.1f,				8.f,				params[i] );		++i;
			pass.Slider( "iPersistence",	0.1f,				8.f,				params[i] );		++i;
			pass.Slider( "iVScale",			float2(0.1f, -2.f),	float2(4.f, 2.f),	float2( params[i], params[i+1] ));	i += 2;
		}

		Present( rt );

	//	DbgView( tex, ImageLayer(0), DbgViewFlags::NoCopy );
	//	DbgView( tex, ImageLayer(1), DbgViewFlags::NoCopy );
	//	DbgView( tex, ImageLayer(2), DbgViewFlags::NoCopy );
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
		out_Noise1.r = Noise( iPScale );
		out_Noise2.r = Noise( iPScale * 4.0 );
		out_Noise3.r = Noise( iPScale * 16.0 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef CIRCLE
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"


	ND_ float  NoiseFBM (in float2 pos, float posScale, float lacunarity, const float persistence, const int octaveCount)
	{
		float	value	= 0.0;
		float	pers	= 1.0;
		float	scale	= 0.0;

		for (int i = 0; i < octaveCount; ++i)
		{
			float3	uvw;
			switch ( int(posScale * 0.5) )
			{
				case 0 :	uvw = float3( pos.x, pos.y, 0.f );	break;
				case 1 :	uvw = float3( pos.y, pos.x, 0.f );	break;
				case 2 :	uvw = float3( pos.x, pos.y, 1.f );	break;
				case 3 :	uvw = float3( pos.y, pos.x, 1.f );	break;
				case 4 :	uvw = float3( pos.x, pos.y, 2.f );	break;
				default :	uvw = float3( pos.y, pos.x, 2.f );	break;
			}

			value	 += gl.texture.Sample( un_Noise, uvw ).r * pers;
			scale	 += pers;
			posScale *= lacunarity;
			pers	 *= persistence;
		}

		float	n = ToSNorm( value / scale );
		return n * iVScale.x + iVScale.y;
	}


	ND_ float  Noise (float a)
	{
		float2	pos	= float2(a, iPBias);
		return NoiseFBM( pos, iPScale, iLacunarity, iPersistence, iOctaves );
	}


	ND_ float  CircleNoise (const float2 uv)
	{
		float	a = ATan( uv.y, uv.x ) * float_InvPi;
		return Noise( ToUNorm( a ));
	}

	ND_ float  ApplyDistortionCurve (float d)
	{
		switch ( iCurve )
		{
			case 0 :	return d;
			case 1 :	return d * SmoothStep( d, 0.0, 0.3 );
			case 2 :	return d * SmoothStep( d, 0.0, 0.3 ) * (1.0 - SmoothStep( d, 0.6, 1.0 ));
		}
	}


	void  Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected();

		float	d	= Length( uv );
		float	d2	= d + CircleNoise( uv ) * ApplyDistortionCurve( d );

		out_Color = float4(0.25);

		if ( iIsolines == 0 )
		{
			out_Color.rgb *= AA_Lines( d * un_PerPass.resolution.x, 1.0/300.0, 3.0 );

			if ( Abs( d2 - 0.5 ) < 0.003 )
				out_Color = float4(1.0);
		}

		if ( iIsolines == 1 )
		{
			out_Color.rgb = SDF_Isolines( (d2 - 0.5) * 20.0 );
		}
	}

#endif
//-----------------------------------------------------------------------------
