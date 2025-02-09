// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Simple fragment shader.
	Demonstrates how to write shaders as in shadertoy.
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
			RC<Postprocess>		pass = Postprocess( EPostprocess::Shadertoy );
			pass.Output( rt );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void mainImage (out float4 fragColor, in float2 fragCoord)
	{
		// default shader from https://www.shadertoy.com/new

		float2	uv = fragCoord / iResolution.xy;

		float3	col = 0.5 + 0.5 * Cos( iTime + uv.xyx + float3(0,2,4) );

		fragColor = float4(col, 1.0);
	}

#endif
//-----------------------------------------------------------------------------
