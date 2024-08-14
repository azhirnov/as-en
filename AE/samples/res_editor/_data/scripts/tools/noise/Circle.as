// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );

		const array<float>	params = {
			0,						// iNoise
			2,						// iOctaves
			15.f,					// iPScale
			0.f, 0.f, 0.f,			// iPBias
			1.f, 1.f,				// iLacunarity
			1.f, 1.f,				// iPersistence
			1.f, 1.f,				// iU
			1.f, 1.f,				// iVV
			1.f, 0.f,				// iVScaleBias
		};

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );

			pass.Slider( "iNoise",			0,						21,						int(params[0]) );
			pass.Slider( "iOctaves",		1,						10,						int(params[1]) );

			pass.Slider( "iPScale",			0.1f,					100.f,					params[2] );
			pass.Slider( "iPBias",			float3(-10.f),			float3(10.f),			float3( params[3],  params[4], params[5] ));
			pass.Slider( "iLacunarity",		float2(-1.f, 0.1f),		float2(1.f, 100.f),		float2( params[6],  params[7]  ));
			pass.Slider( "iPersistence",	float2(-1.f, 0.1f),		float2(1.f, 100.f),		float2( params[8],  params[9]  ));
			pass.Slider( "iU",				float2(-1.f, 0.1f),		float2(1.f, 100.f),		float2( params[10], params[11] ));
			pass.Slider( "iVV",				float2(-1.f, 0.1f),		float2(1.f, 100.f),		float2( params[12], params[13] ));
			pass.Slider( "iVScaleBias",		float2(0.01f, -2.f),	float2(2.f, 1.f),		float2( params[14], params[15] ));
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "GlobalIndex.glsl"

	#define NOISE_3D_FBM	iNoise_max
	#define CIRCLE_NOISE
	#include "Tools-Noise.glsl"

	float  Noise (float a)
	{
		float2	uv		= float2( iU.x * iU.y, iVV.x * iVV.y );
		float	lac		= iLacunarity.x * iLacunarity.y;
		float	pers	= iPersistence.x * iPersistence.y;
		return Noise2( float3(a, 0.f, 0.f), iPScale, iPBias, iNoise, uv, iVScaleBias, lac, pers, iOctaves );
	}

	void  Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected();

		float	d	= Length( uv );
		float	d2	= d + CircleNoise( uv ) * 0.1;

		out_Color = float4(0.25);

		out_Color.rgb *= AA_Lines( d * un_PerPass.resolution.x, 1.0/300.0, 3.0 );

		if ( Abs( d2 - 0.5 ) < 0.003 )
			out_Color = float4(1.0);
	}

#endif
//-----------------------------------------------------------------------------
