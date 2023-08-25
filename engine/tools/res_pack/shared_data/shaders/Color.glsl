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

ND_ float  RGBtoLuminance (const float3 linear);
ND_ float  RGBtoLogLuminance (const float3 linear, float gamma);
ND_ float  RGBtoLogLuminance (const float3 linear);

ND_ float3  FromRGBM (const float4 rgbm);
ND_ float4  ToRGBM (const float3 rgb);
//-----------------------------------------------------------------------------



float3  RGBtoHSV (const float3 c)
{
    float4 K = float4( 0.0f, -1.0f / 3.0f, 2.0f / 3.0f, -1.0f );
    float4 p = c.g < c.b ? float4(c.b, c.g, K.w, K.z) : float4(c.g, c.b, K.x, K.y);
    float4 q = c.r < p.x ? float4(p.x, p.y, p.w, c.r) : float4(c.r, p.y, p.z, p.x);
    float  d = q.x - Min(q.w, q.y);
    float  e = 1.0e-10f;
    return float3(  Abs(q.z + (q.w - q.y) / (6.0f * d + e)),
                    d / (q.x + e),
                    q.x );
}

float3  HSVtoRGB (const float3 hsv)
{
    // from http://chilliant.blogspot.ru/2014/04/rgbhsv-in-hlsl-5.html
    float3 col = float3( Abs( hsv.x * 6.0f - 3.0f ) - 1.0f,
                         2.0f - Abs( hsv.x * 6.0f - 2.0f ),
                         2.0f - Abs( hsv.x * 6.0f - 4.0f ));
    return ((Saturate( col ) - 1.0f) * hsv.y + 1.0f) * hsv.z;
}

float4  Rainbow (const float factor)
{
    return float4( HSVtoRGB( float3( Saturate( factor * 0.74f ), 1.f, 1.f )), 1.f );
}

float4  RainbowWrap (const float factor)
{
    return float4( HSVtoRGB( float3( Wrap( factor * 0.74f, 0.0f, 1.0f ), 1.f, 1.f )), 1.f );
}


float3  HSVtoRGB_v2 (float3 hsv)
{
    // from https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
    hsv.x = Mod( 100.0 + hsv.x, 1.0 );                                       // Ensure [0,1[

    float   HueSlice = 6.0 * hsv.x;                                            // In [0,6[
    float   HueSliceInteger = floor( HueSlice );
    float   HueSliceInterpolant = HueSlice - HueSliceInteger;                   // In [0,1[ for each hue slice

    float3  TempRGB = float3(   hsv.z * (1.0 - hsv.y),
                                hsv.z * (1.0 - hsv.y * HueSliceInterpolant),
                                hsv.z * (1.0 - hsv.y * (1.0 - HueSliceInterpolant)) );

    // The idea here to avoid conditions is to notice that the conversion code can be rewritten:
    //    if      ( var_i == 0 ) { R = V         ; G = TempRGB.z ; B = TempRGB.x }
    //    else if ( var_i == 2 ) { R = TempRGB.x ; G = V         ; B = TempRGB.z }
    //    else if ( var_i == 4 ) { R = TempRGB.z ; G = TempRGB.x ; B = V     }
    // 
    //    else if ( var_i == 1 ) { R = TempRGB.y ; G = V         ; B = TempRGB.x }
    //    else if ( var_i == 3 ) { R = TempRGB.x ; G = TempRGB.y ; B = V     }
    //    else if ( var_i == 5 ) { R = V         ; G = TempRGB.x ; B = TempRGB.y }
    //
    // This shows several things:
    //  . A separation between even and odd slices
    //  . If slices (0,2,4) and (1,3,5) can be rewritten as basically being slices (0,1,2) then
    //      the operation simply amounts to performing a "rotate right" on the RGB components
    //  . The base value to rotate is either (V, B, R) for even slices or (G, V, R) for odd slices
    //
    float   IsOddSlice = Mod( HueSliceInteger, 2.0 );                          // 0 if even (slices 0, 2, 4), 1 if odd (slices 1, 3, 5)
    float   ThreeSliceSelector = 0.5 * (HueSliceInteger - IsOddSlice);          // (0, 1, 2) corresponding to slices (0, 2, 4) and (1, 3, 5)

    float3  ScrollingRGBForEvenSlices = float3( hsv.z, TempRGB.zx );           // (V, Temp Blue, Temp Red) for even slices (0, 2, 4)
    float3  ScrollingRGBForOddSlices = float3( TempRGB.y, hsv.z, TempRGB.x );  // (Temp Green, V, Temp Red) for odd slices (1, 3, 5)
    float3  ScrollingRGB = Lerp( ScrollingRGBForEvenSlices, ScrollingRGBForOddSlices, IsOddSlice );

    float   IsNotFirstSlice = Saturate( ThreeSliceSelector );                   // 1 if NOT the first slice (true for slices 1 and 2)
    float   IsNotSecondSlice = Saturate( ThreeSliceSelector-1.0 );              // 1 if NOT the first or second slice (true only for slice 2)

    return  Lerp( ScrollingRGB.xyz, Lerp( ScrollingRGB.zxy, ScrollingRGB.yzx, IsNotSecondSlice ), IsNotFirstSlice );    // Make the RGB rotate right depending on final slice index
}
//-----------------------------------------------------------------------------


