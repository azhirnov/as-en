/*
    3party noise functions.
*/
#ifdef __cplusplus
# pragma once
#endif

/*
=================================================
    GradientNoise
----
    range [-1..1]

    from https://www.shadertoy.com/view/4dffRH
    The MIT License
    Copyright (c) 2017 Inigo Quilez
=================================================
*/
float  GradientNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)
{
    #define hash( _p_ ) ToSNorm( gl.texture.SampleLod( rgbaNoise, (_p_).xy * 0.01 + (_p_).z * float2(0.01723059, 0.053092949), 0.0 ).rgb )

    // grid
    float3 i = Floor(pos);
    float3 w = Fract(pos);

    #if 1
    // quintic interpolant
    float3 u = w*w*w*(w*(w*6.0-15.0)+10.0);
    float3 du = 30.0*w*w*(w*(w-2.0)+1.0);
    #else
    // cubic interpolant
    float3 u = w*w*(3.0-2.0*w);
    float3 du = 6.0*w*(1.0-w);
    #endif

    // gradients
    float3 ga = hash( i+float3(0.0,0.0,0.0) );
    float3 gb = hash( i+float3(1.0,0.0,0.0) );
    float3 gc = hash( i+float3(0.0,1.0,0.0) );
    float3 gd = hash( i+float3(1.0,1.0,0.0) );
    float3 ge = hash( i+float3(0.0,0.0,1.0) );
    float3 gf = hash( i+float3(1.0,0.0,1.0) );
    float3 gg = hash( i+float3(0.0,1.0,1.0) );
    float3 gh = hash( i+float3(1.0,1.0,1.0) );

    // projections
    float va = Dot( ga, w-float3(0.0,0.0,0.0) );
    float vb = Dot( gb, w-float3(1.0,0.0,0.0) );
    float vc = Dot( gc, w-float3(0.0,1.0,0.0) );
    float vd = Dot( gd, w-float3(1.0,1.0,0.0) );
    float ve = Dot( ge, w-float3(0.0,0.0,1.0) );
    float vf = Dot( gf, w-float3(1.0,0.0,1.0) );
    float vg = Dot( gg, w-float3(0.0,1.0,1.0) );
    float vh = Dot( gh, w-float3(1.0,1.0,1.0) );

    // interpolations
    return  va + u.x*(vb-va) + u.y*(vc-va) + u.z*(ve-va) + u.x*u.y*(va-vb-vc+vd) +
            u.y*u.z*(va-vc-ve+vg) + u.z*u.x*(va-vb-ve+vf) + (-va+vb+vc-vd+ve-vf-vg+vh)*u.x*u.y*u.z;
    #undef hash
}

float  GradientNoise (const float3 pos)
{
    #define hash( _p_ ) ToSNorm( DHash33( _p_ ))

    // grid
    float3 i = Floor(pos);
    float3 w = Fract(pos);

    #if 1
    // quintic interpolant
    float3 u = w*w*w*(w*(w*6.0-15.0)+10.0);
    float3 du = 30.0*w*w*(w*(w-2.0)+1.0);
    #else
    // cubic interpolant
    float3 u = w*w*(3.0-2.0*w);
    float3 du = 6.0*w*(1.0-w);
    #endif

    // gradients
    float3 ga = hash( i+float3(0.0,0.0,0.0) );
    float3 gb = hash( i+float3(1.0,0.0,0.0) );
    float3 gc = hash( i+float3(0.0,1.0,0.0) );
    float3 gd = hash( i+float3(1.0,1.0,0.0) );
    float3 ge = hash( i+float3(0.0,0.0,1.0) );
    float3 gf = hash( i+float3(1.0,0.0,1.0) );
    float3 gg = hash( i+float3(0.0,1.0,1.0) );
    float3 gh = hash( i+float3(1.0,1.0,1.0) );

    // projections
    float va = Dot( ga, w-float3(0.0,0.0,0.0) );
    float vb = Dot( gb, w-float3(1.0,0.0,0.0) );
    float vc = Dot( gc, w-float3(0.0,1.0,0.0) );
    float vd = Dot( gd, w-float3(1.0,1.0,0.0) );
    float ve = Dot( ge, w-float3(0.0,0.0,1.0) );
    float vf = Dot( gf, w-float3(1.0,0.0,1.0) );
    float vg = Dot( gg, w-float3(0.0,1.0,1.0) );
    float vh = Dot( gh, w-float3(1.0,1.0,1.0) );

    // interpolations
    return  va + u.x*(vb-va) + u.y*(vc-va) + u.z*(ve-va) + u.x*u.y*(va-vb-vc+vd) +
            u.y*u.z*(va-vc-ve+vg) + u.z*u.x*(va-vb-ve+vf) + (-va+vb+vc-vd+ve-vf-vg+vh)*u.x*u.y*u.z;
    #undef hash
}

