// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Noise functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Hash.glsl"


ND_ float  GradientNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos);                                            // range [-1..1]
ND_ float  GradientNoise (const float3 pos);                                                                                // range [-1..1]

ND_ float  IQNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, float u, float v);                                // range [-1..1]
ND_ float  IQNoise (const float3 pos, float u, float v);                                                                    // range [-1..1]

ND_ float  ValueNoise (gl::CombinedTex2D<float> greyNoise, const float3 pos);                                               // range [-1..1]
ND_ float  ValueNoise (const float3 pos);                                                                                   // range [-1..1]
ND_ float  ValueNoiseFBM (const float3 pos);                                                                                // range [-1..1]
ND_ float  ValueNoiseFBM2 (const float3 pos);                                                                               // range [-1..1]

ND_ float  PerlinNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos);                                              // range [-1..1]
ND_ float  PerlinNoise (const float3 pos);                                                                                  // range [-1..1]
ND_ float  PerlinFBM (in float3 pos, const float lacunarity, const float persistence, const int octaveCount);               // range [-1..1]
ND_ float  PerlinFBM2 (in float3 pos, const float lacunarity, const float persistence, const int octaveCount);              // range [-1..1]
ND_ float  PerlinFBM (gl::CombinedTex2D<float> rgbaNoise, in float3 pos, const float lacunarity, const float persistence, const int octaveCount);   // range [-1..1]
ND_ float  PerlinFBM2 (gl::CombinedTex2D<float> rgbaNoise, in float3 pos, const float lacunarity, const float persistence, const int octaveCount);  // range [-1..1]

ND_ float3 Turbulence (const float3 pos, const float power, const float lacunarity, const float persistence, const int octaveCount);    // returns position
ND_ float3 Turbulence2 (const float3 pos, const float power, const float lacunarity, const float persistence, const int octaveCount);   // returns position
ND_ float3 Turbulence (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const float power, const float lacunarity, const float persistence, const int octaveCount);    // returns position
ND_ float3 Turbulence2 (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const float power, const float lacunarity, const float persistence, const int octaveCount);   // returns position

ND_ float  SimplexNoise (gl::CombinedTex2D<float> rgbaNoise, const float3 pos);                                             // range [-1..1]
ND_ float  SimplexNoise (const float3 pos);                                                                                 // range [-1..1]

struct VoronoiResult2
{
    float2  icenter;
    float2  offset;     // ceil center = icenter + offset
    float   minDist;    // squared distance in range [0..inf]
};
ND_ VoronoiResult2  Voronoi (const float2 coord, const float2 seedScaleBias);

struct VoronoiResult3
{
    float3  icenter;
    float3  offset;     // ceil center = icenter + offset
    float   minDist;    // squared distance in range [0..inf]
};
ND_ VoronoiResult3  Voronoi (const float3 coord, const float2 seedScaleBias);

ND_ float  WarleyFBM (in float3 pos, const float lacunarity, const float persistence, const int octaveCount);               // range [0..1]
ND_ float  WarleyFBM2 (in float3 pos, const float lacunarity, const float persistence, const int octaveCount);              // range [0..1]

ND_ float  VoronoiContour (const float2 coord, const float2 seedScaleBias);                                                 // range [0..inf]
ND_ float  VoronoiCircles (const float2 coord, const float radiusScale, const float2 seedScaleBias);                        // range [0..inf]

ND_ float  TilableVoronoiNoise (const float3 pos, const float3 tileSize, const float2 seedScaleBias);                       // range [0..inf]
ND_ float  TilableWarleyNoise (const float3 pos, const float3 tileSize, const float2 seedScaleBias);                        // range [0..1]
ND_ float  TilableVoronoiFBM (in float3 pos, in float3 tileSize, const float lacunarity, const float persistence, const int octaveCount, const float2 seedScaleBias);   // range [0..inf]
ND_ float  TilableWarleyFBM (in float3 pos, in float3 tileSize, const float lacunarity, const float persistence, const int octaveCount, const float2 seedScaleBias);    // range [0..inf]

ND_ float  WaveletNoise (float2 coord, const float z, const float k);                                                       // range [0..1]
//-----------------------------------------------------------------------------


#include "../3party_shaders/Noise-1.glsl"
#include "../3party_shaders/Noise-2.glsl"
//-----------------------------------------------------------------------------


void  _TurbulenceTransform (const float3 pos, out float3 p0, out float3 p1, out float3 p2)
{
    p0 = pos + float3( 0.189422, 0.993713, 0.478164 );
    p1 = pos + float3( 0.404647, 0.276611, 0.923049 );
    p2 = pos + float3( 0.821228, 0.171096, 0.684280 );
}

ND_ float3  _FBMRotate (const float3 pos)
{
    const float3x3  rot = float3x3( 0.00,  0.80,  0.60,
                                   -0.80,  0.36, -0.48,
                                   -0.60, -0.48,  0.64 );
    return rot * pos;
}
//-----------------------------------------------------------------------------



