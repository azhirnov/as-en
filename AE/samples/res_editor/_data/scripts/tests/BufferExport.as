// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );
		RC<Buffer>		buf	= Buffer();

		{
			buf.ArrayLayout(
				"RayPath",
				"	uint	count;" +
				"	float2	pointArr [" + 8 + "];" +
				"	float	wavelengthArr [" + 8 + "];" +
				"	float	energyArr [" + 8 + "];",
				64 );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_OutImage", rt );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );

			ClearBuffer( buf, 0x12345678 );
		}

		Export( buf, "buffer-.bin" );
		DbgExport( buf, "buffer-.hpp" );

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
