// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Can be used for intellisense or to compile GLSL as C++ source (linking will always fail).
*/

#pragma once

#include <aestyle.glsl.h>
#include <array>

using half2x2_storage			= half2x2;
using half2x3_storage			= half2x3;
using half2x4_storage			= half2x4;
using half3x2_storage			= half3x2;
using half3x3_storage			= half3x3;
using half3x4_storage			= half3x4;
using half4x2_storage			= half4x2;
using half4x3_storage			= half4x3;
using half4x4_storage			= half4x4;

using packed_half2x2_storage	= half2x2;
using packed_half2x3_storage	= half2x3;
using packed_half2x4_storage	= half2x4;
using packed_half3x2_storage	= half3x2;
using packed_half3x3_storage	= half3x3;
using packed_half3x4_storage	= half3x4;
using packed_half4x2_storage	= half4x2;
using packed_half4x3_storage	= half4x3;
using packed_half4x4_storage	= half4x4;

using float2x2_storage			= float2x2;
using float2x3_storage			= float2x3;
using float2x4_storage			= float2x4;
using float3x2_storage			= float3x2;
using float3x3_storage			= float3x3;
using float3x4_storage			= float3x4;
using float4x2_storage			= float4x2;
using float4x3_storage			= float4x3;
using float4x4_storage			= float4x4;

using packed_float2x2_storage	= float2x2;
using packed_float2x3_storage	= float2x3;
using packed_float2x4_storage	= float2x4;
using packed_float3x2_storage	= float3x2;
using packed_float3x3_storage	= float3x3;
using packed_float3x4_storage	= float3x4;
using packed_float4x2_storage	= float4x2;
using packed_float4x3_storage	= float4x3;
using packed_float4x4_storage	= float4x4;

using double2x2_storage			= double2x2;
using double2x3_storage			= double2x3;
using double2x4_storage			= double2x4;
using double3x2_storage			= double3x2;
using double3x3_storage			= double3x3;
using double3x4_storage			= double3x4;
using double4x2_storage			= double4x2;
using double4x3_storage			= double4x3;
using double4x4_storage			= double4x4;

using packed_double2x2_storage	= double2x2;
using packed_double2x3_storage	= double2x3;
using packed_double2x4_storage	= double2x4;
using packed_double3x2_storage	= double3x2;
using packed_double3x3_storage	= double3x3;
using packed_double3x4_storage	= double3x4;
using packed_double4x2_storage	= double4x2;
using packed_double4x3_storage	= double4x3;
using packed_double4x4_storage	= double4x4;

template <typename T, std::size_t S>
using StaticArray = std::array<T,S>;

#define StaticAssert(...)

struct HashVal32
{
	constexpr explicit HashVal32 (uint) {}
};

struct ShaderStructName
{
	constexpr explicit ShaderStructName (HashVal32) {}
};

#include <../cpp/vk_types.h>
