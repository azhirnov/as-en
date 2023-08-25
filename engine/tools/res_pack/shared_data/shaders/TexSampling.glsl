/*
    Texture sampling.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


ND_ float4  TriplanarMapping (const float3 pos, const float3 absDir, gl::CombinedTex2D<float> samp);

//-----------------------------------------------------------------------------



float4  TriplanarMapping (const float3 pos, const float3 absDir, gl::CombinedTex2D<float> samp)
{
    float4  a = gl.texture.Sample( samp, pos.zy );
    float4  b = gl.texture.Sample( samp, pos.xz );
    float4  c = gl.texture.Sample( samp, pos.xy );
    return (a * absDir.x) + (b * absDir.y) + (c * absDir.z);
}
