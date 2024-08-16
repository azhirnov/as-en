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

			pass.Slider( "iNoise",			0,						16,						int(params[0]) );
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
	#include "Noise.glsl"
	#include "GlobalIndex.glsl"

	FBM_NOISE_Hash( GradientNoise )
	FBM_NOISE_Hash( ValueNoise )
	FBM_NOISE_Hash( PerlinNoise )
	FBM_NOISE_Hash( SimplexNoise )
	FBM_NOISE_Hash( IQNoise )
	FBM_NOISE_Hash( WarleyNoise )
	FBM_NOISE_Hash( VoronoiContour )

	FBM_NOISE_A1_Hash( IQNoise,			float2, uv )
	FBM_NOISE_A1_Hash( WarleyNoise,		float3, seedScaleBias_offsetScale )
	FBM_NOISE_A1_Hash( VoronoiContour,	float3, seedScaleBias_offsetScale )
	FBM_NOISE_A2_Hash( VoronoiContour3,	float3, seedScaleBias_offsetScale, float3, hashScaleBiasOff )


	ND_ float  Noise (float a)
	{
		float3	pos	= (float3(a, 0.f, 0.f) * iPScale) + (iPBias * iPScale * float3(0.25, 0.25, 0.1));

		float	n = 0.f;
		float	c = 0.f;

		const float		lac		= iLacunarity.x * iLacunarity.y;
		const float		pers	= iPersistence.x * iPersistence.y;
		const float3	uv		= float3( iU.x * iU.y, iVV.x * iVV.y, 0.f );
		const float		vcos	= 0.75;	// scale for voronoi cell offset
		const float3	uv2		= float3( uv.xy, vcos );

		#if iNoise_max != 16
		#	error iNoise max value must be 16
		#endif
		#define CastUNorm	c = n * iVScaleBias.x + iVScaleBias.y;
		#define CastSNorm	c = ToUNorm( n * iVScaleBias.x + iVScaleBias.y );

		switch ( iNoise )
		{
			case 0 :	n = GradientNoise( pos );										CastSNorm;	break;
			case 1 :	n = ValueNoise( pos );											CastSNorm;	break;
			case 2 :	n = PerlinNoise( pos );											CastSNorm;	break;
			case 3 :	n = SimplexNoise( pos * 0.5 );									CastSNorm;	break;
			case 4 :	n = IQNoise( pos * 2.0, uv.xy );								CastSNorm;	break;
			case 5 :	n = Voronoi( pos, uv2 ).minDist;								CastUNorm;	break;
			case 6 :	n = VoronoiContour( pos, uv2 );									CastUNorm;	break;
			case 7 :	n = WarleyNoise( pos, uv2 );									CastUNorm;	break;
			case 8 :	n = VoronoiContour3( pos, float3(1.0,0.0,vcos), uv.xzy );		CastSNorm;	break;

			// FBM
			case 9 :	n = GradientNoiseFBM( pos, lac, pers, iOctaves );				CastSNorm;	break;
			case 10 :	n = ValueNoiseFBM( pos, lac, pers, iOctaves );					CastSNorm;	break;
			case 11 :	n = PerlinNoiseFBM( pos, lac, pers, iOctaves );					CastSNorm;	break;
			case 12 :	n = SimplexNoiseFBM( pos * 0.5, lac, pers, iOctaves );			CastSNorm;	break;
			case 13 :	n = WarleyNoiseFBM( pos, uv2, lac, pers, iOctaves );			CastUNorm;	break;
			case 14 :	n = IQNoiseFBM( pos * 2.0, uv.xy, lac, pers, iOctaves );		CastUNorm;	break;
			case 15 :	n = VoronoiContourFBM( pos, uv2, lac, pers, iOctaves );			CastUNorm;	break;
			case 16 :	n = VoronoiContour3FBM( pos, float3(1.0,0.0,vcos), uv.xzy, lac, pers, iOctaves );	CastSNorm;	break;
		}
		return c;
	}

	ND_ float  CircleNoise (const float2 uv)
	{
		float	a	= ATan( -uv.y, -uv.x );

		float	n0	= Noise( a );
		float	n1	= (n0 + Noise( -a )) * 0.5;

		const float	start_blend = 0.8f;
		float	b	= Abs( a ) * float_InvPi;
		b = RemapSmooth( float2(start_blend, 1.0), float2(0.0, 1.0), b );

		return Lerp( n0, n1, b );
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
