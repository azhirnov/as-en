// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Bloom and tonemapping
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define MAIN
#	define INIT_BLOOM
#	define DOWNSAMPLE_HOR_BLUR
#	define DOWNSAMPLE_VERT_BLUR
#	define UPSAMPLE_PASS
#	define FINAL
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		const EPixelFormat	fmt = EPixelFormat::RGBA16F;

		RC<Image>	main_rt		= Image( fmt, SurfaceSize() );						main_rt.Name( "RT" );
		RC<Image>	bloom_src	= Image( fmt, SurfaceSize() );						bloom_src.Name( "Bloom source" );
		RC<Image>	final_rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );final_rt.Name( "RT" );

		RC<Image>	down_rt1h	= Image( fmt, main_rt.Dimension() / 2 );			down_rt1h.Name( "RT-up/2" );
		RC<Image>	down_rt1v	= Image( fmt, main_rt.Dimension() / 2 );			down_rt1v.Name( "RT-down/2" );
		RC<Image>	down_rt2h	= Image( fmt, main_rt.Dimension() / 4 );			down_rt2h.Name( "RT-up/4" );
		RC<Image>	down_rt2v	= Image( fmt, main_rt.Dimension() / 4 );			down_rt2v.Name( "RT-down/4" );
		RC<Image>	down_rt3h	= Image( fmt, main_rt.Dimension() / 8 );			down_rt3h.Name( "RT-up/8" );
		RC<Image>	down_rt3v	= Image( fmt, main_rt.Dimension() / 8 );			down_rt3v.Name( "RT-down/8" );
		RC<Image>	down_rt4h	= Image( fmt, main_rt.Dimension() / 16 );			down_rt4h.Name( "RT-up/16" );
		RC<Image>	down_rt4v	= Image( fmt, main_rt.Dimension() / 16 );			down_rt4v.Name( "RT-down/16" );
		RC<Image>	down_rt5h	= Image( fmt, main_rt.Dimension() / 32 );			down_rt5h.Name( "RT-up/32" );
		RC<Image>	down_rt5v	= Image( fmt, main_rt.Dimension() / 32 );			down_rt5v.Name( "RT-down/32" );
		RC<Image>	down_rt6h	= Image( fmt, main_rt.Dimension() / 64 );			down_rt6h.Name( "RT-up/64" );
		RC<Image>	down_rt6v	= Image( fmt, main_rt.Dimension() / 64 );			down_rt6v.Name( "RT-down/64" );

		RC<Image>	up_rt1		= down_rt1h;
		RC<Image>	up_rt2		= down_rt2h;
		RC<Image>	up_rt3		= down_rt3h;
		RC<Image>	up_rt4		= down_rt4h;
		RC<Image>	up_rt5		= down_rt5h;

		// render loop
		{
			RC<Postprocess>		main_pass = Postprocess( "", "MAIN" );
			main_pass.Output(  "out_Color",			main_rt );
			main_pass.Slider( "iBrightnessScale",	0.5,	20.0,	15.0 );
			main_pass.Slider( "iWhiteFactor",		0.0,	0.25,	0.006 );

			RC<Postprocess>		init_bloom = Postprocess( "", "INIT_BLOOM" );
			init_bloom.ArgIn(  "un_Texture",	main_rt,		"NearestClamp" );
			init_bloom.Output( "out_Color",		bloom_src );
			init_bloom.Slider( "iMinBrightness", 0.0, 80.0 );


			// Downsample
			RC<Postprocess>		down_pass1h = Postprocess( "", "DOWNSAMPLE_HOR_BLUR" );
			down_pass1h.ArgIn(  "un_Texture",	bloom_src,		Sampler_LinearClamp );
			down_pass1h.Output( "out_Color",	down_rt1h );

			RC<Postprocess>		down_pass1v = Postprocess( "", "DOWNSAMPLE_VERT_BLUR" );
			down_pass1v.ArgIn(  "un_Texture",	down_rt1h,		Sampler_LinearClamp );
			down_pass1v.Output( "out_Color",	down_rt1v );

			RC<Postprocess>		down_pass2h = Postprocess( "", "DOWNSAMPLE_HOR_BLUR" );
			down_pass2h.ArgIn(  "un_Texture",	down_rt1v,		Sampler_LinearClamp );
			down_pass2h.Output( "out_Color",	down_rt2h );

			RC<Postprocess>		down_pass2v = Postprocess( "", "DOWNSAMPLE_VERT_BLUR" );
			down_pass2v.ArgIn(  "un_Texture",	down_rt2h,		Sampler_LinearClamp );
			down_pass2v.Output( "out_Color",	down_rt2v );

			RC<Postprocess>		down_pass3h = Postprocess( "", "DOWNSAMPLE_HOR_BLUR" );
			down_pass3h.ArgIn(  "un_Texture",	down_rt2v,		Sampler_LinearClamp );
			down_pass3h.Output( "out_Color",	down_rt3h );

			RC<Postprocess>		down_pass3v = Postprocess( "", "DOWNSAMPLE_VERT_BLUR" );
			down_pass3v.ArgIn(  "un_Texture",	down_rt3h,		Sampler_LinearClamp );
			down_pass3v.Output( "out_Color",	down_rt3v );

			RC<Postprocess>		down_pass4h = Postprocess( "", "DOWNSAMPLE_HOR_BLUR" );
			down_pass4h.ArgIn(  "un_Texture",	down_rt3v,		Sampler_LinearClamp );
			down_pass4h.Output( "out_Color",	down_rt4h );

			RC<Postprocess>		down_pass4v = Postprocess( "", "DOWNSAMPLE_VERT_BLUR" );
			down_pass4v.ArgIn(  "un_Texture",	down_rt4h,		Sampler_LinearClamp );
			down_pass4v.Output( "out_Color",	down_rt4v );

			RC<Postprocess>		down_pass5h = Postprocess( "", "DOWNSAMPLE_HOR_BLUR" );
			down_pass5h.ArgIn(  "un_Texture",	down_rt4v,		Sampler_LinearClamp );
			down_pass5h.Output( "out_Color",	down_rt5h );

			RC<Postprocess>		down_pass5v = Postprocess( "", "DOWNSAMPLE_VERT_BLUR" );
			down_pass5v.ArgIn(  "un_Texture",	down_rt5h,		Sampler_LinearClamp );
			down_pass5v.Output( "out_Color",	down_rt5v );

			RC<Postprocess>		down_pass6h = Postprocess( "", "DOWNSAMPLE_HOR_BLUR" );
			down_pass6h.ArgIn(  "un_Texture",	down_rt5v,		Sampler_LinearClamp );
			down_pass6h.Output( "out_Color",	down_rt6h );

			RC<Postprocess>		down_pass6v = Postprocess( "", "DOWNSAMPLE_VERT_BLUR" );
			down_pass6v.ArgIn(  "un_Texture",	down_rt6h,		Sampler_LinearClamp );
			down_pass6v.Output( "out_Color",	down_rt6v );


			// Upsample
			RC<Postprocess>		up_pass5 = Postprocess( "", "UPSAMPLE_PASS" );
			up_pass5.ArgIn(  "un_HalfSize",		down_rt6v,		Sampler_LinearClamp );
			up_pass5.ArgIn(  "un_FullSize",		down_rt5v,		Sampler_LinearClamp );
			up_pass5.Output( "out_Color",		up_rt5 );

			RC<Postprocess>		up_pass4 = Postprocess( "", "UPSAMPLE_PASS" );
			up_pass4.ArgIn(  "un_HalfSize",		up_rt5,			Sampler_LinearClamp );
			up_pass4.ArgIn(  "un_FullSize",		down_rt4v,		Sampler_LinearClamp );
			up_pass4.Output( "out_Color",		up_rt4 );

			RC<Postprocess>		up_pass3 = Postprocess( "", "UPSAMPLE_PASS" );
			up_pass3.ArgIn(  "un_HalfSize",		up_rt4,			Sampler_LinearClamp );
			up_pass3.ArgIn(  "un_FullSize",		down_rt3v,		Sampler_LinearClamp );
			up_pass3.Output( "out_Color",		up_rt3 );

			RC<Postprocess>		up_pass2 = Postprocess( "", "UPSAMPLE_PASS" );
			up_pass2.ArgIn(  "un_HalfSize",		up_rt3,			Sampler_LinearClamp );
			up_pass2.ArgIn(  "un_FullSize",		down_rt2v,		Sampler_LinearClamp );
			up_pass2.Output( "out_Color",		up_rt2 );

			RC<Postprocess>		up_pass1 = Postprocess( "", "UPSAMPLE_PASS" );
			up_pass1.ArgIn(  "un_HalfSize",		up_rt2,			Sampler_LinearClamp );
			up_pass1.ArgIn(  "un_FullSize",		down_rt1v,		Sampler_LinearClamp );
			up_pass1.Output( "out_Color",		up_rt1 );


			RC<Postprocess>		final_pass = Postprocess( "", "FINAL" );
			final_pass.ArgIn(  "un_Bloom",		up_rt1,		Sampler_LinearClamp );
			final_pass.ArgIn(  "un_Origin",		main_rt,	Sampler_LinearClamp );
			final_pass.Output( "out_Color",		final_rt );
			final_pass.Slider( "iToneMapping",	0, 9,	6 );
		}
		Present( final_rt );
	}

