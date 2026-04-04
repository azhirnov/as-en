// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	FeatureSet is used to:
	- validate shaders & pipelines:
		- resource count
		- workgroup size
		- local memory size
		- extensions, shader version, ...
	- validate render state
	- validate vertex input type
	- select render technique

	Special behaviour:
		Metal:
			max dynamic buffers	per stage limit	= 31
			max dynamic buffers	(vertex stage)	= maxVertexBuffers + maxDescriptorSets + dynamicUB + dynamicSB
			max dynamic buffers	(other stages)	= maxDescriptorSets + dynamicUB + dynamicSB

			max buffers per desc set limit		= Min( maxStorageBuffers, maxUniformBuffers, maxAccelStructures )
			max buffers per desc set			= maxStorageBuffers + maxUniformBuffers + maxAccelStructures

	[docs](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/FeatureSet.md)
*/

#pragma once

#include "graphics_rhi/Public/ShaderEnums.h"
#include "graphics_rhi/Public/RenderState.h"
#include "graphics_rhi/Public/FeatureSetEnums.h"
#include "graphics_rhi/Public/ResourceEnums.h"
#include "graphics_rhi/Public/VertexEnums.h"
#include "graphics_rhi/Public/BufferDesc.h"
#include "graphics_rhi/Public/ImageDesc.h"
#include "graphics_rhi/Public/Queue.h"
#include "graphics_rhi/Public/PipelineDesc.h"

namespace AE::Graphics
{
	enum class EResourceState : uint;


	//
	// Feature Set
	//

	struct FeatureSet
	{
	// types
		enum class EFeature : ubyte
		{
			Ignore		= 0,
			RequireFalse,
			RequireTrue,
			_Count
		};

		struct PerDescriptorSet
		{
			uint	maxInputAttachments;	// maxDescriptorSetInputAttachments			-	maxPerStageDescriptorInputAttachments
			uint	maxSampledImages;		// maxDescriptorSetSampledImages			-	maxPerStageDescriptorSampledImages
			uint	maxSamplers;			// maxDescriptorSetSamplers					-	maxPerStageDescriptorSamplers
			uint	maxStorageBuffers;		// maxDescriptorSetStorageBuffers			-	maxPerStageDescriptorStorageBuffers
			uint	maxStorageImages;		// maxDescriptorSetStorageImages			-	maxPerStageDescriptorStorageImages
			uint	maxUniformBuffers;		// maxDescriptorSetUniformBuffers			-	maxPerStageDescriptorUniformBuffers
			uint	maxAccelStructures;		// maxDescriptorSetAccelerationStructures	-	maxPerStageDescriptorAccelerationStructures
			uint	maxTotalResources;		// maxPerSetDescriptors						-	maxPerStageResources

			ND_ bool  operator == (const PerDescriptorSet &rhs) C_NE___;
			ND_ bool  operator >= (const PerDescriptorSet &rhs) C_NE___;

			ND_ HashVal  CalcHash ()							C_NE___;
		};
		using PerShaderStage		= PerDescriptorSet;	// Metal: for argument buffer

		using SubgroupOperationBits	= EnumSet< ESubgroupOperation >;
		using PixelFormatSet_t		= EnumSet< EPixelFormat >;
		using VertexFormatSet_t		= EnumSet< EVertexType >;
		using SurfaceFormatSet_t	= EnumSet< ESurfaceFormat >;
		using ShadingRateSet_t		= FixedArray< EShadingRate, uint(EShadingRate::_SizeCount) >;	// 10 bytes
		using CoopMatrixSet_t		= EnumSet< ECoopMatrixCfg >;
		using CoopVecSet_t			= EnumSet< ECoopVecCfg >;

		enum class SampleCountBits : uint { Unknown = 0 };

		template <typename E>
		struct IncludeExcludeBits
		{
			EnumSet<E>	include;
			EnumSet<E>	exclude;

			ND_ bool	operator == (const IncludeExcludeBits<E> &rhs)	C_NE___	{ return include == rhs.include and exclude == rhs.exclude; }
			ND_ HashVal	CalcHash ()										C_NE___	{ return HashOf(include) + HashOf(exclude); }
			ND_ bool	None ()											C_NE___	{ return include.None() and exclude.None(); }
		};
		using VendorIDs_t		= IncludeExcludeBits< EGPUVendor >;
		using GraphicsDevices_t	= IncludeExcludeBits< EGraphicsDeviceID >;

		struct Queues
		{
			EQueueMask	supported	= Default;
			EQueueMask	required	= Default;

			ND_ bool	operator == (const Queues &rhs)	C_NE___	{ return supported == rhs.supported and required == rhs.required; }
			ND_ HashVal	CalcHash ()						C_NE___	{ return HashOf(supported) + HashOf(required); }
		};

		static constexpr uint	MinSpirvVersion	= 100;
		static constexpr uint	MaxSpirvVersion	= 160;

		static constexpr uint	MinMetalVersion	= 200;
		static constexpr uint	MaxMetalVersion	= 320;

		struct ShaderVersion
		{
			ushort	spirv	= 0;
			ushort	metal	= 0;

			ND_ HashVal  CalcHash ()		C_NE___	{ return HashOf(spirv) + HashOf(metal); }
		};

		struct VRSTexelSize
		{
			uint	minX		: 5;	// power of 2, range: 1..1024
			uint	minY		: 5;	// power of 2, range: 1..1024
			uint	maxX		: 5;	// power of 2, range: 1..1024
			uint	maxY		: 5;	// power of 2, range: 1..1024
			uint	aspectRatio	: 5;	// power of 2, range: 1..1024

			VRSTexelSize ()					__NE___	{ std::memset( this, 0, sizeof(*this) ); }

