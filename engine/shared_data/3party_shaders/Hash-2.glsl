#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


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