#endif
//-------------------------------------------------------------------------------------------------
#ifdef MAIN
	#include "InvocationID.glsl"
	#include "SDF.glsl"

	float4  DrawDot (float2 uv, float2 pos, float3 col, float colScale)
	{
		const float r = 0.001;
		float	f = Max( 0.0, (r - LengthSq( uv - pos )) / r );
		float4	c;	c.rgb = f * Lerp( col, float3(1.0), iWhiteFactor );	c.a = 1.0;
		return c * colScale;
	}

	float4  DrawQuad (float2 uv, float2 pos, float3 col, float colScale)
	{
		float	f = Max( 0.0, -SDF2_Rect( uv - pos, float2(0.02) ) / 0.04 );
		float4	c;	c.rgb = f * Lerp( col, float3(1.0), iWhiteFactor );	c.a = 1.0;
		return c * colScale;
	}

	float4  Blend (float4 src, float4 col)
	{
		return src + col;
	}

	void Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected();

		out_Color = float4(0.0);
		out_Color = Blend( out_Color, DrawDot( uv, float2( 0.00, 0.00), float3(1.0, 0.0, 0.0), 8.0 ));
		out_Color = Blend( out_Color, DrawDot( uv, float2( 0.05, 0.05), float3(0.0, 1.0, 0.0), 4.0 ));
		out_Color = Blend( out_Color, DrawDot( uv, float2(-0.05, 0.05), float3(0.0, 0.0, 1.0), 6.0 ));

		out_Color = Blend( out_Color, DrawDot( uv, float2( 0.6, -0.5), float3(0.0, 1.0, 0.0),  1.0 ));
		out_Color = Blend( out_Color, DrawDot( uv, float2(-0.3, -0.4), float3(0.0, 0.0, 1.0),  2.0 ));
		out_Color = Blend( out_Color, DrawDot( uv, float2(-0.7,  0.3), float3(1.0, 0.0, 1.0),  4.0 ));
		out_Color = Blend( out_Color, DrawDot( uv, float2( 0.6,  0.4), float3(1.0, 1.0, 0.0),  3.0 ));
		out_Color = Blend( out_Color, DrawDot( uv, float2( 0.8, -0.1), float3(1.0, 0.0, 0.0), 10.0 ));

		out_Color = Blend( out_Color, DrawQuad( uv, float2( 0.0, 0.5), float3(0.5, 1.0, 0.2), 6.0 ));
		out_Color *= iBrightnessScale;
	}

