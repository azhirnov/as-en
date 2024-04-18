// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __cplusplus
#	define SH_COMPUTE
#	include <aestyle.glsl.h>
#	define COUNT	100u
	const gl::CombinedTex2D<float>	un_Texture;
	struct {
		uint	maxRGB;
		uint	maxLuma;
		uint4	RGBLuma [COUNT];
	} un_Histogram;
#endif
//-----------------------------------------------------------------------------

#include "GlobalIndex.glsl"

void Main ()
{
	const uint	idx			= GetGlobalIndex();
	uint4		rgbl		= un_Histogram.RGBLuma [idx];
	uint		max_rgb		= Max( rgbl.r, Max( rgbl.g, rgbl.b ));
	uint		max_luma	= rgbl.a;

	gl.AtomicMax( un_Histogram.maxRGB,  max_rgb );
	gl.AtomicMax( un_Histogram.maxLuma, max_luma );
}
