// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Dual filter blur with round shape.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define INIT_BLOOM
#	define DOWNSAMPLE_PASS
#	define UPSAMPLE_PASS
#	define HDR_DEBUG
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const EPixelFormat	hdr_fmt	= EPixelFormat::RGBA16F;		// HDR
	//	const EPixelFormat	hdr_fmt	= EPixelFormat::R11G11B10F;	// HDR on mobile

		RC<Image>	rt				= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );	rt.Name( "RT" );
		RC<DynamicUInt>	tex_dim		= DynamicUInt();
		RC<DynamicDim>	dim			= tex_dim.Mul( 512 ).Dimension2();

		RC<Image>	emission_hdr	= Image( hdr_fmt, dim );			emission_hdr.Name( "RT-emission" );

		RC<Image>	down_rt1		= Image( hdr_fmt, dim / 2 );		down_rt1.Name( "RT-down/2" );
		RC<Image>	down_rt2		= Image( hdr_fmt, dim / 4 );		down_rt2.Name( "RT-down/4" );
		RC<Image>	down_rt3		= Image( hdr_fmt, dim / 8 );		down_rt3.Name( "RT-down/8" );
		RC<Image>	down_rt4		= Image( hdr_fmt, dim / 16 );		down_rt4.Name( "RT-down/16" );
		RC<Image>	down_rt5		= Image( hdr_fmt, dim / 32 );		down_rt5.Name( "RT-down/32" );
		RC<Image>	down_rt6		= Image( hdr_fmt, dim / 64 );		down_rt6.Name( "RT-down/64" );

		RC<Image>	up_rt0			= Image( hdr_fmt, dim );			up_rt0.Name( "RT-up" );
		RC<Image>	up_rt1			= Image( hdr_fmt, dim / 2 );		up_rt1.Name( "RT-up/2" );
		RC<Image>	up_rt2			= Image( hdr_fmt, dim / 4 );		up_rt2.Name( "RT-up/4" );
		RC<Image>	up_rt3			= Image( hdr_fmt, dim / 8 );		up_rt3.Name( "RT-up/8" );
		RC<Image>	up_rt4			= Image( hdr_fmt, dim / 16 );		up_rt4.Name( "RT-up/16" );
		RC<Image>	up_rt5			= Image( hdr_fmt, dim / 32 );		up_rt5.Name( "RT-up/32" );

		const array<RC<Image>>	down_images = { down_rt1, down_rt2, down_rt3, down_rt4, down_rt5, down_rt6 };
		const array<RC<Image>>	up_images	= { up_rt5, up_rt4, up_rt3, up_rt2, up_rt1, up_rt0 };

		RC<DynamicUInt>		gen_image	= DynamicUInt();
		RC<DynamicUInt>		down_passes	= DynamicUInt();
		RC<DynamicUInt>		up_passes	= DynamicUInt();
		RC<DynamicFloat4>	blur_factor	= DynamicFloat4();
		RC<DynamicFloat2>	blur_factor2= DynamicFloat2();
		RC<DynamicFloat>	rgbm_range	= DynamicFloat();
		RC<DynamicFloat>	min_bright	= DynamicFloat();

		const array<float>	params = {
			0.5360f, 0.4780f, 0.3570f, 0.2820f, 	// BlurFactor
			0.0f, 0.0f,								// BlurFactor2
		};
		int	i = 0;

		Slider( tex_dim,		"TexDim",			1,				16 );
		Slider( down_passes,	"DownPasses",		0,				down_images.size(),	down_images.size() );
		Slider( up_passes,		"UpPasses",			0,				up_images.size(),	up_images.size() );
		Slider( blur_factor,	"BlurFactor",		float4(0.0),	float4(1.0),		float4(params[i], params[i+1], params[i+2], params[i+3]) );		i += 4;
		Slider( blur_factor2,	"BlurFactor2",		float2(0.0),	float2(1.0),		float2(params[i], params[i+1]) );								i += 2;
		Slider( min_bright,		"MinBrightness",	0.0,			10.0,				4.0 );
		Slider( gen_image,		"GenImage",			0,				1,					1 );

		Label( dim.XY(),		"Dimension" );

		// render loop
		{
			#if 0
				for (uint j = 0; j < down_images.size(); ++j) {
					ClearImage( down_images[j], RGBA32f(0.0) );
				}
				for (uint j = 0; j < up_images.size(); ++j) {
					ClearImage( up_images[j], RGBA32f(0.0) );
				}
			#endif

			// HDR
			{
				RC<Postprocess>		pass = Postprocess( "", "INIT_BLOOM" );
				pass.Output( "out_Color",		emission_hdr );
				pass.Slider( "iHDR",			0.0,	40.0,	20.0 );
				pass.Constant( "iMinBrightness",	min_bright );
				pass.EnableIfEqual( gen_image,	1 );
				pass.SetDebugLabel( "Init", RGBA32f(0.2) );
			}

			// Downscale
			{
				RC<Postprocess>		pass = Postprocess( "", "DOWNSAMPLE_PASS" );
				pass.ArgIn( "un_Texture",	emission_hdr,	Sampler_LinearClamp );
				pass.Output( "out_Color",	down_rt1 );
				pass.EnableIfGreater( down_passes, 0 );
				pass.SetDebugLabel( "downsample 1/2", RGBA32f(1.0, 0.0, 0.0, 1.0) );
			}{
				RC<Postprocess>		pass = Postprocess( "", "DOWNSAMPLE_PASS" );
				pass.ArgIn( "un_Texture",	down_rt1,		Sampler_LinearClamp );
				pass.Output( "out_Color",	down_rt2 );
				pass.EnableIfGreater( down_passes, 1 );
				pass.SetDebugLabel( "downsample 1/4", RGBA32f(0.5, 1.0, 0.0, 1.0) );
			}{
				RC<Postprocess>		pass = Postprocess( "", "DOWNSAMPLE_PASS" );
				pass.ArgIn( "un_Texture",	down_rt2,		Sampler_LinearClamp );
				pass.Output( "out_Color",	down_rt3 );
				pass.EnableIfGreater( down_passes, 2 );
				pass.SetDebugLabel( "downsample 1/8", RGBA32f(0.5, 1.0, 0.0, 1.0) );
			}{
				RC<Postprocess>		pass = Postprocess( "", "DOWNSAMPLE_PASS" );
				pass.ArgIn( "un_Texture",	down_rt3,		Sampler_LinearClamp );
				pass.Output( "out_Color",	down_rt4 );
				pass.EnableIfGreater( down_passes, 3 );
				pass.SetDebugLabel( "downsample 1/16", RGBA32f(0.5, 1.0, 0.0, 1.0) );
			}{
				RC<Postprocess>		pass = Postprocess( "", "DOWNSAMPLE_PASS" );
				pass.ArgIn( "un_Texture",	down_rt4,		Sampler_LinearClamp );
				pass.Output( "out_Color",	down_rt5 );
				pass.EnableIfGreater( down_passes, 4 );
				pass.SetDebugLabel( "downsample 1/32", RGBA32f(0.5, 1.0, 0.0, 1.0) );
			}{
				RC<Postprocess>		pass = Postprocess( "", "DOWNSAMPLE_PASS" );
				pass.ArgIn( "un_Texture",	down_rt5,		Sampler_LinearClamp );
				pass.Output( "out_Color",	down_rt6 );
				pass.EnableIfGreater( down_passes, 5 );
				pass.SetDebugLabel( "downsample 1/64", RGBA32f(0.5, 1.0, 0.0, 1.0) );
			}


			// Upscale
			{
				RC<Postprocess>		pass = Postprocess( "", "UPSAMPLE_PASS" );
				pass.Output( "out_Color",		up_rt5 );
				pass.ArgIn( "un_HalfSize",		down_rt6,		Sampler_LinearClamp );
				pass.ArgIn( "un_FullSize",		down_rt5,		Sampler_NearestClamp );
				pass.Constant( "iBlurFactor",	blur_factor.X() );
				pass.EnableIfGreater( up_passes, 0 );
				pass.SetDebugLabel( "upsample 1/32", RGBA32f(0.0, 0.5, 1.0, 1.0) );
			}{
				RC<Postprocess>		pass = Postprocess( "", "UPSAMPLE_PASS" );
				pass.Output( "out_Color",		up_rt4 );
				pass.ArgIn( "un_HalfSize",		up_rt5,			Sampler_LinearClamp );
				pass.ArgIn( "un_FullSize",		down_rt4,		Sampler_NearestClamp );
				pass.Constant( "iBlurFactor",	blur_factor.Y() );
				pass.EnableIfGreater( up_passes, 1 );
				pass.SetDebugLabel( "upsample 1/16", RGBA32f(0.0, 0.5, 1.0, 1.0) );
			}{
				RC<Postprocess>		pass = Postprocess( "", "UPSAMPLE_PASS" );
				pass.Output( "out_Color",		up_rt3 );
				pass.ArgIn( "un_HalfSize",		up_rt4,			Sampler_LinearClamp );
				pass.ArgIn( "un_FullSize",		down_rt3,		Sampler_NearestClamp );
				pass.Constant( "iBlurFactor",	blur_factor.Z() );
				pass.EnableIfGreater( up_passes, 2 );
				pass.SetDebugLabel( "upsample 1/8", RGBA32f(0.0, 0.5, 1.0, 1.0) );
			}{
				RC<Postprocess>		pass = Postprocess( "", "UPSAMPLE_PASS" );
				pass.Output( "out_Color",		up_rt2 );
				pass.ArgIn( "un_HalfSize",		up_rt3,			Sampler_LinearClamp );
				pass.ArgIn( "un_FullSize",		down_rt2,		Sampler_NearestClamp );
				pass.Constant( "iBlurFactor",	blur_factor.W() );
				pass.EnableIfGreater( up_passes, 3 );
				pass.SetDebugLabel( "upsample 1/4", RGBA32f(0.0, 0.5, 1.0, 1.0) );
			}{
				RC<Postprocess>		pass = Postprocess( "", "UPSAMPLE_PASS" );
				pass.Output( "out_Color",		up_rt1 );
				pass.ArgIn( "un_HalfSize",		up_rt2,			Sampler_LinearClamp );
				pass.ArgIn( "un_FullSize",		down_rt1,		Sampler_NearestClamp );
				pass.Constant( "iBlurFactor",	blur_factor2.X() );
				pass.EnableIfGreater( up_passes, 4 );
				pass.SetDebugLabel( "upsample 1/2", RGBA32f(0.0, 0.5, 1.0, 1.0) );
			}{
				RC<Postprocess>		pass = Postprocess( "", "UPSAMPLE_PASS" );
				pass.Output( "out_Color",		up_rt0 );
				pass.ArgIn( "un_HalfSize",		up_rt1,			Sampler_LinearClamp );
				pass.ArgIn( "un_FullSize",		emission_hdr,	Sampler_NearestClamp );
				pass.Constant( "iBlurFactor",	blur_factor2.Y() );
				pass.EnableIfGreater( up_passes, 5 );
				pass.SetDebugLabel( "upsample 1/1", RGBA32f(0.0, 1.0, 0.0, 1.0) );
			}
		}

		// Final
		#if 1
			{
				RC<Postprocess>		pass = Postprocess( "", "HDR_DEBUG" );
				pass.Output( "out_Color",		 rt );
				pass.ArgIn( "un_Emission",		emission_hdr,	Sampler_LinearClamp );
				pass.ArgIn( "un_DownImages",	down_images,	Sampler_LinearClamp );
				pass.ArgIn( "un_UpImages",		up_images,		Sampler_LinearClamp );
				pass.Constant( "iDownPass",		down_passes );
				pass.Constant( "iUpPass",		up_passes );
				pass.SetDebugLabel( "Final", RGBA32f(0.3) );
			}
			Present( rt );
		#endif
	}

