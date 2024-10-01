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
		RC<DynamicUInt>		count	= DynamicUInt();
		RC<DynamicUInt>		tex_pot	= DynamicUInt();
		RC<DynamicDim>		tex_dim	= tex_pot.PowOf2().Dimension2();
		RC<Image>			rt		= Image( EPixelFormat::RGBA8_UNorm, tex_dim );	rt.Name( "RT" );

		Slider( tex_pot,	"TexDim",	1,	12,		4 );
		Slider( count,		"Repeat",	1,	16 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Slider( "iNoise",		0,		21 );
			pass.Slider( "iOctaves",	0,		16,		2 );
			pass.Slider( "iPScale",		1.f,	50.f,	10.f );
			pass.Repeat( count );
		}

	//	Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Noise.glsl"
	#include "GlobalIndex.glsl"

	FBM_NOISE_Hash( GradientNoise )
	FBM_NOISE_Hash( ValueNoise )
	FBM_NOISE_Hash( PerlinNoise )
	FBM_NOISE_Hash( SimplexNoise )

	FBM_NOISE_A1_Hash( IQNoise,			float2, uv )
	FBM_NOISE_A1_Hash( WarleyNoise,		float3, seedScaleBias_offsetScale )
	FBM_NOISE_A1_Hash( VoronoiContour,	float3, seedScaleBias_offsetScale )
	FBM_NOISE_A2_Hash( VoronoiContour3,	float3, seedScaleBias_offsetScale, float3, hashScaleBiasOff )

	void Main ()
	{
		const float		lac		= 5.0;
		const float		pers	= 1.0;
		const float3	uv		= float3( 1.f );
		const float		vcos	= 0.75;	// scale for voronoi cell offset
		const float3	uv2		= float3( uv.xy, vcos );
		float3			pos		= GetGlobalCoordSNorm() * iPScale;
		float			n		= 0.f;

		#if iNoise_max != 21
		#	error iNoise max value must be 21
		#endif
		switch ( iNoise )
		{
			case 0 :	n = GradientNoise( pos );										break;
			case 1 :	n = ValueNoise( pos );											break;
			case 2 :	n = PerlinNoise( pos );											break;
			case 3 :	n = SimplexNoise( pos * 0.5 );									break;
			case 4 :	n = IQNoise( pos * 2.0, uv.xy );								break;
			case 5 :	n = WaveletNoise( pos.xy * 0.25, uv.xy );						break;
			case 6 :	n = Voronoi( pos.xy, uv2 ).minDist;								break;
			case 7 :	n = Voronoi( pos, uv2 ).minDist;								break;
			case 8 :	n = VoronoiContour2( pos.xy, uv2 ).minDist;						break;
			case 9 :	n = VoronoiContour2( pos, uv2 ).minDist;						break;
			case 10 :	n = VoronoiContour3( pos.xy, float3(1.0,0.0,vcos), uv.xzy );	break;
			case 11 :	n = VoronoiContour3( pos, float3(1.0,0.0,vcos), uv.xzy );		break;
			case 12 :	n = VoronoiCircles( pos.xy, 0.5f, uv2 );						break;
			case 13 :	n = WarleyNoise( pos, uv2 );									break;

			// FBM
			case 14 :	n = GradientNoiseFBM( pos, lac, pers, iOctaves );				break;
			case 15 :	n = ValueNoiseFBM( pos, lac, pers, iOctaves );					break;
			case 16 :	n = PerlinNoiseFBM( pos, lac, pers, iOctaves );					break;
			case 17 :	n = SimplexNoiseFBM( pos * 0.5, lac, pers, iOctaves );			break;
			case 18 :	n = WarleyNoiseFBM( pos, uv2, lac, pers, iOctaves );			break;
			case 19 :	n = IQNoiseFBM( pos * 2.0, uv.xy, lac, pers, iOctaves );		break;
			case 20 :	n = VoronoiContourFBM( pos, uv2, lac, pers, iOctaves );			break;
			case 21 :	n = VoronoiContour3FBM( pos, float3(1.0,0.0,vcos), uv.xzy, lac, pers, iOctaves );	break;
		}

		out_Color = float4(n);
	}

#endif
//-----------------------------------------------------------------------------
