// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Can be used for intellisense or to compile GLSL as C++ source (linking will always fail).
*/

#pragma once

#define AE_memory_scope_semantics
#define AE_HAS_ATOMICS
#define AE_shader_subgroup_basic
#define AE_shader_subgroup_vote
#define AE_shader_subgroup_ballot
#define AE_shader_subgroup_arithmetic
#define AE_shader_subgroup_shuffle
#define AE_shader_subgroup_shuffle_relative
#define AE_shader_subgroup_clustered
#define AE_shader_subgroup_quad
#define AE_nonuniform_qualifier
#define AE_NV_shader_sm_builtins
#define AE_ARM_shader_core_builtins
#define AE_fragment_shading_rate
#define AE_fragment_shader_barycentric
#define AE_demote_to_helper_invocation

#define AE_AMD_GPU
#define AE_NVidia_GPU
#define AE_Intel_GPU
#define AE_ARM_Mali_GPU
#define AE_Qualcomm_Adreno_GPU
#define AE_IMG_PowerVR_GPU
#define AE_Microsoft_GPU
#define AE_Apple_GPU
#define AE_Mesa_GPU_driver
#define AE_Broadcom_GPU
#define AE_Samsung_GPU
#define AE_VeriSilicon_GPU
#define AE_Huawei_GPU

#include <aestyle.glsl.h>
#include <array>

// half
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

using half2x2_storage_std140	= half2x2;
using half2x3_storage_std140	= half2x3;
using half2x4_storage_std140	= half2x4;
using half3x2_storage_std140	= half3x2;
using half3x3_storage_std140	= half3x3;
using half3x4_storage_std140	= half3x4;
using half4x2_storage_std140	= half4x2;
using half4x3_storage_std140	= half4x3;
using half4x4_storage_std140	= half4x4;

// float
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

using float2x2_storage_std140	= float2x2;
using float2x3_storage_std140	= float2x3;
using float2x4_storage_std140	= float2x4;
using float3x2_storage_std140	= float3x2;
using float3x3_storage_std140	= float3x3;
using float3x4_storage_std140	= float3x4;
using float4x2_storage_std140	= float4x2;
using float4x3_storage_std140	= float4x3;
using float4x4_storage_std140	= float4x4;

// double
using double2x2_storage			= double2x2;
using double2x3_storage			= double2x3;
using double2x4_storage			= double2x4;
using double3x2_storage			= double3x2;
using double3x3_storage			= double3x3;
using double3x4_storage			= double3x4;
using double4x2_storage			= double4x2;
using double4x3_storage			= double4x3;
using double4x4_storage			= double4x4;

using double2x2_storage_std140	= double2x2;
using double2x3_storage_std140	= double2x3;
using double2x4_storage_std140	= double2x4;
using double3x2_storage_std140	= double3x2;
using double3x3_storage_std140	= double3x3;
using double3x4_storage_std140	= double3x4;
using double4x2_storage_std140	= double4x2;
using double4x3_storage_std140	= double4x3;
using double4x4_storage_std140	= double4x4;

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


struct PerPassUB
{
#ifdef SH_COMPUTE // or ray tracing
	float		time;			// shader playback time (in seconds)
	float		timeDelta;		// frame render time (in seconds), max value: 1/30s
	uint		frame;			// shader playback frame, global frame counter
	uint		passFrameId;	// current pass frame index
	uint		seed;			// unique value, updated on each shader reloading
	float4		mouse;			// mouse unorm coords. xy: current (if MRB down), zw: click
	float2		customKeys;
	float		pixPerMm;		// pix / mm
	float		mmPerPix;		// mm / pix

	// controller //
	CameraData	camera;
#endif

#ifdef SH_FRAG
	float3		resolution;				// viewport resolution (in pixels)
	float		time;					// shader playback time (in seconds)
	float2		invResolution;			// 1.0/resolution, used for optimization
	float		timeDelta;				// frame render time (in seconds), max value: 1/30s
	uint		frame;					// shader playback frame, global frame counter
	uint		passFrameId;			// current pass frame index
	uint		seed;					// unique value, updated on each shader reloading
	uint		colorSpace;				// swapchain color space (EColorSpace)
	float4		channelTime;			// channel playback time (in seconds)
	float4		channelResolution [4];	// channel resolution (in pixels)
	float4		mouse;					// mouse unorm coords. xy: current (if MRB down), zw: click
	float4		date;					// (year, month, day, time in seconds)
	float		sampleRate;				// sound sample rate (i.e., 44100)
	float		pixPerMm;				// pix / mm
	float		mmPerPix;				// mm / pix
	float2		customKeys;

	// controller //
	CameraData	camera;
#endif
};
static const PerPassUB un_PerPass;


#ifdef SH_COMPUTE
static const ComputePassPC pc;
#endif
