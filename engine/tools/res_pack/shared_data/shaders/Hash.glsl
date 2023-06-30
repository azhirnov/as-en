/*
    Hash functions
*/

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
//-----------------------------------------------------------------------------



ND_ float  WeylHash (const int2 c)
{
    // from https://www.shadertoy.com/view/MsV3z3
    // LICENSE: http://unlicense.org/

    const int x = 0x3504f333 * c.x * c.x + c.y;
    const int y = 0xf1bbcdcb * c.y * c.y + c.x;

    return float(x*y) * (2.0 / 8589934592.0) + 0.5;
}

ND_ float  WeylHash (const float2 c)
{
    // from https://www.shadertoy.com/view/Xdy3Rc
    // LICENSE: http://unlicense.org/

    // a pair of Weyl values with low star discrepancy
    const float2 W = float2( 0.5545497, 0.308517 );

    const float2 v = c * Fract( c * W );
    return Fract( v.x * v.y );
}

ND_ float  ModHash (float2 uv)
{
    // from https://www.shadertoy.com/view/Xts3R7
    // license CC BY-NC-SA 3.0

    uv = Abs( Mod( 10.0 * Fract( (uv + 1.1312) * 31.0 ), uv + 2.0 ));
    uv = Abs( Mod( uv.x * Fract( (uv + 1.721711) * 17.0 ), uv ));
    return Fract( 10.0 * (7.0 * uv.y + 31.0 * uv.x) );
}

ND_ float  HEHash11 (uint n)
{
    // from https://www.shadertoy.com/view/llGSzw
    // The MIT License
    // Copyright (c) 2017 Inigo Quilez

    // integer hash copied from Hugo Elias
    n = (n << 13U) ^ n;
    n = n * (n * n * 15731U + 789221U) + 1376312589U;

    // floating point conversion from http://iquilezles.org/www/articles/sfrand/sfrand.htm
    return uintBitsToFloat( (n>>9U) | 0x3f800000U ) - 1.0;
}

float2  HEHash22 (uint2 uv)
{
    return float2(HEHash11(uv.x), HEHash11(uv.y));
}

float  HEHash12 (uint2 uv)
{
    return HEHash11((uv.x * 0x72a34u) ^ (uv.y * 0x3e6cdu));
}

float  HEHash11 (int n)   { return HEHash11( uint(n)   ); }
float2 HEHash22 (int2 uv) { return HEHash22( uint2(uv) ); }
float  HEHash12 (int2 uv) { return HEHash12( uint2(uv) ); }
//-----------------------------------------------------------------------------



uint3 _IWeylConst ()
{
    return uint3(
        0x3504f333u,    // W0 = 3*2309*128413 
        0xf1bbcdcbu,    // W1 = 7*349*1660097 
        741103597u      // M = 13*83*686843
    );
}

ND_ uint  IWeylHash (const uint2 p)
{
    // from https://www.shadertoy.com/view/4dlcR4
    // LICENSE: http://unlicense.org/

    uint    x = p.x;
    uint    y = p.y;

    x *= _IWeylConst().x;   // x' = Fx(x)
    y *= _IWeylConst().y;   // y' = Fy(y)
    x ^= y;                 // combine
    x *= _IWeylConst().z;   // MLCG constant
    return x;
}

ND_ uint  IWeylHash2 (const uint2 p)
{
    // from https://www.shadertoy.com/view/4dlcR4
    // LICENSE: http://unlicense.org/

    uint    x = p.x;
    uint    y = p.y;

    x *= _IWeylConst().x;   // x' = Fx(x)
    y *= _IWeylConst().y;   // y' = Fy(y)
    x += _IWeylConst().y;   // some odd constant
    y += _IWeylConst().x;   // some odd constant
    x ^= y;                 // combine
    x *= _IWeylConst().z;   // MLCG constant
    return x;
}
//-----------------------------------------------------------------------------



// from https://www.shadertoy.com/view/4ssXRX
// license CC BY-NC-SA 3.0

//note: uniformly distributed, normalized rand, [0;1]
#define nrand( n ) Fract( Sin( Dot( (n).xy, float2(12.9898, 78.233) )) * 43758.5453 )

ND_ float  Hash_Uniform (const float2 n, const float seed)
{
    const float t = Fract( seed );
    const float nrnd0 = nrand( n + 0.07*t );
    return nrnd0;
}

ND_ float  Hash_Triangular (const float2 n, const float seed)
{
    const float t = Fract( seed );
    const float nrnd0 = nrand( n + 0.07*t );
    const float nrnd1 = nrand( n + 0.11*t );
    return (nrnd0+nrnd1) / 2.0;
}

ND_ float  Hash_Gaussianish (const float2 n, const float seed)
{
    const float t = Fract( seed );
    const float nrnd0 = nrand( n + 0.07*t );
    const float nrnd1 = nrand( n + 0.11*t );    
    const float nrnd2 = nrand( n + 0.13*t );
    const float nrnd3 = nrand( n + 0.17*t );
    return (nrnd0+nrnd1+nrnd2+nrnd3) / 4.0;
}

