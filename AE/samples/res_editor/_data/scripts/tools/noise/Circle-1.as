// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Use tileable noise to generate circle with distortion.
	Can not be used for 2D space distortion!
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );

		const array<float>	params = {
			0, 								// iNoise
			3, 								// iOctaves
			20.0f,							// iPScale
			0.0000f, 1.2500f, 2.3210f, 		// iPBias
			0.5540f, 2.3380f, 				// iLacunarity
			1.0000f, 23.7450f, 				// iPersistence
			1.0000f, 0.0000f, 				// iVScaleBias
		};

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.Slider( "iIsolines",		0,						1,						1 );
			pass.Slider( "iCurve",			0,						4,						1 );
			pass.Slider( "iNoise",			0,						16,						int(params[0]) );
			pass.Slider( "iOctaves",		1,						4,						int(params[1]) );
			pass.Slider( "iPScale",			0.1f,					60.f,					params[2] );
			pass.Slider( "iPBias",			float3(-10.f),			float3(10.f),			float3( params[3],  params[4], params[5] ));
			pass.Slider( "iLacunarity",		float2(-1.f, 0.1f),		float2(1.f, 100.f),		float2( params[6],  params[7]  ));
			pass.Slider( "iPersistence",	float2(-1.f, 0.1f),		float2(1.f, 100.f),		float2( params[8],  params[9]  ));
			pass.Slider( "iVScaleBias",		float2(0.01f, -2.f),	float2(4.f, 1.f),		float2( params[10], params[11] ));
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#define GEN_CIRCLE_NOISE
	#include "tools/TileableNoise.glsl"
	#include "InvocationID.glsl"
	#include "SDF.glsl"


	ND_ float  Noise (float a)
	{
		TileableNoiseParams		params;
		params.seedScale	= float3(1.0);
		params.seedBias		= float3(0.0);
		params.custom		= float4(-float_max);

		float	pos_scale	= Max( 1.0, RoundToBase( iPScale, 2.0 ));
		float3	pos			= float3(a, 0.f, 0.f) + iPBias * float3(0.1, 0.025, 0.0625);
		float	lac			= iLacunarity.x * iLacunarity.y;
		float	pers		= iPersistence.x * iPersistence.y;

		params.tileSize		= float3(pos_scale);

		return CircleNoise( pos, params, CreateFBMParams( lac, pers, iOctaves ));	// unorm
	}

	ND_ float  CircleNoise (const float2 uv)
	{
		float	a = ATan( uv.y, uv.x ) * float_InvPi;
		return Noise( ToUNorm( a ));	// unorm
	}

	ND_ float  ApplyDistortionCurve (float d, float n)
	{
		switch ( iCurve )
		{
			case 0 :	return d + n * d;
			case 1 :	return d + n * d * SmoothStep( d, 0.0, 0.3 );
			case 2 :	return d + ToSNorm(n) * 0.1 * LinearStep( d, 0.0, 0.5 );
			case 3 :	return d + n * d * SmoothStep( d, 0.0, 0.4 ) * (1.0 - SmoothStep( d, 0.3, 1.0 ));
		}
	}


	void  Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected();

		float	d	= Length( uv );
		float	d2	= ApplyDistortionCurve( d, CircleNoise( uv ));

		out_Color = float4(0.25);

		if ( iIsolines == 0 )
		{
			out_Color.rgb *= AA_Lines( d * un_PerPass.resolution.x, 1.0/600.0, 3.0 );

			if ( Abs( d2 - 0.5 ) < 0.003 )
				out_Color = float4(1.0);
		}

		if ( iIsolines == 1 )
		{
			out_Color.rgb = SDF_Isolines( (d2 - 0.5) * 40.0 );
		}
	}

#endif
//-----------------------------------------------------------------------------
