// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_NOISE
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<Image>			noise_tex	= Image( EPixelFormat::R32F,	SurfaceSize() );
		RC<DynamicFloat>	pos_scale	= DynamicFloat();

		Slider( pos_scale, "PosScale",	1.0,	100.0,		30.0 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_NOISE" );
			pass.Output(	"out_Noise",	noise_tex,		RGBA32f(0.0) );
			pass.Slider(	"iNoise",			0,		22,		16 );
			pass.Slider(	"iOctaves",			1,		10,		3 );
			pass.Slider(	"iLacunarity",		0.0,	1.5,	0.35 );
			pass.Slider(	"iPersistance",		0.0,	10.0,	2.5 );
			pass.Slider(	"iColScaleBias",	float2(0.5, -1.0),	float2(3.0, 1.0),	float2(1.0, 0.0) );
			pass.Constant(	"iPosScale",		pos_scale );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output(	"out_Color",	rt,			RGBA32f(0.0) );
			pass.ArgIn(		"un_Noise",		noise_tex,	Sampler_LinearClamp );
			pass.Slider(	"iView",		0,		4,		1 );
			pass.Constant(	"iPosScale",	pos_scale );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_NOISE
	#include "Noise.glsl"
	#include "InvocationID.glsl"

	FBM_NOISE_Hash( GradientNoise )
	FBM_NOISE_Hash( ValueNoise )
	FBM_NOISE_Hash( PerlinNoise )
	FBM_NOISE_Hash( SimplexNoise )
	FBM_NOISE_Hash( IQNoise )
	FBM_NOISE_Hash( Voronoi )
	FBM_NOISE_Hash( WarleyNoise )
	FBM_NOISE_Hash( VoronoiContour )
	FBM_NOISE_Hash( VoronoiContourSparse )

	void Main ()
	{
		float3				pos		= float3( Floor(gl.FragCoord.xy) * un_PerPass.invResolution, 0.0 ) * iPosScale;
		float				n		= 0.f;
		const FBMParams		fbm		= CreateFBMParams( iLacunarity, iPersistance, iOctaves );

		switch ( iNoise )
		{
			case 0 :	n = GradientNoise( pos );								break;
			case 1 :	n = ValueNoise( pos );									break;
			case 2 :	n = PerlinNoise( pos );									break;
			case 3 :	n = SimplexNoise( pos );								break;
			case 4 :	n = IQNoise( pos );					n = ToSNorm( n );	break;
			case 5 :	n = WaveletNoise( pos.xy );								break;
			case 6 :	n = Voronoi( pos.xy );				n = ToSNorm( n );	break;
			case 7 :	n = Voronoi( pos );					n = ToSNorm( n );	break;
			case 8 :	n = VoronoiContour( pos.xy );		n = ToSNorm( n );	break;
			case 9 :	n = VoronoiContour( pos );			n = ToSNorm( n );	break;
			case 10 :	n = VoronoiContourSparse( pos.xy );						break;
			case 11 :	n = VoronoiContourSparse( pos );						break;
			case 12 :	n = VoronoiCircles( pos.xy, 0.5 );	n = ToSNorm( n );	break;
			case 13 :	n = WarleyNoise( pos );				n = ToSNorm( n );	break;
			case 14 :	n = GradientNoiseFBM( pos, fbm );						break;
			case 15 :	n = ValueNoiseFBM( pos, fbm );							break;
			case 16 :	n = PerlinNoiseFBM( pos, fbm );							break;
			case 17 :	n = SimplexNoiseFBM( pos, fbm );						break;
			case 18 :	n = VoronoiFBM( pos, fbm );								break;
			case 19 :	n = WarleyNoiseFBM( pos, fbm );							break;
			case 20 :	n = IQNoiseFBM( pos, fbm );			n = ToSNorm( n );	break;
			case 21 :	n = VoronoiContourFBM( pos, fbm );	n = ToSNorm( n );	break;
			case 22 :	n = VoronoiContourSparseFBM( pos, fbm );				break;
		}

		out_Noise.r = ToUNorm( -n ) * iColScaleBias.x + iColScaleBias.y;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Color.glsl"
	#include "Normal.glsl"
	#include "InvocationID.glsl"

	float3  WorldPos (int2 coord, int2 offset)
	{
		float2	uv	= float2(coord + offset) * un_PerPass.invResolution.xy;
		float2	pos	= ToSNorm( uv ) * iPosScale;

		float	n	= gl.texture.Fetch( un_Noise, coord + offset, 0 ).r;
		return float3( pos, n * 4.0 );
	}

	void Main ()
	{
		float3	l_dir	= float3( 0.0, 0.6, 1.0 );

		if ( un_PerPass.mouse.z > 0.0 )
			l_dir = float3( -ToSNorm( un_PerPass.mouse.xy ), 0.6f );

		float	n = gl.texture.Fetch( un_Noise, int2(gl.FragCoord.xy), 0 ).r;

		float3	norm;
		SmoothNormal3x3i( OUT norm, WorldPos, int2(gl.FragCoord.xy) );

		switch ( iView )
		{
			case 0 :	out_Color.rgb = float3( n );  break;
			case 1 :	out_Color = Rainbow( n );  break;
			case 2 :	out_Color.rgb = ToUNorm( norm );  break;
			case 3 :	out_Color.rgb = float3(Dot( norm, Normalize(l_dir) ));  break;
			case 4 :	out_Color.rgb = float3(TriangleWave( n * 40.0 ));  break;
		}
		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
