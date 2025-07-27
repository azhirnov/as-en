// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Generate image and compare compression algorithms.
*/
#ifdef __INTELLISENSE__
#	define SH_COMPUTE
# 	include <res_editor.as>
#	include <glsl.h>
#	define GEN_IMAGE
#	define COMPARE
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const uint2			dim			= uint2(512);
	//	const EPixelFormat	src_fmt		= EPixelFormat::RGBA16F;
		const EPixelFormat	src_fmt		= EPixelFormat::RGBA8_UNorm;

		const EPixelFormat	comp1_req	= EPixelFormat::ASTC_RGBA8_4x4;
		const EPixelFormat	comp2_req	= EPixelFormat::ASTC_RGBA8_8x8;

		const EPixelFormat	comp1_fmt	= Supports_LinearSampledFormat( comp1_req ) ? comp1_req : src_fmt;
		const EPixelFormat	comp2_fmt	= Supports_LinearSampledFormat( comp2_req ) ? comp2_req : src_fmt;
	//	const EPixelFormat	comp1_fmt	= src_fmt;
	//	const EPixelFormat	comp2_fmt	= src_fmt;

		RC<Image>	rt			= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<Image>	non_comp	= Image( src_fmt, dim );
		RC<Image>	comp1		= Image( comp1_fmt, dim );
		RC<Image>	comp2		= Image( comp2_fmt, dim );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( "", "GEN_IMAGE" );
			pass.Output( "out_Color",	non_comp );
			pass.Slider( "iMode",		0,		2,		0 );
		}{
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
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef GEN_IMAGE
	#include "Hash.glsl"
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv = GetGlobalCoordUNorm().xy;

		switch ( iMode )
		{
			case 0 :	out_Color = float4(uv, 0.f, 1.f);				break;
			case 1 :	out_Color = Rainbow( DHash12( uv * 100.0 ));	break;
			case 2 :	out_Color = float4( ToUNorm( Cos( un_PerPass.time * 0.1 + uv.xyx + float3(0,2,4) )), 1.f );	break;
		}
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
		float4	col1	= Swizzle( gl.texture.Sample( un_NonComp, uv ));
		float4	col2	= Swizzle( gl.texture.Sample( un_Comp1, uv ));
		float4	col3	= Swizzle( gl.texture.Sample( un_Comp2, uv ));

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
