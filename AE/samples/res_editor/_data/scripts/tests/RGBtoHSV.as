// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	for screenshot test
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.Slider( "iCmp",	0,	1,		1 );
			pass.Slider( "iDiff",	0,	10,		3 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Color.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv	= GetGlobalCoordUNorm().xy;
		float3	c0	= float3( uv, Fract( un_PerPass.time * 0.1 )).xzy;
		float3	c1	= HSVtoRGB( c0 );
		float3	c2	= RGBtoHSV( c1 );
		float3	dc	= Abs( c0 - c2 ) * Exp10(float(iDiff));

		if ( iCmp == 0 )
			out_Color = float4(c1, 1.0);
		else
			out_Color = float4(dc, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