			ND_ HashVal  CalcHash ()		C_NE___	{ return HashOf( BitCast<uint>( *this )); }
			ND_ explicit operator bool ()	C_NE___	{ return (minX + minY + maxX + maxY) != 0; }

			ND_ uint2	Min ()				C_NE___	{ return uint2{ 1u << minX, 1u << minY }; }
			ND_ uint2	Max ()				C_NE___	{ return uint2{ 1u << maxX, 1u << maxY }; }
			ND_ uint	MaxAspectRatio ()	C_NE___	{ return 1u << aspectRatio; }
		};

		struct KiBytes
		{
			ushort		_value = 0;

			KiBytes ()								__NE___ {}

			template <typename T> requires( IsInteger<T> )
			explicit KiBytes (T value)				__NE___	: _value{ushort( value >> 10 )}			{ /*ASSERT( T{*this}  == value );*/ }
			explicit KiBytes (Bytes value)			__NE___	: _value{ushort( ulong{value} >> 10 )}	{ /*ASSERT( Bytes{*this} == value );*/ }

			template <typename T> requires( IsInteger<T> )
			ND_ explicit operator T ()				C_NE___	{ return T{_value} << 10; }
			ND_ explicit operator Bytes ()			C_NE___	{ return Bytes{_value} << 10; }

			ND_ bool  operator == (KiBytes rhs)		C_NE___	{ return _value == rhs._value; }
			ND_ bool  operator != (KiBytes rhs)		C_NE___	{ return _value != rhs._value; }
			ND_ bool  operator <  (KiBytes rhs)		C_NE___	{ return _value <  rhs._value; }
			ND_ bool  operator <= (KiBytes rhs)		C_NE___	{ return _value <= rhs._value; }
			ND_ bool  operator >  (KiBytes rhs)		C_NE___	{ return _value >  rhs._value; }
			ND_ bool  operator >= (KiBytes rhs)		C_NE___	{ return _value >= rhs._value; }
		};


	#define AE_FEATURE_SET_FIELDS_ALL( _visitor_ )\
		AE_FEATURE_SET_FIELDS( _visitor_, _visitor_, _visitor_, _visitor_, _visitor_ )

