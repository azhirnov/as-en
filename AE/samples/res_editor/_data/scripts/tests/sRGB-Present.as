// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	.--------------------------------------------------------------.
	|   rt    |  swapchain  | sRGBInput |  display  |  screenshot  |
	|---------|-------------|-----------|-----------|--------------|
	| linear  |   linear    |     0     |    ==     |      ==      | -- screenshot image equal to displayed image
	|  sRGB   |   linear    |     0     |    ==     |   brighter   | -- screenshot is much brighter
	|  sRGB   |   linear    |     1     |   darker  |      ==      | -- displayed image is much darker
	| linear  |    sRGB     |     0     |  brighter |      ==      | -- displayed image is much brighter
	| linear  |    sRGB     |     1     |    ==     |    darker    | -- screenshot is much darker
	|  sRGB   |    sRGB     |     0     |  brighter |   brighter   | -- screenshot and displayed images are much brighter
	|  sRGB   |    sRGB     |     1     |    ==     |    darker    | -- screenshot is much darker
	'--------------------------------------------------------------'
	src			- result of color selector dialog.
	==			- visible result match to 'src' color.
	brighter	- sRGB conversion applied multiple times.
	darker		- sRGB to linear conversion applied multiple times.
	swapchain	- 'Surface format' in 'Graphics' tab.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <aestyle.glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm,	SurfaceSize() );
	//	RC<Image>	rt = Image( EPixelFormat::sRGB8_A8,		SurfaceSize() );

		// render loop
		{
			RC<Postprocess>		pass = Postprocess();
			pass.Output( "out_Color",	rt );
			pass.Slider( "sRGBInput",	0,	1 );
			pass.ColorSelector( "iColor",	RGBA32f(0.5, 0.25, 0.125, 1.0) );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_FRAG
	#include "ColorSpaceUtility.glsl"

	void Main ()
	{
		if ( sRGBInput == 0 )
			out_Color = iColor;
		else
			out_Color = RemoveSRGBCurve( iColor );
	}

#endif
//-----------------------------------------------------------------------------
