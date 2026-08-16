// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	uint4	- 128bit block
*/

#include "Math.glsl"

#ifndef USE_LOW_PRECISION
#	define USE_LOW_PRECISION	1
#endif

#if USE_LOW_PRECISION and AE_ENABLE_HALF_TYPE
#	define float		half
#	define float2		half2		// 1 packed op  instead of 2 scalar ops for fp32
#	define float3		half3		// 2 packed ops instead of 3 scalar ops for fp32
#	define float4		half4		// 2 packed ops instead of 4 scalar ops for fp32
#endif
//-----------------------------------------------------------------------------


ND_ float4  DecodeASTC (uint4 block, int2 localTexel);		// RGBA8_UNorm
ND_ float4  DecodeASTC16F (uint4 block, int2 localTexel);	// RGBA16F
//-----------------------------------------------------------------------------


//#include "Compression/dec_astc.glsl"
//-----------------------------------------------------------------------------


#if USE_LOW_PRECISION and AE_ENABLE_HALF_TYPE
#	undef float
#	undef float2
#	undef float3
#	undef float4
#endif