	#define AE_FEATURE_SET_FIELDS( _visitorF_, _visitor1_, _visitor2_, _visitor4_, _visitor8_ )\
	/*---- render states ----*/\
		_visitorF_( EFeature,			alphaToOne,								: 2 )\
		_visitorF_( EFeature,			depthBiasClamp,							: 2 )\
		_visitorF_( EFeature,			depthBounds,							: 2 )\
		_visitorF_( EFeature,			depthClamp,								: 2 )\
		_visitorF_( EFeature,			dualSrcBlend,							: 2 )\
		_visitorF_( EFeature,			fillModeNonSolid,						: 2 )\
		_visitorF_( EFeature,			independentBlend,						: 2 )\
		_visitorF_( EFeature,			logicOp,								: 2 )\
		_visitorF_( EFeature,			sampleRateShading,						: 2 )\
		/*_visitorF_( EFeature,			depthClipControl,						: 2 )	/* VK_EXT_depth_clip_control - allow depth range in NDC [-1..1] instead of [0..1]	*/\
		_visitorF_( EFeature,			constantAlphaColorBlendFactors,			: 2 )\
		_visitorF_( EFeature,			pointPolygons,							: 2 )\
		_visitorF_( EFeature,			triangleFans,							: 2 )\
		_visitorF_( EFeature,			largePoints,							: 2 )\
		_visitorF_( EFeature,			wideLines,								: 2 )\
		_visitorF_( EFeature,			conservativeRasterization,				: 2 )	/* VK_EXT_conservative_rasterization												*/\
		\
		\
	/*---- shader variable types and functions ----*/\
		/* subgroup */\
		_visitor8_( SubgroupOperationBits,subgroupOperations,						)	/* GL_KHR_shader_subgroup															*/\
		_visitor1_( ESubgroupTypes,		subgroupTypes,								)	/* GL_EXT_shader_subgroup_extended_types_int8, *_int16, *_int64, *_float16			*/\
		_visitor2_( EShaderStages,		subgroupStages,								)\
		_visitor2_( EShaderStages,		subgroupQuadStages,							)\
		_visitor2_( EShaderStages,		requiredSubgroupSizeStages,					)\
		_visitor1_( POTValue,			minSubgroupSize,							)\
		_visitor1_( POTValue,			maxSubgroupSize,							)\
		_visitorF_( EFeature,			subgroup,								: 2 )\
		_visitorF_( EFeature,			subgroupBroadcastDynamicId,				: 2 )	/* GL_ARB_shader_ballot																*/\
		_visitorF_( EFeature,			subgroupSizeControl,					: 2 )	/* VK_EXT_subgroup_size_control														*/\
		_visitorF_( EFeature,			shaderSubgroupUniformControlFlow,		: 2 )	/* GL_EXT_subgroupuniform_qualifier, GL_EXT_subgroup_uniform_control_flow			*/\
		_visitorF_( EFeature,			shaderMaximalReconvergence,				: 2 )	/* GL_EXT_maximal_reconvergence														*/\
		_visitorF_( EFeature,			shaderQuadControl,						: 2 )	/* GL_EXT_shader_quad_control														*/\
		/* types */\
		_visitorF_( EFeature,			shaderInt8,								: 2 )	/* GL_EXT_shader_8bit_storage														*/\
		_visitorF_( EFeature,			shaderInt16,							: 2 )	/* GL_EXT_shader_16bit_storage														*/\
		_visitorF_( EFeature,			shaderInt64,							: 2 )	/* GL_ARB_gpu_shader_int64															*/\
		_visitorF_( EFeature,			shaderFloat16,							: 2 )\
		_visitorF_( EFeature,			shaderFloat64,							: 2 )	/* GL_ARB_gpu_shader_fp64															*/\
		_visitorF_( EFeature,			shaderBFloat16Type,						: 2 )	/* -\																				*/\
		_visitorF_( EFeature,			shaderBFloat16DotProduct,				: 2 )	/* -|-- GL_EXT_bfloat16																*/\
		_visitorF_( EFeature,			shaderBFloat16CooperativeMatrix,		: 2 )	/* -/																				*/\
		_visitorF_( EFeature,			shaderFloat8,							: 2 )	/* -\__ GL_EXT_float_e5m2, GL_EXT_float_e4m3										*/\
		_visitorF_( EFeature,			shaderFloat8CooperativeMatrix,			: 2 )	/* -/																				*/\
		/* uniform/storage buffer */\
		_visitorF_( EFeature,			storageBuffer16BitAccess,				: 2 )\
		_visitorF_( EFeature,			uniformAndStorageBuffer16BitAccess,		: 2 )\
		_visitorF_( EFeature,			storageInputOutput16,					: 2 )\
		_visitorF_( EFeature,			storageBuffer8BitAccess,				: 2 )\
		_visitorF_( EFeature,			uniformAndStorageBuffer8BitAccess,		: 2 )\
		_visitorF_( EFeature,			uniformBufferStandardLayout,			: 2 )\
		_visitorF_( EFeature,			scalarBlockLayout,						: 2 )	/* GL_EXT_scalar_block_layout														*/\
		_visitorF_( EFeature,			bufferDeviceAddress,					: 2 )	/* GL_EXT_buffer_reference, GL_EXT_buffer_reference2, GL_EXT_buffer_reference_uvec2 */\
		/* push constant */\
		_visitorF_( EFeature,			storagePushConstant8,					: 2 )\
		_visitorF_( EFeature,			storagePushConstant16,					: 2 )\
		/* atomic */\
		_visitorF_( EFeature,			fragmentStoresAndAtomics,				: 2 )\
		_visitorF_( EFeature,			vertexPipelineStoresAndAtomics,			: 2 )\
		_visitorF_( EFeature,			shaderImageInt64Atomics,				: 2 )	/* GL_EXT_shader_image_int64														*/\
		_visitorF_( EFeature,			shaderBufferInt64Atomics,				: 2 )	/*\																					*/\
		_visitorF_( EFeature,			shaderSharedInt64Atomics,				: 2 )	/*-'-- GL_ARB_gpu_shader_int64, GL_EXT_shader_atomic_int64							*/\
		_visitorF_( EFeature,			shaderBufferFloat32Atomics,				: 2 )	/*\																					*/\
		_visitorF_( EFeature,			shaderBufferFloat32AtomicAdd,			: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderBufferFloat64Atomics,				: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderBufferFloat64AtomicAdd,			: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderSharedFloat32Atomics,				: 2 )	/*-|-- GL_EXT_shader_atomic_float													*/\
		_visitorF_( EFeature,			shaderSharedFloat32AtomicAdd,			: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderSharedFloat64Atomics,				: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderSharedFloat64AtomicAdd,			: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderImageFloat32Atomics,				: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderImageFloat32AtomicAdd,			: 2 )	/*/																					*/\
		_visitorF_( EFeature,			shaderBufferFloat16Atomics,				: 2 )	/*\																					*/\
		_visitorF_( EFeature,			shaderBufferFloat16AtomicAdd,			: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderBufferFloat16AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderBufferFloat32AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderBufferFloat64AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderSharedFloat16Atomics,				: 2 )	/*-|--GL_EXT_shader_atomic_float2													*/\
		_visitorF_( EFeature,			shaderSharedFloat16AtomicAdd,			: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderSharedFloat16AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderSharedFloat32AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderSharedFloat64AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			shaderImageFloat32AtomicMinMax,			: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			sparseImageFloat32AtomicMinMax,			: 2 )	/*/																					*/\
		_visitorF_( EFeature,			shaderAtomicPackedFp16,					: 2 )	/* VK_NV_shader_atomic_float16_vector, GL_NV_shader_atomic_fp16_vector				*/\
		/* output */\
		_visitorF_( EFeature,			shaderOutputViewportIndex,				: 2 )	/*\																					*/\
		_visitorF_( EFeature,			shaderOutputLayer,						: 2 )	/*-'--GL_ARB_shader_viewport_layer_array											*/\
		/* clock */\
		_visitorF_( EFeature,			shaderSubgroupClock,					: 2 )	/* GL_ARB_shader_clock																*/\
		_visitorF_( EFeature,			shaderDeviceClock,						: 2 )	/* GL_EXT_shader_realtime_clock														*/\
		/* cooperative matrix/vector */\
		_visitorF_( EFeature,			cooperativeMatrix,						: 2 )	/*\ 																				*/\
		_visitor2_( EShaderStages,		cooperativeMatrixStages,					)	/*-|-- GL_KHR_cooperative_matrix													*/\
		_visitor2_( CoopMatrixSet_t,	cooperativeMatrixConfig,					)	/*/		- can be empty if default config is not supported							*/\
		_visitorF_( EFeature,			cooperativeVector,						: 2 )	/*\																					*/\
		_visitorF_( EFeature,			cooperativeVectorTraining,				: 2 )	/*-|-- GLSL_NV_cooperative_vector													*/\
		_visitor1_( CoopVecSet_t,		cooperativeVectorConfig,					)	/*/		- can be empty if default config is not supported							*/\
		_visitorF_( EFeature,			shaderIntegerDotProduct,				: 2 )	/* \__ GL_EXT_integer_dot_product													*/\
		_visitor4_( EIntegerDotProductFeats, integerDotProductFeatures,				)	/* /																				*/\
		\
		\
	/*---- shader features/limits ----*/\
		_visitorF_( EFeature,			shaderClipDistance,						: 2 )\
		_visitorF_( EFeature,			shaderCullDistance,						: 2 )\
		_visitorF_( EFeature,			shaderResourceMinLod,					: 2 )	/* GL_ARB_sparse_texture_clamp														*/\
		_visitorF_( EFeature,			shaderDrawParameters,					: 2 )	/* BaseVertexID, BaseInstanceID, DrawIndexID										*/\
		_visitorF_( EFeature,			runtimeDescriptorArray,					: 2 )	/* SPIRV: RuntimeDescriptorArrayEXT													*/\
		_visitorF_( EFeature,			shaderSMBuiltinsNV,						: 2 )	/* GL_NV_shader_sm_builtins															*/\
		_visitorF_( EFeature,			shaderCoreBuiltinsARM,					: 2 )	/* GL_ARM_shader_core_builtins														*/\
		_visitorF_( EFeature,			shaderSampleRateInterpolationFunctions,	: 2 )\
		_visitorF_( EFeature,			shaderStencilExport,					: 2 )	/* VK_EXT_shader_stencil_export, GL_ARB_shader_stencil_export						*/\
		_visitorF_( EFeature,			shaderExpectAssume,						: 2 )	/* VK_KHR_shader_expect_assume, GL_EXT_expect_assume								*/\
		/* array dynamic indexing */\
		_visitorF_( EFeature,			shaderSampledImageArrayDynamicIndexing,			: 2 )\
		_visitorF_( EFeature,			shaderStorageBufferArrayDynamicIndexing,		: 2 )\
		_visitorF_( EFeature,			shaderStorageImageArrayDynamicIndexing,			: 2 )\
		_visitorF_( EFeature,			shaderUniformBufferArrayDynamicIndexing,		: 2 )\
		_visitorF_( EFeature,			shaderInputAttachmentArrayDynamicIndexing,		: 2 )\
		_visitorF_( EFeature,			shaderUniformTexelBufferArrayDynamicIndexing,	: 2 )\
		_visitorF_( EFeature,			shaderStorageTexelBufferArrayDynamicIndexing,	: 2 )\
		/* non uniform indexing */\
		_visitorF_( EFeature,			shaderUniformBufferArrayNonUniformIndexing,			: 2 )	/*\																		*/\
		_visitorF_( EFeature,			shaderSampledImageArrayNonUniformIndexing,			: 2 )	/*-|																	*/\
		_visitorF_( EFeature,			shaderStorageBufferArrayNonUniformIndexing,			: 2 )	/*-|																	*/\
		_visitorF_( EFeature,			shaderStorageImageArrayNonUniformIndexing,			: 2 )	/*-|-- GL_EXT_nonuniform_qualifier										*/\
		_visitorF_( EFeature,			shaderInputAttachmentArrayNonUniformIndexing,		: 2 )	/*-|																	*/\
		_visitorF_( EFeature,			shaderUniformTexelBufferArrayNonUniformIndexing,	: 2 )	/*-|																	*/\
		_visitorF_( EFeature,			shaderStorageTexelBufferArrayNonUniformIndexing,	: 2 )	/*-/																	*/\
		_visitorF_( EFeature,			quadDivergentImplicitLod,							: 2 )	/* derivative calculation for non-uniform image 						*/\
		/* storage image format */\
		_visitorF_( EFeature,			shaderStorageImageMultisample,						: 2 )\
		_visitorF_( EFeature,			shaderStorageImageReadWithoutFormat,				: 2 )\
		_visitorF_( EFeature,			shaderStorageImageWriteWithoutFormat,				: 2 )\
		/* memory model */\
		_visitorF_( EFeature,			vulkanMemoryModel,									: 2 )	/*\																		*/\
		_visitorF_( EFeature,			vulkanMemoryModelDeviceScope,						: 2 )	/*-|--GL_KHR_memory_scope_semantics										*/\
		_visitorF_( EFeature,			vulkanMemoryModelAvailabilityVisibilityChains,		: 2 )	/*/																		*/\
		/* */\
		_visitorF_( EFeature,			shaderDemoteToHelperInvocation,						: 2 )	/* GL_EXT_demote_to_helper_invocation									*/\
		_visitorF_( EFeature,			shaderTerminateInvocation,							: 2 )\
		_visitorF_( EFeature,			shaderZeroInitializeWorkgroupMemory,				: 2 )\
		/*_visitorF_( EFeature,			shaderIntegerDotProduct,							: 2 )*/\
		/* fragment shader interlock */\
		_visitorF_( EFeature,			fragmentShaderSampleInterlock,						: 2 )	/*\																		*/\
		_visitorF_( EFeature,			fragmentShaderPixelInterlock,						: 2 )	/*-|-- GL_ARB_fragment_shader_interlock									*/\
		_visitorF_( EFeature,			fragmentShaderShadingRateInterlock,					: 2 )	/*/																		*/\
		/* fragment shader barycentric */\
		_visitorF_( EFeature,			fragmentShaderBarycentric,							: 2)	/*	VK_KHR_fragment_shader_barycentric, GL_EXT_fragment_shader_barycentric */\
		/* fragment shading rate */\
		_visitorF_( EFeature,			pipelineFragmentShadingRate,						: 2 )	/*\																		*/\
		_visitorF_( EFeature,			primitiveFragmentShadingRate,						: 2 )	/*-|																	*/\
		_visitorF_( EFeature,			attachmentFragmentShadingRate,						: 2 )	/*-|																	*/\
		_visitorF_( EFeature,			primitiveFragmentShadingRateWithMultipleViewports,	: 2 )	/*-|																	*/\
		_visitorF_( EFeature,			layeredShadingRateAttachments,						: 2 )	/*-|--GL_EXT_fragment_shading_rate										*/\
		_visitorF_( EFeature,			fragmentShadingRateWithShaderDepthStencilWrites,	: 2 )	/*-|																	*/\
		_visitorF_( EFeature,			fragmentShadingRateWithSampleMask,					: 2 )	/*-|																	*/\
		_visitorF_( EFeature,			fragmentShadingRateWithShaderSampleMask,			: 2 )	/*-|																	*/\
		_visitorF_( EFeature,			fragmentShadingRateWithFragmentShaderInterlock,		: 2 )	/*-|																	*/\
		_visitorF_( EFeature,			fragmentShadingRateWithCustomSampleLocations,		: 2 )	/*-|																	*/\
		_visitor4_( VRSTexelSize,		fragmentShadingRateTexelSize,							)	/*-|																	*/\
		_visitor1_( ShadingRateSet_t,	fragmentShadingRates,									)	/*-/																	*/\
		/* fragment density map */\
		_visitorF_( EFeature,			fragmentDensityMap,						: 2 )	/*\																					*/\
		_visitorF_( EFeature,			fragmentDensityMapDynamic,				: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			fragmentDensityMapNonSubsampledImages,	: 2 )	/*-|--GL_EXT_fragment_invocation_density											*/\
		_visitorF_( EFeature,			fragmentDensityInvocations,				: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			subsampledLoads,						: 2 )	/*-|																				*/\
		_visitor1_( POTValue,			maxSubsampledArrayLayers,					)	/*-|																				*/\
		_visitor1_( ubyte,				perPipeline_maxSubsampledSamplers,			)	/*/	 	maxDescriptorSetSubsampledSamplers											*/\
		/* acceleration structure */\
		_visitorF_( EFeature,			accelerationStructureIndirectBuild,		: 2 )\
		_visitorF_( EFeature,			clusterAccelerationStructure,			: 2 )	/* VK_NV_cluster_acceleration_structure, GL_NV_cluster_acceleration_structure		*/\
		_visitorF_( EFeature,			partitionedAccelerationStructure,		: 2 )	/* VK_NV_partitioned_acceleration_structure											*/\
		/* inline ray tracing */\
		_visitorF_( EFeature,			rayQuery,								: 2 )	/* GL_EXT_ray_query																	*/\
		_visitor2_( EShaderStages,		rayQueryStages,								)\
		/* ray tracing */\
		_visitorF_( EFeature,			rayTracingPipeline,						: 2 )	/* GL_EXT_ray_tracing, VK_KHR_ray_tracing_pipeline									*/\
		/*_visitorF_( EFeature,			rayTracingPipelineTraceRaysIndirect,	: 2 )*/\
		_visitorF_( EFeature,			rayTraversalPrimitiveCulling,			: 2 )	/* GL_EXT_ray_flags_primitive_culling												*/\
		_visitor2_( ushort,				maxRayRecursionDepth,						)\
		/*_visitor2_( KiBytes,			maxRayHitAttributeSize,						)*/\
		/* opacity micromap */\
		_visitorF_( EFeature,			opacityMicromap,						: 2 )	/*-\																				*/\
		_visitor2_( ushort,				maxOpacity2StateSubdivisionLevel,			)	/*-|-- VK_EXT_opacity_micromap														*/\
		_visitor2_( ushort,				maxOpacity4StateSubdivisionLevel,			)	/*-/																				*/\
		_visitorF_( EFeature,			displacementMicromap,					: 2 )	/*-\___	VK_NV_displacement_micromap													*/\
		_visitor2_( ushort,				maxDisplacementMicromapSubdivisionLevel,	)	/*-/																				*/\
		/* shader version */\
		_visitor2_( ShaderVersion,		maxShaderVersion,							)\
		/* draw indirect */\
		_visitorF_( EFeature,			drawIndirectFirstInstance,				: 2 )	/* Vulkan feature: drawIndirectFirstInstance										*/\
		_visitorF_( EFeature,			drawIndirectCount,						: 2 )	/* VK_KHR_draw_indirect_count														*/\
		_visitor4_( uint,				maxDrawIndirectCount,						)\
		/* multi view */\
		_visitorF_( EFeature,			multiview,								: 2 )	/*\																					*/\
		_visitorF_( EFeature,			multiviewGeometryShader,				: 2 )	/*-|																				*/\
		_visitorF_( EFeature,			multiviewTessellationShader,			: 2 )	/*-|--GL_EXT_multiview																*/\
		_visitor1_( ubyte,				maxMultiviewViewCount,						)	/*/																					*/\
		/* multi viewport */\
		_visitorF_( EFeature,			multiViewport,							: 2 )\
		_visitor1_( ubyte,				maxViewports,								)\
		/* sample locations */\
		_visitorF_( EFeature,			sampleLocations,						: 2 )	/* VK_EXT_sample_locations															*/\
		_visitorF_( EFeature,			variableSampleLocations,				: 2 )\
		/*_visitor4_( SampleCountBits,	sampleLocationSampleCounts,					)*/\
		/* tessellation */\
		_visitorF_( EFeature,			tessellationIsolines,					: 2 )\
		_visitorF_( EFeature,			tessellationPointMode,					: 2 )\
		/* shader limits */\
		_visitor4_( uint,				maxTexelBufferElements,						)\
		_visitor4_( Bytes32u,			maxUniformBufferSize,						)	/* maxUniformBufferRange															*/\
		_visitor4_( Bytes32u,			maxStorageBufferSize,						)	/* maxStorageBufferRange															*/\
		_visitor1_( ubyte,				perPipeline_maxUniformBuffersDynamic,		)	/* maxDescriptorSetUniformBuffersDynamic, maxDescriptorSetTotalUniformBuffersDynamic*/\
		_visitor1_( ubyte,				perPipeline_maxStorageBuffersDynamic,		)	/* maxDescriptorSetStorageBuffersDynamic, maxDescriptorSetTotalStorageBuffersDynamic*/\
		_visitor1_( ubyte,				perPipeline_maxTotalBuffersDynamic,			)	/* maxDescriptorSetTotalBuffersDynamic												*/\
		_visitor4_( PerDescriptorSet,	perPipeline,								)	/* Metal: no limits																	*/\
		_visitor4_( PerShaderStage,		perStage,									)\
		_visitor1_( ubyte,				maxDescriptorSets,							)	/* maxBoundDescriptorSets															*/\
		_visitor1_( ubyte,				maxTexelOffset,								)	/* maxTexelOffset, minTexelOffset  - [-N-1...+N] for textureOffset()				*/\
		_visitor1_( ubyte,				maxTexelGatherOffset,						)	/* maxTexelGatherOffset, minTexelGatherOffset										*/\
		_visitor1_( ubyte,				maxFragmentOutputAttachments,				)	/* maxFragmentOutputAttachments, maxColorAttachments								*/\
		_visitor1_( ubyte,				maxFragmentDualSrcAttachments,				)\
		_visitor4_( uint,				maxFragmentCombinedOutputResources,			)	/* = storage buffers + storage images + color attachments							*/\
		_visitor1_( POTBytes,			maxPushConstantsSize,						)\
		_visitor1_( ubyte,				maxVertAmplification,						)	/* only for Metal																	*/\
		_visitor2_( KiBytes,			maxTotalThreadgroupSize,					)	/* only for Metal																	*/\
		_visitor2_( KiBytes,			maxTotalTileMemory,							)	/* only for Metal																	*/\
		/* compute shader */\
		_visitor2_( KiBytes,			maxComputeSharedMemorySize,					)\
		_visitor1_( POTValue,			maxComputeWorkGroupInvocations,				)\
		_visitor1_( POTValue,			maxComputeWorkGroupSizeX,					)	/* local_size_x, maxComputeWorkGroupCount											*/\
		_visitor1_( POTValue,			maxComputeWorkGroupSizeY,					)	/* local_size_y																		*/\
		_visitor1_( POTValue,			maxComputeWorkGroupSizeZ,					)	/* local_size_z																		*/\
		/* mesh shader */\
		_visitorF_( EFeature,			taskShader,								: 2 )	/*\																					*/\
		_visitorF_( EFeature,			meshShader,								: 2 )	/*-|--GL_EXT_mesh_shader															*/\
		_visitor1_( POTValue,			maxTaskWorkGroupSize,						)	/*-|	- local_size_x/y/z															*/\
		_visitor1_( POTValue,			maxMeshWorkGroupSize,						)	/*-|	- local_size_x/y/z															*/\
		_visitor1_( POTValue,			maxMeshOutputVertices,						)	/*-|																				*/\
		_visitor1_( POTValue,			maxMeshOutputPrimitives,					)	/*-|																				*/\
		_visitor1_( POTValue,			maxMeshOutputPerVertexGranularity,			)	/*-|	- meshOutputPerVertexGranularity											*/\
		_visitor1_( POTValue,			maxMeshOutputPerPrimitiveGranularity,		)	/*-|	- meshOutputPerPrimitiveGranularity											*/\
		_visitor2_( KiBytes,			maxTaskPayloadSize,							)	/*-|																				*/\
		_visitor2_( KiBytes,			maxTaskSharedMemorySize,					)	/*-|																				*/\
		_visitor2_( KiBytes,			maxMeshSharedMemorySize,					)	/*-|																				*/\
		_visitor2_( KiBytes,			maxMeshOutputMemorySize,					)	/*-|																				*/\
		_visitor2_( KiBytes,			maxTaskPayloadAndSharedMemorySize,			)	/*-|																				*/\
		_visitor2_( KiBytes,			maxMeshPayloadAndSharedMemorySize,			)	/*-|																				*/\
		_visitor2_( KiBytes,			maxMeshPayloadAndOutputMemorySize,			)	/*-|																				*/\
		_visitor1_( ubyte,				maxMeshMultiviewViewCount,					)	/*-|																				*/\
		_visitor1_( POTValue,			maxPreferredTaskWorkGroupInvocations,		)	/*-|																				*/\
		_visitor1_( POTValue,			maxPreferredMeshWorkGroupInvocations,		)	/*/																					*/\
		/* raster order group */\
		_visitor2_( ushort,				maxRasterOrderGroups,						)	/* only for Metal																	*/\
		/* shaders */\
		_visitorF_( EFeature,			geometryShader,							: 2 )\
		_visitorF_( EFeature,			tessellationShader,						: 2 )\
		_visitorF_( EFeature,			computeShader,							: 2 )\
		_visitorF_( EFeature,			tileShader,								: 2 )	/* GL_HUAWEI_subpass_shading														*/\
		/* vertex buffer */\
		_visitorF_( EFeature,			vertexDivisor,							: 2 )	/* \ 																				*/\
		_visitor4_( uint,				maxVertexAttribDivisor,						)	/*-'-- VK_EXT_vertex_attribute_divisor												*/\
		_visitor1_( ubyte,				maxVertexAttributes,						)	/* maxVertexInputAttributes															*/\
		_visitor1_( ubyte,				maxVertexBuffers,							)	/* maxVertexInputBindings															*/\
		/* rasterization order attachment access */\
		_visitorF_( EFeature,			rasterizationOrderColorAttachmentAccess,	: 2 )	/*\ 																			*/\
		_visitorF_( EFeature,			rasterizationOrderDepthAttachmentAccess,	: 2 )	/*-|-- VK_EXT_rasterization_order_attachment_access								*/\
		_visitorF_( EFeature,			rasterizationOrderStencilAttachmentAccess,	: 2 )	/*/ 																			*/\
		\
		\
	/*---- buffer ----*/\
		_visitor8_( VertexFormatSet_t,	vertexFormats,								)	/* VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT												*/\
		_visitor8_( PixelFormatSet_t,	uniformTexBufferFormats,					)	/* VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT										*/\
		_visitor8_( PixelFormatSet_t,	storageTexBufferFormats,					)	/* VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT										*/\
		_visitor8_( PixelFormatSet_t,	storageTexBufferAtomicFormats,				)	/* VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT								*/\
		_visitor8_( VertexFormatSet_t,	accelStructVertexFormats,					)	/* VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR, TODO: use another enum to save space	*/\
		\
		\
	/*---- image -----*/\
		_visitorF_( EFeature,			imageCubeArray,							: 2 )\
		_visitorF_( EFeature,			textureCompressionASTC_LDR,				: 2 )\
		_visitorF_( EFeature,			textureCompressionASTC_HDR,				: 2 )\
		_visitorF_( EFeature,			textureCompressionBC,					: 2 )\
		_visitorF_( EFeature,			textureCompressionETC2,					: 2 )\
		/*_visitorF_( EFeature,			imageViewMinLod,						: 2 )	/ * VK_EXT_image_view_min_lod, minLod												*/\
		_visitorF_( EFeature,			multisampleArrayImage,					: 2 )\
		_visitorF_( EFeature,			imageViewFormatList,					: 2 )	/* VK_KHR_image_format_list															*/\
		_visitorF_( EFeature,			imageViewExtendedUsage,					: 2 )	/* VK_KHR_maintenance2																*/\
		_visitor2_( SurfaceFormatSet_t,	surfaceFormats,								)\
		_visitor1_( POTValue,			maxImageDimension1D,						)\
		_visitor1_( POTValue,			maxImageDimension2D,						)\
		_visitor1_( POTValue,			maxImageDimension3D,						)\
		_visitor1_( POTValue,			maxImageDimensionCube,						)\
		_visitor1_( POTValue,			maxImageArrayLayers,						)\
		_visitor8_( PixelFormatSet_t,	storageImageAtomicFormats,					)	/* VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT										*/\
		_visitor8_( PixelFormatSet_t,	storageImageFormats,						)	/* VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT		TODO: R or W							*/\
		_visitor8_( PixelFormatSet_t,	attachmentBlendFormats,						)	/* VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT										*/\
		_visitor8_( PixelFormatSet_t,	attachmentFormats,							)	/* VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |	VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT */\
		_visitor8_( PixelFormatSet_t,	linearSampledFormats,						)	/* VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT								*/\
		_visitor8_( PixelFormatSet_t,	minmaxFilterFormats,						)	/* VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT								*/\
		/*_visitor8_( PixelFormatSet_t,	sparseImageFormats,							)*/\
		/*_visitor8_( PixelFormatSet_t,	multisampleImageFormats,					)*/\
		_visitor8_( PixelFormatSet_t,	hwCompressedAttachmentFormats,				)	/* formats which is compatible with lossless hardware compression					*/\
		_visitor8_( PixelFormatSet_t,	lossyCompressedAttachmentFormats,			)	/* formats which is compatible with lossy hardware compression						*/\
		\
		\
	/*---- sampler ----*/\
		_visitorF_( EFeature,			samplerAnisotropy,						: 2 )\
		_visitorF_( EFeature,			samplerMirrorClampToEdge,				: 2 )	/* VK_KHR_sampler_mirror_clamp_to_edge												*/\
		_visitorF_( EFeature,			samplerFilterMinmax,					: 2 )\
		_visitorF_( EFeature,			filterMinmaxImageComponentMapping,		: 2 )\
		_visitorF_( EFeature,			samplerMipLodBias,						: 2 )\
		_visitorF_( EFeature,			samplerYcbcrConversion,					: 2 )	/* VK_KHR_sampler_ycbcr_conversion													*/\
		_visitorF_( EFeature,			ycbcr2Plane444,							: 2 )	/* VK_EXT_ycbcr_2plane_444_formats													*/\
		_visitorF_( EFeature,			nonSeamlessCubeMap,						: 2 )	/* VK_EXT_non_seamless_cube_map														*/\
		_visitor4_( float,				maxSamplerAnisotropy,						)\
		_visitor4_( float,				maxSamplerLodBias,							)\
		/*_visitor4_( SampleCountBits,	sampledImageColorSampleCounts,				)*/\
		/*_visitor4_( SampleCountBits,	sampledImageDepthSampleCounts,				)*/\
		/*_visitor4_( SampleCountBits,	sampledImageIntegerSampleCounts,			)*/\
		/*_visitor4_( SampleCountBits,	sampledImageStencilSampleCounts,			)*/\
		/*_visitor4_( SampleCountBits,	storageImageSampleCounts,					)*/\
		\
		\
	/*---- framebuffer ----*/\
		_visitor4_( SampleCountBits,	framebufferColorSampleCounts,				)\
		_visitor4_( SampleCountBits,	framebufferDepthSampleCounts,				)\
		/*_visitor4_( SampleCountBits,	framebufferIntegerColorSampleCounts,		)*/\
		_visitor1_( POTValue,			maxFramebufferLayers,						)\
		\
		\
	/*---- render pass ----*/\
		_visitorF_( EFeature,			variableMultisampleRate,				: 2 )\
		_visitorF_( EFeature,			separateDepthStencilRW,					: 2 )	/* VK_KHR_maintenance2 or Vulkan 1.1												*/\
		\
		\
	/*---- indirect command buffer ----*/\
		_visitorF_( EFeature,			deviceGeneratedCommands,				: 2 )			/*-\ 																		*/\
		_visitorF_( EFeature,			deviceGeneratedCommandsMultiDrawIndirectCount, :2)		/*-|																		*/\
		_visitor2_( EShaderStages,		supportedIndirectCommandsShaderStages,		)			/*-|-- VK_EXT_device_generated_commands										*/\
		_visitor2_( EShaderStages,		supportedIndirectCommandsShaderStagesPipelineBinding,)	/*-|																		*/\
		_visitor2_( ushort,				maxIndirectPipelineCount,					)			/*-/																		*/\
		\
		\
	/*---- android ----*/\
		_visitorF_( EFeature,			externalFormatAndroid,					: 2 )	/* VK_ANDROID_external_memory_android_hardware_buffer								*/\
		\
		\
	/*---- metal ----*/\
		_visitor1_( ubyte,				metalArgBufferTier,							)\
		\
		\
	/* HW info */\
		_visitor1_( Queues,				queues,										)\
		_visitor2_( VendorIDs_t,		vendorIds,									)\
		_visitor4_( GraphicsDevices_t,	devicesIds,									)\


