// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	sRGB <-> linear color space
*/

// to sRGB
ND_ float   ApplySRGBCurve (float  x);
ND_ float3  ApplySRGBCurve (float3 v);
ND_ float4  ApplySRGBCurve (float4 v);

// to linear
ND_ float   RemoveSRGBCurve (float  x);
ND_ float3  RemoveSRGBCurve (float3 v);
ND_ float4  RemoveSRGBCurve (float4 v);


// to sRGB
ND_ float   ApplySRGBCurve_Fast (float  x);
ND_ float3  ApplySRGBCurve_Fast (float3 v);
ND_ float4  ApplySRGBCurve_Fast (float4 v);

// to linear
ND_ float   RemoveSRGBCurve_Fast (float  x);
ND_ float3  RemoveSRGBCurve_Fast (float3 v);
ND_ float4  RemoveSRGBCurve_Fast (float4 v);


ND_ float   ApplyREC709Curve (float  x);
ND_ float3  ApplyREC709Curve (float3 v);
ND_ float4  ApplyREC709Curve (float4 v);

ND_ float   RemoveREC709Curve (float  x);
ND_ float3  RemoveREC709Curve (float3 v);
ND_ float4  RemoveREC709Curve (float4 v);


ND_ float3  ApplyREC2084Curve (float3 L);
ND_ float3  RemoveREC2084Curve (float3 N);


ND_ float3  REC709toREC2020 (float3 RGB709);
ND_ float3  REC2020toREC709 (float3 RGB2020);
ND_ float3  REC709toDCIP3 (float3 RGB709);
ND_ float3  DCIP3toREC709 (float3 RGBP3);


ND_ float3  RGBFullToLimited10bit (float3 x);


ND_ float4  LerpSRGB (float4 x, float4 y, const float factor);
ND_ float4  LerpSRGBFast (float4 x, float4 y, const float factor);


#define COLOR_FORMAT_LINEAR				0	// EColorSpace::PassThrough
#define COLOR_FORMAT_sRGB_FULL			1	// EColorSpace::sRGB_nonlinear, EColorSpace::Extended_sRGB_linear, EColorSpace:Display_P3_linear
#define COLOR_FORMAT_sRGB_LIMITED		2	// 10 bit
#define COLOR_FORMAT_Rec709_FULL		3	// EColorSpace::BT709_linear / BT709_nonlinear
#define COLOR_FORMAT_Rec709_LIMITED		4	// 10 bit
#define COLOR_FORMAT_HDR10				5	// EColorSpace::HDR10_HLG
#define COLOR_FORMAT_Rec2020_FULL		6	// EColorSpace::BT2020_linear
#define COLOR_FORMAT_Rec2020_LIMITED	7	// 10 bit

ND_ float3  ApplyDisplayProfile (const float3 color, const int displayFormat);
//-----------------------------------------------------------------------------

#include "../3party_shaders/ColorSpaceUtility-1.glsl"

//-----------------------------------------------------------------------------


float4  LerpSRGB (float4 x, float4 y, const float factor)
{
	x = RemoveSRGBCurve( x );
	y = RemoveSRGBCurve( y );
	return ApplySRGBCurve( Lerp( x, y, factor ));
}

float4  LerpSRGBFast (float4 x, float4 y, const float factor)
{
	x = RemoveSRGBCurve_Fast( x );
	y = RemoveSRGBCurve_Fast( y );
	return ApplySRGBCurve_Fast( Lerp( x, y, factor ));
}
