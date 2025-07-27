// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Load 2D image from file and compare different compression methods.
*/
#ifdef __INTELLISENSE__
#	define SH_COMPUTE
# 	include <res_editor.as>
#	include <glsl.h>
#	define COMPARE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const EPixelFormat	src_fmt		= EPixelFormat::RGBA8_UNorm;

		const EPixelFormat	comp1_req	= EPixelFormat::BC7_RGBA8_UNorm;
		const EPixelFormat	comp2_req	= EPixelFormat::ASTC_RGBA8_8x8;

		const EPixelFormat	comp1_fmt	= Supports_LinearSampledFormat( comp1_req ) ? comp1_req : src_fmt;
		const EPixelFormat	comp2_fmt	= Supports_LinearSampledFormat( comp2_req ) ? comp2_req : src_fmt;

		RC<Image>	rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	non_comp	= Image( EImageType::Float_2D, "shadertoy/Abstract_1.jpg" );
		RC<Image>	comp1		= Image( comp1_fmt, non_comp.Dimension() );
		RC<Image>	comp2		= Image( comp2_fmt, non_comp.Dimension() );

		// render loop
		{
			CompressImage( non_comp, comp1, comp1_req );
			CompressImage( non_comp, comp2, comp2_req );
		}{
			RC<Postprocess>		pass = Postprocess( "", "COMPARE" );
			pass.Output( "out_Color",	rt );
			pass.ArgIn( "un_NonComp",	non_comp,	Sampler_NearestClamp );
			pass.ArgIn( "un_Comp1",		comp1,		Sampler_NearestClamp );
			pass.ArgIn( "un_Comp2",		comp2,		Sampler_NearestClamp );
			pass.Slider( "iCmp",		0,		5,		4 );
			pass.Slider( "iDiff",		1.f,	100.f,	10.f );
			pass.Slider( "iChannel",	0,		4,		0 );
			pass.Slider( "iLevel",		0,		16 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef COMPARE
	#include "InvocationID.glsl"

	float4  Swizzle (float4 c)
	{
		switch ( iChannel )
		{
			case 0 :	return c;
			case 1 :	return float4(c.r);
			case 2 :	return float4(c.g);
			case 3 :	return float4(c.b);
			case 4 :	return float4(c.a);
		}
	}

	void  Main ()
	{
		float2	uv		= MapPixCoordToUNormCorrected( gl.FragCoord.xy, un_PerPass.resolution.xy, float2(gl.texture.GetSize( un_NonComp, 0 )) );
		float4	col1	= Swizzle( gl.texture.SampleLod( un_NonComp, uv, iLevel ));
		float4	col2	= Swizzle( gl.texture.SampleLod( un_Comp1, uv, iLevel ));
		float4	col3	= Swizzle( gl.texture.SampleLod( un_Comp2, uv, iLevel ));

		if ( IsNotUNorm( uv ))
		{
			out_Color = float4(0.0);
			return;
		}

		switch ( iCmp )
		{
			case 0 :	out_Color = col1;	break;
			case 1 :	out_Color = col2;	break;
			case 2 :	out_Color = col3;	break;

			case 3 :	out_Color = Abs( col1 - col2 ) * iDiff;	break;
			case 4 :	out_Color = Abs( col1 - col3 ) * iDiff;	break;
			case 5 :	out_Color = Abs( col2 - col3 ) * iDiff;	break;
		}
	}

#endif
//-----------------------------------------------------------------------------
