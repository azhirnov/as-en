// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	uint4	- 128bit block
*/

#include "Math.glsl"

#ifndef USE_LOW_PRECISION
#	define USE_LOW_PRECISION	0
#endif

#if USE_LOW_PRECISION and AE_ENABLE_HALF_TYPE
#	define float		half
#	define float2		half2		// 1 packed op  instead of 2 scalar ops for fp32
#	define float3		half3		// 2 packed ops instead of 3 scalar ops for fp32
#	define float4		half4		// 2 packed ops instead of 4 scalar ops for fp32
#endif
//-----------------------------------------------------------------------------

ND_ bool  DecodeASTC (uint4 block, const uint2 blockSize, const int2 localTexel, out float4 outColor);	// RGBA8_UNorm


ND_ float4  DecodeASTC_4x4 (uint4 block, int2 localTexel);		// RGBA8_UNorm / RGBA16F
ND_ float4  DecodeASTC_8x8 (uint4 block, int2 localTexel);		// RGBA8_UNorm / RGBA16F
//-----------------------------------------------------------------------------


#include "../3party_shaders/Compression/dec_astc.glsl"
//-----------------------------------------------------------------------------


float4  DecodeASTC_4x4 (uint4 block, int2 localTexel)
{
	float4	col;
	if ( !DecodeASTC( block, uint2(4,4), localTexel, OUT col ))
		return float4(-1.0);

	return col;
}

float4  DecodeASTC_8x8 (uint4 block, int2 localTexel)
{
	float4	col;
	if ( !DecodeASTC( block, uint2(8,8), localTexel, OUT col ))
		return float4(-1.0);

	return col;
}
//-----------------------------------------------------------------------------


#if USE_LOW_PRECISION and AE_ENABLE_HALF_TYPE
#	undef float
#	undef float2
#	undef float3
#	undef float4
#endif