#endif
//-------------------------------------------------------------------------------------------------
#ifdef INIT_BLOOM
	#include "SDF.glsl"
	#include "Color.glsl"
	#include "ColorSpace.glsl"
	#include "InvocationID.glsl"

	float3  DrawDot (float2 uv, float2 pos, float3 col, float colScale)
	{
		const float r = 0.001;
		float	f = Max( 0.0, (r - LengthSq( uv - pos )) / r );
		return f * col * colScale;
	}

	float3  DrawQuad (float2 uv, float2 pos, float3 col, float colScale)
	{
		float	f = Max( 0.0, -SDF2_Rect( uv - pos, float2(0.02) ) / 0.04 );
		return f * col * colScale;
	}

	float3  Blend (float3 src, float3 col)
	{
		return src + col;
	}

	void Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected();

		float3	color = float3(0.0);
		color = Blend( color, DrawDot( uv, float2( 0.00, 0.00), float3(1.0, 0.0, 0.0), 8.0 ));
		color = Blend( color, DrawDot( uv, float2( 0.05, 0.05), float3(0.0, 1.0, 0.0), 4.0 ));
		color = Blend( color, DrawDot( uv, float2(-0.05, 0.05), float3(0.0, 0.0, 1.0), 6.0 ));

		color = Blend( color, DrawDot( uv, float2( 0.6, -0.5), float3(0.0, 1.0, 0.0),  1.0 ));
		color = Blend( color, DrawDot( uv, float2(-0.3, -0.4), float3(0.0, 0.0, 1.0),  2.0 ));
		color = Blend( color, DrawDot( uv, float2(-0.7,  0.3), float3(1.0, 0.0, 1.0),  4.0 ));
		color = Blend( color, DrawDot( uv, float2( 0.6,  0.4), float3(1.0, 1.0, 0.0),  3.0 ));
		color = Blend( color, DrawDot( uv, float2( 0.8, -0.1), float3(1.0, 0.0, 0.0), 10.0 ));

		color = Blend( color, DrawQuad( uv, float2( 0.0, 0.5), float3(0.5, 1.0, 0.2), 6.0 ));

		float	luma		= RGBtoLuminance( color );
		float	factor		= Min( 1.0, luma / iMinBrightness );

		color *= Pow( factor, 2.0 );
		out_Color = float4( color * iHDR, 1.0 );
	}