/*
=================================================
    IQNoise
----
    range [-1..1]

    from https://www.shadertoy.com/view/Xd23Dh
    The MIT License
    Copyright (c) 2014 Inigo Quilez
=================================================
*/
float  IQNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, float u, float v)
{
    #define hash( _p_ ) gl.texture.SampleLod( rgbaNoise, (_p_).xy * 0.01 + (_p_).z * float2(0.01723059, 0.053092949), 0.0 ).rgb

    float3 p = Floor(pos);
    float3 f = Fract(pos);

    float k = 1.0 + 63.0 * Pow( 1.0 - v, 6.0 );

    float va = 0.0;
    float wt = 0.0;
    [[unroll]] for (int z = -2; z <= 2; ++z)
    [[unroll]] for (int y = -2; y <= 2; ++y)
    [[unroll]] for (int x = -2; x <= 2; ++x)
    {
        float3 g = float3( float(x),float(y), float(z) );
        float3 o = hash( p + g ) * float3(u,u,1.0);
        float3 r = g - f + o;
        float d = Dot( r, r );
        float ww = Pow( 1.0 - SmoothStep(0.0,1.414, Sqrt(d)), k );
        va += o.z*ww;
        wt += ww;
    }

    return va/wt;
    #undef hash
}

float  IQNoise (const float3 pos, float u, float v)
{
    #define hash( _p_ ) DHash33( _p_ )

    float3 p = Floor(pos);
    float3 f = Fract(pos);

    float k = 1.0 + 63.0 * Pow( 1.0 - v, 6.0 );

    float va = 0.0;
    float wt = 0.0;
    [[unroll]] for (int z = -2; z <= 2; ++z)
    [[unroll]] for (int y = -2; y <= 2; ++y)
    [[unroll]] for (int x = -2; x <= 2; ++x)
    {
        float3 g = float3( float(x),float(y), float(z) );
        float3 o = hash( p + g ) * float3(u,u,1.0);
        float3 r = g - f + o;
        float d = Dot( r, r );
        float ww = Pow( 1.0 - SmoothStep(0.0,1.414, Sqrt(d)), k );
        va += o.z*ww;
        wt += ww;
    }

    return va/wt;
    #undef hash
}

/*
=================================================
    ValueNoise
----
    range [-1..1]

    from https://www.shadertoy.com/view/4sc3z2
    license CC BY-NC-SA 3.0
=================================================
*/
float  ValueNoise (gl::CombinedTex2D<float> greyNoise, const float3 pos)
{
    #define hash( _p_ ) ToSNorm( gl.texture.SampleLod( greyNoise, (_p_).xy * 0.01 + (_p_).z * float2(0.01723059, 0.053092949), 0.0 ).r )

    float3 pi = Floor(pos);
    float3 pf = pos - pi;

    float3 w = pf * pf * (3.0 - 2.0 * pf);

    return  Lerp(
                Lerp(
                    Lerp(hash(pi + float3(0, 0, 0)), hash(pi + float3(1, 0, 0)), w.x),
                    Lerp(hash(pi + float3(0, 0, 1)), hash(pi + float3(1, 0, 1)), w.x),
                    w.z),
                Lerp(
                    Lerp(hash(pi + float3(0, 1, 0)), hash(pi + float3(1, 1, 0)), w.x),
                    Lerp(hash(pi + float3(0, 1, 1)), hash(pi + float3(1, 1, 1)), w.x),
                    w.z),
                w.y);
    #undef hash
}

float  ValueNoise (const float3 pos)
{
    #define hash( _p_ ) ToSNorm( DHash13( _p_ ))

    float3 pi = Floor(pos);
    float3 pf = pos - pi;

    float3 w = pf * pf * (3.0 - 2.0 * pf);

    return  Lerp(
                Lerp(
                    Lerp(hash(pi + float3(0, 0, 0)), hash(pi + float3(1, 0, 0)), w.x),
                    Lerp(hash(pi + float3(0, 0, 1)), hash(pi + float3(1, 0, 1)), w.x),
                    w.z),
                Lerp(
                    Lerp(hash(pi + float3(0, 1, 0)), hash(pi + float3(1, 1, 0)), w.x),
                    Lerp(hash(pi + float3(0, 1, 1)), hash(pi + float3(1, 1, 1)), w.x),
                    w.z),
                w.y);
    #undef hash
}

