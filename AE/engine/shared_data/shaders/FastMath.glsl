// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Approximations for math functions.

	Warning: may be slower than native functions!
*/

#ifdef __cplusplus
# pragma once
#endif


#include "Math.glsl"

#if 0
	// T: halfX, floatX, doubleX
	T  FastSin (T x);
	T  FastCos (T x);
	T  FastTan (T x);

	T  FastACos (T x);
	T  FastASin (T x);
	T  FastATan (T y_over_x);
	T  FastATan (T y, T x);

	T  FastASin2 (T x);
	T  FastACos2 (T x);
	T  FastATan2 (T y_over_x);
	T  FastATan2 (T y, T x);
#endif
//-----------------------------------------------------------------------------

#include "../3party_shaders/FastMath-1.glsl"


// TODO:
// Exponentiation  https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#exponentiation-with-small-fractional-arguments