#endif
//-------------------------------------------------------------------------------------------------
#if 0
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void Main ()
	{
		float2	uv	= GetGlobalCoordSNormCorrected() * 111.0;

		float3	color = float3(0.0);

		if ( DHash12( uv ) > 0.75 )
			color = DHash32( uv );

		float	luma		= RGBtoLuminance( color );
		float	factor		= Min( 1.0, luma / iMinBrightness );

		color *= Pow( factor, 2.0 );
		out_Color = float4( color * iHDR, 1.0 );
	}

#endif
//-------------------------------------------------------------------------------------------------
#ifdef DOWNSAMPLE_PASS
	#include "Blur.glsl"

	void  Main ()
	{
		float2	uv	= gl.FragCoord.xy * un_PerPass.invResolution;
		float2	off = un_PerPass.invResolution * 0.5;

		float4	c0	= gl.texture.Sample( un_Texture, uv ) * 0.5;

		float4	c1	= gl.texture.Sample( un_Texture, uv + float2(-off.x, -off.y) );
		float4	c2	= gl.texture.Sample( un_Texture, uv + float2( off.x, -off.y) );
		float4	c3	= gl.texture.Sample( un_Texture, uv + float2(-off.x,  off.y) );
		float4	c4	= gl.texture.Sample( un_Texture, uv + float2( off.x,  off.y) );

		out_Color = c0 + (c1 + c2 + c3 + c4) * (1.0/8.0);
	}