/*
=================================================
    PerlinNoise
----
    range [-1..1]

    from https://www.shadertoy.com/view/4sc3z2
    license CC BY-NC-SA 3.0
=================================================
*/
float  PerlinNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)
{
    #define hash( _p_ ) ToSNorm( gl.texture.SampleLod( rgbaNoise, (_p_).xy * 0.01 + (_p_).z * float2(0.08723059, 0.053092949), 0.0 ).rgb )

    float3 pi = Floor(pos);
    float3 pf = pos - pi;

    float3 w = pf * pf * (3.0 - 2.0 * pf);

    return  Lerp(
                Lerp(
                    Lerp(Dot(pf - float3(0, 0, 0), hash(pi + float3(0, 0, 0))),
                         Dot(pf - float3(1, 0, 0), hash(pi + float3(1, 0, 0))),
                         w.x),
                    Lerp(Dot(pf - float3(0, 0, 1), hash(pi + float3(0, 0, 1))),
                         Dot(pf - float3(1, 0, 1), hash(pi + float3(1, 0, 1))),
                         w.x),
                    w.z),
                Lerp(
                    Lerp(Dot(pf - float3(0, 1, 0), hash(pi + float3(0, 1, 0))),
                         Dot(pf - float3(1, 1, 0), hash(pi + float3(1, 1, 0))),
                         w.x),
                    Lerp(Dot(pf - float3(0, 1, 1), hash(pi + float3(0, 1, 1))),
                         Dot(pf - float3(1, 1, 1), hash(pi + float3(1, 1, 1))),
                         w.x),
                    w.z),
                w.y);
    #undef hash
}

float  PerlinNoise (const float3 pos)
{
    #define hash( _p_ ) ToSNorm( DHash33( _p_ ))

    float3 pi = Floor(pos);
    float3 pf = pos - pi;

    float3 w = pf * pf * (3.0 - 2.0 * pf);

    return  Lerp(
                Lerp(
                    Lerp(Dot(pf - float3(0, 0, 0), hash(pi + float3(0, 0, 0))),
                         Dot(pf - float3(1, 0, 0), hash(pi + float3(1, 0, 0))),
                         w.x),
                    Lerp(Dot(pf - float3(0, 0, 1), hash(pi + float3(0, 0, 1))),
                         Dot(pf - float3(1, 0, 1), hash(pi + float3(1, 0, 1))),
                         w.x),
                    w.z),
                Lerp(
                    Lerp(Dot(pf - float3(0, 1, 0), hash(pi + float3(0, 1, 0))),
                         Dot(pf - float3(1, 1, 0), hash(pi + float3(1, 1, 0))),
                         w.x),
                    Lerp(Dot(pf - float3(0, 1, 1), hash(pi + float3(0, 1, 1))),
                         Dot(pf - float3(1, 1, 1), hash(pi + float3(1, 1, 1))),
                         w.x),
                    w.z),
                w.y);
    #undef hash
}

/*
=================================================
    SimplexNoise
----
    range [-1..1]

    from https://www.shadertoy.com/view/4sc3z2
    license CC BY-NC-SA 3.0
=================================================
*/
float  SimplexNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos)
{
    #define hash( _p_ ) ToSNorm( gl.texture.SampleLod( rgbaNoise, (_p_).xy * 0.02 + (_p_).z * float2(0.01723059, 0.053092949), 0.0 ).rgb )

    const float K1 = 0.333333333;
    const float K2 = 0.166666667;

    float3 i = Floor(pos + (pos.x + pos.y + pos.z) * K1);
    float3 d0 = pos - (i - (i.x + i.y + i.z) * K2);

    float3 e = Step(float3(0.0), d0 - d0.yzx);
    float3 i1 = e * (1.0 - e.zxy);
    float3 i2 = 1.0 - e.zxy * (1.0 - e);

    float3 d1 = d0 - (i1 - 1.0 * K2);
    float3 d2 = d0 - (i2 - 2.0 * K2);
    float3 d3 = d0 - (1.0 - 3.0 * K2);

    float4 h = Max(0.6 - float4(Dot(d0, d0), Dot(d1, d1), Dot(d2, d2), Dot(d3, d3)), 0.0);
    float4 n = h * h * h * h * float4(Dot(d0, hash(i)), Dot(d1, hash(i + i1)), Dot(d2, hash(i + i2)), Dot(d3, hash(i + 1.0)));

    return Dot(float4(31.316), n);
    #undef hash
}

float  SimplexNoise (const float3 pos)
{
    #define hash( _p_ ) ToSNorm( DHash33( _p_ ))

    const float K1 = 0.333333333;
    const float K2 = 0.166666667;

    float3 i = Floor(pos + (pos.x + pos.y + pos.z) * K1);
    float3 d0 = pos - (i - (i.x + i.y + i.z) * K2);

    float3 e = Step(float3(0.0), d0 - d0.yzx);
    float3 i1 = e * (1.0 - e.zxy);
    float3 i2 = 1.0 - e.zxy * (1.0 - e);

    float3 d1 = d0 - (i1 - 1.0 * K2);
    float3 d2 = d0 - (i2 - 2.0 * K2);
    float3 d3 = d0 - (1.0 - 3.0 * K2);

    float4 h = Max(0.6 - float4(Dot(d0, d0), Dot(d1, d1), Dot(d2, d2), Dot(d3, d3)), 0.0);
    float4 n = h * h * h * h * float4(Dot(d0, hash(i)), Dot(d1, hash(i + i1)), Dot(d2, hash(i + i2)), Dot(d3, hash(i + 1.0)));

    return Dot(float4(31.316), n);
    #undef hash
}

