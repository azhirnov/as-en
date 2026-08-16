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


ND_ float4  DecodeBC1 (uint2 block, int2 localTexel);		// RGB & RGB_A1 mode
ND_ float4  DecodeBC4S (uint2 block, int2 localTexel);		// R8_SNorm
ND_ float4  DecodeBC4U (uint2 block, int2 localTexel);		// R8_UNorm	- better accuracy
ND_ float4  DecodeBC5S (uint4 block, int2 localTexel);		// RG8_SNorm
ND_ float4  DecodeBC5U (uint4 block, int2 localTexel);		// RG8_UNorm - better accuracy
ND_ float4  DecodeBC6_UF16 (uint4 block, int2 localTexel);	// RGB16F
ND_ float4  DecodeBC6_SF16 (uint4 block, int2 localTexel);	// RGB16F
ND_ float4  DecodeBC7 (uint4 block, int2 localTexel);		// RGBA high quality
//-----------------------------------------------------------------------------


#include "Compression/dec_bc1.glsl"
#include "Compression/dec_bc4_bc5.glsl"

#include "3party_shaders/Compression/dec_bc_bitextract.glsl"
#include "3party_shaders/Compression/dec_bc6h.glsl"
#include "3party_shaders/Compression/dec_bc7.glsl"
//-----------------------------------------------------------------------------


#if USE_LOW_PRECISION and AE_ENABLE_HALF_TYPE
#	undef float
#	undef float2
#	undef float3
#	undef float4

ND_ uint  EncodeRGB565 (float3 color)	{ return EncodeRGB565( half3(color) ); }
#endif
