// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/Networking.h"
#include "pch/Serializing.h"

#include "graphics/Public/FeatureSet.h"
#include "graphics/Public/DeviceProperties.h"
#include "graphics/Public/Queue.h"
#include "graphics/Public/GraphicsCreateInfo.h"
#include "graphics/Public/ResourceManager.h"
#include "graphics/Public/RemoteGraphicsTypes.h"
#include "graphics/Public/BufferDesc.h"
#include "graphics/Public/CommandBufferTypes.h"
#include "graphics/Public/QueryManager.h"
#include "graphics/Public/IDevice.h"

#include "profiler/Profilers/ArmProfiler.h"
#include "profiler/Profilers/MaliProfiler.h"
#include "profiler/Profilers/NVidiaProfiler.h"
#include "profiler/Profilers/AdrenoProfiler.h"
#include "profiler/Profilers/PowerVRProfiler.h"
#include "profiler/Profilers/GeneralProfiler.h"

#include "PipelineCompilerImpl.h"

# ifdef AE_ENABLE_GLSL_TRACE
#	include "ShaderTrace.h"
# else
#	include "Packer/ShaderTraceDummy.h"
# endif

namespace AE::RemoteGraphics::Msg
{
	using namespace AE::Graphics;
	using namespace AE::Serializing;

	using ShaderTracePtr = Unique< const PipelineCompiler::ShaderTrace >;


	struct BaseMsg : public ISerializable, public EnableRC<BaseMsg>
	{
		BaseMsg ()					__NE___ {}
		BaseMsg (const BaseMsg &)	__NE___	{}
		BaseMsg (BaseMsg &&)		__NE___ {}

		ND_ virtual TypeId  GetTypeId () const = 0;
	};


	struct BaseResponse : public BaseMsg
	{};


	#define DECL_MSG( _name_, ... )															\
		struct _name_ final : BaseMsg														\
		{																					\
			__VA_ARGS__																		\
																							\
			_name_ () __NE___ {}															\
			bool	Serialize (Serializer &)		C_NE_OV;								\
			bool	Deserialize (Deserializer &)	__NE_OV;								\
			TypeId	GetTypeId ()					C_NE_OV	{ return TypeIdOf<_name_>(); }	\
		};

	#define DECL_RESP( _name_, ... )														\
		struct _name_ final : BaseResponse													\
		{																					\
			__VA_ARGS__																		\
																							\
			_name_ () __NE___ {}															\
			bool	Serialize (Serializer &)		C_NE_OV;								\
			bool	Deserialize (Deserializer &)	__NE_OV;								\
			TypeId	GetTypeId ()					C_NE_OV	{ return TypeIdOf<_name_>(); }	\
		};

	DECL_RESP( DefaultResponse,
		bool			ok = false;
	)

	DECL_MSG( UploadData,
		Bytes			size;
		const void*		data	= null;
		RC<RStream>		stream;
	)

	DECL_RESP( UploadDataAndCopy,
		Bytes			size;
		const void*		data	= null;
		RmHostPtr		dst		= Default;
	)

	DECL_RESP( Log,
		StringView			message;
		StringView			func;
		StringView			file;
		uint				line;
		ILogger::ELevel		level;
		ILogger::EScope		scope;
	)
//-----------------------------------------------------------------------------



	DECL_MSG( Device_Init,
		GraphicsCreateInfo		info;
		bool					initSyncLog	= false;
	)

	DECL_RESP( Device_Init_Response,
		struct QueueInfo
		{
			EQueueType		type				= Default;
			packed_uint3	minImageTransferGranularity	{1};	// TODO: ushort or POT
		};
		using QueueInfos_t = StaticArray< QueueInfo, uint(EQueueType::_Count) >;

		enum class EFeature : uint
		{
			Unknown		= 0,

			// IDrawContext (Vulkan)
			DepthBounds,
			StencilCompareMask,
			StencilWriteMask,
			FragmentShadingRate,
			DrawIndirectCount,
			DrawIndexedIndirectCount,
			DrawMeshTasksIndirectCount,
			ViewportWScaling,

			// ITransferContext (Vulkan)
			ClearColorImage,
			ClearDepthStencilImage,
			ResolveImage,

			// IRayTracingContext (Vulkan)
			TraceRaysIndirect_DevAddr,
			TraceRaysIndirect2_DevAddr,

			// IASBuildContext (Vulkan)
			BuildIndirect,
			SerializeToMemory,
			DeserializeFromMemory,

			// All contexts (Vulkan / Metal)
			WriteTimestamp,		// on Metal with ARM HW supported time query only on begin/end of encoder

			// Device
			TimelineSemaphore,
			HostQueryReset,

			_Count
		};
		using EFeatureBits = EnumSet< EFeature >;


		Version3				engineVersion;
		String					name;
		FeatureSet				fs;
		DeviceProperties		props;
		DeviceResourceFlags		resFlags;
		QueueInfos_t			queues;
		DeviceMemoryInfo		memInfo;
		EFeatureBits			features;
		EGraphicsAPI			api;
		ECPUArch				cpuArch;
		EOperationSystem		os;
		EGraphicsAdapterType	adapterType;
		bool					checkConstantLimitsOK	= false;	// IDevice::CheckConstantLimits()
		bool					checkExtensionsOK		= false;	// IDevice::CheckExtensions()
		bool					initialized				= false;	// IDevice::IsInitialized()
		bool					underDebugger			= false;	// IDevice::IsUnderDebugger()
	)

	DECL_MSG( Device_DestroyLogicalDevice, )

	DECL_MSG( Device_DestroyInstance, )

	DECL_MSG( Device_EnableSyncLog,
		bool					enable	= true;
	)

	DECL_MSG( Device_GetSyncLog, )

	DECL_RESP( Device_GetSyncLog_Response,
		String					log;
	)
//-----------------------------------------------------------------------------



	DECL_MSG( Surface_Create, )

	DECL_RESP( Surface_Create_Response,
		bool					created		= false;
		Array<SurfaceFormat>	formats;
		Array<EPresentMode>		presentModes;
	)

	DECL_MSG( Surface_Destroy, )

	DECL_MSG( Swapchain_Create,
		uint2					viewSize;
		SwapchainDesc			desc;
		StringView				dbgName;
	)

	DECL_MSG( Swapchain_Recreate,
		uint2					viewSize;
		SwapchainDesc			desc;
	)

	DECL_RESP( Swapchain_Create_Response,
		using Semaphores_t = StaticArray< RmSemaphoreID, GraphicsConfig::MaxSwapchainLength >;

		bool						created		= false;
		bool						minimized	= false;
		uint2						viewSize;
		SwapchainDesc				desc;
		Semaphores_t				imageAvailable;
		Semaphores_t				renderFinished;
		ArrayView< RmImageID >		imageIds;
		ArrayView< RmImageViewID >	viewIds;
	)

	DECL_MSG( Swapchain_Destroy, )

	DECL_MSG( Swapchain_IsSupported,
		EPresentMode			presentMode;
		EPixelFormat			colorFormat;
		EImageUsage				colorImageUsage;
	)

	DECL_MSG( Swapchain_AcquireNextImage, )

	DECL_RESP( Swapchain_AcquireNextImage_Response,
		enum class EAcquireResult {
			OK					= 0,
			OK_RecreateLater,
			Error_RecreateImmediately,
			Error
		};
		EAcquireResult			result;
		ushort					semaphoreId;
		ushort					imageIdx;
	)