float  ValueNoiseFBM (in float3 pos, const float lacunarity, const float persistence, const int octaveCount)
{
    float   value   = 0.0;
    float   pers    = 1.0;

    for (int octave = 0; octave < octaveCount; ++octave)
    {
        value += ValueNoise( pos ) * pers;
        pos   *= lacunarity;
        pers  *= persistence;
    }
    return value;
}

float  ValueNoiseFBM2 (in float3 pos, const float lacunarity, const float persistence, const int octaveCount)
{
    float   value   = 0.0;
    float   pers    = 1.0;

    for (int octave = 0; octave < octaveCount; ++octave)
    {
        value += ValueNoise( pos ) * pers;
        pos    = _FBMRotate( pos * lacunarity );
        pers  *= persistence;
    }
    return value;
}
//-----------------------------------------------------------------------------


// range [-1..1]
float  PerlinFBM (in float3 pos, const float lacunarity, const float persistence, const int octaveCount)
{
    float   value   = 0.0;
    float   pers    = 1.0;

    for (int octave = 0; octave < octaveCount; ++octave)
    {
        value += PerlinNoise( pos ) * pers;
        pos   *= lacunarity;
        pers  *= persistence;
    }
    return value;
}

// range [-1..1]
float  PerlinFBM2 (in float3 pos, const float lacunarity, const float persistence, const int octaveCount)
{
    float   value   = 0.0;
    float   pers    = 1.0;

    for (int octave = 0; octave < octaveCount; ++octave)
    {
        value += PerlinNoise( pos ) * pers;
        pos    = _FBMRotate( pos * lacunarity );
        pers  *= persistence;
    }
    return value;
}

// range [-1..1]
float  PerlinFBM (gl::CombinedTex2D<float> rgbaNoise, in float3 pos, const float lacunarity, const float persistence, const int octaveCount)
{
    float   value   = 0.0;
    float   pers    = 1.0;

    for (int octave = 0; octave < octaveCount; ++octave)
    {
        value += PerlinNoise( rgbaNoise, pos ) * pers;
        pos   *= lacunarity;
        pers  *= persistence;
    }
    return value;
}

// range [-1..1]
float  PerlinFBM2 (gl::CombinedTex2D<float> rgbaNoise, in float3 pos, const float lacunarity, const float persistence, const int octaveCount)
{
    float   value   = 0.0;
    float   pers    = 1.0;

    for (int octave = 0; octave < octaveCount; ++octave)
    {
        value += PerlinNoise( rgbaNoise, pos ) * pers;
        pos    = _FBMRotate( pos * lacunarity );
        pers  *= persistence;
    }
    return value;
}
//-----------------------------------------------------------------------------



// returns position
float3  Turbulence (const float3 pos, const float power, const float lacunarity, const float persistence, const int octaveCount)
{
    float3 p0, p1, p2;
    _TurbulenceTransform( pos, p0, p1, p2 );
    const float3 distort = float3(PerlinFBM2( p0, lacunarity, persistence, octaveCount ),
                                  PerlinFBM2( p1, lacunarity, persistence, octaveCount ),
                                  PerlinFBM2( p2, lacunarity, persistence, octaveCount )) * power + pos;
    return distort;
}

float3  Turbulence2 (const float3 pos, const float power, const float lacunarity, const float persistence, const int octaveCount)
{
    float3 p0, p1, p2;
    _TurbulenceTransform( pos, p0, p1, p2 );
    const float3 distort = float3(PerlinFBM( p0, lacunarity, persistence, octaveCount ),
                                  PerlinFBM( p1, lacunarity, persistence, octaveCount ),
                                  PerlinFBM( p2, lacunarity, persistence, octaveCount )) * power + pos;
    return distort;
}

float3  Turbulence (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const float power, const float lacunarity, const float persistence, const int octaveCount)
{
    float3 p0, p1, p2;
    _TurbulenceTransform( pos, p0, p1, p2 );
    const float3 distort = float3(PerlinFBM2( rgbaNoise, p0, lacunarity, persistence, octaveCount ),
                                  PerlinFBM2( rgbaNoise, p1, lacunarity, persistence, octaveCount ),
                                  PerlinFBM2( rgbaNoise, p2, lacunarity, persistence, octaveCount )) * power + pos;
    return distort;
}

float3  Turbulence2 (gl::CombinedTex2D<float> rgbaNoise, const float3 pos, const float power, const float lacunarity, const float persistence, const int octaveCount)
{
    float3 p0, p1, p2;
    _TurbulenceTransform( pos, p0, p1, p2 );
    const float3 distort = float3(PerlinFBM( rgbaNoise, p0, lacunarity, persistence, octaveCount ),
                                  PerlinFBM( rgbaNoise, p1, lacunarity, persistence, octaveCount ),
                                  PerlinFBM( rgbaNoise, p2, lacunarity, persistence, octaveCount )) * power + pos;
    return distort;
}
//-----------------------------------------------------------------------------


