// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Wave functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

// Wave
ND_ float  SineWave (float samp, float freq, float sampleRate);         // --U`U--
ND_ float  SawWave (float samp, float freq, float sampleRate);          //  --/|/|--
ND_ float  TriangleWave (float samp, float freq, float sampleRate);     // --/\/\--

// Gain
ND_ float  LinearGain (float samp, float value, float startTime, float endTime, float sampleRate);
ND_ float  ExpGain (float samp, float value, float startTime, float endTime, float sampleRate);
//-----------------------------------------------------------------------------



float  SineWave (float samp, float freq, float sampleRate)  // --U`U--
{
    return Sin( Pi() * samp * freq / sampleRate );
}

float  SawWave (float samp, float freq, float sampleRate)   //  --/|/|--
{
    return ToSNOrm( Fract( samp * freq / sampleRate ));
}

float  TriangleWave (float samp, float freq, float sampleRate)  // --/\/\--
{
    float value = Fract( samp * freq / sampleRate );
    return Min( value, 1.0 - value ) * 4.0 - 1.0;
}

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