	DECL_MSG( Swapchain_Present,
		uint					submitIdx;
		EQueueType				presentQueue;
		ushort					semaphoreId;
		ushort					imageIdx;
	)

	DECL_RESP( Swapchain_Present_Response,
		enum class EPresentResult {
			OK					= 0,
			OK_RecreateLater,
			Error_RecreateImmediately,
			Error
		};
		EPresentResult			result;
	)
//-----------------------------------------------------------------------------



	DECL_MSG( RTS_Initialize,
		GraphicsCreateInfo		info;
	)

	DECL_MSG( RTS_Deinitialize, )

	DECL_MSG( RTS_BeginFrame,
		FrameUID				frameId;
		BeginFrameConfig		cfg;
	)

	DECL_MSG( RTS_EndFrame,
		FrameUID				frameId;
		uint					submitIdx;
	)

	DECL_MSG( RTS_WaitNextFrame,
		FrameUID				frameId;
		nanoseconds				timeout;
	)

	DECL_RESP( RTS_WaitNextFrame_Response,
		using DevMemoryUsageOpt	= Optional< DeviceMemoryUsage >;

		bool					ok;
		DevMemoryUsageOpt		memUsage;
	)

	DECL_MSG( RTS_WaitAll,
		FrameUID				frameId;
		nanoseconds				timeout;
	)

	DECL_MSG( RTS_SkipCmdBatches,
		EQueueType				queue;
		uint					bits;
	)

	DECL_MSG( RTS_CreateBatch,
		EQueueType				queue;
		CmdBatchDesc::EFlags	flags;
		FrameUID				frameId;
		uint					submitIdx;
		DebugLabel				dbgLabel	= Default;
	)

	DECL_RESP( RTS_CreateBatch_Response,
		RmCommandBatchID		batchId;
		RmSemaphoreID			semaphoreId;
	)

	DECL_MSG( RTS_SubmitBatch,
		RmCommandBatchID						id;
		uint									submitIdx;
		ArrayView<RmCommandBufferID>			cmdbufs;
		ArrayView<Pair<RmSemaphoreID, ulong>>	inputDeps;
		ArrayView<Pair<RmSemaphoreID, ulong>>	outputDeps;
	)

	DECL_MSG( RTS_WaitBatch,
		RmCommandBatchID		id;
		nanoseconds				timeout;
	)

	DECL_MSG( RTS_DestroyBatch,
		RmCommandBatchID		batchId;
		RmSemaphoreID			semaphoreId;
	)

	DECL_MSG( RTS_CreateDrawBatch,
	)

	DECL_RESP( RTS_CreateDrawBatch_Response,
		RmDrawCommandBatchID	id;
	)
//-----------------------------------------------------------------------------



	DECL_MSG( ResMngr_CreateImage,
		ImageDesc				desc;
		RmGfxMemAllocatorID		gfxAlloc;
		StringView				dbgName;
	)


	DECL_RESP( ResMngr_CreateImage_Response,
		RmImageID				imageId;
		RmMemoryID				memoryId;
		ImageDesc				desc;
	)

	DECL_MSG( ResMngr_CreateImageView,
		ImageViewDesc			desc;
		RmImageID				imageId;
		StringView				dbgName;
	)

	DECL_RESP( ResMngr_CreateImageView_Response,
		RmImageViewID			viewId;
		ImageViewDesc			desc;
	)

	DECL_MSG( ResMngr_CreateBuffer,
		BufferDesc				desc;
		RmGfxMemAllocatorID		gfxAlloc;
		StringView				dbgName;
	)

	DECL_RESP( ResMngr_CreateBuffer_Response,
		RmBufferID				bufferId;
		RmMemoryID				memoryId;
		DeviceAddress			addr;
		BufferDesc				desc;
		RmDevicePtr				mapped	= Default;
	)

	DECL_MSG( ResMngr_CreateBufferView,
		BufferViewDesc			desc;
		RmBufferID				bufferId;
		StringView				dbgName;
	)

	DECL_RESP( ResMngr_CreateBufferView_Response,
		RmBufferViewID			viewId;
		BufferViewDesc			desc;
	)

	DECL_MSG( ResMngr_CreateRTGeometry,
		RTGeometryDesc			desc;
		RmGfxMemAllocatorID		gfxAlloc;
		StringView				dbgName;
	)

	DECL_RESP( ResMngr_CreateRTGeometry_Response,
		RmRTGeometryID			geomId;
		RmMemoryID				memoryId;
		DeviceAddress			addr;
		RTGeometryDesc			desc;
	)

	DECL_MSG( ResMngr_CreateRTScene,
		RTSceneDesc				desc;
		RmGfxMemAllocatorID		gfxAlloc;
		StringView				dbgName;
	)

	DECL_RESP( ResMngr_CreateRTScene_Response,
		RmRTSceneID				sceneId;
		RmMemoryID				memoryId;
		DeviceAddress			addr;
		RTSceneDesc				desc;
	)

	DECL_MSG( ResMngr_GetRTGeometrySizes,
		RTGeometryBuild			desc;
	)

	DECL_RESP( ResMngr_GetRTGeometrySizes_Response,
		RTASBuildSizes			sizes;
	)

	DECL_MSG( ResMngr_GetRTSceneSizes,
		RTSceneBuild			desc;
	)

	DECL_RESP( ResMngr_GetRTSceneSizes_Response,
		RTASBuildSizes			sizes;
	)

	DECL_MSG( ResMngr_IsSupported_BufferDesc,
		BufferDesc				desc;
	)

	DECL_MSG( ResMngr_IsSupported_ImageDesc,
		ImageDesc				desc;
	)

	DECL_MSG( ResMngr_IsSupported_VideoImageDesc,
		VideoImageDesc			desc;
	)

	DECL_MSG( ResMngr_IsSupported_VideoBufferDesc,
		VideoBufferDesc			desc;
	)

	DECL_MSG( ResMngr_IsSupported_VideoSessionDesc,
		VideoSessionDesc		desc;
	)

	DECL_MSG( ResMngr_IsSupported_BufferViewDesc,
		RmBufferID				bufferId;
		BufferViewDesc			desc;
	)

	DECL_MSG( ResMngr_IsSupported_ImageViewDesc,
		RmImageID				imageId;
		ImageViewDesc			desc;
	)

	DECL_MSG( ResMngr_IsSupported_RTGeometryDesc,
		RTGeometryDesc			desc;
	)

	DECL_MSG( ResMngr_IsSupported_RTGeometryBuild,
		RTGeometryBuild			desc;
	)

	DECL_MSG( ResMngr_IsSupported_RTSceneDesc,
		RTSceneDesc				desc;
	)

	DECL_MSG( ResMngr_IsSupported_RTSceneBuild,
		RTSceneBuild			desc;
	)

	DECL_RESP( ResMngr_IsSupported_Response,
		bool					supported;
	)

