// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/Serializing.h"
#include "pch/Scripting.h"

#include "graphics_rhi/Public/FeatureSet.h"
#include "graphics_rhi/Public/ResourceEnums.h"
#include "graphics_rhi/Public/SamplerDesc.h"
#include "graphics_rhi/Public/PipelineDesc.h"
#include "graphics_rhi/Public/RenderPassDesc.h"
#include "graphics_rhi/Public/IDs.h"
#include "graphics_rhi/Public/EResourceState.h"
#include "graphics_rhi/Public/ShaderEnums.h"
#include "graphics_rhi/Public/RenderStateEnums.h"
#include "graphics_rhi/Public/VertexEnums.h"
#include "graphics_rhi/Public/DescriptorSet.h"

#include "graphics_rhi/Private/EnumUtils.h"
#include "graphics_rhi/Private/EnumToString.h"
#include "graphics_rhi/Private/PixelFormatDefines.h"

#include "graphics_rhi/Scripting/GraphicsBindings.h"

#include "res_pack/pipeline_compiler/PipelineCompiler.h"

#include "res_pack/pipeline_compiler/Packer/PipelinePack.h"
#include "res_pack/pipeline_compiler/Compiler/IShaderPreprocessor.h"

namespace AE::PipelineCompiler
{
	using AE::Scripting::ScriptEnginePtr;

	template <typename T>
	using ScriptRC				= Scripting::AngelScriptHelper::SharedPtr<T>;
	using EnableScriptRC		= Scripting::AngelScriptHelper::SimpleRefCounter;

	template <typename T>
	using ScriptArray			= AE::Scripting::ScriptArray<T>;

	struct DescriptorCount
	{
		uint	uniformBuffers			= 0;
		uint	dynamicUniformBuffers	= 0;
		uint	storageBuffers			= 0;
		uint	dynamicStorageBuffers	= 0;
		uint	storageImages			= 0;
		uint	sampledImages			= 0;
		uint	subpassInputs			= 0;
		uint	samplers				= 0;
		uint	rayTracingScenes		= 0;
		uint	subsampledSamplers		= 0;	// always counted in 'samplers'

		DescriptorCount () {}

		void  Add (const DescriptorSetLayoutDesc::Uniform &);

		ND_ uint  TotalCount () const {
			return	uniformBuffers + dynamicUniformBuffers + storageBuffers + dynamicStorageBuffers +
					storageImages + sampledImages + subpassInputs + samplers + rayTracingScenes;
		}
	};

	using ShaderDefines_t		= Array< String >;
	using PerStageDescCount_t	= FixedMap< EShader, DescriptorCount, uint(EShader::_Count) >;

	struct GraphicsPipelineScriptBinding;
	struct MeshPipelineScriptBinding;
	struct ComputePipelineScriptBinding;
	struct TilePipelineScriptBinding;
	struct RayTracingPipelineScriptBinding;

	struct GraphicsPipelineSpecScriptBinding;
	struct MeshPipelineSpecScriptBinding;
	struct ComputePipelineSpecScriptBinding;
	struct TilePipelineSpecScriptBinding;
	struct RayTracingPipelineSpecScriptBinding;

	struct SubpassShaderIO;

	struct RenderTechnique;
	struct RayTracingShaderBinding;

	using RenderTechniquePtr			= ScriptRC< RenderTechnique >;
	using RayTracingShaderBindingPtr	= ScriptRC< RayTracingShaderBinding >;


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
		_BITOPS_
	};


	Nd__In Version2  EShaderVersion_Ver2 (EShaderVersion value)
	{
		const uint	ver = uint(value & ~EShaderVersion::_Mask);
		return Version2{ (ver >> 4) & 0xF, (ver & 0xF) };
	}



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
		All					= ((_Last - 1) << 1) - 1,
		Unknown				= 0,
	};


	//
	// Access Type
	//
	enum class EAccessType : uint
	{
		Unknown				= 0,
		Coherent,
		Volatile,
		Restrict,			// better for performance

		_MemoryModel,		// GL_KHR_memory_scope_semantics
		DeviceCoherent,
		QueueFamilyCoherent,
		WorkgroupCoherent,
		SubgroupCoherent,
		NonPrivate,

		//NonTemporal,		// GL_EXT_nontemporal_keyword

		_Count,
	};



	//
	// Structure Layout
	//
	enum class EStructLayout : ubyte
	{
		Compatible_Std140,		// compatible with Metal, Std140, HLSL_Const
		Compatible_Std430,		// compatible with Metal, Std430, HLSL_Struct; GLSL uniform buffer may requires 'scalarLayout' feature
		Metal,					// as in MSL
		Std140,					// as in GLSL, uniform/storage buffer
		Std430,					// as in GLSL, storage buffer; GLSL uniform buffer may requires 'scalarLayout' feature
		InternalIO,				// for vertex input and internal shader input/output, GLSL can use precision instead of 8/16 bit types
		//Scalar,				// TODO: GL_EXT_scalar_block_layout
		HLSL_Const,				// as in HLSL, ConstBuffer
		HLSL_Struct,			// as in HLSL, StructBuffer
		_Count,
		Unknown = 0xFF,
	};



	//
	// Value Type
	//
	enum class EValueType : ubyte
	{
		Unknown	= 0,

		Bool8,
		Bool32,

		Int8,
		Int16,
		Int32,
		Int64,

		UInt8,
		UInt16,
		UInt32,
		UInt64,

		//FloatLowp,	// sign: yes, exponent: 0,  mantissa: 8
		//UFloat10,		// sign: no,  exponent: 5,  mantissa: 5
		//UFloat11,		// sign: no,  exponent: 5,  mantissa: 6
		//BFloat16,		// sign: yes, exponent: 8,  mantissa: 7
		Float16,		// sign: yes, exponent: 5,  mantissa: 10
		Float32,		// sign: yes, exponent: 8,  mantissa: 23
		Float64,		// sign: yes, exponent: 11, mantissa: 52

		Int8_Norm,
		Int16_Norm,

		UInt8_Norm,
		UInt16_Norm,

		DeviceAddress,

		_Count
	};



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
	// Metal Limits
	//
	struct MetalLimits
	{
		// per stage:
		static constexpr uint	maxBuffers		= 31;	// uniform + storage + vertex + index + argument + acceleration structure + push constant
		static constexpr uint	maxImages		= 31;	// image + texture + texel buffer	// TODO: can be 96, 128
		static constexpr uint	maxSamplers		= 16;
	};


	struct CompiledShader;
	using CompiledShaderPtr = Ptr<const CompiledShader>;



	//
	// Shader Preprocessor
	//
	enum class EShaderPreprocessor : uint
	{
		None	= 0,
		AEStyle,
		_Count
	};


} // AE::PipelineCompiler