/*
=================================================
    VoronoiContour
----
    range [0..inf]

    from https://www.shadertoy.com/view/ldl3W8
    The MIT License
    Copyright (c) 2013 Inigo Quilez
=================================================
*/
float  VoronoiContour (const float2 coord, const float2 seedScaleBias)
{
    float2  ipoint  = Floor( coord );
    float2  fpoint  = Fract( coord );

    float2  icenter = float2(0.0);
    float   md      = 2147483647.0;
    float2  mr;

    [[unroll]] for (int y = -1; y <= 1; ++y)
    [[unroll]] for (int x = -1; x <= 1; ++x)
    {
        float2  cur = float2(x, y);
        float2  off = DHash22( (cur + ipoint) * seedScaleBias.x + seedScaleBias.y ) + cur - fpoint;
        float   d   = Dot( off, off );

        if ( d < md )
        {
            md = d;
            mr = off;
            icenter = cur;
        }
    }

    md = 2147483647.0;
    [[unroll]] for (int y = -2; y <= 2; ++y)
    [[unroll]] for (int x = -2; x <= 2; ++x)
    {
        float2  cur = icenter + float2(x, y);
        float2  off = DHash22( (cur + ipoint) * seedScaleBias.x + seedScaleBias.y ) + cur - fpoint;
        float   d   = Dot( mr - off, mr - off );

        if ( d > 0.00001 )
            md = Min( md, Dot( 0.5*(mr + off), Normalize(off - mr) ));
    }

    return md;
}

float  VoronoiCircles (const float2 coord, const float radiusScale, const float2 seedScaleBias)
{
    const int radius = 1;

    float2  ipoint  = Floor( coord );
    float2  fpoint  = Fract( coord );

    float2  icenter = float2(0.0);
    float   md      = 2147483647.0;
    float   mr      = 2147483647.0;

    // find nearest circle
    [[unroll]] for (int y = -1; y <= 1; ++y)
    [[unroll]] for (int x = -1; x <= 1; ++x)
    {
        float2  cur = float2(x, y);
        float2  off = DHash22( (cur + ipoint) * seedScaleBias.x + seedScaleBias.y ) + cur - fpoint;
        float   d   = Dot( off, off );

        if ( d < md )
        {
            md = d;
            icenter = cur;
        }
    }

    // calc circle radius
    [[unroll]] for (int y = -2; y <= 2; ++y)
    [[unroll]] for (int x = -2; x <= 2; ++x)
    {
        if ( AllEqual( int2(x,y), int2(0) ))
            continue;

        float2  cur = icenter + float2(x, y);
        float2  off = DHash22( (cur + ipoint) * seedScaleBias.x + seedScaleBias.y ) + cur - fpoint;
        float   d   = Dot( off, off );

        if ( d < mr )
            mr = d;
    }

    md = Sqrt( md );
    mr = Sqrt( mr ) * 0.5 * radiusScale;

    if ( md < mr )
        return 1.0 / (Square( md / mr ) * 16.0) - 0.07;

    return 0.0;
}

/*
=================================================
    WaveletNoise
----

    from https://www.shadertoy.com/view/wsBfzK
    The MIT License
    Copyright (c) 2020 Martijn Steinrucken
=================================================
*/
float  WaveletNoise (float2 coord, const float z, const float k)
{
    float d = 0.0f;
    float s = 1.0f;
    float m = 0.0f;
    for (float i = 0.0f; i < 4.0f; ++i)
    {
        float2 q = coord * s;
        float2 g = Fract(Floor(q) * float2(123.34f, 233.53f));
        g += Dot(g, g + 23.234f);
        float a = Fract(g.x * g.y) * 1.0e+3f;// +z*(mod(g.x+g.y, 2.)-1.); // add vorticity
        q = (Fract(q) - 0.5) * float2x2(Cos(a), -Sin(a), Sin(a), Cos(a));
        d += Sin(q.x * 10.0 + z) * SmoothStep(0.25f, 0.0f, Dot(q,q)) / s;
        coord = coord * float2x2(0.54f, -0.84f, 0.84f, 0.54f) + i;
        m += 1.0 / s;
        s *= k;
    }
    return d / m;
}

