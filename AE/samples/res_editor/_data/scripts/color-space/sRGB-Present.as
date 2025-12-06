// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	.---------------------------------------------------------------.
	|   rt    |  swapchain  | sRGBInput  |  display  |  screenshot  |
	|---------|-------------|------------|-----------|--------------|
	| linear  |   linear    |  sRGB  (0) |    ==     |      ==      |
	|  sRGB   |   linear    |  sRGB  (0) |    ==     |   brighter   |
	|  sRGB   |   linear    | linear (1) |   darker  |      ==      |
	| linear  |    sRGB     |  sRGB  (0) |  brighter |      ==      |
	| linear  |    sRGB     | linear (1) |    ==     |    darker    |
	|  sRGB   |    sRGB     |  sRGB  (0) |  brighter |   brighter   |
	|  sRGB   |    sRGB     | linear (1) |    ==     |    darker    |
	'--------------------------------------------------------------'

	src			- result of color selector dialog.
	==			- visible result match to 'src' color.
	brighter	- sRGB conversion applied multiple times.
	darker		- sRGB to linear conversion applied multiple times.
	swapchain	- 'Surface format' in 'Graphics' tab.
	rt			- intermediate render target which will blit/copy to the swapchain image.
	display		- result of blit from 'rt'  to the swapchain image.
	screenshot	- in current implementation screenshot is memcopied from 'rt',
				  in other cases screenshot copied from swapchain image, so result may be different.
				  Same result will be displayed if in 'Graphics' tab checked 'Copy instead of blit'.
	linear		- RGBA8 format, color can be in gamma space, but requires implicit conversion to/from gamma space.
	sRGB		- sRGB8 format, color stored in gamma space for better bit packing.
	sRGBInput	- 0: used sRGB color, 1: used linear color.
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
	#include "ColorSpace.glsl"

	void Main ()
	{
		if ( sRGBInput == 0 )
			out_Color = iColor;		// sRGB
		else
			out_Color = RemoveSRGBCurve( iColor );	// linear
	}

#endif
//-----------------------------------------------------------------------------