	DECL_MSG( ResMngr_ReleaseResource,
		using Types = TypeList< RmImageID, RmBufferID, RmImageViewID, RmBufferViewID, RmGfxMemAllocatorID, RmDescriptorAllocatorID,
								RmRTGeometryID, RmRTSceneID, RmDescriptorSetID, RmRayTracingPipelineID, RmTilePipelineID,
								RmComputePipelineID, RmGraphicsPipelineID, RmMeshPipelineID, RmPipelinePackID,
								RmPipelineCacheID, RmRenderTechPipelinesID >;

		ulong					id;
		uint					index;		// from 'Types'

		template <typename ID> ResMngr_ReleaseResource (ID resId)	__NE___ : id{0}, index{uint(Types::Index<ID>)}  { std::memcpy( OUT &id, &resId, sizeof(resId) ); }
	)

	DECL_RESP( ResMngr_ReleaseResource_Response,
		using Types = ResMngr_ReleaseResource::Types;

		uint					index;		// from 'Types'
		bool					released;
	)

	DECL_MSG( ResMngr_CreateDescriptorSets2,
		RmPipelinePackID				packId;
		uint							count;
		DSLayoutName::Optimized_t		dslName;
		RmDescriptorAllocatorID			dsAlloc;
		StringView						dbgName;
	)

	DECL_MSG( ResMngr_CreateDescriptorSets3,
		RmDescriptorSetLayoutID			layoutId;
		uint							count;
		RmDescriptorAllocatorID			dsAlloc;
		StringView						dbgName;
	)

	DECL_RESP( ResMngr_CreateDescriptorSets_Response,
		Array<RmDescriptorSetID>		ds;
	)

	DECL_MSG( ResMngr_GetRTechPipeline,
		using Types = TypeList< RmGraphicsPipelineID, RmMeshPipelineID, RmComputePipelineID, RmRayTracingPipelineID, RmTilePipelineID >;

		RmRenderTechPipelinesID			rtechId;
		PipelineName::Optimized_t		name;
		uint							index;	// from Types
	)

	DECL_MSG( ResMngr_CreateGraphicsPipeline,
		RmPipelinePackID				packId;
		PipelineTmplName::Optimized_t	name;
		GraphicsPipelineDesc			desc;
		RmPipelineCacheID				cacheId;
	)

	DECL_RESP( ResMngr_CreateGraphicsPipeline_Response,
		using VBtoIndex_t	= FixedMap< VertexBufferName::Optimized_t, ubyte, GraphicsConfig::MaxVertexBuffers >;

		RmGraphicsPipelineID			pplnId;
		EPipelineDynamicState			dynamicState;
		EPipelineOpt					options;
		EPrimitive						topology;
		ubyte							subpassIndex;
		VBtoIndex_t						vertexBuffers;
		RmPipelineLayoutID				pipelineLayoutId;
		Array<ShaderTracePtr>			shaderTrace;
	)

	DECL_MSG( ResMngr_CreateMeshPipeline,
		RmPipelinePackID				packId;
		PipelineTmplName::Optimized_t	name;
		MeshPipelineDesc				desc;
		RmPipelineCacheID				cacheId;
	)

	DECL_RESP( ResMngr_CreateMeshPipeline_Response,
		RmMeshPipelineID				pplnId;
		EPipelineDynamicState			dynamicState;
		EPipelineOpt					options;
		ushort3							meshLocalSize;
		ushort3							taskLocalSize;
		ubyte							subpassIndex;
		RmPipelineLayoutID				pipelineLayoutId;
		Array<ShaderTracePtr>			shaderTrace;
	)

	DECL_MSG( ResMngr_CreateComputePipeline,
		RmPipelinePackID				packId;
		PipelineTmplName::Optimized_t	name;
		ComputePipelineDesc				desc;
		RmPipelineCacheID				cacheId;
	)

	DECL_RESP( ResMngr_CreateComputePipeline_Response,
		RmComputePipelineID				pplnId;
		EPipelineDynamicState			dynamicState;
		EPipelineOpt					options;
		ushort3							localSize;
		RmPipelineLayoutID				pipelineLayoutId;
		ShaderTracePtr					shaderTrace;
	)

	DECL_MSG( ResMngr_CreateRayTracingPipeline,
		RmPipelinePackID				packId;
		PipelineTmplName::Optimized_t	name;
		RayTracingPipelineDesc			desc;
		RmPipelineCacheID				cacheId;
	)

	DECL_RESP( ResMngr_CreateRayTracingPipeline_Response,
		using NameToHandle_t	= FlatHashMap< RayTracingGroupName::Optimized_t, uint >;

		RmRayTracingPipelineID			pplnId;
		EPipelineDynamicState			dynamicState;
		EPipelineOpt					options;
		Bytes16u						shaderGroupHandleSize;
		NameToHandle_t					nameToHandle;
		Array< ulong >					groupHandles;
		RmPipelineLayoutID				pipelineLayoutId;
		Array<ShaderTracePtr>			shaderTrace;
	)

	DECL_MSG( ResMngr_CreateTilePipeline,
		RmPipelinePackID				packId;
		PipelineTmplName::Optimized_t	name;
		TilePipelineDesc				desc;
		RmPipelineCacheID				cacheId;
	)

	DECL_RESP( ResMngr_CreateTilePipeline_Response,
		RmTilePipelineID				pplnId;
		EPipelineDynamicState			dynamicState;
		EPipelineOpt					options;
		ushort2							localSize;
		ubyte							subpassIndex;
		RmPipelineLayoutID				pipelineLayoutId;
		ShaderTracePtr					shaderTrace;
	)

	DECL_MSG( ResMngr_CreateVideoSession,
		VideoSessionDesc		desc;
		StringView				dbgName;
		RmGfxMemAllocatorID		gfxAlloc;
	)

	DECL_RESP( ResMngr_CreateVideoSession_Response,
		RmVideoSessionID		id;
		VideoSessionDesc		desc;
	)

	DECL_MSG( ResMngr_CreateVideoBuffer,
		VideoBufferDesc			desc;
		StringView				dbgName;
		RmGfxMemAllocatorID		gfxAlloc;
	)

	DECL_RESP( ResMngr_CreateVideoBuffer_Response,
		RmVideoBufferID			id;
		VideoBufferDesc			desc;
	)

	DECL_MSG( ResMngr_CreateVideoImage,
		VideoImageDesc			desc;
		StringView				dbgName;
		RmGfxMemAllocatorID		gfxAlloc;
	)

	DECL_RESP( ResMngr_CreateVideoImage_Response,
		RmVideoImageID			id;
		VideoImageDesc			desc;
	)

	DECL_MSG( ResMngr_CreatePipelineCache, )

	DECL_RESP( ResMngr_CreatePipelineCache_Response,
		RmPipelineCacheID		id;
	)

	DECL_MSG( ResMngr_InitializeResources,
		RmPipelinePackID		packId;
	)

	DECL_MSG( ResMngr_LoadPipelinePack,
		PipelinePackDesc		desc;
		RmGfxMemAllocatorID		sbtAllocatorId;		// replace 'desc.sbtAllocator'
	)

	DECL_RESP( ResMngr_LoadPipelinePack_Response,
		using FeatureNames_t	= FlatHashSet< FeatureSetName::Optimized_t >;
		using SamplerRefs_t		= FlatHashMap< SamplerName::Optimized_t, RmSamplerID >;
		using DSLayouts_t		= Array< RmDescriptorSetLayoutID >;
		using RPasses_t			= Array< RenderPassName::Optimized_t >;
		using PplnLayouts_t		= Array< RmPipelineLayoutID >;

		RmPipelinePackID		packId;
		FeatureNames_t			unsupportedFS;
		SamplerRefs_t			samplerRefs;
		DSLayouts_t				dsLayouts;
		RPasses_t				renderPasses;
		PplnLayouts_t			pplnLayouts;
	)