#endif
//-------------------------------------------------------------------------------------------------
#ifdef UPSAMPLE_PASS
	#include "Math.glsl"

	void  Main ()
	{
		float2		uv			= gl.FragCoord.xy * un_PerPass.invResolution;
		float2		off1		= un_PerPass.invResolution * 2.0 * 1.1;
		float2		off2		= un_PerPass.invResolution * 4.0 * 0.9;
		float3		half_sized	= float3(0.0);
		float3		full_sized	= gl.texture.Sample( un_FullSize, uv ).rgb;

		// filter
		{
			float3	c0 = gl.texture.Sample( un_HalfSize, uv ).rgb * 2.0;

			float3	c1;
			c1  = gl.texture.Sample( un_HalfSize, uv + float2(-off1.x, -off1.y) ).rgb;
			c1 += gl.texture.Sample( un_HalfSize, uv + float2( off1.x, -off1.y) ).rgb;
			c1 += gl.texture.Sample( un_HalfSize, uv + float2(-off1.x,  off1.y) ).rgb;
			c1 += gl.texture.Sample( un_HalfSize, uv + float2( off1.x,  off1.y) ).rgb;

			float3	c2;
			c2  = gl.texture.Sample( un_HalfSize, uv + float2( 0.0,    -off2.y) ).rgb;
			c2 += gl.texture.Sample( un_HalfSize, uv + float2(-off2.x,   0.0  ) ).rgb;
			c2 += gl.texture.Sample( un_HalfSize, uv + float2( off2.x,   0.0  ) ).rgb;
			c2 += gl.texture.Sample( un_HalfSize, uv + float2( 0.0,     off2.y) ).rgb;

			half_sized = c0 * (1.0/14.0) + c1 * (2.0/14.0) + c2 * (1.0/14.0);
		}

		out_Color = float4( Lerp( half_sized, full_sized, iBlurFactor ), 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef HDR_DEBUG
	#include "InvocationID.glsl"
	#include "ColorSpace.glsl"

	void  Main ()
	{
		float2	uv = MapPixCoordToUNormCorrected(
						gl.FragCoord.xy,	// with subpixel offset
						un_PerPass.resolution.xy,
						float2(gl.texture.GetSize( un_Emission, 0 ))
					  );

		out_Color = float4(0.0);

		if ( IsUNorm( uv ))
		{
			if ( iDownPass == 0 )
			{
				out_Color = gl.texture.Sample( un_Emission, uv );
			}
			else
			if ( iUpPass > 0 and iUpPass <= un_UpImages.length() )
			{
				out_Color = gl.texture.Sample( un_UpImages[iUpPass-1], uv );
			}
			else
			if ( iDownPass > 0 and iDownPass <= un_DownImages.length() )
			{
				out_Color = gl.texture.Sample( un_DownImages[iDownPass-1], uv );
			}

			out_Color.rgb = ApplySRGBCurve( out_Color.rgb );
			out_Color.a   = 1.0;
		}
	}

#endif
//-----------------------------------------------------------------------------
