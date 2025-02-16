// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Simple compute shader.
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
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_OutImage", rt );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv = GetGlobalCoordUNormCorrected();

		float3	col = 0.5 + 0.5 * Cos( un_PerPass.time + uv.xyx + float3(0,2,4) );

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(col, 1.0) );
	}

#endif
//-----------------------------------------------------------------------------
