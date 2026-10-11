// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

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

#include "res_pack/pipeline_compiler/Compiler/Common.h"

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