ND_ float  Hash_MoarGaussianish (const float2 n, const float seed)
{
    const float t = Fract( seed );
    const float nrnd0 = nrand( n + 0.07*t );
    const float nrnd1 = nrand( n + 0.11*t );    
    const float nrnd2 = nrand( n + 0.13*t );
    const float nrnd3 = nrand( n + 0.17*t );

    const float nrnd4 = nrand( n + 0.19*t );
    const float nrnd5 = nrand( n + 0.23*t );
    const float nrnd6 = nrand( n + 0.29*t );
    const float nrnd7 = nrand( n + 0.31*t );

    return (nrnd0 + nrnd1 + nrnd2 + nrnd3 + nrnd4 + nrnd5 + nrnd6 + nrnd7) / 8.0;
}

#undef nrand
//-----------------------------------------------------------------------------

// from https://www.shadertoy.com/view/ttc3zr

ND_ uint  MHash11 (uint src) {
    const uint M = 0x5bd1e995u;
    uint h = 1190494759u;
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float  MHash11 (float src) {
    uint h = MHash11(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint  MHash12 (uint2 src) {
    const uint M = 0x5bd1e995u;
    uint h = 1190494759u;
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float  MHash12 (float2 src) {
    uint h = MHash12(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint  MHash13 (uint3 src) {
    const uint M = 0x5bd1e995u;
    uint h = 1190494759u;
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float  MHash13 (float3 src) {
    uint h = MHash13(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint  MHash14 (uint4 src) {
    const uint M = 0x5bd1e995u;
    uint h = 1190494759u;
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z; h *= M; h ^= src.w;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float  MHash14 (float4 src) {
    uint h = MHash14(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint2  MHash21 (uint src) {
    const uint M = 0x5bd1e995u;
    uint2 h = uint2(1190494759u, 2147483647u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float2  MHash21 (float src) {
    uint2 h = MHash21(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint2  MHash22 (uint2 src) {
    const uint M = 0x5bd1e995u;
    uint2 h = uint2(1190494759u, 2147483647u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float2  MHash22 (float2 src) {
    uint2 h = MHash22(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint2  MHash23 (uint3 src) {
    const uint M = 0x5bd1e995u;
    uint2 h = uint2(1190494759u, 2147483647u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float2  MHash23 (float3 src) {
    uint2 h = MHash23(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint2  MHash24 (uint4 src) {
    const uint M = 0x5bd1e995u;
    uint2 h = uint2(1190494759u, 2147483647u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z; h *= M; h ^= src.w;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float2  MHash24 (float4 src) {
    uint2 h = MHash24(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint3  MHash31 (uint src) {
    const uint M = 0x5bd1e995u;
    uint3 h = uint3(1190494759u, 2147483647u, 3559788179u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float3  MHash31 (float src) {
    uint3 h = MHash31(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint3  MHash32 (uint2 src) {
    const uint M = 0x5bd1e995u;
    uint3 h = uint3(1190494759u, 2147483647u, 3559788179u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float3  MHash32 (float2 src) {
    uint3 h = MHash32(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint3  MHash33 (uint3 src) {
    const uint M = 0x5bd1e995u;
    uint3 h = uint3(1190494759u, 2147483647u, 3559788179u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float3  MHash33 (float3 src) {
    uint3 h = MHash33(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint3  MHash34 (uint4 src) {
    const uint M = 0x5bd1e995u;
    uint3 h = uint3(1190494759u, 2147483647u, 3559788179u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z; h *= M; h ^= src.w;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float3  MHash34 (float4 src) {
    uint3 h = MHash34(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint4  MHash41 (uint src) {
    const uint M = 0x5bd1e995u;
    uint4 h = uint4(1190494759u, 2147483647u, 3559788179u, 179424673u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float4  MHash41 (float src) {
    uint4 h = MHash41(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint4  MHash42 (uint2 src) {
    const uint M = 0x5bd1e995u;
    uint4 h = uint4(1190494759u, 2147483647u, 3559788179u, 179424673u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float4  MHash42 (float2 src) {
    uint4 h = MHash42(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint4  MHash43 (uint3 src) {
    const uint M = 0x5bd1e995u;
    uint4 h = uint4(1190494759u, 2147483647u, 3559788179u, 179424673u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float4  MHash43 (float3 src) {
    uint4 h = MHash43(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

ND_ uint4  MHash44 (uint4 src) {
    const uint M = 0x5bd1e995u;
    uint4 h = uint4(1190494759u, 2147483647u, 3559788179u, 179424673u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z; h *= M; h ^= src.w;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

ND_ float4  MHash44 (float4 src) {
    uint4 h = MHash44(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}
//-----------------------------------------------------------------------------
