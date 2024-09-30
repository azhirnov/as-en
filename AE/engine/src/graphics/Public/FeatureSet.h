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

#include "graphics/Public/ShaderEnums.h"
#include "graphics/Public/RenderState.h"
#include "graphics/Public/FeatureSetEnums.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/VertexEnums.h"
#include "graphics/Public/BufferDesc.h"
#include "graphics/Public/ImageDesc.h"
#include "graphics/Public/Queue.h"
#include "graphics/Public/PipelineDesc.h"

namespace AE::Graphics
{

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
		static constexpr uint	MaxMetalVersion	= 310;

		struct ShaderVersion
		{
			ushort	spirv	= 0;
			ushort	metal	= 0;

			ND_ HashVal  CalcHash ()		C_NE___	{ return HashOf(spirv) + HashOf(metal); }
		};

		struct VRSTexelSize
		{
			ushort	minX	: 3;	// power of 2, range: 1..256
			ushort	minY	: 3;	// power of 2, range: 1..256
			ushort	maxX	: 3;	// power of 2, range: 1..256
			ushort	maxY	: 3;	// power of 2, range: 1..256
			ushort	aspect	: 4;	// power of 2, range: 1..512

			VRSTexelSize ()					__NE___	{ std::memset( this, 0, sizeof(*this) ); }

			ND_ HashVal  CalcHash ()		C_NE___	{ return HashOf( BitCast<ushort>( *this )); }
			ND_ explicit operator bool ()	C_NE___	{ return (minX + minY + maxX + maxY) != 0; }

			ND_ uint2	Min ()				C_NE___	{ return uint2{ 1u << minX, 1u << minY }; }
			ND_ uint2	Max ()				C_NE___	{ return uint2{ 1u << maxX, 1u << maxY }; }
			ND_ uint	MaxAspect ()		C_NE___	{ return 1u << aspect; }
		};


