// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Shows how to hide steps/lanes in gradient in 8 bit per channel.
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
			pass.Slider( "iRange",		float2(0.0),	float2(0.5, 0.1),	float2(0.3, 0.1) );
			pass.Slider( "iHashScale",	0.0,			0.05,				0.005 );
			pass.Slider( "iTimeScale",	0.0,			5.0,				1.0 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Hash.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv		= GetGlobalCoordUNorm().xy;
		float3	color	= float3(iRange.x + iRange.y * uv.x);
		float3	hash	= DHash32( Floor(gl.FragCoord.xy * 0.5) * 10.0 + un_PerPass.time * iTimeScale * 0.001 );

		if ( uv.y > 0.5 )
			color += hash * iHashScale;

		out_Color = float4(color, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
