#ifdef __cplusplus
# pragma once
#endif

#include "Math.glsl"


#ifdef AE_LICENSE_MIT

// DHash from https://www.shadertoy.com/view/4djSRW
// MIT License...
// Copyright (c) 2014 David Hoskins.

// range [0..1]

float4 _DHashScale ()  { return float4( 0.1031, 0.1030, 0.0973, 0.1099 ); }

ND_ float  DHash11 (const float p)
{
	float3 p3 = Fract( float3(p) * _DHashScale().x );
	p3 += Dot( p3, p3.yzx + float(19.19) );
	return Fract( (p3.x + p3.y) * p3.z );
}

ND_ float  DHash12 (const float2 p)
{
	float3 p3 = Fract( float3(p.xyx) * _DHashScale().x );
	p3 += Dot( p3, p3.yzx + float(19.19) );
	return Fract( (p3.x + p3.y) * p3.z );
}

ND_ float  DHash13 (const float3 p)
{
	float3 p3 = Fract( p * _DHashScale().x );
	p3 += Dot( p3, p3.yzx + float(19.19) );
	return Fract( (p3.x + p3.y) * p3.z );
}

ND_ float2  DHash21 (const float p)
{
	float3 p3 = Fract( float3(p) * _DHashScale().xyz );
	p3 += Dot( p3, p3.yzx + float(19.19) );
	return Fract( (p3.xx + p3.yz) * p3.zy );
}

ND_ float2  DHash22 (const float2 p)
{
	float3 p3 = Fract( float3(p.xyx) * _DHashScale().xyz );
	p3 += Dot( p3, p3.yzx + float(19.19) );
	return Fract( (p3.xx + p3.yz) * p3.zy );
}

ND_ float2  DHash23 (const float3 p)
{
	float3 p3 = Fract( p * _DHashScale().xyz );
	p3 += Dot( p3, p3.yzx + float(19.19) );
	return Fract( (p3.xx + p3.yz) * p3.zy );
}

ND_ float3  DHash31 (const float p)
{
	float3 p3 = Fract( float3(p) * _DHashScale().xyz );
	p3 += Dot( p3, p3.yzx + float(19.19) );
	return Fract( (p3.xxy + p3.yzz) * p3.zyx );
}

ND_ float3  DHash32 (const float2 p)
{
	float3 p3 = Fract( float3(p.xyx) * _DHashScale().xyz );
	p3 += Dot( p3, p3.yxz + float(19.19) );
	return Fract( (p3.xxy + p3.yzz) * p3.zyx );
}

ND_ float3  DHash33 (const float3 p)
{
	float3 p3 = Fract( p * _DHashScale().xyz );
	p3 += Dot( p3, p3.yxz + float(19.19) );
	return Fract( (p3.xxy + p3.yxx) * p3.zyx );
}

ND_ float4  DHash41 (const float p)
{
	float4 p4 = Fract( float4(p) * _DHashScale() );
	p4 += Dot( p4, p4.wzxy + float(19.19) );
	return Fract( (p4.xxyz + p4.yzzw) * p4.zywx );
}

ND_ float4  DHash42 (const float2 p)
{
	float4 p4 = Fract( float4(p.xyxy) * _DHashScale() );
	p4 += Dot( p4, p4.wzxy + float(19.19) );
	return Fract( (p4.xxyz + p4.yzzw) * p4.zywx );
}

ND_ float4  DHash43 (const float3 p)
{
	float4 p4 = Fract( float4(p.xyzx) * _DHashScale() );
	p4 += Dot( p4, p4.wzxy + float(19.19) );
	return Fract( (p4.xxyz + p4.yzzw) * p4.zywx );
}

ND_ float4  DHash44 (const float4 p)
{
	float4 p4 = Fract( p * _DHashScale() );
	p4 += Dot( p4, p4.wzxy + float(19.19) );
	return Fract( (p4.xxyz + p4.yzzw) * p4.zywx );
}

#endif // AE_LICENSE_MIT
//-----------------------------------------------------------------------------


#ifdef AE_LICENSE_MIT

// HEHash from https://www.shadertoy.com/view/llGSzw, https://www.shadertoy.com/view/XlXcW4, https://www.shadertoy.com/view/4tXyWN
// The MIT License
// Copyright (c) 2017 Inigo Quilez

// float range [0, 1]

