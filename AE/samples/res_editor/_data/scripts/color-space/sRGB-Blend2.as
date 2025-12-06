// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Difference between linear space blending and sRGB space blending.

	top to bottom:
	  1. sRGB space blending (incorrect)
	  2. linear space blending (correct), linear space render target with 8 bits per channel.
	  3. linear space blending (correct), sRGB render target with better accuracy.
	  4. linear space blending (correct), linear space render target with half-float format.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define PASS1
#	define PASS2
#	define PASS3
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt		= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );	// present as is
		RC<Image>	rt_0	= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );
		RC<Image>	rt_1	= Image( EPixelFormat::sRGB8_A8,	SurfaceSize() );
		RC<Image>	rt_2	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	rt_3	= Image( EPixelFormat::RGBA16F,		SurfaceSize() );
		RC<Buffer>	cbuf	= Buffer();

		// color selector
		{
			cbuf.UseLayout(
				"CBuffer",
				"float4		colorA;" +
				"float4		colorB;"
			);

			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_Buffer",	cbuf );
			pass.ColorSelector( "iColorA",	RGBA32f(0.9f, 0.6f, 0.1f, 1.f) );
			pass.ColorSelector( "iColorB",	RGBA32f(0.2f, 0.5f, 0.7f, 1.f) );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
		}

		// in linear space
		{
			RC<Postprocess>		pass = Postprocess( "", "PASS1;SRGB" );
			pass.Output( "out_Color",	rt_0 );
			pass.ArgIn(  "un_CBuf",		cbuf );
		}{
			RC<Postprocess>		pass = Postprocess( "", "PASS2;SRGB" );
			pass.OutputBlend( "out_Color",	rt_0, EBlendFactor::One, EBlendFactor::OneMinusSrcAlpha, EBlendOp::Add );
			pass.ArgIn(  "un_CBuf",		cbuf );
		}

		// in linear space, stored in sRGB
		{
			RC<Postprocess>		pass = Postprocess( "", "PASS1;SRGB" );
			pass.Output( "out_Color",	rt_1 );
			pass.ArgIn(  "un_CBuf",		cbuf );
		}{
			RC<Postprocess>		pass = Postprocess( "", "PASS2;SRGB" );
			pass.OutputBlend( "out_Color",	rt_1, EBlendFactor::One, EBlendFactor::OneMinusSrcAlpha, EBlendOp::Add );
			pass.ArgIn(  "un_CBuf",		cbuf );
		}

		// in sRGB space
		{
			RC<Postprocess>		pass = Postprocess( "", "PASS1" );
			pass.Output( "out_Color",	rt_2 );
			pass.ArgIn(  "un_CBuf",		cbuf );
		}{
			RC<Postprocess>		pass = Postprocess( "", "PASS2" );
			pass.OutputBlend( "out_Color",	rt_2, EBlendFactor::One, EBlendFactor::OneMinusSrcAlpha, EBlendOp::Add );
			pass.ArgIn(  "un_CBuf",		cbuf );
		}

		// in linear space, stored in half-float
		{
			RC<Postprocess>		pass = Postprocess( "", "PASS1;SRGB" );
			pass.Output( "out_Color",	rt_3 );
			pass.ArgIn(  "un_CBuf",		cbuf );
		}{
			RC<Postprocess>		pass = Postprocess( "", "PASS2;SRGB" );
			pass.OutputBlend( "out_Color",	rt_3, EBlendFactor::One, EBlendFactor::OneMinusSrcAlpha, EBlendOp::Add );
			pass.ArgIn(  "un_CBuf",		cbuf );
		}

		// final
		{
			RC<Postprocess>		pass = Postprocess( "", "PASS3" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Tex0",		rt_0,	Sampler_NearestClamp );
			pass.ArgIn(  "un_Tex1",		rt_1,	Sampler_NearestClamp );
			pass.ArgIn(  "un_Tex2",		rt_2,	Sampler_NearestClamp );
			pass.ArgIn(  "un_Tex3",		rt_3,	Sampler_NearestClamp );
			pass.Slider( "iCmp",		0,		4 );
			pass.Slider( "iDiff",		1.f,	500.f,	10.f );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PASS1
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	void Main ()
	{
		float	x = GetGlobalCoordUNorm().x;
	  #ifdef SRGB
		out_Color = RemoveSRGBCurve( un_CBuf.colorA );
	  #else
		out_Color = un_CBuf.colorA;
	  #endif
		out_Color.a	*= 1.0 - LinearStep( x, 0.25, 0.75 );
		out_Color.rgb *= out_Color.a;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PASS2
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	void Main ()
	{
		float	x = GetGlobalCoordUNorm().x;
	  #ifdef SRGB
		out_Color = RemoveSRGBCurve( un_CBuf.colorB );
	  #else
		out_Color = un_CBuf.colorB;
	  #endif
		out_Color.a *= LinearStep( x, 0.25, 0.75 );
		out_Color.rgb *= out_Color.a;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef PASS3
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	// texture already in sRGB space
	float4  SampleSRGB (float2 uv)
	{
		return gl.texture.Sample( un_Tex2, uv );
	}

	// texture in linear space and must be converted to sRGB space
	float4  SampleLinear (gl::CombinedTex2D<float> tex, float2 uv)
	{
		return ApplySRGBCurve( gl.texture.Sample( tex, uv ));
	}


	void Main ()
	{
		float2	uv = GetGlobalCoordUNorm().xy;

		if ( iCmp == 0 )
		{
			if ( Abs( uv.y - 0.25 ) < 0.001 or
				 Abs( uv.y - 0.50 ) < 0.001 or
				 Abs( uv.y - 0.75 ) < 0.001 )
				out_Color = float4(0.0);
			else
			if ( uv.y < 0.25 )
				out_Color = SampleSRGB( uv );
			else
			if ( uv.y < 0.5 )
				out_Color = SampleLinear( un_Tex0, uv );
			else
			if ( uv.y < 0.75 )
				out_Color = SampleLinear( un_Tex1, uv );	// same result with better accuracy
			else
				out_Color = SampleLinear( un_Tex3, uv );
		}


		if ( iCmp == 1 )
			out_Color = Abs( SampleLinear( un_Tex0, uv ) - SampleSRGB( uv )) * iDiff;

		if ( iCmp == 2 )
			out_Color = Abs( SampleLinear( un_Tex1, uv ) - SampleSRGB( uv )) * iDiff;

		if ( iCmp == 3 )
			out_Color = Abs( SampleLinear( un_Tex1, uv ) - SampleLinear( un_Tex0, uv ) ) * iDiff;

		if ( iCmp == 4 )
			out_Color = Abs( SampleLinear( un_Tex1, uv ) - SampleLinear( un_Tex3, uv ) ) * iDiff;

		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE

	void Main ()
	{
		un_Buffer.colorA	= iColorA;
		un_Buffer.colorB	= iColorB;
	}

#endif
//-----------------------------------------------------------------------------