		// TODO:
		//	min total memory size
		//	min/max resolution ?
		//	supported queries - for dynamic resolution rendering and other techniques
		//	combinedImageSamplerDescriptorCount


	// variables
		#define AE_FS_ADDFIELD( _type_, _name_, _bits_ )		_type_	_name_	_bits_;
		#define AE_FS_SKIP( _type_, _name_, _bits_ )

		AE_FEATURE_SET_FIELDS( AE_FS_ADDFIELD,	AE_FS_SKIP,		AE_FS_SKIP,		AE_FS_SKIP,		AE_FS_SKIP		)
		AE_FEATURE_SET_FIELDS( AE_FS_SKIP,		AE_FS_ADDFIELD,	AE_FS_SKIP,		AE_FS_SKIP,		AE_FS_SKIP		)
		AE_FEATURE_SET_FIELDS( AE_FS_SKIP,		AE_FS_SKIP,		AE_FS_ADDFIELD,	AE_FS_SKIP,		AE_FS_SKIP		)
		AE_FEATURE_SET_FIELDS( AE_FS_SKIP,		AE_FS_SKIP,		AE_FS_SKIP,		AE_FS_ADDFIELD,	AE_FS_SKIP		)
		AE_FEATURE_SET_FIELDS( AE_FS_SKIP,		AE_FS_SKIP,		AE_FS_SKIP,		AE_FS_SKIP,		AE_FS_ADDFIELD	)