	DECL_MSG( ResMngr_GetSupportedRenderTechs,
		RmPipelinePackID		id;
	)

	DECL_RESP( ResMngr_GetSupportedRenderTechs_Response,
		Array<RenderTechName>	result;
	)

	DECL_MSG( ResMngr_LoadRenderTech,
		RmPipelinePackID				packId;
		RenderTechName::Optimized_t		name;
		RmPipelineCacheID				cacheId;
	)

	DECL_RESP( ResMngr_LoadRenderTech_Response,
		struct SerPass final : public ISerializable {
			RenderTechPassName::Optimized_t		name;
			IRenderTechPipelines::PassInfo		info;
			bool  Serialize (Serializer &)		C_NE_OV;
			bool  Deserialize (Deserializer &)	__NE_OV;
		};

		using PassMap_t		= Array< SerPass >;
		using PplnSpecMap_t	= Array<Pair< PipelineName::Optimized_t, PipelineCompiler::PipelineSpecUID >>;
		using RTSBTMap_t	= Array<Pair< RTShaderBindingName::Optimized_t, RmRTShaderBindingID >>;

		RmRenderTechPipelinesID			id;
		RenderTechName::Optimized_t		name;
		PplnSpecMap_t					pipelines;
		RTSBTMap_t						rtSbtMap;
		PassMap_t						passes;
	)

	DECL_MSG( ResMngr_CreateLinearGfxMemAllocator,
		Bytes				pageSize;
	)

	DECL_MSG( ResMngr_CreateBlockGfxMemAllocator,
		Bytes				blockSize;
		Bytes				pageSize;
	)

	DECL_MSG( ResMngr_CreateUnifiedGfxMemAllocator,
		Bytes				pageSize;
	)

	DECL_RESP( ResMngr_CreateGfxMemAllocator_Response,
		RmGfxMemAllocatorID	id;
		Bytes				minAlign;
		Bytes				maxSize;
	)

	DECL_MSG( ResMngr_GetRenderPass,
		RmPipelinePackID			packId;
		RenderPassName::Optimized_t	name;
	)

	DECL_RESP( ResMngr_GetRenderPass_Response,
		using PixFormatMap_t		= FixedMap< AttachmentName::Optimized_t, Pair< EPixelFormat, ubyte >, GraphicsConfig::MaxAttachments >;
		using AttStates_t			= StaticArray< Pair<EResourceState, EResourceState>, GraphicsConfig::MaxAttachments >;		// initial, final

		ubyte						subpassCount;
		PixFormatMap_t				pixFormats;
		AttStates_t					attStates;
	)

	DECL_MSG( ResMngr_ForceReleaseResources, )

	DECL_MSG( ResMngr_GetShaderGroupStackSize,
		RmRayTracingPipelineID			pplnId;
		ArrayView<RayTracingGroupName>	names;
		ERTShaderGroup					type;
	)

	DECL_RESP( ResMngr_GetShaderGroupStackSize_Response,
		Bytes							size;
	)
//-----------------------------------------------------------------------------


	DECL_MSG( Query_Init, )

	DECL_RESP( Query_Init_Response,
		bool						calibratedTimestamps;
	)

	DECL_MSG( Query_Alloc,
		EQueueType					queueType;
		EQueryType					queryType;
		uint						count;
	)

	DECL_RESP( Query_Alloc_Response,
		struct SerQuery
		{
			RmQueryID					queryId;
			ushort						first		= UMax;
			ushort						count		= 0;
			ushort						numPasses	= 0;
			EQueryType					type		= Default;
			EQueueType					queue		= Default;
		};
		SerQuery					query;
	)

	DECL_MSG( Query_GetTimestampUL,
		using SerQuery = Query_Alloc_Response::SerQuery;

		SerQuery					query;
		Bytes						size;
	)

	DECL_RESP( Query_GetTimestampUL_Response,
		ArrayView<ulong>			result;
	)

	DECL_MSG( Query_GetTimestampD,
		using SerQuery = Query_Alloc_Response::SerQuery;

		SerQuery					query;
		Bytes						size;
	)

	DECL_RESP( Query_GetTimestampD_Response,
		ArrayView<double>			result;
	)

	DECL_MSG( Query_GetTimestampNs,
		using SerQuery = Query_Alloc_Response::SerQuery;

		SerQuery					query;
		Bytes						size;
	)

	DECL_RESP( Query_GetTimestampNs_Response,
		ArrayView<nanosecondsd>		result;
	)

	DECL_MSG( Query_GetTimestampCalibratedUL,
		using SerQuery = Query_Alloc_Response::SerQuery;

		SerQuery					query;
		Bytes						size;
	)

	DECL_RESP( Query_GetTimestampCalibratedUL_Response,
		ArrayView<ulong>			result;
		ArrayView<ulong>			maxDeviation;
	)

	DECL_MSG( Query_GetTimestampCalibratedD,
		using SerQuery = Query_Alloc_Response::SerQuery;

		SerQuery					query;
		Bytes						size;
	)

	DECL_RESP( Query_GetTimestampCalibratedD_Response,
		ArrayView<double>			result;
		ArrayView<double>			maxDeviation;
	)

	DECL_MSG( Query_GetTimestampCalibratedNs,
		using SerQuery = Query_Alloc_Response::SerQuery;

		SerQuery					query;
		Bytes						size;
	)

	DECL_RESP( Query_GetTimestampCalibratedNs_Response,
		ArrayView<nanosecondsd>		result;
		ArrayView<nanosecondsd>		maxDeviation;
	)

	DECL_MSG( Query_GetPipelineStatistic,
		using SerQuery = Query_Alloc_Response::SerQuery;

		SerQuery					query;
		Bytes						size;
	)

	DECL_RESP( Query_GetPipelineStatistic_Response,
		using GPplnStat = IQueryManager::GraphicsPipelineStatistic;

		ArrayView<GPplnStat>		result;
	)
//-----------------------------------------------------------------------------


	DECL_MSG( ProfArm_Initialize,
		Profiler::ArmProfiler::ECounterSet		required;
	)

	DECL_RESP( ProfArm_Initialize_Response,
		bool									ok;
		Profiler::ArmProfiler::ECounterSet		enabled;
	)

	DECL_MSG( ProfArm_Sample )

	DECL_RESP( ProfArm_Sample_Response,
		Profiler::ArmProfiler::Counters_t		counters;
	)
//-----------------------------------------------------------------------------


	DECL_MSG( ProfMali_Initialize,
		Profiler::MaliProfiler::ECounterSet		required;
	)

	DECL_RESP( ProfMali_Initialize_Response,
		bool									ok;
		Profiler::MaliProfiler::ECounterSet		enabled;
		Profiler::MaliProfiler::HWInfo			info;
	)

	DECL_MSG( ProfMali_Sample )

	DECL_RESP( ProfMali_Sample_Response,
		Profiler::MaliProfiler::Counters_t		counters;
	)
//-----------------------------------------------------------------------------


	DECL_MSG( ProfAdreno_Initialize,
		Profiler::AdrenoProfiler::ECounterSet	required;
	)