// range [0..inf]
VoronoiResult2  Voronoi (const float2 coord, const float2 seedScaleBias)
{
    float2  ipoint  = Floor( coord );
    float2  fpoint  = Fract( coord );

    VoronoiResult2  result;
    result.minDist = 1.0e+30;

    [[unroll]] for (int y = -1; y <= 1; ++y)
    [[unroll]] for (int x = -1; x <= 1; ++x)
    {
        float2  ioffset = float2( x, y );
        float2  offset  = DHash22( (ipoint + ioffset) * seedScaleBias.x + seedScaleBias.y );
        float2  vec     = offset + ioffset - fpoint;
        float   d       = Dot( vec, vec );

        if ( d < result.minDist )
        {
            result.minDist  = Min( result.minDist, d );
            result.icenter  = ipoint + ioffset;
            result.offset   = offset;
        }
    }
    return result;
}

// range [0..inf]
VoronoiResult3  Voronoi (const float3 coord, const float2 seedScaleBias)
{
    float3  ipoint  = Floor( coord );
    float3  fpoint  = Fract( coord );

    VoronoiResult3  result;
    result.minDist = 1.0e+30;

    [[unroll]] for (int z = -1; z <= 1; ++z)
    [[unroll]] for (int y = -1; y <= 1; ++y)
    [[unroll]] for (int x = -1; x <= 1; ++x)
    {
        float3  ioffset = float3( x, y, z );
        float3  offset  = DHash33( (ipoint + ioffset) * seedScaleBias.x + seedScaleBias.y );
        float3  vec     = offset + ioffset - fpoint;
        float   d       = Dot( vec, vec );

        if ( d < result.minDist )
        {
            result.minDist  = Min( result.minDist, d );
            result.icenter  = ipoint + ioffset;
            result.offset   = offset;
        }
    }
    return result;
}
//-----------------------------------------------------------------------------


// range [0..1]
float  WarleyFBM (in float3 pos, const float lacunarity, const float persistence, const int octaveCount)
{
    float   value   = 0.0;
    float   pers    = 1.0;

    for (int octave = 0; octave < octaveCount; ++octave)
    {
        value += (1.0 - Voronoi( pos, float2(1.0, 0.0) ).minDist) * pers;
        pos   *= lacunarity;
        pers  *= persistence;
    }
    return value;
}

// range [0..1]
float  WarleyFBM2 (in float3 pos, const float lacunarity, const float persistence, const int octaveCount)
{
    float   value   = 0.0;
    float   pers    = 1.0;

    for (int octave = 0; octave < octaveCount; ++octave)
    {
        value += (1.0 - Voronoi( pos, float2(1.0, 0.0) ).minDist) * pers;
        pos    = _FBMRotate( pos * lacunarity );
        pers  *= persistence;
    }
    return value;
}
//-----------------------------------------------------------------------------


// range [0..inf]
float  TilableVoronoiNoise (const float3 pos, const float3 tileSize, const float2 seedScaleBias)
{
    float3  ipoint  = Floor( pos * tileSize );
    float3  fpoint  = Fract( pos * tileSize );
    float   md      = 1.0e+30;

    [[unroll]] for (int z = -1; z <= 1; ++z)
    [[unroll]] for (int y = -1; y <= 1; ++y)
    [[unroll]] for (int x = -1; x <= 1; ++x)
    {
        float3  ioffset = float3(x, y, z);
        float3  offset  = DHash33( Mod( ipoint + ioffset, tileSize ) * seedScaleBias.x + seedScaleBias.y );
        float3  vec     = offset + ioffset - fpoint;
        float   d       = Dot( vec, vec );
        md = Min( md, d );
    }
    return md;
}

// range [0..1]
float  TilableWarleyNoise (const float3 pos, const float3 tileSize, const float2 seedScaleBias)
{
    return Max( 1.0 - TilableVoronoiNoise( pos, tileSize, seedScaleBias ), 0.0 );
}


// range [0..inf]
float  TilableVoronoiFBM (in float3 pos, in float3 tileSize, const float lacunarity, const float persistence, const int octaveCount, const float2 seedScaleBias)
{
    float   value   = 0.0;
    float   pers    = 1.0;

    for (int octave = 0; octave < octaveCount; ++octave)
    {
        value    += TilableVoronoiNoise( pos, tileSize, seedScaleBias ) * pers;
        tileSize *= lacunarity;
        pers     *= persistence;
    }
    return value;
}

// range [0..inf]
float  TilableWarleyFBM (in float3 pos, in float3 tileSize, const float lacunarity, const float persistence, const int octaveCount, const float2 seedScaleBias)
{
    float   value   = 0.0;
    float   pers    = 1.0;

    for (int octave = 0; octave < octaveCount; ++octave)
    {
        value    += TilableWarleyNoise( pos, tileSize, seedScaleBias ) * pers;
        tileSize *= lacunarity;
        pers     *= persistence;
    }
    return value;
}
//-----------------------------------------------------------------------------