		#undef AE_FS_ADDFIELD
		#undef AE_FS_SKIP


	// methods
		FeatureSet ()																__NE___	{}

			void  Init (EFeature value)												__NE___;
			void  SetDefault ()														__NE___;

			void  AddDevice (uint vendorId, uint deviceId, StringView name)			__NE___;

		ND_ bool  IsValid ()														C_NE___;
			void  Validate ()														__NE___;

		ND_ bool  IsSupported (const RenderState &rs)								C_NE___;
		ND_ bool  IsSupported (const BufferDesc &desc)								C_NE___;
		ND_ bool  IsSupported (const BufferDesc &desc, const BufferViewDesc &view)	C_NE___;
		ND_ bool  IsSupported (const ImageDesc &desc)								C_NE___;
		ND_ bool  IsSupported (const ImageDesc &desc, const ImageViewDesc &view)	C_NE___;
		ND_ bool  IsSupported (EResourceState state)								C_NE___;

			void  MergeMin (const FeatureSet &rhs)									__NE___;
			void  MergeMax (const FeatureSet &rhs)									__NE___;

		ND_ bool  IsCompatible (const FeatureSet &rhs)								C_NE___;
		ND_ bool  DbgIsCompatible (const FeatureSet &rhs)							C_NE___;

		ND_ bool  operator == (const FeatureSet &rhs)								C_NE___;
		ND_ bool  operator != (const FeatureSet &rhs)								C_NE___	{ return not (rhs == *this);}
		ND_ bool  operator >= (const FeatureSet &rhs)								C_NE___;
		ND_ bool  operator <= (const FeatureSet &rhs)								C_NE___	{ return rhs >= *this; }