	DECL_RESP( ProfAdreno_Initialize_Response,
		bool									ok;
		Profiler::AdrenoProfiler::ECounterSet	enabled;
		Profiler::AdrenoProfiler::HWInfo		info;
	)

	DECL_MSG( ProfAdreno_Sample )

	DECL_RESP( ProfAdreno_Sample_Response,
		Profiler::AdrenoProfiler::Counters_t	counters;
	)
//-----------------------------------------------------------------------------


	DECL_MSG( ProfPVR_Initialize,
		Profiler::PowerVRProfiler::ECounterSet	required;
	)

	DECL_RESP( ProfPVR_Initialize_Response,
		bool									ok;
		Profiler::PowerVRProfiler::ECounterSet	enabled;
	)

	DECL_MSG( ProfPVR_GetTiming )

	DECL_RESP( ProfPVR_GetTiming_Response,
		Profiler::PowerVRProfiler::TimeScopeArr_t	timings;
	)

	DECL_MSG( ProfPVR_Sample )

	DECL_RESP( ProfPVR_Sample_Response,
		Profiler::PowerVRProfiler::Counters_t	counters;
	)
//-----------------------------------------------------------------------------


	DECL_MSG( ProfNVidia_Initialize,
		Profiler::NVidiaProfiler::ECounterSet	required;
	)

	DECL_RESP( ProfNVidia_Initialize_Response,
		bool									ok;
		Profiler::NVidiaProfiler::ECounterSet	enabled;
	//	Profiler::NVidiaProfiler::HWInfo		info;
	)

	DECL_MSG( ProfNVidia_Sample )

	DECL_RESP( ProfNVidia_Sample_Response,
		Profiler::NVidiaProfiler::Counters_t	counters;
	)
//-----------------------------------------------------------------------------


	DECL_MSG( ProfGeneral_Initialize,
		Profiler::GeneralProfiler::ECounterSet	required;
	)

	DECL_RESP( ProfGeneral_Initialize_Response,

		struct SerCpuCluster final : Profiler::GeneralProfiler::CpuCluster, ISerializable
		{
			SerCpuCluster ()						__NE___	= default;
			SerCpuCluster (const CpuCluster &other) : CpuCluster{other} {}
			SerCpuCluster (const SerCpuCluster &)	= default;
			SerCpuCluster (SerCpuCluster &&)		= default;

			bool  Serialize (Serializer &)		C_NE_OV;
			bool  Deserialize (Deserializer &)	__NE_OV;
		};

		bool													ok;
		FixedArray< SerCpuCluster, CpuArchInfo::MaxCoreTypes >	cpuClusters;
	)

	DECL_MSG( ProfGeneral_Sample )

	DECL_RESP( ProfGeneral_Sample_Response,
		Profiler::GeneralProfiler::Counters_t	counters;
		ArrayView<float>						totalCpuUsage;
		ArrayView<float>						kernelUsage;
	)
//-----------------------------------------------------------------------------


	struct DescUpd_Flush final : BaseMsg
	{
	// types
		#ifdef AE_ENABLE_REMOTE_GRAPHICS
			struct BaseUpdCmd : public ISerializable
			{};

			#define DECL_CMD( _name_, ... )						\
			struct _name_ final : public BaseUpdCmd				\
			{													\
				__VA_ARGS__										\
																\
				_name_ () __NE___ {}							\
				bool  Serialize (Serializer &)		C_NE_OV;	\
				bool  Deserialize (Deserializer &)	__NE_OV;	\
			};
		#else
			struct BaseUpdCmd : public ISerializable
			{
				virtual void  Execute (void *)	__Th___ = 0;
			};

			#define DECL_CMD( _name_, ... )						\
			struct _name_ final : public BaseUpdCmd				\
			{													\
				__VA_ARGS__										\
																\
				_name_ () __NE___ {}							\
				bool  Serialize (Serializer &)		C_NE_OV;	\
				bool  Deserialize (Deserializer &)	__NE_OV;	\
				void  Execute (void *)				__Th_OV;	\
			};
		#endif

		DECL_CMD( SetDescSet,
			RmDescriptorSetID				descSet;
			EDescUpdateMode					mode;
		)

		DECL_CMD( BindVideoImage,
			UniformName::Optimized_t		unName;
			ushort							firstIndex;
			RmVideoImageID					videoImageId;
		)

		DECL_CMD( BindImages,
			UniformName::Optimized_t		unName;
			ushort							firstIndex;
			Array<RmImageViewID>			images;
		)

		DECL_CMD( BindBuffers,
			UniformName::Optimized_t		unName;
			ShaderStructName::Optimized_t	typeName;
			ushort							firstIndex;
			Array<RmBufferID>				buffers;
		)

		DECL_CMD( BindBufferRange,
			UniformName::Optimized_t		unName;
			ShaderStructName::Optimized_t	typeName;
			ushort							index;
			RmBufferID						buffer;
			Bytes							offset;
			Bytes							size;
		)

		DECL_CMD( BindTextures,
			UniformName::Optimized_t		unName;
			ushort							firstIndex;
			Array<RmImageViewID>			images;
			SamplerName::Optimized_t		sampler;
		)

		DECL_CMD( BindSamplers,
			UniformName::Optimized_t		unName;
			ushort							firstIndex;
			Array<SamplerName::Optimized_t>	samplers;
		)

		DECL_CMD( BindTexelBuffers,
			UniformName::Optimized_t		unName;
			ushort							firstIndex;
			Array<RmBufferViewID>			views;
		)

		DECL_CMD( BindRayTracingScenes,
			UniformName::Optimized_t		unName;
			ushort							firstIndex;
			Array<RmRTSceneID>				scenes;
		)
		#undef DECL_CMD


	// variables
		uint						count;


	// methods
		DescUpd_Flush () __NE___ {}
		bool	Serialize (Serializer &)		C_NE_OV;
		bool	Deserialize (Deserializer &)	__NE_OV;
		TypeId	GetTypeId ()					C_NE_OV	{ return TypeIdOf<DescUpd_Flush>(); }
	};

//-----------------------------------------------------------------------------


	DECL_MSG( SBM_GetBufferRanges,
		Bytes						reqSize;
		Bytes						blockSize;
		Bytes						memOffsetAlign;
		FrameUID					frameId;
		EStagingHeapType			heap;
		bool						upload;
	)

	DECL_RESP( SBM_GetBufferRanges_Response,
		struct Result final : ISerializable {
			RmBufferID		buffer;
			Bytes32u		bufferOffset;
			Bytes32u		size;
			RmDevicePtr		mapped		= Default;

			Result () __NE___ {}
			bool  Serialize (Serializer &)		C_NE_OV;
			bool  Deserialize (Deserializer &)	__NE_OV;
		};
		FixedArray< Result, BufferMemView::Count >		ranges;
	)

	DECL_MSG( SBM_GetImageRanges,
		UploadImageDesc				uploadDesc;
		ImageDesc					imageDesc;
		uint3						imageGranularity;
		FrameUID					frameId;
		bool						upload;
	)

	DECL_MSG( SBM_GetImageRanges2,
		UploadImageDesc				uploadDesc;
		VideoImageDesc				videoDesc;
		uint3						imageGranularity;
		FrameUID					frameId;
		bool						upload;
	)