ND_ uint  HEHash11i (uint n)
{
	// integer hash copied from Hugo Elias
	n = (n << 13U) ^ n;
	n = n * (n * n * 15731U + 789221U) + 1376312589U;
	return n;
}

ND_ uint3  HEHash33i (uint3 x)
{
	const uint k = 1103515245u;
	x = ((x>>8U)^x.yzx)*k;
	x = ((x>>8U)^x.yzx)*k;
	x = ((x>>8U)^x.yzx)*k;
	return x;
}

ND_ uint  HEHash12i (uint2 x)
{
	uint2 q = 1103515245U * ( (x>>1U) ^ (x.yx   ) );
	uint  n = 1103515245U * ( (q.x  ) ^ (q.y>>3U) );
	return n;
}

ND_ float  HEHash11 (uint n)
{
	n = HEHash11i( n );
	// floating point conversion from http://iquilezles.org/www/articles/sfrand/sfrand.htm
	return float(uintBitsToFloat( (n>>9U) | 0x3f800000U ) - 1.0);
	//return 1.f - (1.0f / 32767.0f) * float((n>>16) & 0x7FFF);
}

ND_ float2  HEHash22 (uint2 uv)		{ return float2(HEHash11(uv.x), HEHash11(uv.y)); }
ND_ float3  HEHash33 (uint3 uv)		{ return float3(HEHash11(uv.x), HEHash11(uv.y), HEHash11(uv.z)); }
ND_ float4  HEHash44 (uint4 uv)		{ return float4(HEHash11(uv.x), HEHash11(uv.y), HEHash11(uv.z), HEHash11(uv.w)); }

ND_ float   HEHash12 (uint2 uv)		{ return HEHash11( (uv.x * 0x72A34u) ^ (uv.y * 0x3E6CDu) ); }
ND_ float   HEHash13 (uint3 uv)		{ return HEHash11( (uv.x * 0x72A34u) ^ (uv.y * 0x3E6CDu) ^ (uv.z * 0x4AC81u) ); }
ND_ float2  HEHash24 (uint4 uv)		{ return float2(HEHash12(uv.xy), HEHash12(uv.zw)); }

ND_ float   HEHash11 (int n)		{ return HEHash11( uint(n)   ); }
ND_ float2  HEHash22 (int2 uv)		{ return HEHash22( uint2(uv) ); }
ND_ float3  HEHash33 (int3 uv)		{ return HEHash33( uint3(uv) ); }
ND_ float4  HEHash44 (int4 uv)		{ return HEHash44( uint4(uv) ); }
ND_ float   HEHash12 (int2 uv)		{ return HEHash12( uint2(uv) ); }
ND_ float   HEHash13 (int3 uv)		{ return HEHash13( uint3(uv) ); }
ND_ float2  HEHash24 (int4 uv)		{ return HEHash24( uint4(uv) ); }


# if AE_ENABLE_HALF_TYPE
	ND_ half  HEHash11hf (uint n)
	{
		n = HEHash11i( n );
		return uint16BitsToFloat16( ushort( ((n>>22u) & 0x3FF) | 0x3c00u )) - 1.0hf;
	}

	ND_ half   HEHash12hf (uint2 uv)		{ return HEHash11hf( (uv.x * 0x72A34u) ^ (uv.y * 0x3E6CDu) ); }
	ND_ half   HEHash13hf (uint3 uv)		{ return HEHash11hf( (uv.x * 0x72A34u) ^ (uv.y * 0x3E6CDu) ^ (uv.z * 0x4AC81u) ); }
	ND_ half2  HEHash24hf (uint4 uv)		{ return half2(HEHash12hf(uv.xy), HEHash12hf(uv.zw)); }

	ND_ half2  HEHash22hf (uint2 uv)		{ return half2(HEHash11hf(uv.x), HEHash11hf(uv.y)); }
	ND_ half3  HEHash33hf (uint3 uv)		{ return half3(HEHash11hf(uv.x), HEHash11hf(uv.y), HEHash11hf(uv.z)); }
	ND_ half4  HEHash44hf (uint4 uv)		{ return half4(HEHash11hf(uv.x), HEHash11hf(uv.y), HEHash11hf(uv.z), HEHash11hf(uv.w)); }
# endif

#endif // AE_LICENSE_MIT
//-----------------------------------------------------------------------------