	#define AE_FEATURE_SET_FIELDS( _visitor_ )\
	/*---- render states ----*/\
		_visitor_( EFeature,			alphaToOne,								: 2 )\
		_visitor_( EFeature,			depthBiasClamp,							: 2 )\
		_visitor_( EFeature,			depthBounds,							: 2 )\
		_visitor_( EFeature,			depthClamp,								: 2 )\
		_visitor_( EFeature,			dualSrcBlend,							: 2 )\
		_visitor_( EFeature,			fillModeNonSolid,						: 2 )\
		_visitor_( EFeature,			independentBlend,						: 2 )\
		_visitor_( EFeature,			logicOp,								: 2 )\
		_visitor_( EFeature,			sampleRateShading,						: 2 )\
		/*_visitor_( EFeature,			depthClipControl,						: 2 )	/* VK_EXT_depth_clip_control - allow depth range in NDC [-1..1] instead of [0..1]	*/\
		_visitor_( EFeature,			constantAlphaColorBlendFactors,			: 2 )\
		_visitor_( EFeature,			pointPolygons,							: 2 )\
		_visitor_( EFeature,			triangleFans,							: 2 )\
		\
		\
	/*---- shader variable types and functions ----*/\
		/* subgroup */\
		_visitor_( SubgroupOperationBits,subgroupOperations,						)	/* GL_KHR_shader_subgroup															*/\
		_visitor_( ESubgroupTypes,		subgroupTypes,								)	/* GL_EXT_shader_subgroup_extended_types_int8, *_int16, *_int64, *_float16			*/\
		_visitor_( EShaderStages,		subgroupStages,								)\
		_visitor_( EShaderStages,		subgroupQuadStages,							)\
		_visitor_( EShaderStages,		requiredSubgroupSizeStages,					)\
		_visitor_( ushort,				minSubgroupSize,							)\
		_visitor_( ushort,				maxSubgroupSize,							)\
		_visitor_( EFeature,			subgroup,								: 2 )\
		_visitor_( EFeature,			subgroupBroadcastDynamicId,				: 2 )	/* GL_ARB_shader_ballot																*/\
		_visitor_( EFeature,			subgroupSizeControl,					: 2 )	/* VK_EXT_subgroup_size_control														*/\
		_visitor_( EFeature,			shaderSubgroupUniformControlFlow,		: 2 )	/* GL_EXT_subgroupuniform_qualifier, GL_EXT_subgroup_uniform_control_flow			*/\
		_visitor_( EFeature,			shaderMaximalReconvergence,				: 2 )	/* GL_EXT_maximal_reconvergence														*/\
		_visitor_( EFeature,			shaderQuadControl,						: 2 )	/* GL_EXT_shader_quad_control														*/\
		_visitor_( EFeature,			clipSpaceWScalingNV,					: 2 )	/* VK_NV_clip_space_w_scaling														*/\
		/* types */\
		_visitor_( EFeature,			shaderInt8,								: 2 )	/* GL_EXT_shader_8bit_storage														*/\
		_visitor_( EFeature,			shaderInt16,							: 2 )	/* GL_EXT_shader_16bit_storage														*/\
		_visitor_( EFeature,			shaderInt64,							: 2 )	/* GL_ARB_gpu_shader_int64															*/\
		_visitor_( EFeature,			shaderFloat16,							: 2 )\
		_visitor_( EFeature,			shaderFloat64,							: 2 )	/* GL_ARB_gpu_shader_fp64															*/\
		/* uniform/storage buffer */\
		_visitor_( EFeature,			storageBuffer16BitAccess,				: 2 )\
		_visitor_( EFeature,			uniformAndStorageBuffer16BitAccess,		: 2 )\
		_visitor_( EFeature,			storageInputOutput16,					: 2 )\
		_visitor_( EFeature,			storageBuffer8BitAccess,				: 2 )\
		_visitor_( EFeature,			uniformAndStorageBuffer8BitAccess,		: 2 )\
		_visitor_( EFeature,			uniformBufferStandardLayout,			: 2 )\
		_visitor_( EFeature,			scalarBlockLayout,						: 2 )	/* GL_EXT_scalar_block_layout														*/\
		_visitor_( EFeature,			bufferDeviceAddress,					: 2 )	/* GL_EXT_buffer_reference, GL_EXT_buffer_reference2, GL_EXT_buffer_reference_uvec2 */\
		/* push constant */\
		_visitor_( EFeature,			storagePushConstant8,					: 2 )\
		_visitor_( EFeature,			storagePushConstant16,					: 2 )\
		/* atomic */\
		_visitor_( EFeature,			fragmentStoresAndAtomics,				: 2 )\
		_visitor_( EFeature,			vertexPipelineStoresAndAtomics,			: 2 )\
		_visitor_( EFeature,			shaderImageInt64Atomics,				: 2 )	/* GL_EXT_shader_image_int64														*/\
		_visitor_( EFeature,			shaderBufferInt64Atomics,				: 2 )	/*\																					*/\
		_visitor_( EFeature,			shaderSharedInt64Atomics,				: 2 )	/*-'-- GL_ARB_gpu_shader_int64, GL_EXT_shader_atomic_int64							*/\
		_visitor_( EFeature,			shaderBufferFloat32Atomics,				: 2 )	/*\																					*/\
		_visitor_( EFeature,			shaderBufferFloat32AtomicAdd,			: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderBufferFloat64Atomics,				: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderBufferFloat64AtomicAdd,			: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderSharedFloat32Atomics,				: 2 )	/*-|-- GL_EXT_shader_atomic_float													*/\
		_visitor_( EFeature,			shaderSharedFloat32AtomicAdd,			: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderSharedFloat64Atomics,				: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderSharedFloat64AtomicAdd,			: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderImageFloat32Atomics,				: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderImageFloat32AtomicAdd,			: 2 )	/*/																					*/\
		_visitor_( EFeature,			shaderBufferFloat16Atomics,				: 2 )	/*\																					*/\
		_visitor_( EFeature,			shaderBufferFloat16AtomicAdd,			: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderBufferFloat16AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderBufferFloat32AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderBufferFloat64AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderSharedFloat16Atomics,				: 2 )	/*-|--GL_EXT_shader_atomic_float2													*/\
		_visitor_( EFeature,			shaderSharedFloat16AtomicAdd,			: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderSharedFloat16AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderSharedFloat32AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderSharedFloat64AtomicMinMax,		: 2 )	/*-|																				*/\
		_visitor_( EFeature,			shaderImageFloat32AtomicMinMax,			: 2 )	/*-|																				*/\
		_visitor_( EFeature,			sparseImageFloat32AtomicMinMax,			: 2 )	/*/																					*/\
		/* output */\
		_visitor_( EFeature,			shaderOutputViewportIndex,				: 2 )	/*\																					*/\
		_visitor_( EFeature,			shaderOutputLayer,						: 2 )	/*-'--GL_ARB_shader_viewport_layer_array											*/\
		/* clock */\
		_visitor_( EFeature,			shaderSubgroupClock,					: 2 )	/* GL_ARB_shader_clock																*/\
		_visitor_( EFeature,			shaderDeviceClock,						: 2 )	/* GL_EXT_shader_realtime_clock														*/\
		/*  */\
		_visitor_( EFeature,			cooperativeMatrix,						: 2 )	/*\ 																				*/\
		_visitor_( EShaderStages,		cooperativeMatrixStages,					)	/*-'-- GL_KHR_cooperative_matrix													*/\
		\
		\
	/*---- shader features/limits ----*/\
		_visitor_( EFeature,			shaderClipDistance,						: 2 )\
		_visitor_( EFeature,			shaderCullDistance,						: 2 )\
		_visitor_( EFeature,			shaderResourceMinLod,					: 2 )	/* GL_ARB_sparse_texture_clamp														*/\
		_visitor_( EFeature,			shaderDrawParameters,					: 2 )	/* BaseVertexID, BaseInstanceID, DrawIndexID										*/\
		_visitor_( EFeature,			runtimeDescriptorArray,					: 2 )	/* SPIRV: RuntimeDescriptorArrayEXT													*/\
		_visitor_( EFeature,			shaderSMBuiltinsNV,						: 2 )	/* GL_NV_shader_sm_builtins															*/\
		_visitor_( EFeature,			shaderCoreBuiltinsARM,					: 2 )	/* GL_ARM_shader_core_builtins														*/\
		_visitor_( EFeature,			shaderSampleRateInterpolationFunctions,	: 2 )\
		_visitor_( EFeature,			shaderStencilExport,					: 2 )	/* VK_EXT_shader_stencil_export, GL_ARB_shader_stencil_export						*/\
		/* array dynamic indexing */\
		_visitor_( EFeature,			shaderSampledImageArrayDynamicIndexing,			: 2 )\
		_visitor_( EFeature,			shaderStorageBufferArrayDynamicIndexing,		: 2 )\
		_visitor_( EFeature,			shaderStorageImageArrayDynamicIndexing,			: 2 )\
		_visitor_( EFeature,			shaderUniformBufferArrayDynamicIndexing,		: 2 )\
		_visitor_( EFeature,			shaderInputAttachmentArrayDynamicIndexing,		: 2 )\
		_visitor_( EFeature,			shaderUniformTexelBufferArrayDynamicIndexing,	: 2 )\
		_visitor_( EFeature,			shaderStorageTexelBufferArrayDynamicIndexing,	: 2 )\
		/* non uniform indexing */\
		_visitor_( EFeature,			shaderUniformBufferArrayNonUniformIndexing,			: 2 )	/*\																		*/\
		_visitor_( EFeature,			shaderSampledImageArrayNonUniformIndexing,			: 2 )	/*-|																	*/\
		_visitor_( EFeature,			shaderStorageBufferArrayNonUniformIndexing,			: 2 )	/*-|																	*/\
		_visitor_( EFeature,			shaderStorageImageArrayNonUniformIndexing,			: 2 )	/*-|-- GL_EXT_nonuniform_qualifier										*/\
		_visitor_( EFeature,			shaderInputAttachmentArrayNonUniformIndexing,		: 2 )	/*-|																	*/\
		_visitor_( EFeature,			shaderUniformTexelBufferArrayNonUniformIndexing,	: 2 )	/*-|																	*/\
		_visitor_( EFeature,			shaderStorageTexelBufferArrayNonUniformIndexing,	: 2 )	/*-|																	*/\
		_visitor_( EFeature,			shaderUniformBufferArrayNonUniformIndexingNative,	: 2 )	/*-|\																	*/\
		_visitor_( EFeature,			shaderSampledImageArrayNonUniformIndexingNative,	: 2 )	/*-|-|																	*/\
		_visitor_( EFeature,			shaderStorageBufferArrayNonUniformIndexingNative,	: 2 )	/*-|-|-- without native support branching will be used					*/\
		_visitor_( EFeature,			shaderStorageImageArrayNonUniformIndexingNative,	: 2 )	/*-|-|																	*/\
		_visitor_( EFeature,			shaderInputAttachmentArrayNonUniformIndexingNative, : 2 )	/*/-/																	*/\
		_visitor_( EFeature,			quadDivergentImplicitLod,							: 2 )	/* derivative calculation for non-uniform image 						*/\
		/* storage image format */\
		_visitor_( EFeature,			shaderStorageImageMultisample,					: 2 )\
		_visitor_( EFeature,			shaderStorageImageReadWithoutFormat,			: 2 )\
		_visitor_( EFeature,			shaderStorageImageWriteWithoutFormat,			: 2 )\
		/* memory model */\
		_visitor_( EFeature,			vulkanMemoryModel,								: 2 )	/*\																			*/\
		_visitor_( EFeature,			vulkanMemoryModelDeviceScope,					: 2 )	/*-|--GL_KHR_memory_scope_semantics											*/\
		_visitor_( EFeature,			vulkanMemoryModelAvailabilityVisibilityChains,	: 2 )	/*/																			*/\
		/* */\
		_visitor_( EFeature,			shaderDemoteToHelperInvocation,			: 2 )	/* GL_EXT_demote_to_helper_invocation												*/\
		_visitor_( EFeature,			shaderTerminateInvocation,				: 2 )\
		_visitor_( EFeature,			shaderZeroInitializeWorkgroupMemory,	: 2 )\
		_visitor_( EFeature,			shaderIntegerDotProduct,				: 2 )\
		/* fragment shader interlock */\
		_visitor_( EFeature,			fragmentShaderSampleInterlock,			: 2 )	/*\																					*/\
		_visitor_( EFeature,			fragmentShaderPixelInterlock,			: 2 )	/*-|-- GL_ARB_fragment_shader_interlock												*/\
		_visitor_( EFeature,			fragmentShaderShadingRateInterlock,		: 2 )	/*/																					*/\
		/* fragment shader barycentric */\
		_visitor_( EFeature,			fragmentShaderBarycentric,				: 2)	/*	VK_KHR_fragment_shader_barycentric, GL_EXT_fragment_shader_barycentric			*/\
		/* fragment shading rate */\
		_visitor_( EFeature,			pipelineFragmentShadingRate,						: 2 )	/*\																		*/\
		_visitor_( EFeature,			primitiveFragmentShadingRate,						: 2 )	/*-|																	*/\
		_visitor_( EFeature,			attachmentFragmentShadingRate,						: 2 )	/*-|																	*/\
		_visitor_( EFeature,			primitiveFragmentShadingRateWithMultipleViewports,	: 2 )	/*-|																	*/\
		_visitor_( EFeature,			layeredShadingRateAttachments,						: 2 )	/*-|--GL_EXT_fragment_shading_rate										*/\
		_visitor_( EFeature,			fragmentShadingRateWithShaderDepthStencilWrites,	: 2 )	/*-|																	*/\
		_visitor_( EFeature,			fragmentShadingRateWithSampleMask,					: 2 )	/*-|																	*/\
		_visitor_( EFeature,			fragmentShadingRateWithShaderSampleMask,			: 2 )	/*-|																	*/\
		_visitor_( EFeature,			fragmentShadingRateWithFragmentShaderInterlock,		: 2 )	/*-|																	*/\
		_visitor_( EFeature,			fragmentShadingRateWithCustomSampleLocations,		: 2 )	/*-|																	*/\
		_visitor_( VRSTexelSize,		fragmentShadingRateTexelSize,							)	/*-|																	*/\
		_visitor_( ShadingRateSet_t,	fragmentShadingRates,									)	/*-/																	*/\
		/* acceleration structure */\
		_visitor_( EFeature,			accelerationStructureIndirectBuild,		: 2 )\
		/* inline ray tracing */\
		_visitor_( EFeature,			rayQuery,								: 2 )	/* GL_EXT_ray_query																	*/\
		_visitor_( EShaderStages,		rayQueryStages,								)\
		/* ray tracing */\
		_visitor_( EFeature,			rayTracingPipeline,						: 2 )	/* GL_EXT_ray_tracing, VK_KHR_ray_tracing_pipeline									*/\
		/*_visitor_( EFeature,			rayTracingPipelineTraceRaysIndirect,	: 2 )*/\
		_visitor_( EFeature,			rayTraversalPrimitiveCulling,			: 2 )	/* GL_EXT_ray_flags_primitive_culling												*/\
		_visitor_( uint,				maxRayRecursionDepth,						)\
		/*_visitor_( uint,				maxRayHitAttributeSize,						)*/\
		/* shader version */\
		_visitor_( ShaderVersion,		maxShaderVersion,							)\
		/* draw indirect */\
		_visitor_( EFeature,			drawIndirectFirstInstance,				: 2 )	/* Vulkan feature: drawIndirectFirstInstance										*/\
		_visitor_( EFeature,			drawIndirectCount,						: 2 )	/* VK_KHR_draw_indirect_count														*/\
		/* multi view */\
		_visitor_( EFeature,			multiview,								: 2 )	/*\																					*/\
		_visitor_( EFeature,			multiviewGeometryShader,				: 2 )	/*-|																				*/\
		_visitor_( EFeature,			multiviewTessellationShader,			: 2 )	/*-|--GL_EXT_multiview																*/\
		_visitor_( uint,				maxMultiviewViewCount,						)	/*/																					*/\
		/* multi viewport */\
		_visitor_( EFeature,			multiViewport,							: 2 )\
		_visitor_( uint,				maxViewports,								)\
		/* sample locations */\
		_visitor_( EFeature,			sampleLocations,						: 2 )	/* VK_EXT_sample_locations															*/\
		_visitor_( EFeature,			variableSampleLocations,				: 2 )\
		/*_visitor_( SampleCountBits,	sampleLocationSampleCounts,					)*/\
		/* tessellation */\
		_visitor_( EFeature,			tessellationIsolines,					: 2 )\
		_visitor_( EFeature,			tessellationPointMode,					: 2 )\
		/* shader limits */\
		_visitor_( uint,				maxTexelBufferElements,						)\
		_visitor_( uint,				maxUniformBufferSize,						)	/* maxUniformBufferRange															*/\
		_visitor_( uint,				maxStorageBufferSize,						)	/* maxStorageBufferRange															*/\
		_visitor_( uint,				perDescrSet_maxUniformBuffersDynamic,		)	/* maxDescriptorSetUniformBuffersDynamic											*/\
		_visitor_( uint,				perDescrSet_maxStorageBuffersDynamic,		)	/* maxDescriptorSetStorageBuffersDynamic											*/\
		_visitor_( PerDescriptorSet,	perDescrSet,								)	/* Metal: no limits																	*/\
		_visitor_( PerShaderStage,		perStage,									)\
		_visitor_( ushort,				maxDescriptorSets,							)	/* maxBoundDescriptorSets															*/\
		_visitor_( ushort,				maxTexelOffset,								)	/* maxTexelOffset, minTexelOffset  - [-N-1...+N] for textureOffset()				*/\
		_visitor_( ushort,				maxTexelGatherOffset,						)	/* maxTexelGatherOffset, minTexelGatherOffset										*/\
		_visitor_( ushort,				maxFragmentOutputAttachments,				)	/* maxFragmentOutputAttachments, maxColorAttachments								*/\
		_visitor_( ushort,				maxFragmentDualSrcAttachments,				)\
		_visitor_( uint,				maxFragmentCombinedOutputResources,			)	/* = storage buffers + storage images + color attachments							*/\
		_visitor_( uint,				maxPushConstantsSize,						)\
		_visitor_( uint,				maxTotalThreadgroupSize,					)	/* only for Metal																	*/\
		_visitor_( uint,				maxTotalTileMemory,							)	/* only for Metal																	*/\
		_visitor_( uint,				maxVertAmplification,						)	/* only for Metal																	*/\
		/* compute shader */\
		_visitor_( uint,				maxComputeSharedMemorySize,					)\
		_visitor_( uint,				maxComputeWorkGroupInvocations,				)\
		_visitor_( uint,				maxComputeWorkGroupSizeX,					)	/* local_size_x, maxComputeWorkGroupCount											*/\
		_visitor_( uint,				maxComputeWorkGroupSizeY,					)	/* local_size_y																		*/\
		_visitor_( uint,				maxComputeWorkGroupSizeZ,					)	/* local_size_z																		*/\
		/* mesh shader */\
		_visitor_( EFeature,			taskShader,								: 2 )	/*\																					*/\
		_visitor_( EFeature,			meshShader,								: 2 )	/*-|--GL_EXT_mesh_shader															*/\
		_visitor_( uint,				maxTaskWorkGroupSize,						)	/*-|	- local_size_x/y/z															*/\
		_visitor_( uint,				maxMeshWorkGroupSize,						)	/*-|	- local_size_x/y/z															*/\
		_visitor_( uint,				maxMeshOutputVertices,						)	/*-|																				*/\
		_visitor_( uint,				maxMeshOutputPrimitives,					)	/*-|																				*/\
		_visitor_( uint,				maxMeshOutputPerVertexGranularity,			)	/*-|	- meshOutputPerVertexGranularity											*/\
		_visitor_( uint,				maxMeshOutputPerPrimitiveGranularity,		)	/*-|	- meshOutputPerPrimitiveGranularity											*/\
		_visitor_( uint,				maxTaskPayloadSize,							)	/*-|																				*/\
		_visitor_( uint,				maxTaskSharedMemorySize,					)	/*-|																				*/\
		_visitor_( uint,				maxTaskPayloadAndSharedMemorySize,			)	/*-|																				*/\
		_visitor_( uint,				maxMeshSharedMemorySize,					)	/*-|																				*/\
		_visitor_( uint,				maxMeshPayloadAndSharedMemorySize,			)	/*-|																				*/\
		_visitor_( uint,				maxMeshOutputMemorySize,					)	/*-|																				*/\
		_visitor_( uint,				maxMeshPayloadAndOutputMemorySize,			)	/*-|																				*/\
		_visitor_( uint,				maxMeshMultiviewViewCount,					)	/*-|																				*/\
		_visitor_( uint,				maxPreferredTaskWorkGroupInvocations,		)	/*-|																				*/\
		_visitor_( uint,				maxPreferredMeshWorkGroupInvocations,		)	/*/																					*/\
		/* raster order group */\
		_visitor_( ushort,				maxRasterOrderGroups,						)	/* only for Metal																	*/\
		/* shaders */\
		_visitor_( EFeature,			geometryShader,							: 2 )\
		_visitor_( EFeature,			tessellationShader,						: 2 )\
		_visitor_( EFeature,			computeShader,							: 2 )\
		_visitor_( EFeature,			tileShader,								: 2 )	/* GL_HUAWEI_subpass_shading														*/\
		/* vertex buffer */\
		_visitor_( EFeature,			vertexDivisor,							: 2 )	/* \ 																				*/\
		_visitor_( uint,				maxVertexAttribDivisor,						)	/*-'-- VK_EXT_vertex_attribute_divisor												*/\
		_visitor_( uint,				maxVertexAttributes,						)	/* maxVertexInputAttributes															*/\
		_visitor_( uint,				maxVertexBuffers,							)	/* maxVertexInputBindings															*/\
		\
		\
	/*---- buffer ----*/\
		_visitor_( VertexFormatSet_t,	vertexFormats,								)	/* VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT												*/\
		_visitor_( PixelFormatSet_t,	uniformTexBufferFormats,					)	/* VK_FORMAT_FEATURE_UNIFORM_TEXEL_BUFFER_BIT										*/\
		_visitor_( PixelFormatSet_t,	storageTexBufferFormats,					)	/* VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_BIT										*/\
		_visitor_( PixelFormatSet_t,	storageTexBufferAtomicFormats,				)	/* VK_FORMAT_FEATURE_STORAGE_TEXEL_BUFFER_ATOMIC_BIT								*/\
		_visitor_( VertexFormatSet_t,	accelStructVertexFormats,					)	/* VK_FORMAT_FEATURE_ACCELERATION_STRUCTURE_VERTEX_BUFFER_BIT_KHR					*/\
		\
		\
	/*---- image -----*/\
		_visitor_( EFeature,			imageCubeArray,							: 2 )\
		_visitor_( EFeature,			textureCompressionASTC_LDR,				: 2 )\
		_visitor_( EFeature,			textureCompressionASTC_HDR,				: 2 )\
		_visitor_( EFeature,			textureCompressionBC,					: 2 )\
		_visitor_( EFeature,			textureCompressionETC2,					: 2 )\
		_visitor_( EFeature,			imageViewMinLod,						: 2 )	/* VK_EXT_image_view_min_lod, minLod												*/\
		_visitor_( EFeature,			multisampleArrayImage,					: 2 )\
		_visitor_( EFeature,			imageViewFormatList,					: 2 )	/* VK_KHR_image_format_list															*/\
		_visitor_( EFeature,			imageViewExtendedUsage,					: 2 )	/* VK_KHR_maintenance2																*/\
		_visitor_( SurfaceFormatSet_t,	surfaceFormats,								)\
		_visitor_( uint,				maxImageArrayLayers,						)\
		_visitor_( PixelFormatSet_t,	storageImageAtomicFormats,					)	/* VK_FORMAT_FEATURE_STORAGE_IMAGE_ATOMIC_BIT										*/\
		_visitor_( PixelFormatSet_t,	storageImageFormats,						)	/* VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT		TODO: R or W							*/\
		_visitor_( PixelFormatSet_t,	attachmentBlendFormats,						)	/* VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT										*/\
		_visitor_( PixelFormatSet_t,	attachmentFormats,							)	/* VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT |	VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT */\
		_visitor_( PixelFormatSet_t,	linearSampledFormats,						)	/* VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT								*/\
		/*_visitor_( PixelFormatSet_t,	minmaxFilterFormats,						)	/ * VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_MINMAX_BIT								*/\
		/*_visitor_( PixelFormatSet_t,	sparseImageFormats,							)*/\
		/*_visitor_( PixelFormatSet_t,	multisampleImageFormats,					)*/\
		_visitor_( PixelFormatSet_t,	hwCompressedAttachmentFormats,				)	/* formats which is compatible with lossless hardware compression					*/\
		_visitor_( PixelFormatSet_t,	lossyCompressedAttachmentFormats,			)	/* formats which is compatible with lossy hardware compression						*/\
		\
		\
	/*---- sampler ----*/\
		_visitor_( EFeature,			samplerAnisotropy,						: 2 )\
		_visitor_( EFeature,			samplerMirrorClampToEdge,				: 2 )	/* VK_KHR_sampler_mirror_clamp_to_edge												*/\
		_visitor_( EFeature,			samplerFilterMinmax,					: 2 )\
		_visitor_( EFeature,			filterMinmaxImageComponentMapping,		: 2 )\
		/*_visitor_( EFeature,			filterMinmaxSingleComponentFormats,		: 2 )*/\
		_visitor_( EFeature,			samplerMipLodBias,						: 2 )\
		_visitor_( EFeature,			samplerYcbcrConversion,					: 2 )	/* VK_KHR_sampler_ycbcr_conversion													*/\
		_visitor_( EFeature,			ycbcr2Plane444,							: 2 )	/* VK_EXT_ycbcr_2plane_444_formats													*/\
		_visitor_( EFeature,			nonSeamlessCubeMap,						: 2 )	/* VK_EXT_non_seamless_cube_map														*/\
		_visitor_( float,				maxSamplerAnisotropy,						)\
		_visitor_( float,				maxSamplerLodBias,							)\
		/*_visitor_( SampleCountBits,	sampledImageColorSampleCounts,				)*/\
		/*_visitor_( SampleCountBits,	sampledImageDepthSampleCounts,				)*/\
		/*_visitor_( SampleCountBits,	sampledImageIntegerSampleCounts,			)*/\
		/*_visitor_( SampleCountBits,	sampledImageStencilSampleCounts,			)*/\
		/*_visitor_( SampleCountBits,	storageImageSampleCounts,					)*/\
		\
		\
	/*---- framebuffer ----*/\
		_visitor_( SampleCountBits,		framebufferColorSampleCounts,				)\
		_visitor_( SampleCountBits,		framebufferDepthSampleCounts,				)\
		/*_visitor_( SampleCountBits,	framebufferIntegerColorSampleCounts,		)*/\
		_visitor_( uint,				maxFramebufferLayers,						)\
		\
		\
	/*---- render pass ----*/\
		_visitor_( EFeature,			variableMultisampleRate,				: 2 )\
		\
		\
	/*---- android ----*/\
		_visitor_( EFeature,			externalFormatAndroid,					: 2 )	/* VK_ANDROID_external_memory_android_hardware_buffer								*/\
		\
		\
	/*---- metal ----*/\
		_visitor_( ubyte,				metalArgBufferTier,							)\
		\
		\
	/* HW info */\
		_visitor_( Queues,				queues,										)\
		_visitor_( VendorIDs_t,			vendorIds,									)\
		_visitor_( GraphicsDevices_t,	devicesIds,									)\