	DECL_RESP( SBM_GetImageRanges_Response,
		struct Result final : ISerializable {
			RmBufferID		buffer;
			Bytes32u		bufferOffset;
			Bytes32u		size;
			RmDevicePtr		mapped		= Default;
			uint3			imageOffset;
			uint3			imageDim;
			Bytes			bufferSlicePitch;

			Result () __NE___ {}
			bool  Serialize (Serializer &)		C_NE_OV;
			bool  Deserialize (Deserializer &)	__NE_OV;
		};
		FixedArray< Result, ImageMemView::Count >	ranges;
		uint										bufferRowLength;
		uint										planeScaleY;
		EPixelFormat								format;
		Bytes										dataRowPitch;
		Bytes										dataSlicePitch;
	)

	DECL_MSG( SBM_AllocVStream,
		FrameUID					frameId;
		Bytes						size;
	)

	DECL_RESP( SBM_AllocVStream_Response,
		RmDevicePtr					mapped		= Default;
		RmBufferID					handle;
		Bytes						offset;
		Bytes						size;
	)

	DECL_MSG( SBM_GetFrameStat,
		FrameUID					frameId;
	)

	DECL_RESP( SBM_GetFrameStat_Response,
		IResourceManager::StagingBufferStat	stat;
	)
//-----------------------------------------------------------------------------


	struct CmdBuf_Bake final : BaseMsg
	{
	// types
		#ifdef AE_ENABLE_REMOTE_GRAPHICS
			struct BaseCmd : public ISerializable
			{};

			#define DECL_CMD( _name_, ... )						\
			struct _name_ final : public BaseCmd				\
			{													\
				__VA_ARGS__										\
																\
				_name_ () __NE___ {}							\
				bool  Serialize (Serializer &)		C_NE_OV;	\
				bool  Deserialize (Deserializer &)	__NE_OV;	\
			};
		#else
			struct BaseCmd : public ISerializable
			{
				virtual void  Execute (void *)	__Th___ = 0;
			};

			#define DECL_CMD( _name_, ... )						\
			struct _name_ final : public BaseCmd				\
			{													\
				__VA_ARGS__										\
																\
				_name_ () __NE___ {}							\
				bool  Serialize (Serializer &)		C_NE_OV;	\
				bool  Deserialize (Deserializer &)	__NE_OV;	\
				void  Execute (void *)				__Th_OV;	\
			};
		#endif
		//-------------------------------------------------

		DECL_CMD( BeginGraphics,
			DebugLabel		dbgLabel = Default;

			explicit BeginGraphics (DebugLabel dbg) __NE___ : dbgLabel{dbg} {}
		)

		DECL_CMD( BeginCompute,
			DebugLabel		dbgLabel = Default;

			explicit BeginCompute (DebugLabel dbg) __NE___ : dbgLabel{dbg} {}
		)

		DECL_CMD( BeginTransfer,
			DebugLabel		dbgLabel = Default;

			explicit BeginTransfer (DebugLabel dbg) __NE___ : dbgLabel{dbg} {}
		)

		DECL_CMD( BeginASBuild,
			DebugLabel		dbgLabel = Default;

			explicit BeginASBuild (DebugLabel dbg) __NE___ : dbgLabel{dbg} {}
		)

		DECL_CMD( BeginRayTracing,
			DebugLabel		dbgLabel = Default;

			explicit BeginRayTracing (DebugLabel dbg) __NE___ : dbgLabel{dbg} {}
		)

		DECL_CMD( DebugMarkerCmd,
			DebugLabel		dbgLabel = Default;

			explicit DebugMarkerCmd (DebugLabel dbg) __NE___ : dbgLabel{dbg} {}
		)

		DECL_CMD( PushDebugGroupCmd,
			DebugLabel		dbgLabel = Default;

			explicit PushDebugGroupCmd (DebugLabel dbg) __NE___ : dbgLabel{dbg} {}
		)

		DECL_CMD( PopDebugGroupCmd, )

		DECL_CMD( WriteTimestampCmd,
			using SerQuery = Query_Alloc_Response::SerQuery;

			SerQuery				query;
			uint					index;
			EPipelineScope			scope;
		)

		//-------------------------------------------------
		// pipeline barriers

		DECL_CMD( BufferBarrierCmd,
			RmBufferID				buffer;
			EResourceState			srcState;
			EResourceState			dstState;
		)

		DECL_CMD( BufferViewBarrierCmd,
			RmBufferViewID			bufferView;
			EResourceState			srcState;
			EResourceState			dstState;
		)

		DECL_CMD( ImageBarrierCmd,
			RmImageID				image;
			EResourceState			srcState;
			EResourceState			dstState;
		)

		DECL_CMD( ImageRangeBarrierCmd,
			RmImageID				image;
			EResourceState			srcState;
			EResourceState			dstState;
			ImageSubresourceRange	subRes;
		)

		DECL_CMD( ImageViewBarrierCmd,
			RmImageViewID			imageView;
			EResourceState			srcState;
			EResourceState			dstState;
		)

		DECL_CMD( MemoryBarrierCmd,
			EResourceState			srcState;
			EResourceState			dstState;
		)

		DECL_CMD( MemoryBarrier2Cmd,
			EPipelineScope			srcScope;
			EPipelineScope			dstScope;
		)

		DECL_CMD( MemoryBarrier3Cmd, )

		DECL_CMD( ExecutionBarrierCmd,
			EPipelineScope			srcScope;
			EPipelineScope			dstScope;
		)

		DECL_CMD( ExecutionBarrier2Cmd, )

		DECL_CMD( AcquireBufferOwnershipCmd,
			RmBufferID				buffer;
			EQueueType				srcQueue;
			EResourceState			srcState;
			EResourceState			dstState;
		)

		DECL_CMD( ReleaseBufferOwnershipCmd,
			RmBufferID				buffer;
			EResourceState			srcState;
			EResourceState			dstState;
			EQueueType				dstQueue;
		)

		DECL_CMD( AcquireImageOwnershipCmd,
			RmImageID				image;
			EQueueType				srcQueue;
			EResourceState			srcState;
			EResourceState			dstState;
		)

		DECL_CMD( ReleaseImageOwnershipCmd,
			RmImageID				image;
			EResourceState			srcState;
			EResourceState			dstState;
			EQueueType				dstQueue;
		)

		DECL_CMD( CommitBarriersCmd, )

		//-------------------------------------------------
		// transfer commands

		DECL_CMD( ClearColorImageCmd,
			RmImageID							image;
			Union< RGBA32f, RGBA32i, RGBA32u >	color;
			ArrayView<ImageSubresourceRange>	ranges;
		)

		DECL_CMD( ClearDepthStencilImageCmd,
			RmImageID							image;
			DepthStencil						depthStencil;
			ArrayView<ImageSubresourceRange>	ranges;
		)

		DECL_CMD( UpdateBufferCmd,
			RmBufferID							buffer;
			Bytes								offset;
			Bytes								size;
			RmDeviceOffset						data;
		)

		DECL_CMD( FillBufferCmd,
			RmBufferID							buffer;
			Bytes								offset;
			Bytes								size;
			uint								data;
		)

		DECL_CMD( CopyBufferCmd,
			RmBufferID							srcBuffer;
			RmBufferID							dstBuffer;
			ArrayView<BufferCopy>				ranges;
		)

