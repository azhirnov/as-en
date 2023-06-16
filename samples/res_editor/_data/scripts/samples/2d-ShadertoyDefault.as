// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor>
#	include <aestyle.glsl.h>
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

	void mainImage (out float4 fragColor, in float2 fragCoord)
	{
		// Normalized pixel coordinates (from 0 to 1)
		float2 uv = fragCoord / iResolution.xy;

		// Time varying pixel color
		float3 col = 0.5 + 0.5*cos(iTime+uv.xyx + float3(0,2,4));

		// Output to screen
		fragColor = float4(col,1.0);
	}

#endif
//-----------------------------------------------------------------------------
