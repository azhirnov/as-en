// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Texture sampling.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


ND_ float4  TriplanarMapping (const float3 uvw, float3 dir, gl::CombinedTex2D<float> samp);

//-----------------------------------------------------------------------------



float4  TriplanarMapping (const float3 uvw, float3 dir, gl::CombinedTex2D<float> samp)
{
    float4  a = gl.texture.Sample( samp, uvw.zy );
    float4  b = gl.texture.Sample( samp, uvw.xz );
    float4  c = gl.texture.Sample( samp, uvw.xy );
    dir = Abs(Normalize( dir ));
    return (a * dir.x) + (b * dir.y) + (c * dir.z);
}
