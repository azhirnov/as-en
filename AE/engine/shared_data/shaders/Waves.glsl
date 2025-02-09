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
ND_ float   SineWave (const float samp, const float freq, const float sampleRate);			// --U`U--

// SawWave
ND_ float   SawWave (const float samp, const float freq, const float sampleRate);			//  --/|/|--

// TriangleWave
ND_ float   TriangleWave (const float samp, const float freq, const float sampleRate);		// --/\/\--

// Gain //
ND_ float   LinearGain (float samp, float value, float startTime, float endTime, float sampleRate);
ND_ float   ExpGain (float samp, float value, float startTime, float endTime, float sampleRate);
//-----------------------------------------------------------------------------



float  SineWave (const float samp, const float freq, const float sampleRate)		// --U`U--
{
	return Sin( float_Pi * samp * freq / sampleRate );
}


float  SawWave (const float samp, const float freq, const float sampleRate)			//  --/|/|--
{
	return ToSNorm( Fract( samp * freq / sampleRate ));
}

float  TriangleWave (const float samp, const float freq, const float sampleRate)	// --/\/\--
{
	return ToSNorm( TriangleWave( samp * freq / sampleRate ));
}

float  LinearGain (float samp, float value, float startTime, float endTime, float sampleRate)
{
	float start = startTime * sampleRate;
	float end   = endTime * sampleRate;
	return All2( samp > start, samp < end ) ?
			value * (1.0 - (samp - start) / (end - start)) :
			0.0;
}

float  ExpGain (float samp, float value, float startTime, float endTime, float sampleRate)
{
	float start = startTime * sampleRate;
	float end   = endTime * sampleRate;
	float power = 4.0;
	return All2( samp > start, samp < end ) ?
			value * (1.0 - Pow( power, (samp - start) / (end - start) ) / power) :
			0.0;
}
//-----------------------------------------------------------------------------
