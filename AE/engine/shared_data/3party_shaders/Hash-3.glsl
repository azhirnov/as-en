#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


#ifdef AE_LICENSE_CC_BY_NC_SA_3
/*
=================================================
	MHash (uint)
----
	from https://www.shadertoy.com/view/ttc3zr
	license CC BY-NC-SA 3.0
=================================================
*/
ND_ uint  MHash11 (uint src)
{
	const uint M = 0x5bd1e995u;
	uint h = 1190494759u;
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint  MHash12 (uint2 src)
{
	const uint M = 0x5bd1e995u;
	uint h = 1190494759u;
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint  MHash13 (uint3 src)
{
	const uint M = 0x5bd1e995u;
	uint h = 1190494759u;
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint  MHash14 (uint4 src)
{
	const uint M = 0x5bd1e995u;
	uint h = 1190494759u;
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z; h *= M; h ^= src.w;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint2  MHash21 (uint src)
{
	const uint M = 0x5bd1e995u;
	uint2 h = uint2(1190494759u, 2147483647u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint2  MHash22 (uint2 src)
{
	const uint M = 0x5bd1e995u;
	uint2 h = uint2(1190494759u, 2147483647u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint2  MHash23 (uint3 src)
{
	const uint M = 0x5bd1e995u;
	uint2 h = uint2(1190494759u, 2147483647u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint2  MHash24 (uint4 src)
{
	const uint M = 0x5bd1e995u;
	uint2 h = uint2(1190494759u, 2147483647u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z; h *= M; h ^= src.w;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint3  MHash31 (uint src)
{
	const uint M = 0x5bd1e995u;
	uint3 h = uint3(1190494759u, 2147483647u, 3559788179u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint3  MHash32 (uint2 src)
{
	const uint M = 0x5bd1e995u;
	uint3 h = uint3(1190494759u, 2147483647u, 3559788179u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint3  MHash33 (uint3 src)
{
	const uint M = 0x5bd1e995u;
	uint3 h = uint3(1190494759u, 2147483647u, 3559788179u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint3  MHash34 (uint4 src)
{
	const uint M = 0x5bd1e995u;
	uint3 h = uint3(1190494759u, 2147483647u, 3559788179u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z; h *= M; h ^= src.w;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint4  MHash41 (uint src)
{
	const uint M = 0x5bd1e995u;
	uint4 h = uint4(1190494759u, 2147483647u, 3559788179u, 179424673u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint4  MHash42 (uint2 src)
{
	const uint M = 0x5bd1e995u;
	uint4 h = uint4(1190494759u, 2147483647u, 3559788179u, 179424673u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint4  MHash43 (uint3 src)
{
	const uint M = 0x5bd1e995u;
	uint4 h = uint4(1190494759u, 2147483647u, 3559788179u, 179424673u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

ND_ uint4  MHash44 (uint4 src)
{
	const uint M = 0x5bd1e995u;
	uint4 h = uint4(1190494759u, 2147483647u, 3559788179u, 179424673u);
	src *= M; src ^= src>>24u; src *= M;
	h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z; h *= M; h ^= src.w;
	h ^= h>>13u; h *= M; h ^= h>>15u;
	return h;
}

/*
=================================================
	MHash (float)
----
	from https://www.shadertoy.com/view/ttc3zr
	license CC BY-NC-SA 3.0
=================================================
*/
ND_ float  MHash11 (float src)
{
	uint h = MHash11(floatBitsToUint(src));
	return float(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float  MHash12 (float2 src)
{
	uint h = MHash12(floatBitsToUint(src));
	return float(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float  MHash13 (float3 src)
{
	uint h = MHash13(floatBitsToUint(src));
	return float(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float  MHash14 (float4 src)
{
	uint h = MHash14(floatBitsToUint(src));
	return float(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float2  MHash21 (float src)
{
	uint2 h = MHash21(floatBitsToUint(src));
	return float2(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float2  MHash22 (float2 src)
{
	uint2 h = MHash22(floatBitsToUint(src));
	return float2(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float2  MHash23 (float3 src)
{
	uint2 h = MHash23(floatBitsToUint(src));
	return float2(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float2  MHash24 (float4 src)
{
	uint2 h = MHash24(floatBitsToUint(src));
	return float2(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float3  MHash31 (float src)
{
	uint3 h = MHash31(floatBitsToUint(src));
	return float3(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float3  MHash32 (float2 src)
{
	uint3 h = MHash32(floatBitsToUint(src));
	return float3(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float3  MHash33 (float3 src)
{
	uint3 h = MHash33(floatBitsToUint(src));
	return float3(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float3  MHash34 (float4 src)
{
	uint3 h = MHash34(floatBitsToUint(src));
	return float3(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float4  MHash41 (float src)
{
	uint4 h = MHash41(floatBitsToUint(src));
	return float4(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float4  MHash42 (float2 src)
{
	uint4 h = MHash42(floatBitsToUint(src));
	return float4(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float4  MHash43 (float3 src)
{
	uint4 h = MHash43(floatBitsToUint(src));
	return float4(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

ND_ float4  MHash44 (float4 src)
{
	uint4 h = MHash44(floatBitsToUint(src));
	return float4(uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0);
}

#endif // AE_LICENSE_CC_BY_NC_SA_3
