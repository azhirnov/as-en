#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


// DHash from https://www.shadertoy.com/view/4djSRW
// MIT License...
// Copyright (c) 2014 David Hoskins.

// range [0..1]

float4 _DHashScale ()  { return float4( 0.1031, 0.1030, 0.0973, 0.1099 ); }

ND_ float  DHash11 (const float p)
{
    float3 p3 = Fract( float3(p) * _DHashScale().x );
    p3 += Dot( p3, p3.yzx + 19.19 );
    return Fract( (p3.x + p3.y) * p3.z );
}

ND_ float  DHash12 (const float2 p)
{
    float3 p3 = Fract( float3(p.xyx) * _DHashScale().x );
    p3 += Dot( p3, p3.yzx + 19.19 );
    return Fract( (p3.x + p3.y) * p3.z );
}

ND_ float  DHash13 (const float3 p)
{
    float3 p3 = Fract( p * _DHashScale().x );
    p3 += Dot( p3, p3.yzx + 19.19 );
    return Fract( (p3.x + p3.y) * p3.z );
}

ND_ float2  DHash21 (const float p)
{
    float3 p3 = Fract( float3(p) * _DHashScale().xyz );
    p3 += Dot( p3, p3.yzx + 19.19 );
    return Fract( (p3.xx + p3.yz) * p3.zy );
}

ND_ float2  DHash22 (const float2 p)
{
    float3 p3 = Fract( float3(p.xyx) * _DHashScale().xyz );
    p3 += Dot( p3, p3.yzx + 19.19 );
    return Fract( (p3.xx + p3.yz) * p3.zy );
}

ND_ float2  DHash23 (const float3 p)
{
    float3 p3 = Fract( p * _DHashScale().xyz );
    p3 += Dot( p3, p3.yzx + 19.19 );
    return Fract( (p3.xx + p3.yz) * p3.zy );
}

ND_ float3  DHash31 (const float p)
{
    float3 p3 = Fract( float3(p) * _DHashScale().xyz );
    p3 += Dot( p3, p3.yzx + 19.19 );
    return Fract( (p3.xxy + p3.yzz) * p3.zyx );
}

ND_ float3  DHash32 (const float2 p)
{
    float3 p3 = Fract( float3(p.xyx) * _DHashScale().xyz );
    p3 += Dot( p3, p3.yxz + 19.19 );
    return Fract( (p3.xxy + p3.yzz) * p3.zyx );
}

ND_ float3  DHash33 (const float3 p)
{
    float3 p3 = Fract( p * _DHashScale().xyz );
    p3 += Dot( p3, p3.yxz + 19.19 );
    return Fract( (p3.xxy + p3.yxx) * p3.zyx );
}

ND_ float4  DHash41 (const float p)
{
    float4 p4 = Fract( float4(p) * _DHashScale() );
    p4 += Dot( p4, p4.wzxy + 19.19 );
    return Fract( (p4.xxyz + p4.yzzw) * p4.zywx );
}

ND_ float4  DHash42 (const float2 p)
{
    float4 p4 = Fract( float4(p.xyxy) * _DHashScale() );
    p4 += Dot( p4, p4.wzxy + 19.19 );
    return Fract( (p4.xxyz + p4.yzzw) * p4.zywx );
}

ND_ float4  DHash43 (const float3 p)
{
    float4 p4 = Fract( float4(p.xyzx) * _DHashScale() );
    p4 += Dot( p4, p4.wzxy + 19.19 );
    return Fract( (p4.xxyz + p4.yzzw) * p4.zywx );
}

ND_ float4  DHash44 (const float4 p)
{
    float4 p4 = Fract( p * _DHashScale() );
    p4 += Dot( p4, p4.wzxy + 19.19 );
    return Fract( (p4.xxyz + p4.yzzw) * p4.zywx );
}
