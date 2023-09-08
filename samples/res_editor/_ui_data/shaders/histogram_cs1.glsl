// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __cplusplus
#   define SH_COMPUTE
#   include <aestyle.glsl.h>
#   define COUNT    100u
    const gl::CombinedTex2D<float>  un_Texture;
    struct {
        uint    maxRGB;
        uint    maxLuma;
        uint4   RGBLuma [COUNT];
    } un_Histogram;
#endif
//-----------------------------------------------------------------------------

#include "Math.glsl"
#include "Color.glsl"
#include "GlobalIndex.glsl"

uint  ColorToIndex (float col)
{
    return Clamp( uint(col * (COUNT-2) + 0.5), 1u, COUNT-2 );
}

void Main ()
{
    float4  col = gl.texture.Sample( un_Texture, GetGlobalCoordUNorm().xy );
    col.a = RGBtoLuminance( col.rgb );

    if ( col.r > 0.0 )
        gl.AtomicAdd( un_Histogram.RGBLuma[ ColorToIndex( col.r )].r, 1u );

    if ( col.g > 0.0 )
        gl.AtomicAdd( un_Histogram.RGBLuma[ ColorToIndex( col.g )].g, 1u );

    if ( col.b > 0.0 )
        gl.AtomicAdd( un_Histogram.RGBLuma[ ColorToIndex( col.b )].b, 1u );

    gl.AtomicAdd( un_Histogram.RGBLuma[ ColorToIndex( col.a )].a, 1u );
}
