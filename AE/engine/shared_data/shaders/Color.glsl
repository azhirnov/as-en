// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Color functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

ND_ float4  Rainbow (const float factor);
ND_ float4  RainbowWrap (const float factor);

ND_ float3  RGBtoHSV (const float3 c);
ND_ float3  HSVtoRGB (const float3 hsv);
ND_ float3  HSVtoRGB_v2 (float3 hsv);

ND_ float3  RGBtoYUV (const float3 rgb);
ND_ float3  YUVtoRGB (float3 yuv);

ND_ float3  RGBtoYUV_v2 (const float3 rgb);
ND_ float3  YUVtoRGB_v2 (float3 yuv);

ND_ float3  RGBtoXYZ (const float3 rgb);
ND_ float3  XYZtoRGB (const float3 xyz);
ND_ float3  RGBtoXYY (const float3 rgb);
ND_ float3  XYYtoRGB (const float3 xyY);

ND_ float3  RGBtoXYY_v2 (float3 rgb);
ND_ float3  XYYtoRGB_v2 (float3 xyY);

ND_ float3  RGBtoOklab (float3 rgb);
ND_ float3  OklabToRGB (float3 oklab);

ND_ float  RGBtoLuminance (const float3 linear);
ND_ float  RGBtoLogLuminance (const float3 linear, float gamma);
ND_ float  RGBtoLogLuminance (const float3 linear);

ND_ float3  FromRGBM (const float4 rgbm);
ND_ float4  ToRGBM (const float3 rgb);

ND_ float3  LerpHSV (float3 a, float3 b, float factor);
ND_ float3  RGBLerpHSV (float3 a, float3 b, const float factor);
ND_ float3  RGBLerpOklab (const float3 a, const float3 b, const float factor);
//-----------------------------------------------------------------------------

#include "../3party_shaders/ColorUtils-1.glsl"



float4  Rainbow (const float factor)
{
	return float4( HSVtoRGB( float3( Saturate( factor * 0.8f ), 1.f, 1.f )), 1.f );
}

float4  RainbowWrap (const float factor)
{
	return float4( HSVtoRGB( float3( Wrap( factor * 0.8f, 0.0f, 1.0f ), 1.f, 1.f )), 1.f );
}
//-----------------------------------------------------------------------------


float3  RGBLerpHSV (float3 a, float3 b, const float factor)
{
	a = RGBtoHSV( a );
	b = RGBtoHSV( b );
	return HSVtoRGB( LerpHSV( a, b, factor ));
}
//-----------------------------------------------------------------------------


float  RGBtoLuminance (const float3 linear)
{
	return Dot( linear, float3(0.212671f, 0.715160f, 0.072169f) );
}


float  RGBtoLogLuminance (const float3 linear, float gamma)
{
	return Log2( Lerp( 1.0f, Exp2(gamma), RGBtoLuminance( linear ))) / gamma;
}

float  RGBtoLogLuminance (const float3 linear)
{
	return RGBtoLogLuminance( linear, 4.0f );
}
//-----------------------------------------------------------------------------
