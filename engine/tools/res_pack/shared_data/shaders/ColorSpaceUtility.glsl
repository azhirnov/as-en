// from https://github.com/Microsoft/DirectX-Graphics-Samples/blob/master/MiniEngine/Core/Shaders/ColorSpaceUtility.hlsli

// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).

#ifdef __cplusplus
# pragma once
#endif

// Approximately pow(x, 1.0 / 2.2)
ND_ float   ApplySRGBCurve (float  x)       { return x < 0.0031308 ? 12.92 * x : 1.055 * pow(x, 1.0 / 2.4) - 0.055; }
ND_ float3  ApplySRGBCurve (float3 v)       { return float3( ApplySRGBCurve(v.r), ApplySRGBCurve(v.g), ApplySRGBCurve(v.b) ); }
ND_ float4  ApplySRGBCurve (float4 v)       { return float4( ApplySRGBCurve(v.r), ApplySRGBCurve(v.g), ApplySRGBCurve(v.b), v.a ); }

// Approximately pow(x, 2.2)
ND_ float   RemoveSRGBCurve (float  x)      { return x < 0.04045 ? x / 12.92 : pow( (x + 0.055) / 1.055, 2.4 ); }
ND_ float3  RemoveSRGBCurve (float3 v)      { return float3( RemoveSRGBCurve(v.r), RemoveSRGBCurve(v.g), RemoveSRGBCurve(v.b) ); }
ND_ float4  RemoveSRGBCurve (float4 v)      { return float4( RemoveSRGBCurve(v.r), RemoveSRGBCurve(v.g), RemoveSRGBCurve(v.b), v.a ); }


// These functions avoid pow() to efficiently approximate sRGB with an error < 0.4%.
ND_ float   ApplySRGBCurve_Fast (float  x)  { return x < 0.0031308 ? 12.92 * x : 1.13005 * sqrt(x - 0.00228) - 0.13448 * x + 0.005719; }
ND_ float3  ApplySRGBCurve_Fast (float3 v)  { return float3( ApplySRGBCurve_Fast(v.r), ApplySRGBCurve_Fast(v.g), ApplySRGBCurve_Fast(v.b) ); }
ND_ float4  ApplySRGBCurve_Fast (float4 v)  { return float4( ApplySRGBCurve_Fast(v.r), ApplySRGBCurve_Fast(v.g), ApplySRGBCurve_Fast(v.b), v.a ); }

ND_ float   RemoveSRGBCurve_Fast (float  x) { return x < 0.04045 ? x / 12.92 : -7.43605 * x - 31.24297 * sqrt(-0.53792 * x + 1.279924) + 35.34864; }
ND_ float3  RemoveSRGBCurve_Fast (float3 v) { return float3( RemoveSRGBCurve_Fast(v.r), RemoveSRGBCurve_Fast(v.g), RemoveSRGBCurve_Fast(v.b) ); }
ND_ float4  RemoveSRGBCurve_Fast (float4 v) { return float4( RemoveSRGBCurve_Fast(v.r), RemoveSRGBCurve_Fast(v.g), RemoveSRGBCurve_Fast(v.b), v.a ); }


// The OETF recommended for content shown on HDTVs.  This "gamma ramp" may increase contrast as
// appropriate for viewing in a dark environment.  Always use this curve with Limited RGB as it is
// used in conjunction with HDTVs.
ND_ float   ApplyREC709Curve (float  x)     { return x < 0.0181 ? 4.5 * x : 1.0993 * pow(x, 0.45) - 0.0993; }
ND_ float3  ApplyREC709Curve (float3 v)     { return float3( ApplyREC709Curve(v.r), ApplyREC709Curve(v.g), ApplyREC709Curve(v.b) ); }
ND_ float4  ApplyREC709Curve (float4 v)     { return float4( ApplyREC709Curve(v.r), ApplyREC709Curve(v.g), ApplyREC709Curve(v.b), v.a ); }

ND_ float   RemoveREC709Curve (float  x)    { return x < 0.08145 ? x / 4.5 : pow((x + 0.0993) / 1.0993, 1.0 / 0.45); }
ND_ float3  RemoveREC709Curve (float3 v)    { return float3( RemoveREC709Curve(v.r), RemoveREC709Curve(v.g), RemoveREC709Curve(v.b) ); }
ND_ float4  RemoveREC709Curve (float4 v)    { return float4( RemoveREC709Curve(v.r), RemoveREC709Curve(v.g), RemoveREC709Curve(v.b), v.a ); }


// This is the new HDR transfer function, also called "PQ" for perceptual quantizer.  Note that REC2084
// does not also refer to a color space.  REC2084 is typically used with the REC2020 color space.
ND_ float3  ApplyREC2084Curve (float3 L)
{
    float  m1 = 2610.0 / 4096.0 / 4;
    float  m2 = 2523.0 / 4096.0 * 128;
    float  c1 = 3424.0 / 4096.0;
    float  c2 = 2413.0 / 4096.0 * 32;
    float  c3 = 2392.0 / 4096.0 * 32;
    float3 Lp = pow( L, float3(m1) );
    return pow( (c1 + c2 * Lp) / (1 + c3 * Lp), float3(m2) );
}

