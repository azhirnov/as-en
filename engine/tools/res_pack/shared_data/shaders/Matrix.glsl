// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Matrix functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

ND_ float3x3    LookAt (const float3 dir, const float3 up);
ND_ float2      Transform2D (const float4x4 mat, const float2 point);
ND_ float3      Project (const float4x4 mat, const float3 pos, const float4 viewport);
ND_ float3      UnProject (const float4x4 invMat, const float3 pos, const float4 viewport);
//-----------------------------------------------------------------------------


float3x3  LookAt (const float3 dir, const float3 up)
{
    float3x3 m;
    m[2] = dir;
    m[0] = Normalize( Cross( up, m[2] ));
    m[1] = Cross( m[2], m[0] );
    return m;
}


float2  Transform2D (const float4x4 mat, const float2 point)
{
    return float2( mat * float4(point, 0.0f, 1.0f) );
}


float3  Project (const float4x4 mat, const float3 pos, const float4 viewport)
{
    float4  temp    = mat * float4( pos, 1.0 );
    float2  size    = float2( viewport[2] - viewport[0], viewport[3] - viewport[0] );
    temp = ToUNorm( temp * (1.0 / temp.w) );
    temp.xy = temp.xy * size + viewport.xy;
    return temp.xyz;
}

float3  UnProject (const float4x4 invMat, const float3 pos, const float4 viewport)
{
    float4  temp    = float4( pos, 1.0 );
    float2  size    = float2( viewport[2] - viewport[0], viewport[3] - viewport[0] );
    temp.xy = (temp.xy - viewport.xy) / size;
    temp = invMat * ToSNorm( temp );
    temp *= (1.0 / temp.w);
    return temp.xyz;
}
