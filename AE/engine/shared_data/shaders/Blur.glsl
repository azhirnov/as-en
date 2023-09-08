// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Blur functions.

    Requires AEStyle preprocessor.
*/

#ifdef __cplusplus
# pragma once
#endif

ND_ float4  Blur5 (gl::CombinedTex2D<float> image, float2 uv, float2 invResolution, float2 direction);
ND_ float4  Blur9 (gl::CombinedTex2D<float> image, float2 uv, float2 invResolution, float2 direction);
ND_ float4  Blur13 (gl::CombinedTex2D<float> image, float2 uv, float2 invResolution, float2 direction);
//-----------------------------------------------------------------------------


float4  Blur5v2 (gl::CombinedTex2D<float> image, float2 uv, float2 invResolution, float2 direction)
{
    const float     weights [5] = { 0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216 };
    const float2    tex_offset  = direction / invResolution;
    float4          color       = gl.texture.Sample( image, uv ) * weights[0];

    for (int i = 1; i < 5; ++i) {
        color += gl.texture.Sample( image, uv + tex_offset * i ) * weights[i];
        color += gl.texture.Sample( image, uv - tex_offset * i ) * weights[i];
    }
    return color;
}


#include "../3party_shaders/Blur-1.glsl"