ND_ float3  RemoveREC2084Curve (float3 N)
{
    float  m1 = 2610.0 / 4096.0 / 4;
    float  m2 = 2523.0 / 4096.0 * 128;
    float  c1 = 3424.0 / 4096.0;
    float  c2 = 2413.0 / 4096.0 * 32;
    float  c3 = 2392.0 / 4096.0 * 32;
    float3 Np = pow( N, float3(1.0 / m2) );
    return pow( max( Np - c1, float3(0.0) ) / (c2 - c3 * Np), float3(1.0 / m1) );
}

//
// Color space conversions
//
// These assume linear (not gamma-encoded) values.  A color space conversion is a change
// of basis (like in Linear Algebra).  Since a color space is defined by three vectors--
// the basis vectors--changing space involves a matrix-vector multiplication.  Note that
// changing the color space may result in colors that are "out of bounds" because some
// color spaces have larger gamuts than others.  When converting some colors from a wide
// gamut to small gamut, negative values may result, which are inexpressible in that new
// color space.
//
// It would be ideal to build a color pipeline which never throws away inexpressible (but
// perceivable) colors.  This means using a color space that is as wide as possible.  The
// XYZ color space is the neutral, all-encompassing color space, but it has the unfortunate
// property of having negative values (specifically in X and Z).  To correct this, a further
// transformation can be made to X and Z to make them always positive.  They can have their
// precision needs reduced by dividing by Y, allowing X and Z to be packed into two UNORM8s.
// This color space is called YUV for lack of a better name.
//

// Note:  Rec.709 and sRGB share the same color primaries and white point.  Their only difference
// is the transfer curve used.

ND_ float3  REC709toREC2020 (float3 RGB709)
{
    const float3x3 ConvMat = float3x3(
        0.627402, 0.329292, 0.043306,
        0.069095, 0.919544, 0.011360,
        0.016394, 0.088028, 0.895578
    );
    return ConvMat * RGB709;
}

ND_ float3  REC2020toREC709 (float3 RGB2020)
{
    const float3x3 ConvMat = float3x3(
        1.660496, -0.587656, -0.072840,
        -0.124547, 1.132895, -0.008348,
        -0.018154, -0.100597, 1.118751
    );
    return ConvMat * RGB2020;
}

ND_ float3  REC709toDCIP3 (float3 RGB709)
{
    const float3x3 ConvMat = float3x3(
        0.822458, 0.177542, 0.000000,
        0.033193, 0.966807, 0.000000,
        0.017085, 0.072410, 0.910505
    );
    return ConvMat * RGB709;
}

ND_ float3  DCIP3toREC709 (float3 RGBP3)
{
    const float3x3 ConvMat = float3x3(
        1.224947, -0.224947, 0.000000,
        -0.042056, 1.042056, 0.000000,
        -0.019641, -0.078651, 1.098291
    );
    return ConvMat * RGBP3;
}

// 10-bit should range from 64 to 940
ND_ float3  RGBFullToLimited10bit (float3 x)
{
    return Saturate(x) * 876.0 / 1023.0 + 64.0 / 1023.0;
}

#define COLOR_FORMAT_LINEAR             0   // EColorSpace::PassThrough
#define COLOR_FORMAT_sRGB_FULL          1   // EColorSpace::sRGB_nonlinear, EColorSpace::Extended_sRGB_linear, EColorSpace:Display_P3_linear
#define COLOR_FORMAT_sRGB_LIMITED       2   // 10 bit
#define COLOR_FORMAT_Rec709_FULL        3   // EColorSpace::BT709_linear / BT709_nonlinear
#define COLOR_FORMAT_Rec709_LIMITED     4   // 10 bit
#define COLOR_FORMAT_HDR10              5   // EColorSpace::HDR10_HLG

ND_ float3  ApplyDisplayProfile (const float3 color, const int displayFormat)
{
    switch ( displayFormat )
    {
        default:
        case COLOR_FORMAT_LINEAR :          return color;
        case COLOR_FORMAT_sRGB_FULL :       return ApplySRGBCurve( color );
        case COLOR_FORMAT_sRGB_LIMITED :    return RGBFullToLimited10bit( ApplySRGBCurve( color ));
        case COLOR_FORMAT_Rec709_FULL :     return ApplyREC709Curve( color );
        case COLOR_FORMAT_Rec709_LIMITED :  return RGBFullToLimited10bit( ApplyREC709Curve( color ));
        case COLOR_FORMAT_HDR10 :           return ApplyREC2084Curve( REC709toREC2020( color ));
    };
}
