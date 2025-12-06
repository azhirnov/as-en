// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Light Shafts like in Horizon.

	v1: single pass radial blur.
	v2: two pass radial blur with less number of samples.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#	define RADIAL_BLUR_V1
#	define RADIAL_BLUR_V2_1
#	define RADIAL_BLUR_V2_2
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void  ASmain ()
	{
		// initialize
		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>			light_mask	= Image( EImageType::Float_2D, "res/hzd/light.dds" );
		RC<Image>			scene_color	= Image( EImageType::Float_2D, "res/hzd/color.dds" );
		RC<Image>			ref_blur	= Image( EImageType::Float_2D, "res/hzd/light-shaft.dds" );
		RC<Image>			blur		= Image( EPixelFormat::RGBA16F, light_mask.Dimension() );
		RC<Image>			blur2		= Image( EPixelFormat::RGBA16F, light_mask.Dimension() );
		RC<Image>			blur3		= Image( EPixelFormat::RGBA16F, light_mask.Dimension() );
		RC<DynamicFloat2>	center		= DynamicFloat2();

		Slider( center,		"Center",	float2(0.0),	float2(1.0),	float2(0.612, 0.375) );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "RADIAL_BLUR_V1" );
			pass.Output(	"out_Color",		blur );
			pass.ArgIn(		"un_LightMask",		light_mask,		Sampler_LinearClamp );
			pass.Slider(	"iSampleCount",		8,				128,			64 );
			pass.Slider(	"iBlur",			0.0,			1.0,			0.3 );
			pass.Constant(	"iCenter",			center );
		}{
			RC<Postprocess>		pass = Postprocess( "", "RADIAL_BLUR_V2_1" );
			pass.Output(	"out_Color",		blur2 );
			pass.ArgIn(		"un_LightMask",		light_mask,		Sampler_LinearClamp );
			pass.Slider(	"iSampleCount",		2,				32,				8 );
			pass.Slider(	"iBlur",			0.0,			1.0,			0.15 );
			pass.Constant(	"iCenter",			center );
		}{
			RC<Postprocess>		pass = Postprocess( "", "RADIAL_BLUR_V2_2" );
			pass.Output(	"out_Color",		blur3 );
			pass.ArgIn(		"un_SrcImage",		blur2,			Sampler_LinearClamp );
			pass.Slider(	"iSampleCount",		2,				32,				16 );
			pass.Slider(	"iBlur",			0.0,			1.0,			0.2 );
			pass.Slider(	"iAttenuation",		0,				1 );
			pass.Constant(	"iCenter",			center );
		}{
			RC<Postprocess>		pass = Postprocess( "", "VIEW" );
			pass.Output( "out_Color",		rt );
			pass.ArgIn(  "un_Blur",			blur,			Sampler_LinearClamp );
			pass.ArgIn(  "un_Blur2",		blur2,			Sampler_LinearClamp );
			pass.ArgIn(  "un_Blur3",		blur3,			Sampler_LinearClamp );
			pass.ArgIn(  "un_LightMask",	light_mask,		Sampler_NearestClamp );
			pass.ArgIn(  "un_Color",		scene_color,	Sampler_NearestClamp );
			pass.ArgIn(  "un_RefBlur",		ref_blur,		Sampler_LinearClamp );
			pass.Slider( "iColScale",		1.0,	10.0,	1.0 );
			pass.Slider( "iUVScale",		0.5,	3.0,	1.5 );
			pass.Slider( "iDbgView",		0,		6,		4 );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RADIAL_BLUR_V1
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv	= GetGlobalCoordUNorm().xy;
		float2	duv	= (iCenter - uv) * iBlur / iSampleCount;
		float3	col	= float3(0.0);

		for (uint i = 0; i < iSampleCount; ++i)
		{
			col += gl.texture.Sample( un_LightMask, uv ).rgb;
			uv  += duv;
		}
		out_Color.rgb = col / iSampleCount;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RADIAL_BLUR_V2_1
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv	= GetGlobalCoordUNorm().xy;
		float2	duv	= (iCenter - uv) * iBlur / iSampleCount;
		float3	col	= float3(0.0);

		for (uint i = 0; i < iSampleCount; ++i)
		{
			col += gl.texture.Sample( un_LightMask, uv ).rgb;
			uv  += duv;
		}
		out_Color.rgb = col / iSampleCount;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef RADIAL_BLUR_V2_2
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv	= GetGlobalCoordUNorm().xy;
		float2	duv	= (iCenter - uv) * iBlur / iSampleCount;
		float3	col	= float3(0.0);

		for (uint i = 0; i < iSampleCount; ++i)
		{
			col += gl.texture.Sample( un_SrcImage, uv ).rgb;
			uv  += duv;
		}
		out_Color.rgb = col / iSampleCount;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW
	#include "Blur.glsl"
	#include "InvocationID.glsl"


	void  Main ()
	{
		float2	uv = MapPixCoordToUNormCorrected(
						gl.FragCoord.xy,	// with subpixel offset
						un_PerPass.resolution.xy,
						float2(gl.texture.GetSize( un_Blur, 0 )),
						1.0/iUVScale
					  );

		out_Color = float4(0.0);
		out_Color.a = 1.0;

		if ( IsUNorm( uv ))
		{
			switch ( iDbgView )
			{
				case 0 :	out_Color.rgb = gl.texture.Sample( un_LightMask, uv ).rgb;		break;
				case 1 :	out_Color.rgb = gl.texture.Sample( un_Blur, uv ).rgb;			break;
				case 2 :	out_Color.rgb = RoundDualFilterBlurPass2( un_Blur, uv ).rgb;	break;
				case 3 :	out_Color.rgb = gl.texture.Sample( un_Blur2, uv ).rgb;			break;
				case 4 :	out_Color.rgb = gl.texture.Sample( un_Blur3, uv ).rgb;			break;
				case 5 :	out_Color.rgb = gl.texture.Sample( un_RefBlur, uv ).rgb * 150;	break;
				case 6 :	out_Color.rgb = gl.texture.Sample( un_Color, uv ).rgb * 100.0;	break;
			}
			out_Color.rgb *= iColScale;
		}
	}

#endif
//-----------------------------------------------------------------------------
