// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	uint2	- 64bit block
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


ND_ float4  DecodeETC2 (uint2 block, int2 localTexel);		// RGB8
ND_ float4  DecodeETC2A1 (uint2 block, int2 localTexel);	// RGB8_A1 mode
ND_ float4  DecodeETC2_EAC (uint4 block, int2 localTexel);	// RGBA8 mode

ND_ float4  DecodeEAC_Ru (uint2 block, int2 localTexel);	// R11_UNorm
ND_ float4  DecodeEAC_Rs (uint2 block, int2 localTexel);	// R11_SNorm
ND_ float4  DecodeEAC_RGu (uint4 block, int2 localTexel);	// RG11_UNorm
ND_ float4  DecodeEAC_RGs (uint4 block, int2 localTexel);	// RG11_SNorm
//-----------------------------------------------------------------------------


#include "Compression/dec_etc2.glsl"
//#include "3party_shaders/Compression/dec_etc2.glsl"

#include "Compression/dec_eac.glsl"
//-----------------------------------------------------------------------------


float4  DecodeETC2_EAC (uint4 block, int2 localTexel)
{
	float4	col;
	col.rgb = DecodeETC2( block.zw, localTexel ).rgb;
	col.a   = DecodeEAC_Ru( block.xy, localTexel ).r;
	return col;
}

float4  DecodeEAC_RGu (uint4 block, int2 localTexel)
{
	float2	col;
	col.r = DecodeEAC_Ru( block.xy, localTexel ).r;
	col.g = DecodeEAC_Ru( block.zw, localTexel ).r;
	return float4(col, 0.0, 0.0);
}

float4  DecodeEAC_RGs (uint4 block, int2 localTexel)
{
	float2	col;
	col.r = DecodeEAC_Rs( block.xy, localTexel ).r;
	col.g = DecodeEAC_Rs( block.zw, localTexel ).r;
	return float4(col, 0.0, 0.0);
}
//-----------------------------------------------------------------------------


#if USE_LOW_PRECISION and AE_ENABLE_HALF_TYPE
#	undef float
#	undef float2
#	undef float3
#	undef float4
#endif