		DECL_CMD( CopyImageCmd,
			RmImageID							srcImage;
			RmImageID							dstImage;
			ArrayView<ImageCopy>				ranges;
		)

		DECL_CMD( CopyBufferToImageCmd,
			RmBufferID							srcBuffer;
			RmImageID							dstImage;
			ArrayView<BufferImageCopy>			ranges;
		)

		DECL_CMD( CopyImageToBufferCmd,
			RmImageID							srcImage;
			RmBufferID							dstBuffer;
			ArrayView<BufferImageCopy>			ranges;
		)

		DECL_CMD( CopyBufferToImage2Cmd,
			RmBufferID							srcBuffer;
			RmImageID							dstImage;
			ArrayView<BufferImageCopy2>			ranges;
		)

		DECL_CMD( CopyImageToBuffer2Cmd,
			RmImageID							srcImage;
			RmBufferID							dstBuffer;
			ArrayView<BufferImageCopy2>			ranges;
		)

		DECL_CMD( BlitImageCmd,
			RmImageID							srcImage;
			RmImageID							dstImage;
			EBlitFilter							filter;
			ArrayView<ImageBlit>				regions;
		)

		DECL_CMD( ResolveImageCmd,
			RmImageID							srcImage;
			RmImageID							dstImage;
			ArrayView<ImageResolve>				regions;
		)

		DECL_CMD( GenerateMipmapsCmd,
			RmImageID							image;
			EResourceState						state;
			ArrayView<ImageSubresourceRange>	ranges;
		)

		DECL_CMD( UpdateHostBufferCmd,
			RmBufferID							buffer;
			Bytes								offset;
			Bytes								size;
			RmDeviceOffset						memOffset;
		)

		DECL_CMD( ReadHostBufferCmd,
			RmBufferID							buffer;
			Bytes								offset;
			Bytes								size;
			RmHostPtr							hostPtr;
		)

		//-------------------------------------------------
		// compute commands

		DECL_CMD( Compute_BindPipelineCmd,
			RmComputePipelineID					ppln;
		)

		DECL_CMD( Compute_BindDescriptorSetCmd,
			DescSetBinding						index;
			RmDescriptorSetID					ds;
			ArrayView<uint>						dynamicOffsets;
		)

		DECL_CMD( Compute_PushConstantCmd,
			ushort								pcIndex_offset;
			EShader								pcIndex_stage;
			ArrayView<ubyte>					data;
			ShaderStructName::Optimized_t		typeName;
		)

		DECL_CMD( Compute_Dispatch,
			uint3								groupCount;
		)

		DECL_CMD( Compute_DispatchIndirect,
			RmBufferID							buffer;
			Bytes								offset;
		)

		//-------------------------------------------------
		// graphics commands

		DECL_CMD( Graphics_BeginRenderPass,
			using ClearValue_t	= RenderPassDesc::ClearValue_t;

			struct SerAttachment final : public ISerializable {
				AttachmentName::Optimized_t		name;
				RmImageViewID					imageView;
				ClearValue_t					clearValue;
				EResourceState					initial					= Default;
				EResourceState					final					= Default;
				bool							relaxedStateTransition	= true;

				bool  Serialize (Serializer &)		C_NE_OV;
				bool  Deserialize (Deserializer &)	__NE_OV;
			};

			struct SerRenderPassDesc final : public ISerializable {
				Array<SerAttachment>			attachments;
				ArrayView<Viewport>				viewports;
				RectI							area;
				ImageLayer						layerCount		= 1_layer;
				RenderPassName::Optimized_t		renderPassName;
				SubpassName::Optimized_t		subpassName;		// optional
				RmPipelinePackID				packId;

				bool  Serialize (Serializer &)		C_NE_OV;
				bool  Deserialize (Deserializer &)	__NE_OV;
			};

			SerRenderPassDesc					desc;
			DebugLabel							dbgLabel		= Default;
		)

		DECL_CMD( Graphics_NextSubpass,
			DebugLabel							dbgLabel		= Default;
		)

		DECL_CMD( Graphics_EndRenderPass, )

		DECL_CMD( Graphics_BeginMtRenderPass,
			using SerRenderPassDesc	= Graphics_BeginRenderPass::SerRenderPassDesc;

			RmDrawCommandBatchID				batchId;
			SerRenderPassDesc					desc;
			DebugLabel							dbgLabel		= Default;
		)

		DECL_CMD( Graphics_NextMtSubpass,
			RmDrawCommandBatchID				prevBatchId;
			RmDrawCommandBatchID				batchId;
			DebugLabel							dbgLabel		= Default;
		)

		DECL_CMD( Graphics_EndMtRenderPass,
			RmDrawCommandBatchID				batchId;
		)

		DECL_CMD( Graphics_ExecuteSecondary,
			RmDrawCommandBatchID				batchId;
			ArrayView<RmDrawCommandBufferID>	cmdbufs;
		)

		//-------------------------------------------------
		// draw commands

		DECL_CMD( Draw_BindGraphicsPipelineCmd,
			RmGraphicsPipelineID				ppln;
		)

		DECL_CMD( Draw_BindMeshPipelineCmd,
			RmMeshPipelineID					ppln;
		)

		DECL_CMD( Draw_BindTilePipelineCmd,
			RmTilePipelineID					ppln;
		)

		DECL_CMD( Draw_BindDescriptorSetCmd,
			DescSetBinding						index;
			RmDescriptorSetID					ds;
			ArrayView<uint>						dynamicOffsets;
		)

		DECL_CMD( Draw_PushConstantCmd,
			ushort								pcIndex_offset;
			EShader								pcIndex_stage;
			ArrayView<ubyte>					data;
			ShaderStructName::Optimized_t		typeName;
		)

		DECL_CMD( Draw_SetViewportsCmd,
			ArrayView<Viewport>					viewports;
		)

		DECL_CMD( Draw_SetScissorsCmd,
			ArrayView<RectI>					scissors;
		)

		DECL_CMD( Draw_SetDepthBiasCmd,
			float								depthBiasConstantFactor;
			float								depthBiasClamp;
			float								depthBiasSlopeFactor;
		)

		DECL_CMD( Draw_SetStencilCompareMaskCmd,
			uint								frontCompareMask;
			uint								backCompareMask;
		)

		DECL_CMD( Draw_SetStencilWriteMaskCmd,
			uint								frontWriteMask;
			uint								backWriteMask;
		)

		DECL_CMD( Draw_SetStencilReferenceCmd,
			uint								frontReference;
			uint								backReference;
		)

		DECL_CMD( Draw_SetBlendConstantsCmd,
			RGBA32f								color;
		)

		DECL_CMD( Draw_SetDepthBoundsCmd,
			float								minDepthBounds;
			float								maxDepthBounds;
		)

		DECL_CMD( Draw_SetFragmentShadingRateCmd,
			EShadingRate						rate;
			EShadingRateCombinerOp				primitiveOp;
			EShadingRateCombinerOp				textureOp;
		)

		DECL_CMD( Draw_SetViewportWScalingCmd,
			ArrayView<packed_float2>			scaling;
		)

		DECL_CMD( Draw_BindIndexBufferCmd,
			RmBufferID							buffer;
			Bytes								offset;
			EIndex								indexType;
		)

		DECL_CMD( Draw_BindVertexBuffersCmd,
			uint								firstBinding;
			ArrayView<RmBufferID>				buffers;
			ArrayView<Bytes>					offsets;
		)

