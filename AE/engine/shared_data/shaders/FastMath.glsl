// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Approximations for math functions
*/

#ifdef __cplusplus
# pragma once
#endif


#include "Math.glsl"

// T: halfX, floatX, doubleX

// T  FastACos (T x)
// T  FastASin (T x)
// T  FastATan (T y_over_x)
// T  FastATan2 (T y, T x)

//-----------------------------------------------------------------------------
#include "../3party_shaders/FastMath-1.glsl"


// TODO:
// Exponentiation  https://docs.nvidia.com/cuda/cuda-c-best-practices-guide/index.html#exponentiation-with-small-fractional-arguments