		// TODO:
		//	min total memory size
		//	min/max resolution ?
		//	supported queries - for dynamic resolution rendering and other techniques
		//	combinedImageSamplerDescriptorCount


	// variables
		#define AE_FEATURE_SET_VISIT( _type_, _name_, _bits_ )		_type_	_name_	_bits_;
		AE_FEATURE_SET_FIELDS( AE_FEATURE_SET_VISIT )
		#undef AE_FEATURE_SET_VISIT


	// methods
		FeatureSet ()																__NE___;

			void  SetAll (EFeature value)											__NE___;
			void  SetDefault ()														__NE___;

			void  AddDevice (uint vendorId, uint deviceId, StringView name)			__NE___;

		ND_ bool  IsValid ()														C_NE___;
			void  Validate ()														__NE___;

		ND_ bool  IsSupported (const RenderState &rs)								C_NE___;
		ND_ bool  IsSupported (const BufferDesc &desc)								C_NE___;
		ND_ bool  IsSupported (const BufferDesc &desc, const BufferViewDesc &view)	C_NE___;
		ND_ bool  IsSupported (const ImageDesc &desc)								C_NE___;
		ND_ bool  IsSupported (const ImageDesc &desc, const ImageViewDesc &view)	C_NE___;

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


		ND_ EFeature  accelerationStructure ()										C_NE___;
		ND_ EFeature  rayShaderBindingTable ()										C_NE___	{ return rayTracingPipeline; }


	private:
		template <bool Mutable>
		bool  _Validate ()															__NE___;
	};
	StaticAssert( sizeof(FeatureSet) == 696 );

} // AE::Graphics


namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::FeatureSet >		: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::FeatureSet >	: CT_True {};

} // AE::Base