#endif
//-------------------------------------------------------------------------------------------------
#ifdef INIT_BLOOM
	#include "Color.glsl"

	void Main ()
	{
		float2	uv		= gl.FragCoord.xy / un_PerPass.resolution.xy;
		float4	col		= gl.texture.Sample( un_Texture, uv );
		float	luma	= RGBtoLuminance( col.rgb );
		float	factor	= Min( 1.0, luma / iMinBrightness );
		factor = Pow( factor, 4.0 );
		out_Color = factor * col;
	}

#endif
//-------------------------------------------------------------------------------------------------
#ifdef DOWNSAMPLE_HOR_BLUR
	#include "Blur.glsl"

	void Main ()
	{
		float2	uv = gl.FragCoord.xy / un_PerPass.resolution.xy;
		out_Color  = Blur13( un_Texture, uv, 1.0/un_PerPass.resolution.xy, float2(1.0, 0.0) );
	}

#endif
//-------------------------------------------------------------------------------------------------
#ifdef DOWNSAMPLE_VERT_BLUR
	#include "Blur.glsl"

	void Main ()
	{
		float2	uv = gl.FragCoord.xy / un_PerPass.resolution.xy;
		out_Color  = Blur13( un_Texture, uv, 1.0/un_PerPass.resolution.xy, float2(0.0, 1.0) );
	}

#endif
//-------------------------------------------------------------------------------------------------
#ifdef UPSAMPLE_PASS
	#include "Math.glsl"

	void Main ()
	{
		float2		uv			= gl.FragCoord.xy / un_PerPass.resolution.xy;
		float4		half_sized	= gl.texture.Sample( un_HalfSize, uv );
		float4		full_sized	= gl.texture.Sample( un_FullSize, uv );

		out_Color = Lerp( half_sized, full_sized, 0.25 );
	}

#endif
//-------------------------------------------------------------------------------------------------
#ifdef FINAL
	#include "Math.glsl"
	#include "ToneMapping.glsl"

	void Main ()
	{
		float2	uv		= gl.FragCoord.xy / un_PerPass.resolution.xy;
		float4	bloom	= gl.texture.Sample( un_Bloom, uv );
		float4	col		= gl.texture.Sample( un_Origin, uv );
		float4	hdr		= Lerp( col, bloom, 0.25 );

		switch ( iToneMapping )
		{
			case 0 :	break;
			case 1 :	hdr.rgb = ToneMap_Reinhard( hdr.rgb );		break;
			case 2 :	hdr.rgb = ToneMap_ReinhardSq( hdr.rgb );	break;
			case 3 :	hdr.rgb = ToneMap_Stanard( hdr.rgb );		break;
			case 4 :	hdr.rgb = ToneMap_ACES( hdr.rgb );			break;
			case 5 :	hdr.rgb = ToneMap_ACESFitted( hdr.rgb );	break;
			case 6 :	hdr.rgb = ToneMap_Unreal( hdr.rgb );		break;
			case 7 :	hdr.rgb = ToneMap_Uchimura( hdr.rgb );		break;
			case 8 :	hdr.rgb = Tonemap_Lottes( hdr.rgb );		break;
			case 9 :	hdr.rgb = ToneMap_whitePreservingLumaBasedReinhard( hdr.rgb );	break;
		}
		out_Color = hdr;
	}

#endif
//-----------------------------------------------------------------------------