// YUV
// Y  - luminance
// UV - coords in plane

float3  RGBtoYUV (const float3 rgb)
{
    // from https://www.shadertoy.com/view/3lycWz
    float y = 0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
    return float3( y,
                   0.493 * (rgb.b - y),
                   0.877 * (rgb.r - y) );
}

float3  YUVtoRGB (float3 yuv)
{
    // from https://www.shadertoy.com/view/3lycWz
    return float3(
                yuv.x + 1.0 / 0.877 * yuv.z,
                yuv.x - 0.39393     * yuv.y - 0.58081 * yuv.z,
                yuv.x + 1.0 / 0.493 * yuv.y );
}
//-----------------------------------------------------------------------------


float3  RGBtoYUV_v2 (const float3 rgb)
{
    // from https://github.com/tobspr/GLSL-Color-Spaces/blob/master/ColorSpaces.inc.glsl
    float y = 0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
    return float3( y,
                   (rgb.b - y) * 0.565,
                   (rgb.r - y) * 0.713 );
}

float3  YUVtoRGB_v2 (float3 yuv)
{
    // from https://www.shadertoy.com/view/3lycWz
    return float3(
                yuv.x + 1.403 * yuv.z,
                yuv.x - 0.344 * yuv.y - 0.714 * yuv.z,
                yuv.x + 1.770 * yuv.y );
}
//-----------------------------------------------------------------------------


// XYZ
// CIE 1931 color space
// Y - luminance
// Z - is quasi-equal to blue
// X - is a mix of the three CIE RGB curves

float3  RGBtoXYZ (const float3 rgb)
{
    const float3x3  RGB_2_XYZ = float3x3(
        0.4124564, 0.2126729, 0.0193339,
        0.3575761, 0.7151522, 0.1191920,
        0.1804375, 0.0721750, 0.9503041
    );
    return RGB_2_XYZ * rgb;
}

float3  XYZtoRGB (const float3 xyz)
{
    const float3x3  XYZ_2_RGB = float3x3(
         3.2404542,-0.9692660, 0.0556434,
        -1.5371385, 1.8760108,-0.2040259,
        -0.4985314, 0.0415560, 1.0572252
    );
    return XYZ_2_RGB * xyz;
}

float3  RGBtoXYY (const float3 rgb)
{
    float3  xyz = RGBtoXYZ( rgb );
    return float3(  xyz.x / (xyz.x + xyz.y + xyz.z),
                    xyz.y,
                    xyz.y / (xyz.x + xyz.y + xyz.z) );
}

float3  XYYtoRGB (const float3 xyY)
{
    float3  xyz = float3( xyY.z * xyY.x / xyY.y,
                          xyY.z,
                          xyY.z * (1.0 - xyY.x - xyY.y) / xyY.y );
    return XYZtoRGB( xyz );
}
//-----------------------------------------------------------------------------


// from https://www.nutty.ca/?page_id=352&link=hdr

float3  RGBtoXYY_v2 (float3 rgb)
{
    const float3x3 RGB2XYZ = float3x3(
        0.4124, 0.3576, 0.1805,
        0.2126, 0.7152, 0.0722,
        0.0193, 0.1192, 0.9505
    );
    float3 XYZ = RGB2XYZ * rgb;

    // XYZ to xyY
    return float3( XYZ.x / (XYZ.x + XYZ.y + XYZ.z),
                   XYZ.y / (XYZ.x + XYZ.y + XYZ.z),
                   XYZ.y );
}

float3  XYYtoRGB_v2 (float3 xyY)
{
    // xyY to XYZ
    float3  XYZ = float3(
        (xyY.z / xyY.y) * xyY.x,
        xyY.z,
        (xyY.z / xyY.y) * (1.0 - xyY.x - xyY.y)
    );
    const float3x3  XYZ2RGB = float3x3(
        3.2406, -1.5372, -0.4986,
        -0.9689, 1.8758, 0.0415, 
        0.0557, -0.2040, 1.0570
    );
    return XYZ2RGB * XYZ;
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



// https://www.shadertoy.com/view/ftfyDl
const float _RGBM_MaxRange = 64.0;

float3  FromRGBM (const float4 rgbm)
{
    return rgbm.rgb * (rgbm.a * _RGBM_MaxRange);
}

float4  ToRGBM (const float3 rgb)
{
    float   max_rgb = Max( Max( rgb.r, rgb.g ), rgb.b );
    float   m       = Ceil( (max_rgb / _RGBM_MaxRange) * 255.0 ) / 255.0;
    return  float4( rgb * (1.0 / (m * _RGBM_MaxRange)), m );
}
//-----------------------------------------------------------------------------
