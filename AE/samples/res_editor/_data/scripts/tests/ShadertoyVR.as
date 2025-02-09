// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Demonstrates how to write VR shaders as in shadertoy.
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
		RC<Image>		rt		= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );
		RC<FPVCamera>	camera	= FPVCamera();

		// setup camera
		{
			camera.ClipPlanes( 0.1f, 10.f );
			camera.FovY( 70.f );
		}

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( EPostprocess::ShadertoyVR );
			pass.Output( rt );
			pass.Set( camera );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void mainVR (out float4 fragColor, in float2 fragCoord, in float3 fragRayOri, in float3 fragRayDir)
	{
		fragColor.rgb = ToUNorm( fragRayDir );
		fragColor.rgb += Sin( fragRayDir * 10.0 ) * 0.1;
		fragColor.a = 1.0;
	}

#endif
//-----------------------------------------------------------------------------
