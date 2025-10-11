// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Visualize gradient and match grid with gradient lanes.

	bit depth = 1 / (iRange / iGridSize)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		RC<Image>			rt			= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<DynamicFloat>	range		= DynamicFloat();
		RC<DynamicUInt>		grid_size	= DynamicUInt();

		Slider( range,		"Range",		0.0,	0.1,	0.059 );
		Slider( grid_size,	"GridSize",		8,		16,		15 );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt,		RGBA32f(0.0) );
			pass.Slider( "iBegin",		0.0,	1.0,	0.222 );
			pass.Slider( "iSRGB",		0,		1 );				// set 1 for sRGB swapchain image
			pass.Slider( "iGridOffset",	0.0,	0.1,	0.071 );
			pass.Constant( "iRange",	range );
			pass.Constant( "iGridSize",	grid_size );
			pass.ColorSelector( "iColor", RGBA8u(255) );
		}
		Present( rt );

		RC<DynamicFloat>	bit_depth = grid_size.ToFloat().Div( range );
		Label( bit_depth, "bit depth" );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "SDF.glsl"
	#include "ColorSpace.glsl"
	#include "InvocationID.glsl"

	void  Main ()
	{
		float2	uv = GetGlobalCoordUNorm().xy;
		float	x = uv.x;
		float	a = AA_LinesX_dxdy( (uv + iGridOffset) * float(iGridSize), float2(1.0, 2.0) ).x;

		x = Saturate( iBegin + x * iRange );

		out_Color = iColor * x;

		if ( iSRGB == 1 )
			out_Color = ApplySRGBCurve( out_Color );

		if ( uv.y > 0.7 )
			out_Color.rgb = Lerp( float3(0.0), out_Color.rgb, a );
	}

#endif
//-----------------------------------------------------------------------------
