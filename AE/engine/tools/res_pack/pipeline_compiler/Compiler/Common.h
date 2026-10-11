// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "pch/Base.h"

namespace AE::PipelineCompiler
{

	//
	// Shader Version
	//
	enum class EShaderVersion : uint
	{
		_GLSL_SPIRV			= 1 << 28,
		_Metal_iOS			= 2 << 28,
		_Metal_Mac			= 3 << 28,
		_Metal				= 4 << 28,
		_Slang_SPIRV		= 5 << 28,
		_Slang_CXX			= 6 << 28,		// used to convert slang to c++ code
		_Slang_GLSL			= 7 << 28,		// used to convert slang to glsl
		_Mask				= 0xFu << 28,

		SPIRV_1_0			= 0x10 | _GLSL_SPIRV,	// Vulkan 1.0
		SPIRV_1_1			= 0x11 | _GLSL_SPIRV,
		SPIRV_1_2			= 0x12 | _GLSL_SPIRV,
		SPIRV_1_3			= 0x13 | _GLSL_SPIRV,	// Vulkan 1.1
		SPIRV_1_4			= 0x14 | _GLSL_SPIRV,	// Vulkan 1.1 extension
		SPIRV_1_5			= 0x15 | _GLSL_SPIRV,	// Vulkan 1.2
		SPIRV_1_6			= 0x16 | _GLSL_SPIRV,	// Vulkan 1.3, 1.4
		_SPIRV_Last			= SPIRV_1_6,

		Slang_SPIRV_1_0		= 0x10 | _Slang_SPIRV,	// Vulkan 1.0
		Slang_SPIRV_1_1		= 0x11 | _Slang_SPIRV,
		Slang_SPIRV_1_2		= 0x12 | _Slang_SPIRV,
		Slang_SPIRV_1_3		= 0x13 | _Slang_SPIRV,	// Vulkan 1.1
		Slang_SPIRV_1_4		= 0x14 | _Slang_SPIRV,	// Vulkan 1.1 extension
		Slang_SPIRV_1_5		= 0x15 | _Slang_SPIRV,	// Vulkan 1.2
		Slang_SPIRV_1_6		= 0x16 | _Slang_SPIRV,	// Vulkan 1.3, 1.4
		Slang_SPIRV_Last	= Slang_SPIRV_1_6,

		Metal_2_0			= 0x20 | _Metal,		// Metal 2.x for iOS/iPad/Mac
		Metal_2_1			= 0x21 | _Metal,
		Metal_2_2			= 0x22 | _Metal,
		Metal_2_3			= 0x23 | _Metal,		// ray tracing
		Metal_2_4			= 0x24 | _Metal,		// ray tracing motion blur
		Metal_3_0			= 0x30 | _Metal,		// mesh shader, unified metal language
		Metal_3_1			= 0x31 | _Metal,		//
		Metal_3_2			= 0x32 | _Metal,		//
		Metal_4_0			= 0x40 | _Metal,		//
		_Metal_Last			= Metal_4_0,

		Metal_iOS_2_0		= 0x20 | _Metal_iOS,	// Metal 2.x for iOS/iPad	// iOS 11.0
		Metal_iOS_2_1		= 0x21 | _Metal_iOS,								// iOS 12.0
		Metal_iOS_2_2		= 0x22 | _Metal_iOS,								// iOS 13.0
		Metal_iOS_2_3		= 0x23 | _Metal_iOS,								// iOS 14.0
		Metal_iOS_2_4		= 0x24 | _Metal_iOS,								// iOS 15.0
		Metal_iOS_3_0		= 0x30 | _Metal_iOS,	// Metal 3.x for iOS/iPad	// iOS 16.0
		Metal_iOS_3_1		= 0x31 | _Metal_iOS,								// iOS 17.0
		Metal_iOS_3_2		= 0x31 | _Metal_iOS,								// iOS 18.0
		_Metal_iOS_Last		= Metal_iOS_3_2,

		Metal_Mac_2_0		= 0x20 | _Metal_Mac,	// Metal 2.x for MacOS		// MacOS 10.13
		Metal_Mac_2_1		= 0x21 | _Metal_Mac,								// MacOS 10.14
		Metal_Mac_2_2		= 0x22 | _Metal_Mac,								// MacOS 10.15
		Metal_Mac_2_3		= 0x23 | _Metal_Mac,								// MacOS 11.0
		Metal_Mac_2_4		= 0x24 | _Metal_Mac,								// MacOS 12.0
		Metal_Mac_3_0		= 0x30 | _Metal_Mac,	// Metal 3.x for MacOS		// MacOS 13.0
		Metal_Mac_3_1		= 0x31 | _Metal_Mac,								// MacOS 14.0
		Metal_Mac_3_2		= 0x32 | _Metal_Mac,								// MacOS 15.0
		_Metal_Mac_Last		= Metal_Mac_3_2,

		Unknown				= 0,
		_BITOPS_			= 0
	};


	Nd__In Version2  EShaderVersion_Ver2 (EShaderVersion value)
	{
		const uint	ver = uint(value & ~EShaderVersion::_Mask);
		return Version2{ (ver >> 4) & 0xF, (ver & 0xF) };
	}


	//
	// Compilation Target
	//
	enum class ECompilationTarget : uint
	{
		Unknown		= 0,
		Vulkan,
		Metal_iOS,
		Metal_Mac,
		// TODO: Metal for both iOS and Mac
		_Count
	};


	//
	// Shader Options
	//
	enum class EShaderOpt : uint
	{
		// debug
		DebugInfo			= 1 << 0,
		Trace				= 1 << 1,
		FnProfiling			= 1 << 2,	// function profiling
		TimeHeatMap			= 1 << 3,	// per fragment/thread profiling
		Asserts				= 1 << 4,
		_ShaderTrace_Mask	= Trace | FnProfiling | TimeHeatMap | Asserts,

		// optimize
		Optimize			= 1 << 5,
		OptimizeSize		= 1 << 6,
		StrongOptimization	= 1 << 7,	// very slow

		// options
		WarnAsError			= 1 << 8,

		_Last,
		All					= CT_AllBitMask2<EShaderOpt>,
		Unknown				= 0,
	};


} // AE::PipelineCompiler