		ND_ HashVal  CalcHash ()													C_NE___;

		ND_ static HashVal64  GetHashOfFieldNames ()								__NE___;
		ND_ static HashVal64  GetHashOfDependencies ()								__NE___;
		ND_ static HashVal64  GetHashOfFS ()										__NE___	{ return GetHashOfFieldNames() + GetHashOfDependencies(); }
		ND_ static HashVal64  GetHashOfFS_Precalculated ()							__NE___;
		NdCx__ static uint    GetFeatureCount ()									__NE___;


		ND_ EFeature  accelerationStructure ()										C_NE___;
		ND_ EFeature  rayShaderBindingTable ()										C_NE___	{ return rayTracingPipeline; }

		ND_ EShaderStages  SupportedShaderStages ()									C_NE___;

		ND_ EGPUVendor			GetGPUVendor ()										C_NE___	{ return vendorIds.include.First(); }
		ND_ EGraphicsDeviceID	GetEGraphicsDeviceID ()								C_NE___	{ return devicesIds.include.First(); }


	private:
		template <bool Mutable>
		bool  _Validate ()															__NE___;
	};
	StaticAssert( sizeof(FeatureSet) == 632 );


	__CxIn uint  FeatureSet::GetFeatureCount () __NE___
	{
		enum class FeatureCount
		{
			#define AE_FS_COUNT( _type_, _name_, _bits_ )	_name_,
			AE_FEATURE_SET_FIELDS_ALL( AE_FS_COUNT )
			#undef AE_FS_COUNT

			_Count
		};
		return uint(FeatureCount::_Count);
	}

} // AE::Graphics


namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::FeatureSet >		: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::FeatureSet >	: CT_True {};

} // AE::Base
