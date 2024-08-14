// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Easing functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

ND_ float  HermiteEaseIn (const float x);
ND_ float  HermiteEaseOut (const float x);
ND_ float  HermiteEaseInOut (const float x);

ND_ float  QuadraticEaseIn (const float x);			// x^2
ND_ float  QuadraticEaseOut (const float x);
ND_ float  QuadraticEaseInOut (const float x);

ND_ float  CubicEaseIn (const float x);				// x^3
ND_ float  CubicEaseOut (float x);
ND_ float  CubicEaseInOut (const float x);

ND_ float  QuarticEaseIn (const float x);			// x^4
ND_ float  QuarticEaseOut (const float x);
ND_ float  QuarticEaseInOut (const float x);

ND_ float  QuinticEaseIn (const float x);			// x^5
ND_ float  QuinticEaseOut (const float x);
ND_ float  QuinticEaseInOut (const float x);
//-----------------------------------------------------------------------------


ND_ float  LogarithmicEaseIn (const float x);			// Ln(x)
ND_ float  Logarithmic2EaseIn (const float x);			// Log2(x)
ND_ float  ReciprocalEaseIn (const float x);			// 1/x
ND_ float  SineEaseIn (const float x);					// Sin(x)
ND_ float  ExponentialEaseIn (const float x);			// Exp(x)
ND_ float  ReciprocalSquaredEaseIn (const float x);		// 1/x^2

#define EaseFlip( fn, x )	(1.0 - fn( 1.0 - (x) ))


//-----------------------------------------------------------------------------
#include "../3party_shaders/Easing-1.glsl"


float  HermiteEaseIn (const float x) {
	return SmoothStep( x * 0.5, 0.0, 1.0 ) * 2.0;
}

float  HermiteEaseOut (const float x) {
	return ToSNorm( SmoothStep( ToUNorm(x), 0.0, 1.0 ));
}

float  HermiteEaseInOut (const float x) {
	return SmoothStep( x, 0.0, 1.0 );
}

float  ReciprocalEaseIn (const float x) {
	return 1.0 / (-x * 0.5 + 1.0) - 1.0;
}

float  ReciprocalSquaredEaseIn (const float x) {
	return 1.0 / (-Saturate(x*x)*0.5 + 1.0) - 1.0;
}

float  LogarithmicEaseIn (const float x) {
	return Ln( x * 1.72 + 1.0 );
}

float  Logarithmic2EaseIn (const float x) {
	return Log2( x * 2.0 + 2.0 ) - 1.0;
}

float  ExponentialEaseIn (const float x) {
	return Exp( x * 2.1268 - 2.0 ) - 0.134;
}

float  SineEaseIn (const float x) {
	return 1.0 - Sin( (1.0 - x) * float_HalfPi );
}
