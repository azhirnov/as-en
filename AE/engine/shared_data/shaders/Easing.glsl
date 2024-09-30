// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Easing functions
*/

#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"

ND_ float  QuadraticEaseIn (const float x);				// x^2
ND_ float  QuadraticEaseOut (const float x);
ND_ float  QuadraticEaseInOut (const float x);

ND_ float  CubicEaseIn (const float x);					// x^3
ND_ float  CubicEaseOut (float x);
ND_ float  CubicEaseInOut (const float x);

ND_ float  QuarticEaseIn (const float x);				// x^4
ND_ float  QuarticEaseOut (const float x);
ND_ float  QuarticEaseInOut (const float x);

ND_ float  QuinticEaseIn (const float x);				// x^5
ND_ float  QuinticEaseOut (const float x);
ND_ float  QuinticEaseInOut (const float x);

ND_ float  SineEaseIn (const float x);					// Sin(x)
ND_ float  SineEaseOut (const float x);
ND_ float  SineEaseInOut (const float x);

ND_ float  CircularEaseIn (const float x);				// Sqrt(1 - x*x)
ND_ float  CircularEaseOut (const float x);
ND_ float  CircularEaseInOut (const float x);

ND_ float  ExponentialEaseIn (const float x);			// Exp2(x)
ND_ float  ExponentialEaseOut (const float x);
ND_ float  ExponentialEaseInOut (const float x);

ND_ float  ElasticEaseIn (const float x);
ND_ float  ElasticEaseOut (const float x);
ND_ float  ElasticEaseInOut (const float x);
//-----------------------------------------------------------------------------


ND_ float  HermiteEaseIn (const float x);
ND_ float  HermiteEaseOut (const float x);
ND_ float  HermiteEaseInOut (const float x);			// smoothstep

ND_ float  LogarithmicEaseIn (const float x);			// Ln(x)
ND_ float  LogarithmicEaseOut (const float x);

ND_ float  Logarithmic2EaseIn (const float x);			// Log2(x)
ND_ float  Logarithmic2EaseOut (const float x);

ND_ float  ReciprocalEaseIn (const float x);			// 1/x
ND_ float  ReciprocalEaseOut (const float x);

ND_ float  ExponentialE_EaseIn (const float x);			// Exp(x)
ND_ float  ExponentialE_EaseOut (const float x);

ND_ float  ReciprocalSquaredEaseIn (const float x);		// 1/x*x
ND_ float  ReciprocalSquaredEaseOut (const float x);

ND_ float  SquareRootEaseIn (const float x);			// Sqrt(x)
ND_ float  SquareRootEaseOut (const float x);

ND_ float  CubicRootEaseIn (const float x);				// Cbrt(x)
ND_ float  CubicRootEaseOut (const float x);

#define EaseFlip( fn, x )	(1.0 - fn( 1.0 - (x) ))		// EaseIn <-> EaseOut


// TODO: https://iquilezles.org/articles/functions/

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
//------------------------------------------------


float  ReciprocalEaseIn (const float x) {
	return 1.0 / (-x * 0.5 + 1.0) - 1.0;
}

float  ReciprocalEaseOut (const float x) {
	return EaseFlip( ReciprocalEaseIn, x );
}
//------------------------------------------------


float  ReciprocalSquaredEaseIn (const float x) {
	return 1.0 / (-Saturate(x*x)*0.5 + 1.0) - 1.0;
}

float  ReciprocalSquaredEaseOut (const float x) {
	return EaseFlip( ReciprocalSquaredEaseIn, x );
}
//------------------------------------------------


float  LogarithmicEaseOut (const float x) {
	return Ln( x * 1.72 + 1.0 );
}

float  LogarithmicEaseIn (const float x) {
	return EaseFlip( LogarithmicEaseOut, x );
}
//------------------------------------------------


float  Logarithmic2EaseOut (const float x) {
	return Log2( x * 2.0 + 2.0 ) - 1.0;
}

float  Logarithmic2EaseIn (const float x) {
	return EaseFlip( Logarithmic2EaseOut, x );
}
//------------------------------------------------


float  ExponentialE_EaseIn (const float x) {
	return Exp( x * 2.1268 - 2.0 ) - 0.134;
}

float  ExponentialE_EaseOut (const float x) {
	return EaseFlip( ExponentialE_EaseIn, x );
}
//------------------------------------------------


float  SquareRootEaseIn (const float x) {
	return EaseFlip( Sqrt, x );
}

float  SquareRootEaseOut (const float x) {
	return Sqrt( x );
}
//------------------------------------------------


float  CubicRootEaseIn (const float x) {
	return EaseFlip( Cbrt, x );
}

float  CubicRootEaseOut (const float x) {
	return Cbrt( x );
}
//------------------------------------------------
