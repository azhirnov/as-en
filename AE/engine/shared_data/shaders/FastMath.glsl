// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
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


/*
=================================================
	FastSubULP
----
	valid for positive values
=================================================
*/
float  FastSubULP (float x)
{
	uint bits = floatBitsToUint( x );
	bits = bits == 0 ? bits : bits - 1u;
	return uintBitsToFloat( bits );
}

#if AE_ENABLE_HALF_TYPE and AE_ENABLE_SHORT_TYPE
	half  FastSubULP (half x)
	{
		ushort bits = halfBitsToUint16( x );
		bits = bits == 0 ? bits : bits - 1us;
		return uint16BitsToHalf( bits );
	}
#endif
#if AE_ENABLE_DOUBLE_TYPE and AE_ENABLE_LONG_TYPE
	double  FastSubULP (double x)
	{
		ulong bits = doubleBitsToUint64( x );
		bits = bits == 0 ? bits : bits - 1ul;
		return uint64BitsToDouble( bits );
	}
#endif

/*
=================================================
	FastAddULP
----
	valid for positive values
=================================================
*/
float  FastAddULP (float x)
{
	uint bits = floatBitsToUint( x );
	bits += 1u;
	return uintBitsToFloat( bits );
}

#if AE_ENABLE_HALF_TYPE and AE_ENABLE_SHORT_TYPE
	half  FastAddULP (half x)
	{
		ushort bits = halfBitsToUint16( x );
		bits += 1us;
		return uint16BitsToHalf( bits );
	}
#endif
#if AE_ENABLE_DOUBLE_TYPE and AE_ENABLE_LONG_TYPE
	double  FastAddULP (double x)
	{
		ulong bits = doubleBitsToUint64( x );
		bits += 1ul;
		return uint64BitsToDouble( bits );
	}
#endif
