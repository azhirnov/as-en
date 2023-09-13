// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Wave functions.
    Can be used for sound generation or as easing functions.
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

// Wave //
ND_ float   SineWave (const float samp, const float freq, const float sampleRate);          // --U`U--

// SawWave
ND_ float   SawWave (const float samp, const float freq, const float sampleRate);           //  --/|/|--
ND_ float   SawWave (float  x);
ND_ float2  SawWave (float2 x);
ND_ float3  SawWave (float3 x);

// TriangleWave
ND_ float   TriangleWave (const float samp, const float freq, const float sampleRate);      // --/\/\--
ND_ float   TriangleWave (float  x);
ND_ float2  TriangleWave (float2 x);
ND_ float3  TriangleWave (float3 x);

// LinearStep
ND_ float   LinearStep (const float  x, const float  edge0, const float  edge1);
ND_ float2  LinearStep (const float2 x, const float2 edge0, const float2 edge1);
ND_ float2  LinearStep (const float2 x, const float  edge0, const float  edge1);
ND_ float3  LinearStep (const float3 x, const float3 edge0, const float3 edge1);
ND_ float3  LinearStep (const float3 x, const float  edge0, const float  edge1);

// BumpStep
ND_ float   BumpStep (const float  x, const float  edge0, const float  edge1);
ND_ float2  BumpStep (const float2 x, const float2 edge0, const float2 edge1);
ND_ float2  BumpStep (const float2 x, const float  edge0, const float  edge1);
ND_ float3  BumpStep (const float3 x, const float3 edge0, const float3 edge1);
ND_ float3  BumpStep (const float3 x, const float  edge0, const float  edge1);

// SmoothBumpStep
ND_ float   SmoothBumpStep (const float  x, const float  edge0, const float  edge1);
ND_ float2  SmoothBumpStep (const float2 x, const float2 edge0, const float2 edge1);
ND_ float2  SmoothBumpStep (const float2 x, const float  edge0, const float  edge1);
ND_ float3  SmoothBumpStep (const float3 x, const float3 edge0, const float3 edge1);
ND_ float3  SmoothBumpStep (const float3 x, const float  edge0, const float  edge1);


// Gain //
ND_ float   LinearGain (float samp, float value, float startTime, float endTime, float sampleRate);
ND_ float   ExpGain (float samp, float value, float startTime, float endTime, float sampleRate);
//-----------------------------------------------------------------------------



float  SineWave (const float samp, const float freq, const float sampleRate)        // --U`U--
{
    return Sin( Pi() * samp * freq / sampleRate );
}


float  SawWave (const float samp, const float freq, const float sampleRate)         //  --/|/|--
{
    return SawWave( samp * freq / sampleRate );
}

float  SawWave (float  x)   { return ToSNorm( Fract( x )); }
float2 SawWave (float2 x)   { return ToSNorm( Fract( x )); }
float3 SawWave (float3 x)   { return ToSNorm( Fract( x )); }


float  TriangleWave (const float samp, const float freq, const float sampleRate)    // --/\/\--
{
    return TriangleWave( samp * freq / sampleRate );
}

float  TriangleWave (float  x)  { x = Fract( x );  return Min( x, 1.f - x ) * 4.f - 1.f; }
float2 TriangleWave (float2 x)  { x = Fract( x );  return Min( x, 1.f - x ) * 4.f - 1.f; }
float3 TriangleWave (float3 x)  { x = Fract( x );  return Min( x, 1.f - x ) * 4.f - 1.f; }


float  LinearGain (float samp, float value, float startTime, float endTime, float sampleRate)
{
    float start = startTime * sampleRate;
    float end   = endTime * sampleRate;
    return All(bool2( samp > start, samp < end )) ?
            value * (1.0 - (samp - start) / (end - start)) :
            0.0;
}

float  ExpGain (float samp, float value, float startTime, float endTime, float sampleRate)
{
    float start = startTime * sampleRate;
    float end   = endTime * sampleRate;
    float power = 4.0;
    return All(bool2( samp > start, samp < end )) ?
            value * (1.0 - Pow( power, (samp - start) / (end - start) ) / power) :
            0.0;
}
//-----------------------------------------------------------------------------


float   LinearStep (const float  x, const float  edge0, const float  edge1)     { return Saturate( (x - edge0) / (edge1 - edge0) ); }
float2  LinearStep (const float2 x, const float2 edge0, const float2 edge1)     { return Saturate( (x - edge0) / (edge1 - edge0) ); }
float2  LinearStep (const float2 x, const float  edge0, const float  edge1)     { return Saturate( (x - edge0) / (edge1 - edge0) ); }
float3  LinearStep (const float3 x, const float3 edge0, const float3 edge1)     { return Saturate( (x - edge0) / (edge1 - edge0) ); }
float3  LinearStep (const float3 x, const float  edge0, const float  edge1)     { return Saturate( (x - edge0) / (edge1 - edge0) ); }

float   BumpStep (const float  x, const float  edge0, const float  edge1)       { return 1.f - Abs(Saturate( (x - edge0) / (edge1 - edge0) ) - 0.5f) * 2.f; }
float2  BumpStep (const float2 x, const float2 edge0, const float2 edge1)       { return 1.f - Abs(Saturate( (x - edge0) / (edge1 - edge0) ) - 0.5f) * 2.f; }
float2  BumpStep (const float2 x, const float  edge0, const float  edge1)       { return 1.f - Abs(Saturate( (x - edge0) / (edge1 - edge0) ) - 0.5f) * 2.f; }
float3  BumpStep (const float3 x, const float3 edge0, const float3 edge1)       { return 1.f - Abs(Saturate( (x - edge0) / (edge1 - edge0) ) - 0.5f) * 2.f; }
float3  BumpStep (const float3 x, const float  edge0, const float  edge1)       { return 1.f - Abs(Saturate( (x - edge0) / (edge1 - edge0) ) - 0.5f) * 2.f; }

float   SmoothBumpStep (const float  x, const float  edge0, const float  edge1) { float  t = BumpStep( x, edge0, edge1 );  return t * t * (3.f - 2.f * t); }
float2  SmoothBumpStep (const float2 x, const float2 edge0, const float2 edge1) { float2 t = BumpStep( x, edge0, edge1 );  return t * t * (3.f - 2.f * t); }
float2  SmoothBumpStep (const float2 x, const float  edge0, const float  edge1) { float2 t = BumpStep( x, edge0, edge1 );  return t * t * (3.f - 2.f * t); }
float3  SmoothBumpStep (const float3 x, const float3 edge0, const float3 edge1) { float3 t = BumpStep( x, edge0, edge1 );  return t * t * (3.f - 2.f * t); }
float3  SmoothBumpStep (const float3 x, const float  edge0, const float  edge1) { float3 t = BumpStep( x, edge0, edge1 );  return t * t * (3.f - 2.f * t); }
//-----------------------------------------------------------------------------
