/*
	Fog functions.

	How to use:
		fog = FogFactorLinear( dist(0..1), density );
		color = Lerp( color, fog_color, fog );
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

ND_ float  FogFactorLinear (const float dist, const float start, const float end);
ND_ float  FogFactorExp (const float dist, const float density);
ND_ float  FogFactorExp2 (const float dist, const float density);
//-----------------------------------------------------------------------------



float  FogFactorLinear (const float dist, const float start, const float end)
{
	// from https://github.com/hughsk/glsl-fog
	// MIT license

	return 1.0 - Saturate( (end - dist) / (end - start));
}

float  FogFactorExp (const float dist, const float density)
{
	// from https://github.com/hughsk/glsl-fog
	// MIT license

	return 1.0 - Saturate( Exp( -density * dist ));
}

float  FogFactorExp2 (const float dist, const float density)
{
	// from https://github.com/hughsk/glsl-fog
	// MIT license

	const float LOG2 = -1.442695;
	float   d = density * dist;
	return  1.0 - Saturate( Exp2( d * d * LOG2 ));
}
