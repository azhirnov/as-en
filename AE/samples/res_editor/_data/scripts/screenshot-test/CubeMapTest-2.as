// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
#	define SH_COMPUTE
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

			pass.Slider( "iScale",	0,	8,	3 );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"
	#include "CubeMap.glsl"
	#include "SDF.glsl"

	float2  GetUV (int dx)
	{
		float2	uv = GetGlobalCoordUNorm( int3(dx,0,0) ).xy;
		uv.x = ToSNorm( uv.x ) * 2.0;
		return uv;
	}


	void  Main ()
	{
		float4			col		= float4(0.0);
		const float		y_max	= 5.0;
		const float		y		= Floor( GetGroupCoordUNorm().y * y_max );
		const float		scale	= Exp10( float(iScale) );
		float2			uv		= GetUV( 0 );	uv.y = (uv.y - y/y_max) * y_max;

		switch ( int(y) )
		{
			case 0 : {
				float2	uv2 = CM_TangentialSC_Forward( uv );
						uv2 = CM_TangentialSC_Inverse( uv2 );
				col.rg = Abs( uv - uv2 ) * scale;
				col.b  = IsFinite( uv2.x ) ? 0.0 : 1.0;
				break;
			}
			case 1 : {
				float2	uv2 = CM_EverittSC_Forward( uv );
						uv2 = CM_EverittSC_Inverse( uv2 );
				col.rg = Abs( uv - uv2 ) * scale;
				col.b  = IsFinite( uv2.x ) ? 0.0 : 1.0;
				break;
			}
			case 2 : {
				float2	uv2 = CM_5thPolySC_Forward( uv );
						uv2 = CM_5thPolySC_Inverse( uv2 );
				col.rg = Abs( uv - uv2 ) * scale;
				col.b  = IsFinite( uv2.x ) ? 0.0 : 1.0;
				break;
			}
			case 3 : {
				float2	uv2 = CM_COBE_SC_Forward( uv );
						uv2 = CM_COBE_SC_Inverse( uv2 );
				col.rg = Abs( uv - uv2 ) * scale;
				col.b  = IsFinite( uv2.x ) ? 0.0 : 1.0;
				break;
			}
			case 4 : {
				float2	uv2 = CM_ArvoSC_Forward( uv );
						uv2 = CM_ArvoSC_Inverse( uv2 );
				col.rg = Abs( uv - uv2 ) * scale;
				col.b  = IsFinite( uv2.x ) ? 0.0 : 1.0;
				break;
			}
		}

		if ( Any(IsNaN( col.rgb )) or Any(IsInfinity( col.rgb )))
		{
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(0.2) );
			return;
		}

		if ( Fract(uv.x) > Fract(GetUV(1).x) )
			col = float4(1.0);

		col.a = 1.0;

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
