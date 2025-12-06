// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Hardware HDR rendering.
	Colors in range [0, 1] has brightness 80 nit.
	Color with value 125.0 has brightness 10 000 nit or max brightness of the screen.

	Requires HDR display with 'Extended_sRGB_linear' color space support.
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
		RC<Image>	rt = Image( EPixelFormat::RGBA16F, SurfaceSize() );		rt.Name( "RT" );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess( EPostprocess::Shadertoy );
			pass.Output( rt );
			pass.Slider( "iFillAll",	0,		1 );
			pass.Slider( "iHDR",		1.0,	1000.0,		50.0 );
			pass.Slider( "iRadius",		4.0,	1000.0,		100.0 );	// pixels
			pass.ColorSelector( "iColor",	RGBA32f(0.f, 1.f, 0.f, 1.f) );
		}
		Present( rt ); //, EColorSpace::Extended_sRGB_linear );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "Math.glsl"

	void mainImage (out float4 fragColor, in float2 fragCoord)
	{
		float2	mpos	= iMouse.z > 0.f ? iMouse.xy : iResolution.xy * 0.5;
		float	factor	= iFillAll == 1 ? 1.0 : Max( 0.0, 1.0 - Distance( fragCoord, mpos ) / iRadius );

		float3	col = iColor.rgb * factor * iHDR;

		fragColor = float4( col, 1.0 );
	}

#endif
//-----------------------------------------------------------------------------
