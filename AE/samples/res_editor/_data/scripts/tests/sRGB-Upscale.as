// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Difference between texture filtering in linear space and sRGB space.

	left	- sRGB space (incorrect)
	right	- linear space (gamma correct)

	There are 2 correct ways:
	  1. RGBA8_UNorm format: convert sRGB to linear and store in image, linear filter applied in linear space.
	  2. sRGB format: write linear space to image, it will stored in sRGB, linear filter applied in linear space.

	sRGB image:
	  * write in linear space, store in sRGB - better packing in 8 bit per channel.
	  * read in linear space, you get exactly the same color what you previously write.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_TEX
#	define VIEW_TEX
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const uint2		dim		= uint2(8) * 4;
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );	// present as is, sRGB conversion must be applied in shader
		RC<Image>		tex0	= Image( EPixelFormat::RGBA8_UNorm, dim );
		RC<Image>		tex1	= Image( EPixelFormat::sRGB8_A8,	dim );
		RC<Image>		tex2	= Image( EPixelFormat::RGBA8_UNorm, dim );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_TEX" );
			pass.Output( "out_Color0",	tex0 );
			pass.Output( "out_Color1",	tex1 );
			pass.Output( "out_Color2",	tex2 );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW_TEX" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn(  "un_Tex0",		tex0,	Sampler_LinearRepeat );
			pass.ArgIn(  "un_Tex1",		tex1,	Sampler_LinearRepeat );
			pass.ArgIn(  "un_Tex2",		tex2,	Sampler_LinearRepeat );
			pass.Slider( "iMode",		0,		3 );	// split, correct, incorrect, diff
			pass.Slider( "iTex",		0,		2 );	// use 'un_Tex0', 'un_Tex1', 'un_Tex2'
			pass.Slider( "iDiff",		1.f,	100.f,		10.f );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_TEX
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	void Main ()
	{
		int2	coord		= GetGlobalCoord().xy / 4;
		float4	srgb_col	= Rainbow( DHash12( float2(coord) ));	// sRGB color space
		float4	linear_col	= RemoveSRGBCurve( srgb_col );			// linear color space

		out_Color0	= srgb_col;		// store in sRGB
		out_Color1	= linear_col;	// write in linear space, store in sRGB
		out_Color2	= linear_col;	// store in linear space
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW_TEX
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	float4x4  TexSample2 (gl::CombinedTex2D<float> tex, inout float2 uv)
	{
		int2		size	= gl.texture.GetSize( tex, 0 );
		float2		p		= Mod( uv * float2(size) - 0.5, float2(size) );
		int2		i		= int2(p);
					uv		= Fract( p );
		float4x4	col;

		col[0] = gl.texture.Fetch( tex, (i + int2(0,0)) % size, 0 );
		col[1] = gl.texture.Fetch( tex, (i + int2(0,1)) % size, 0 );
		col[2] = gl.texture.Fetch( tex, (i + int2(1,0)) % size, 0 );
		col[3] = gl.texture.Fetch( tex, (i + int2(1,1)) % size, 0 );
		return col;
	}


	float4  TexSample (gl::CombinedTex2D<float> tex, float2 uv)
	{
	  #if 0
		float4x4	c = TexSample2( tex, INOUT uv );
		c[0] = Lerp( c[0], c[2], uv.x );
		c[1] = Lerp( c[1], c[3], uv.x );
		return Lerp( c[0], c[1], uv.y );
	  #else
		return gl.texture.Sample( tex, uv );
	  #endif
	}


	// gamma-correct filtering (upscale)
	float4  TexSample_correct (float2 uv)
	{
		if ( iTex == 0 )
		{
			float4x4	c = TexSample2( un_Tex0, INOUT uv );	// store in sRGB, read in sRGB

			c[0] = RemoveSRGBCurve( c[0] );						// convert to linear space
			c[1] = RemoveSRGBCurve( c[1] );
			c[2] = RemoveSRGBCurve( c[2] );
			c[3] = RemoveSRGBCurve( c[3] );

			c[0] = Lerp( c[0], c[2], uv.x );					// filter in linear space
			c[1] = Lerp( c[1], c[3], uv.x );
			c[2] = Lerp( c[0], c[1], uv.y );

			return ApplySRGBCurve( c[2] );						// convert to sRGB
		}
		else
		if ( iTex == 1 )
		{
			float4 c = gl.texture.Sample( un_Tex1, uv );		// store in sRGB, read in linear space, filter in linear space
			return ApplySRGBCurve( c );							// convert to sRGB, not needed only if swapchain has sRGB format
		}
		else
		if ( iTex == 2 )
		{
			float4 c = gl.texture.Sample( un_Tex2, uv );		// store in linear space, filter in linear space
			return ApplySRGBCurve( c );							// convert to sRGB
		}
	}


	void Main ()
	{
		float2	uv = GetGlobalCoordUNorm().xy;

		if ( iMode == 0 )
		{
			uv.y *= GetGlobalSize().y / (float(GetGlobalSize().x) * 0.5);

			if ( Abs( uv.x - 0.5 ) < 0.002 )
			{
				out_Color = float4(0.0);
			}
			else
			if ( uv.x > 0.5 )
			{
				uv.x = (uv.x - 0.5) * 2.0;
				out_Color = TexSample_correct( uv );
			}
			else
			{
				uv.x = uv.x * 2.0;
				out_Color = TexSample( un_Tex0, uv );
			}
		}

		if ( iMode == 1 )
			out_Color = TexSample_correct( uv );

		if ( iMode == 2 )
			out_Color = TexSample( un_Tex0, uv );

		if ( iMode == 3 )
			out_Color = Abs( TexSample_correct( uv ) - TexSample( un_Tex0, uv )) * iDiff;

		out_Color.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