		DECL_CMD( DrawCmd,
			uint								vertexCount;
			uint								instanceCount;
			uint								firstVertex;
			uint								firstInstance;
		)

		DECL_CMD( DrawIndexedCmd,
			uint								indexCount;
			uint								instanceCount;
			uint								firstIndex;
			int									vertexOffset;
			uint								firstInstance;
		)

		DECL_CMD( DrawIndirectCmd,
			RmBufferID							indirectBuffer;
			Bytes								indirectBufferOffset;
			uint								drawCount;
			Bytes								stride;
		)

		DECL_CMD( DrawIndexedIndirectCmd,
			RmBufferID							indirectBuffer;
			Bytes								indirectBufferOffset;
			uint								drawCount;
			Bytes								stride;
		)

		DECL_CMD( DrawMeshTasksCmd,
			uint3								taskCount;
		)

		DECL_CMD( DrawMeshTasksIndirectCmd,
			RmBufferID							indirectBuffer;
			Bytes								indirectBufferOffset;
			uint								drawCount;
			Bytes								stride;
		)

		DECL_CMD( DrawIndirectCountCmd,
			RmBufferID							indirectBuffer;
			Bytes								indirectBufferOffset;
			RmBufferID							countBuffer;
			Bytes								countBufferOffset;
			uint								maxDrawCount;
			Bytes								stride;
		)

		DECL_CMD( DrawIndexedIndirectCountCmd,
			RmBufferID							indirectBuffer;
			Bytes								indirectBufferOffset;
			RmBufferID							countBuffer;
			Bytes								countBufferOffset;
			uint								maxDrawCount;
			Bytes								stride;
		)

		DECL_CMD( DrawMeshTasksIndirectCountCmd,
			RmBufferID							indirectBuffer;
			Bytes								indirectBufferOffset;
			RmBufferID							countBuffer;
			Bytes								countBufferOffset;
			uint								maxDrawCount;
			Bytes								stride;
		)

		DECL_CMD( DispatchTileCmd,
		)

		DECL_CMD( Draw_AttachmentBarrierCmd,
			AttachmentName::Optimized_t			name;
			EResourceState						srcState;
			EResourceState						dstState;
		)

		DECL_CMD( Draw_CommitBarriersCmd, )

		DECL_CMD( Draw_ClearAttachmentCmd,
			using ClearValue_t = RenderPassDesc::ClearValue_t;

			AttachmentName::Optimized_t			name;
			ClearValue_t						clearValue;
			RectI								region;
			ImageLayer							baseLayer;
			uint								layerCount;
		)

		//-------------------------------------------------
		// acceleration structure build commands

		DECL_CMD( ASBuild_BuildGeometryCmd,
			RTGeometryBuild		build;
			RmRTGeometryID		dstId;
		)

		DECL_CMD( ASBuild_BuildSceneCmd,
			RTSceneBuild		build;
			RmRTSceneID			dstId;
		)

		DECL_CMD( ASBuild_UpdateGeometryCmd,
			RTGeometryBuild		build;
			RmRTGeometryID		srcId;
			RmRTGeometryID		dstId;
		)

		DECL_CMD( ASBuild_UpdateSceneCmd,
			RTSceneBuild		build;
			RmRTSceneID			srcId;
			RmRTSceneID			dstId;
		)

		DECL_CMD( ASBuild_CopyGeometryCmd,
			RmRTGeometryID		srcId;
			RmRTGeometryID		dstId;
			ERTASCopyMode		mode;
		)

		DECL_CMD( ASBuild_CopySceneCmd,
			RmRTSceneID			srcId;
			RmRTSceneID			dstId;
			ERTASCopyMode		mode;
		)

		DECL_CMD( ASBuild_WriteGeometryPropertyCmd,
			ERTASProperty		property;
			RmRTGeometryID		as;
			RmBufferID			dstBuffer;
			Bytes				offset;
			Bytes				size;
		)

		DECL_CMD( ASBuild_WriteScenePropertyCmd,
			ERTASProperty		property;
			RmRTSceneID			as;
			RmBufferID			dstBuffer;
			Bytes				offset;
			Bytes				size;
		)

		//-------------------------------------------------
		// ray tracing commands

		DECL_CMD( RayTracing_BindPipelineCmd,
			RmRayTracingPipelineID				ppln;
		)

		DECL_CMD( RayTracing_BindDescriptorSetCmd,
			DescSetBinding						index;
			RmDescriptorSetID					ds;
			ArrayView<uint>						dynamicOffsets;
		)

		DECL_CMD( RayTracing_PushConstantCmd,
			ushort								pcIndex_offset;
			EShader								pcIndex_stage;
			ArrayView<ubyte>					data;
			ShaderStructName::Optimized_t		typeName;
		)

		DECL_CMD( RayTracing_SetStackSizeCmd,
			Bytes								size;
		)

		DECL_CMD( RayTracing_TraceRaysCmd,
			uint3								dim;
			RmRTShaderBindingID					sbt;
		)

		DECL_CMD( RayTracing_TraceRaysIndirectCmd,
			RmRTShaderBindingID					sbt;
			RmBufferID							indirectBuffer;
			Bytes								indirectOffset;
		)

		DECL_CMD( RayTracing_TraceRaysIndirect2Cmd,
			RmBufferID							indirectBuffer;
			Bytes								indirectOffset;
		)
		#undef DECL_CMD
		//-------------------------------------------------

		using HostToDevCopy_t = Array< Tuple< RmDevicePtr, RmDeviceOffset, Bytes >>;
		using DevToHostCopy_t = Array< Tuple< RmDevicePtr, RmHostPtr, Bytes >>;


	// variables
		RmCommandBatchID		batchId;
		uint					count;
		uint					exeIndex;
		Bytes					blockSize;
		HostToDevCopy_t			hostToDev;
		DevToHostCopy_t			devToHost;

	// methods
		CmdBuf_Bake () __NE___ {}
		bool	Serialize (Serializer &)		C_NE_OV;
		bool	Deserialize (Deserializer &)	__NE_OV;
		TypeId	GetTypeId ()					C_NE_OV	{ return TypeIdOf<CmdBuf_Bake>(); }
	};

	DECL_RESP( CmdBuf_Bake_Response,
		RmCommandBufferID		cmdbuf;
	)


	struct CmdBuf_BakeDraw final : BaseMsg
	{
	// types
		using BaseCmd			= CmdBuf_Bake::BaseCmd;
		using HostToDevCopy_t	= CmdBuf_Bake::HostToDevCopy_t;

	// variables
		RmDrawCommandBatchID	batchId;
		uint					count;
		uint					exeIndex;		// draw index
		Bytes					blockSize;
		HostToDevCopy_t			hostToDev;

	// methods
		CmdBuf_BakeDraw () __NE___ {}
		bool	Serialize (Serializer &)		C_NE_OV;
		bool	Deserialize (Deserializer &)	__NE_OV;
		TypeId	GetTypeId ()					C_NE_OV	{ return TypeIdOf<CmdBuf_BakeDraw>(); }
	};

	DECL_RESP( CmdBuf_BakeDraw_Response,
		RmDrawCommandBufferID	cmdbuf;
	)


} // AE::RemoteGraphics::Msg


#undef DECL_MSG
#undef DECL_RESP
