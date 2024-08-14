// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Remote/RMessages.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VQueryManager.h"
#endif

namespace AE::Base
{
	template <> struct TTriviallySerializable< Graphics::ImageSubresourceRange	> : CT_True {};
	template <> struct TTriviallySerializable< Graphics::ImageSubresourceLayers	> : CT_True {};
	template <> struct TTriviallySerializable< Graphics::BufferCopy				> : CT_True {};
	template <> struct TTriviallySerializable< Graphics::ImageCopy				> : CT_True {};
	template <> struct TTriviallySerializable< Graphics::BufferImageCopy		> : CT_True {};
	template <> struct TTriviallySerializable< Graphics::BufferImageCopy2		> : CT_True {};
	template <> struct TTriviallySerializable< Graphics::ImageBlit				> : CT_True {};
	template <> struct TTriviallySerializable< Graphics::ImageResolve			> : CT_True {};

	template <> struct TTriviallyDestructible< RemoteGraphics::Msg::CmdBuf_Bake::Graphics_BeginRenderPass::SerAttachment > : CT_True {};
}

namespace AE::RemoteGraphics::Msg
{
	StaticAssert( sizeof(ImageSubresourceRange) == 10 );
	StaticAssert( sizeof(ImageSubresourceLayers) == 8 );
	StaticAssert( sizeof(BufferCopy) == 24 );
	StaticAssert( sizeof(ImageCopy) == 80 );
	StaticAssert( sizeof(BufferImageCopy) == 64 );
	StaticAssert( sizeof(BufferImageCopy2) == 64 );
	StaticAssert( sizeof(ImageBlit) == 96 );
	StaticAssert( sizeof(ImageResolve) == 80 );

	StaticAssert( sizeof(Viewport) == 24 );

	StaticAssert64( sizeof(SwapchainDesc) == 12 );
	#define Ser_SwapchainDesc( _desc_ )\
		_desc_.colorFormat, _desc_.colorSpace, _desc_.presentMode, _desc_.minImageCount, _desc_.usage, _desc_.options

  #ifdef AE_ENABLE_REMOTE_GRAPHICS
	StaticAssert64( sizeof(GraphicsCreateInfo) == 168 );
  #else
	StaticAssert64( sizeof(GraphicsCreateInfo) == 136 );
  #endif
	#define Ser_GraphicsCreateInfo( _desc_ )\
		_desc_.maxFrames, \
		_desc_.staging.writeStaticSize, _desc_.staging.readStaticSize, \
		_desc_.staging.maxWriteDynamicSize, _desc_.staging.maxReadDynamicSize, \
		_desc_.staging.dynamicBlockSize, _desc_.staging.maxFramesToRelease, _desc_.staging.vstreamSize, \
		_desc_.device.appName, _desc_.device.deviceName, \
		_desc_.device.requiredQueues, _desc_.device.optionalQueues, \
		_desc_.device.validation, _desc_.device.devFlags, \
		Ser_SwapchainDesc( _desc_.swapchain )


	DECL_SERIALIZER( DefaultResponse,		ok )
	DECL_SERIALIZER( UploadData,			size )			// skip 'data'
	DECL_SERIALIZER( UploadDataAndCopy,		size, dst )		// skip 'data'
	DECL_SERIALIZER( Log,					message, func, file, line, level, scope )

	DECL_SERIALIZER( Device_Init,			Ser_GraphicsCreateInfo( info ), initSyncLog )
	DECL_SERIALIZER( Device_Init_Response,
		engineVersion, name, fs, props,
		resFlags.bufferUsage, resFlags.bufferOptions, resFlags.imageUsage,
		resFlags.imageOptions, resFlags.descrTypes, resFlags.memTypes,
		queues, memInfo, features, api, cpuArch, os, adapterType,
		checkConstantLimitsOK, checkExtensionsOK, initialized, underDebugger
	)

	DECL_EMPTY_SERIALIZER( Device_DestroyLogicalDevice	)
	DECL_EMPTY_SERIALIZER( Device_DestroyInstance		)

	DECL_SERIALIZER( Device_EnableSyncLog,				enable )
	DECL_EMPTY_SERIALIZER( Device_GetSyncLog			)
	DECL_SERIALIZER( Device_GetSyncLog_Response,		log )
//-----------------------------------------------------------------------------



	DECL_EMPTY_SERIALIZER( Surface_Create					)
	DECL_SERIALIZER( Surface_Create_Response,				created, formats, presentModes )
	DECL_EMPTY_SERIALIZER( Surface_Destroy					)
	DECL_SERIALIZER( Swapchain_Create,						viewSize, Ser_SwapchainDesc( desc ), dbgName )
	DECL_SERIALIZER( Swapchain_Recreate,					viewSize, Ser_SwapchainDesc( desc ))
	DECL_SERIALIZER( Swapchain_Create_Response,				created, minimized, viewSize, Ser_SwapchainDesc( desc ), imageAvailable, renderFinished, imageIds, viewIds )
	DECL_EMPTY_SERIALIZER( Swapchain_Destroy				)
	DECL_SERIALIZER( Swapchain_IsSupported,					presentMode, colorFormat, colorImageUsage )

	DECL_EMPTY_SERIALIZER( Swapchain_AcquireNextImage		)
	DECL_SERIALIZER( Swapchain_AcquireNextImage_Response,	result, semaphoreId, imageIdx )

	DECL_SERIALIZER( Swapchain_Present,						submitIdx, presentQueue, semaphoreId, imageIdx )
	DECL_SERIALIZER( Swapchain_Present_Response,			result )
//-----------------------------------------------------------------------------


	#define Ser_DebugLabel( _desc_ )\
		_desc_.label, _desc_.color


	DECL_SERIALIZER( RTS_Initialize,				Ser_GraphicsCreateInfo( info ))
	DECL_EMPTY_SERIALIZER( RTS_Deinitialize			)

	DECL_SERIALIZER( RTS_BeginFrame,				frameId, cfg.stagingBufferPerFrameLimits.write, cfg.stagingBufferPerFrameLimits.read )
	DECL_SERIALIZER( RTS_EndFrame,					frameId, submitIdx )
	DECL_SERIALIZER( RTS_WaitNextFrame,				frameId, timeout )
	DECL_SERIALIZER( RTS_WaitNextFrame_Response,	ok, memUsage )
	DECL_SERIALIZER( RTS_WaitAll,					frameId, timeout )
	DECL_SERIALIZER( RTS_SkipCmdBatches,			queue, bits )

	DECL_SERIALIZER( RTS_CreateBatch,				queue, flags, frameId, submitIdx, Ser_DebugLabel( dbgLabel ))
	DECL_SERIALIZER( RTS_CreateBatch_Response,		batchId, semaphoreId )

	DECL_SERIALIZER( RTS_SubmitBatch,				id, submitIdx, cmdbufs, inputDeps, outputDeps )
	DECL_SERIALIZER( RTS_DestroyBatch,				batchId, semaphoreId )
	DECL_SERIALIZER( RTS_WaitBatch,					id, timeout )

	DECL_EMPTY_SERIALIZER( RTS_CreateDrawBatch		)
	DECL_SERIALIZER( RTS_CreateDrawBatch_Response,	id )
//-----------------------------------------------------------------------------


	StaticAssert64( sizeof(ImageDesc) == 48 );
	#define Ser_ImageDesc( _desc_ )\
		_desc_.dimension, _desc_.arrayLayers, _desc_.mipLevels, _desc_.imageDim, _desc_.options, \
		_desc_.usage, _desc_.format, _desc_.samples, _desc_.memType, _desc_.queues, _desc_.viewFormats

	StaticAssert64( sizeof(ImageViewDesc) == 20 );
	#define Ser_ImageViewDesc( _desc_ )\
		_desc_.viewType, _desc_.format, _desc_.aspectMask, _desc_.extUsage, \
		_desc_.baseMipmap, _desc_.mipmapCount, _desc_.baseLayer, _desc_.layerCount, _desc_.swizzle

	StaticAssert64( sizeof(BufferDesc) == 24 );
	#define Ser_BufferDesc( _desc_ )\
		_desc_.size, _desc_.usage, _desc_.options, _desc_.queues, _desc_.memType

	StaticAssert64( sizeof(BufferViewDesc) == 24 );
	#define Ser_BufferViewDesc( _desc_ )\
		_desc_.offset, _desc_.size, _desc_.format

	StaticAssert64( sizeof(RTGeometryDesc) == 16 );
	#define Ser_RTGeometryDesc( _desc_ )\
		_desc_.size, _desc_.options

	StaticAssert64( sizeof(RTSceneDesc) == 16 );
	#define Ser_RTSceneDesc( _desc_ )\
		_desc_.size, _desc_.options

	StaticAssert64( sizeof(RTGeometryBuild::ScratchBuffer) == 16 );
	#define Ser_RTScratchBuffer( _desc_ )\
		_desc_.id, _desc_.offset

	StaticAssert64( sizeof(RTSceneBuild::InstanceBuffer) == 24 );
	#define Ser_RTInstanceBuffer( _desc_ )\
		_desc_.id, _desc_.offset, _desc_.stride

	StaticAssert64( sizeof(RTGeometryBuild) == 72 );
	#define Ser_RTGeometryBuild( _desc_ )\
		_desc_.triangles, _desc_.aabbs, _desc_.options, Ser_RTScratchBuffer( _desc_.scratch )

	StaticAssert64( sizeof(RTSceneBuild) == 376 );
	#define Ser_RTSceneBuild( _desc_ )\
		_desc_.maxInstanceCount, _desc_.options, Ser_RTScratchBuffer( _desc_.scratch ), Ser_RTInstanceBuffer( _desc_.instanceData ), _desc_.uniqueGeoms

	StaticAssert64( sizeof(BasePipelineDesc) == 96 );
	#define Ser_BasePipelineDesc( _desc_ )\
		_desc_.specialization, _desc_.options, _desc_.dynamicState, _desc_.dbgName

	StaticAssert64( sizeof(GraphicsPipelineDesc) == 152 );
	#define Ser_GraphicsPipelineDesc( _desc_ )\
		Ser_BasePipelineDesc( _desc_ ), _desc_.renderStatePtr, _desc_.renderPass, \
		_desc_.subpass, _desc_.vertexInput, _desc_.vertexBuffers, _desc_.viewportCount

	StaticAssert64( sizeof(MeshPipelineDesc) == 128 );
	#define Ser_MeshPipelineDesc( _desc_ )\
		Ser_BasePipelineDesc( _desc_ ), _desc_.renderStatePtr, _desc_.renderPass, \
		_desc_.subpass, _desc_.viewportCount, _desc_.taskLocalSize, _desc_.meshLocalSize

	StaticAssert64( sizeof(ComputePipelineDesc) == 104 );
	#define Ser_ComputePipelineDesc( _desc_ )\
		Ser_BasePipelineDesc( _desc_ ), _desc_.localSize

	StaticAssert64( sizeof(RayTracingPipelineDesc) == 112 );
	#define Ser_RayTracingPipelineDesc( _desc_ )\
		Ser_BasePipelineDesc( _desc_ ), _desc_.maxRecursionDepth, _desc_.maxPipelineRayPayloadSize, _desc_.maxPipelineRayHitAttributeSize

	StaticAssert64( sizeof(TilePipelineDesc) == 112 );
	#define Ser_TilePipelineDesc( _desc_ )\
		Ser_BasePipelineDesc( _desc_ ), _desc_.renderPass, _desc_.subpass, _desc_.localSize

	//StaticAssert64( sizeof(VideoProfile) == 8 );
	#define Ser_VideoProfile( _desc_ )\
		_desc_.mode, _desc_.codec, _desc_.chromaSubsampling, _desc_.lumaBitDepth, _desc_.chromaBitDepth, _desc_.spec

	//StaticAssert64( sizeof(VideoSessionDesc) == 40 );
	#define Ser_VideoSessionDesc( _desc_ )\
		_desc_.queue, _desc_.pictureFormat, Ser_VideoProfile( _desc_.profile ), _desc_.memType, \
		_desc_.maxCodedExtent, _desc_.referencePictureFormat, _desc_.maxDpbSlots, _desc_.maxActiveReferencePictures

	//StaticAssert64( sizeof(VideoBufferDesc) == 32 );
	#define Ser_VideoBufferDesc( _desc_ )\
		_desc_.size, _desc_.usage, _desc_.options, _desc_.videoUsage, _desc_.memType, _desc_.queues, Ser_VideoProfile( _desc_.profile )

	//StaticAssert64( sizeof(VideoImageDesc) == 112 );
	#define Ser_VideoImageDesc( _desc_ )\
		_desc_.dimension, _desc_.arrayLayers, _desc_.format, _desc_.options, _desc_.usage, _desc_.videoUsage, \
		_desc_.memType, _desc_.queues, _desc_.ycbcrConversion, _desc_.ycbcrConvPack, Ser_VideoProfile( _desc_.profile )

	StaticAssert64( sizeof(PipelinePackDesc) == 40 );
	#define Ser_PipelinePackDesc( _desc_ )\
		_desc_.parentPackId, _desc_.options, _desc_.surfaceFormat, _desc_.dbgName


	DECL_SERIALIZER( ResMngr_CreateImage,					Ser_ImageDesc( desc ), gfxAlloc, dbgName )
	DECL_SERIALIZER( ResMngr_CreateImage_Response,			imageId, memoryId, Ser_ImageDesc( desc ))

	DECL_SERIALIZER( ResMngr_CreateImageView,				Ser_ImageViewDesc( desc ), imageId, dbgName )
	DECL_SERIALIZER( ResMngr_CreateImageView_Response,		viewId, Ser_ImageViewDesc( desc ))

	DECL_SERIALIZER( ResMngr_CreateBuffer,					Ser_BufferDesc( desc ), gfxAlloc, dbgName )
	DECL_SERIALIZER( ResMngr_CreateBuffer_Response,			bufferId, memoryId, addr, Ser_BufferDesc( desc ), mapped )

	DECL_SERIALIZER( ResMngr_CreateBufferView,				Ser_BufferViewDesc( desc ), bufferId, dbgName )
	DECL_SERIALIZER( ResMngr_CreateBufferView_Response,		viewId, Ser_BufferViewDesc( desc ))

	DECL_SERIALIZER( ResMngr_CreateRTGeometry,				Ser_RTGeometryDesc( desc ), gfxAlloc, dbgName )
	DECL_SERIALIZER( ResMngr_CreateRTGeometry_Response,		geomId, memoryId, addr, Ser_RTGeometryDesc( desc ))

	DECL_SERIALIZER( ResMngr_CreateRTScene,					Ser_RTSceneDesc( desc ), gfxAlloc, dbgName )
	DECL_SERIALIZER( ResMngr_CreateRTScene_Response,		sceneId, memoryId, addr, Ser_RTSceneDesc( desc ))

	DECL_SERIALIZER( ResMngr_GetRTGeometrySizes,			Ser_RTGeometryBuild( desc ))
	DECL_SERIALIZER( ResMngr_GetRTGeometrySizes_Response,	sizes )

	DECL_SERIALIZER( ResMngr_GetRTSceneSizes,				Ser_RTSceneBuild( desc ))
	DECL_SERIALIZER( ResMngr_GetRTSceneSizes_Response,		sizes )

	DECL_SERIALIZER( ResMngr_IsSupported_BufferDesc,		Ser_BufferDesc( desc ))
	DECL_SERIALIZER( ResMngr_IsSupported_ImageDesc,			Ser_ImageDesc( desc ))
	DECL_SERIALIZER( ResMngr_IsSupported_VideoImageDesc,	Ser_VideoImageDesc( desc ))
	DECL_SERIALIZER( ResMngr_IsSupported_VideoBufferDesc,	Ser_VideoBufferDesc( desc ))
	DECL_SERIALIZER( ResMngr_IsSupported_VideoSessionDesc,	Ser_VideoSessionDesc( desc ))
	DECL_SERIALIZER( ResMngr_IsSupported_BufferViewDesc,	bufferId, Ser_BufferViewDesc( desc ))
	DECL_SERIALIZER( ResMngr_IsSupported_ImageViewDesc,		imageId, Ser_ImageViewDesc( desc ))
	DECL_SERIALIZER( ResMngr_IsSupported_RTGeometryDesc,	Ser_RTGeometryDesc( desc ))
	DECL_SERIALIZER( ResMngr_IsSupported_RTGeometryBuild,	Ser_RTGeometryBuild( desc ))
	DECL_SERIALIZER( ResMngr_IsSupported_RTSceneDesc,		Ser_RTSceneDesc( desc ))
	DECL_SERIALIZER( ResMngr_IsSupported_RTSceneBuild,		Ser_RTSceneBuild( desc ))
	DECL_SERIALIZER( ResMngr_IsSupported_Response,			supported )

	DECL_SERIALIZER( ResMngr_ReleaseResource,				id, index )
	DECL_SERIALIZER( ResMngr_ReleaseResource_Response,		index, released )

	DECL_SERIALIZER( ResMngr_CreateDescriptorSets2,			packId, count, dslName, dsAlloc, dbgName )
	DECL_SERIALIZER( ResMngr_CreateDescriptorSets3,			layoutId, count, dsAlloc, dbgName )
	DECL_SERIALIZER( ResMngr_CreateDescriptorSets_Response,	ds )

	DECL_SERIALIZER( ResMngr_GetRTechPipeline,				rtechId, name, index )

	DECL_SERIALIZER( ResMngr_CreateGraphicsPipeline,		packId, name, Ser_GraphicsPipelineDesc( desc ), cacheId )
	DECL_SERIALIZER( ResMngr_CreateGraphicsPipeline_Response, pplnId, dynamicState, options, topology, subpassIndex, vertexBuffers, pipelineLayoutId, shaderTrace )

	DECL_SERIALIZER( ResMngr_CreateMeshPipeline,			packId, name, Ser_MeshPipelineDesc( desc ), cacheId )
	DECL_SERIALIZER( ResMngr_CreateMeshPipeline_Response,	pplnId, dynamicState, options, meshLocalSize, taskLocalSize, subpassIndex, pipelineLayoutId, shaderTrace )

	DECL_SERIALIZER( ResMngr_CreateComputePipeline,			packId, name, Ser_ComputePipelineDesc( desc ), cacheId )
	DECL_SERIALIZER( ResMngr_CreateComputePipeline_Response, pplnId, dynamicState, options, localSize, pipelineLayoutId, shaderTrace )

	DECL_SERIALIZER( ResMngr_CreateRayTracingPipeline,		packId, name, Ser_RayTracingPipelineDesc( desc ), cacheId )
	DECL_SERIALIZER( ResMngr_CreateRayTracingPipeline_Response, pplnId, dynamicState, options, shaderGroupHandleSize, nameToHandle, groupHandles, pipelineLayoutId, shaderTrace )

	DECL_SERIALIZER( ResMngr_CreateTilePipeline,			packId, name, Ser_TilePipelineDesc( desc ), cacheId )
	DECL_SERIALIZER( ResMngr_CreateTilePipeline_Response,	pplnId, dynamicState, options, localSize, subpassIndex, pipelineLayoutId, shaderTrace )

	DECL_SERIALIZER( ResMngr_CreateVideoSession,			Ser_VideoSessionDesc( desc ), dbgName, gfxAlloc )
	DECL_SERIALIZER( ResMngr_CreateVideoSession_Response,	id, Ser_VideoSessionDesc( desc ))

	DECL_SERIALIZER( ResMngr_CreateVideoBuffer,				Ser_VideoBufferDesc( desc ), dbgName, gfxAlloc )
	DECL_SERIALIZER( ResMngr_CreateVideoBuffer_Response,	id, Ser_VideoBufferDesc( desc ))

	DECL_SERIALIZER( ResMngr_CreateVideoImage,				Ser_VideoImageDesc( desc ), dbgName, gfxAlloc )
	DECL_SERIALIZER( ResMngr_CreateVideoImage_Response,		id, Ser_VideoImageDesc( desc ))

	DECL_EMPTY_SERIALIZER( ResMngr_CreatePipelineCache		)
	DECL_SERIALIZER( ResMngr_CreatePipelineCache_Response,	id )

	DECL_SERIALIZER( ResMngr_InitializeResources,			packId )
	DECL_SERIALIZER( ResMngr_LoadPipelinePack,				Ser_PipelinePackDesc( desc ), sbtAllocatorId )
	DECL_SERIALIZER( ResMngr_LoadPipelinePack_Response,		packId, unsupportedFS, samplerRefs, dsLayouts, renderPasses, pplnLayouts )

	DECL_SERIALIZER( ResMngr_GetSupportedRenderTechs,		id )
	DECL_SERIALIZER( ResMngr_GetSupportedRenderTechs_Response,	result )

	StaticAssert64( sizeof(IRenderTechPipelines::PassInfo) == 24 );
	DECL_SERIALIZER( ResMngr_LoadRenderTech_Response::SerPass,	name, info.dsIndex, info.type, info.submitIdx, info.dsLayoutId, info.renderPass, info.subpass )
	DECL_SERIALIZER( ResMngr_LoadRenderTech,				packId, name, cacheId )
	DECL_SERIALIZER( ResMngr_LoadRenderTech_Response,		id, name, pipelines, rtSbtMap, passes )

	DECL_SERIALIZER( ResMngr_CreateLinearGfxMemAllocator,	pageSize )
	DECL_SERIALIZER( ResMngr_CreateBlockGfxMemAllocator,	blockSize, pageSize )
	DECL_SERIALIZER( ResMngr_CreateUnifiedGfxMemAllocator,	pageSize )
	DECL_SERIALIZER( ResMngr_CreateGfxMemAllocator_Response, id )

	DECL_SERIALIZER( ResMngr_GetRenderPass,					packId, name )
	DECL_SERIALIZER( ResMngr_GetRenderPass_Response,		subpassCount, pixFormats, attStates )

	DECL_EMPTY_SERIALIZER( ResMngr_ForceReleaseResources	)

	DECL_SERIALIZER( ResMngr_GetShaderGroupStackSize,			pplnId, names, type )
	DECL_SERIALIZER( ResMngr_GetShaderGroupStackSize_Response,	size )
//-----------------------------------------------------------------------------


	#ifdef AE_ENABLE_VULKAN
	StaticAssert( sizeof(VQueryManager::Query) == 16 );
	#endif
	#define Ser_Query( _desc_ )\
		_desc_.queryId, _desc_.first, _desc_.count, _desc_.numPasses, _desc_.type, _desc_.queue


	DECL_EMPTY_SERIALIZER( Query_Init							)
	DECL_SERIALIZER( Query_Init_Response,						calibratedTimestamps )
	DECL_SERIALIZER( Query_Alloc,								queueType, queryType, count )
	DECL_SERIALIZER( Query_Alloc_Response,						Ser_Query( query ))

	DECL_SERIALIZER( Query_GetTimestampUL,						Ser_Query( query ), size )
	DECL_SERIALIZER( Query_GetTimestampUL_Response,				result )
	DECL_SERIALIZER( Query_GetTimestampD,						Ser_Query( query ), size )
	DECL_SERIALIZER( Query_GetTimestampD_Response,				result )
	DECL_SERIALIZER( Query_GetTimestampNs,						Ser_Query( query ), size )
	DECL_SERIALIZER( Query_GetTimestampNs_Response,				result )

	DECL_SERIALIZER( Query_GetTimestampCalibratedUL,			Ser_Query( query ), size )
	DECL_SERIALIZER( Query_GetTimestampCalibratedUL_Response,	result, maxDeviation )
	DECL_SERIALIZER( Query_GetTimestampCalibratedD,				Ser_Query( query ), size )
	DECL_SERIALIZER( Query_GetTimestampCalibratedD_Response,	result, maxDeviation )
	DECL_SERIALIZER( Query_GetTimestampCalibratedNs,			Ser_Query( query ), size )
	DECL_SERIALIZER( Query_GetTimestampCalibratedNs_Response,	result, maxDeviation )

	StaticAssert( sizeof(IQueryManager::GraphicsPipelineStatistic) == 24 );
	DECL_SERIALIZER( Query_GetPipelineStatistic,				Ser_Query( query ), size )
	DECL_SERIALIZER( Query_GetPipelineStatistic_Response,		result )
//-----------------------------------------------------------------------------


	DECL_SERIALIZER( ProfArm_Initialize,				required )
	DECL_SERIALIZER( ProfArm_Initialize_Response,		ok, enabled )
	DECL_EMPTY_SERIALIZER( ProfArm_Sample				)
	DECL_SERIALIZER( ProfArm_Sample_Response,			counters )

	DECL_SERIALIZER( ProfMali_Initialize,				required )
	DECL_SERIALIZER( ProfMali_Initialize_Response,		ok, enabled, info )
	DECL_EMPTY_SERIALIZER( ProfMali_Sample				)
	DECL_SERIALIZER( ProfMali_Sample_Response,			counters )

	DECL_SERIALIZER( ProfAdreno_Initialize,				required )
	DECL_SERIALIZER( ProfAdreno_Initialize_Response,	ok, enabled, info )
	DECL_EMPTY_SERIALIZER( ProfAdreno_Sample			)
	DECL_SERIALIZER( ProfAdreno_Sample_Response,		counters )

	DECL_SERIALIZER( ProfPVR_Initialize,				required )
	DECL_SERIALIZER( ProfPVR_Initialize_Response,		ok, enabled )
	DECL_EMPTY_SERIALIZER( ProfPVR_Tick					)
	DECL_SERIALIZER( ProfPVR_Tick_Response,				timings )
	DECL_EMPTY_SERIALIZER( ProfPVR_Sample				)
	DECL_SERIALIZER( ProfPVR_Sample_Response,			counters )

	DECL_SERIALIZER( ProfNVidia_Initialize,				required )
	DECL_SERIALIZER( ProfNVidia_Initialize_Response,	ok, enabled )
	DECL_EMPTY_SERIALIZER( ProfNVidia_Sample			)
	DECL_SERIALIZER( ProfNVidia_Sample_Response,		counters )
//-----------------------------------------------------------------------------


	DECL_SERIALIZER( DescUpd_Flush,							count )
	DECL_SERIALIZER( DescUpd_Flush::SetDescSet,				descSet, mode )
	DECL_SERIALIZER( DescUpd_Flush::BindVideoImage,			unName, firstIndex, videoImageId )
	DECL_SERIALIZER( DescUpd_Flush::BindImages,				unName, firstIndex, images )
	DECL_SERIALIZER( DescUpd_Flush::BindBuffers,			unName, firstIndex, buffers )
	DECL_SERIALIZER( DescUpd_Flush::BindBufferRange,		unName, index, buffer, offset, size )
	DECL_SERIALIZER( DescUpd_Flush::BindTextures,			unName, firstIndex, images, sampler )
	DECL_SERIALIZER( DescUpd_Flush::BindSamplers,			unName, firstIndex, samplers )
	DECL_SERIALIZER( DescUpd_Flush::BindTexelBuffers,		unName, firstIndex, views )
	DECL_SERIALIZER( DescUpd_Flush::BindRayTracingScenes,	unName, firstIndex, scenes )
//-----------------------------------------------------------------------------


	StaticAssert( sizeof(UploadImageDesc) == 64 );
	#define Ser_UploadImageDesc( _desc_ )\
		_desc_.imageOffset, _desc_.imageDim, _desc_.arrayLayer, _desc_.mipLevel, _desc_.dataRowPitch, _desc_.dataSlicePitch, _desc_.aspectMask, _desc_.heapType

	StaticAssert( sizeof(IResourceManager::StagingBufferStat) == 32 );
	#define Ser_StagingBufferStat( _desc_ )\
		_desc_.dynamicWrite, _desc_.dynamicRead, _desc_.staticWrite, _desc_.staticRead


	DECL_SERIALIZER( SBM_GetBufferRanges,					reqSize, blockSize, memOffsetAlign, frameId, heap, upload )
	DECL_SERIALIZER( SBM_GetBufferRanges_Response,			ranges )
	DECL_SERIALIZER( SBM_GetBufferRanges_Response::Result,	buffer, bufferOffset, size, mapped )

	DECL_SERIALIZER( SBM_GetImageRanges,					Ser_UploadImageDesc( uploadDesc ), Ser_ImageDesc( imageDesc ), imageGranularity, frameId, upload )
	DECL_SERIALIZER( SBM_GetImageRanges2,					Ser_UploadImageDesc( uploadDesc ), Ser_VideoImageDesc( videoDesc ), imageGranularity, frameId, upload )
	DECL_SERIALIZER( SBM_GetImageRanges_Response,			ranges, bufferRowLength, planeScaleY, format, dataRowPitch, dataSlicePitch )
	DECL_SERIALIZER( SBM_GetImageRanges_Response::Result,	buffer, bufferOffset, size, mapped, imageOffset, imageDim, bufferSlicePitch )

	DECL_SERIALIZER( SBM_AllocVStream,						frameId, size )
	DECL_SERIALIZER( SBM_AllocVStream_Response,				mapped, handle, offset, size )

	DECL_SERIALIZER( SBM_GetFrameStat,						frameId )
	DECL_SERIALIZER( SBM_GetFrameStat_Response,				Ser_StagingBufferStat( stat ))
//-----------------------------------------------------------------------------


	DECL_SERIALIZER( CmdBuf_Bake,							batchId, count, exeIndex, blockSize, hostToDev, devToHost )
	DECL_SERIALIZER( CmdBuf_Bake_Response,					cmdbuf )

	DECL_SERIALIZER( CmdBuf_BakeDraw,						batchId, count, exeIndex, blockSize, hostToDev )
	DECL_SERIALIZER( CmdBuf_BakeDraw_Response,				cmdbuf )

	DECL_SERIALIZER( CmdBuf_Bake::BeginGraphics,			Ser_DebugLabel( dbgLabel ))
	DECL_SERIALIZER( CmdBuf_Bake::BeginCompute,				Ser_DebugLabel( dbgLabel ))
	DECL_SERIALIZER( CmdBuf_Bake::BeginTransfer,			Ser_DebugLabel( dbgLabel ))
	DECL_SERIALIZER( CmdBuf_Bake::BeginASBuild,				Ser_DebugLabel( dbgLabel ))
	DECL_SERIALIZER( CmdBuf_Bake::BeginRayTracing,			Ser_DebugLabel( dbgLabel ))

	DECL_SERIALIZER( CmdBuf_Bake::DebugMarkerCmd,			Ser_DebugLabel( dbgLabel ))
	DECL_SERIALIZER( CmdBuf_Bake::PushDebugGroupCmd,		Ser_DebugLabel( dbgLabel ))
	DECL_EMPTY_SERIALIZER( CmdBuf_Bake::PopDebugGroupCmd	)
	DECL_SERIALIZER( CmdBuf_Bake::WriteTimestampCmd,		Ser_Query( query ), index, scope )

	// pipeline barriers
	DECL_SERIALIZER( CmdBuf_Bake::BufferBarrierCmd,					buffer, srcState, dstState )
	DECL_SERIALIZER( CmdBuf_Bake::BufferViewBarrierCmd,				bufferView, srcState, dstState )
	DECL_SERIALIZER( CmdBuf_Bake::ImageBarrierCmd,					image, srcState, dstState )
	DECL_SERIALIZER( CmdBuf_Bake::ImageRangeBarrierCmd,				image, srcState, dstState, subRes )
	DECL_SERIALIZER( CmdBuf_Bake::ImageViewBarrierCmd,				imageView, srcState, dstState )
	DECL_SERIALIZER( CmdBuf_Bake::MemoryBarrierCmd,					srcState, dstState )
	DECL_SERIALIZER( CmdBuf_Bake::MemoryBarrier2Cmd,				srcScope, dstScope )
	DECL_EMPTY_SERIALIZER( CmdBuf_Bake::MemoryBarrier3Cmd			)
	DECL_SERIALIZER( CmdBuf_Bake::ExecutionBarrierCmd,				srcScope, dstScope )
	DECL_EMPTY_SERIALIZER( CmdBuf_Bake::ExecutionBarrier2Cmd		)
	DECL_SERIALIZER( CmdBuf_Bake::AcquireBufferOwnershipCmd,		buffer, srcQueue, srcState, dstState )
	DECL_SERIALIZER( CmdBuf_Bake::ReleaseBufferOwnershipCmd,		buffer, srcState, dstState, dstQueue )
	DECL_SERIALIZER( CmdBuf_Bake::AcquireImageOwnershipCmd,			image, srcQueue, srcState, dstState )
	DECL_SERIALIZER( CmdBuf_Bake::ReleaseImageOwnershipCmd,			image, srcState, dstState, dstQueue )
	DECL_EMPTY_SERIALIZER( CmdBuf_Bake::CommitBarriersCmd			)

	// transfer commands
	DECL_SERIALIZER( CmdBuf_Bake::ClearColorImageCmd,				image, color, ranges )
	DECL_SERIALIZER( CmdBuf_Bake::ClearDepthStencilImageCmd,		image, depthStencil, ranges )
	DECL_SERIALIZER( CmdBuf_Bake::UpdateBufferCmd,					buffer, offset, size, data )
	DECL_SERIALIZER( CmdBuf_Bake::FillBufferCmd,					buffer, offset, size, data )
	DECL_SERIALIZER( CmdBuf_Bake::CopyBufferCmd,					srcBuffer, dstBuffer, ranges )
	DECL_SERIALIZER( CmdBuf_Bake::CopyImageCmd,						srcImage, dstImage, ranges )
	DECL_SERIALIZER( CmdBuf_Bake::CopyBufferToImageCmd,				srcBuffer, dstImage, ranges )
	DECL_SERIALIZER( CmdBuf_Bake::CopyImageToBufferCmd,				srcImage, dstBuffer, ranges )
	DECL_SERIALIZER( CmdBuf_Bake::CopyBufferToImage2Cmd,			srcBuffer, dstImage, ranges )
	DECL_SERIALIZER( CmdBuf_Bake::CopyImageToBuffer2Cmd,			srcImage, dstBuffer, ranges )
	DECL_SERIALIZER( CmdBuf_Bake::BlitImageCmd,						srcImage, dstImage, filter, regions )
	DECL_SERIALIZER( CmdBuf_Bake::ResolveImageCmd,					srcImage, dstImage, regions )
	DECL_SERIALIZER( CmdBuf_Bake::GenerateMipmapsCmd,				image, state, ranges )
	DECL_SERIALIZER( CmdBuf_Bake::UpdateHostBufferCmd,				buffer, offset, size, memOffset )
	DECL_SERIALIZER( CmdBuf_Bake::ReadHostBufferCmd,				buffer, offset, size, hostPtr )

	// compute commands
	DECL_SERIALIZER( CmdBuf_Bake::Compute_BindPipelineCmd,			ppln )
	DECL_SERIALIZER( CmdBuf_Bake::Compute_BindDescriptorSetCmd,		index, ds, dynamicOffsets )
	DECL_SERIALIZER( CmdBuf_Bake::Compute_PushConstantCmd,			pcIndex_offset, pcIndex_stage, data, typeName )
	DECL_SERIALIZER( CmdBuf_Bake::Compute_Dispatch,					groupCount )
	DECL_SERIALIZER( CmdBuf_Bake::Compute_DispatchIndirect,			buffer, offset )

	// graphics commands
	DECL_SERIALIZER( CmdBuf_Bake::Graphics_BeginRenderPass::SerAttachment,		name, imageView, clearValue, initial, final, relaxedStateTransition )
	DECL_SERIALIZER( CmdBuf_Bake::Graphics_BeginRenderPass::SerRenderPassDesc,	attachments, viewports, area, layerCount, renderPassName, subpassName, packId )
	DECL_SERIALIZER( CmdBuf_Bake::Graphics_BeginRenderPass,			desc, Ser_DebugLabel( dbgLabel ))
	DECL_SERIALIZER( CmdBuf_Bake::Graphics_NextSubpass,				Ser_DebugLabel( dbgLabel ))
	DECL_EMPTY_SERIALIZER( CmdBuf_Bake::Graphics_EndRenderPass		)
	DECL_SERIALIZER( CmdBuf_Bake::Graphics_BeginMtRenderPass,		batchId, desc, Ser_DebugLabel( dbgLabel ))
	DECL_SERIALIZER( CmdBuf_Bake::Graphics_NextMtSubpass,			prevBatchId, batchId, Ser_DebugLabel( dbgLabel ))
	DECL_SERIALIZER( CmdBuf_Bake::Graphics_EndMtRenderPass,			batchId )
	DECL_SERIALIZER( CmdBuf_Bake::Graphics_ExecuteSecondary,		batchId, cmdbufs )

	// draw commands
	DECL_SERIALIZER( CmdBuf_Bake::Draw_BindGraphicsPipelineCmd,		ppln )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_BindMeshPipelineCmd,			ppln )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_BindTilePipelineCmd,			ppln )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_BindDescriptorSetCmd,		index, ds, dynamicOffsets )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_PushConstantCmd,				pcIndex_offset, pcIndex_stage, data, typeName )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_SetViewportsCmd,				viewports )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_SetScissorsCmd,				scissors )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_SetDepthBiasCmd,				depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_SetStencilCompareMaskCmd,	frontCompareMask, backCompareMask )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_SetStencilWriteMaskCmd,		frontWriteMask, backWriteMask )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_SetStencilReferenceCmd,		frontReference, backReference )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_SetBlendConstantsCmd,		color )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_SetDepthBoundsCmd,			minDepthBounds, maxDepthBounds )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_SetFragmentShadingRateCmd,	rate, primitiveOp, textureOp )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_BindIndexBufferCmd,			buffer, offset, indexType )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_BindVertexBuffersCmd,		firstBinding, buffers, offsets )
	DECL_SERIALIZER( CmdBuf_Bake::DrawCmd,							vertexCount, instanceCount, firstVertex, firstInstance )
	DECL_SERIALIZER( CmdBuf_Bake::DrawIndexedCmd,					indexCount, instanceCount, firstIndex, vertexOffset, firstInstance )
	DECL_SERIALIZER( CmdBuf_Bake::DrawIndirectCmd,					indirectBuffer, indirectBufferOffset, drawCount, stride )
	DECL_SERIALIZER( CmdBuf_Bake::DrawIndexedIndirectCmd,			indirectBuffer, indirectBufferOffset, drawCount, stride )
	DECL_SERIALIZER( CmdBuf_Bake::DrawMeshTasksCmd,					taskCount )
	DECL_SERIALIZER( CmdBuf_Bake::DrawMeshTasksIndirectCmd,			indirectBuffer, indirectBufferOffset, drawCount, stride )
	DECL_SERIALIZER( CmdBuf_Bake::DrawIndirectCountCmd,				indirectBuffer, indirectBufferOffset, countBuffer, countBufferOffset, maxDrawCount, stride )
	DECL_SERIALIZER( CmdBuf_Bake::DrawIndexedIndirectCountCmd,		indirectBuffer, indirectBufferOffset, countBuffer, countBufferOffset, maxDrawCount, stride )
	DECL_SERIALIZER( CmdBuf_Bake::DrawMeshTasksIndirectCountCmd,	indirectBuffer, indirectBufferOffset, countBuffer, countBufferOffset, maxDrawCount, stride )
	DECL_EMPTY_SERIALIZER( CmdBuf_Bake::DispatchTileCmd				)
	DECL_EMPTY_SERIALIZER( CmdBuf_Bake::Draw_CommitBarriersCmd		)
	DECL_SERIALIZER( CmdBuf_Bake::Draw_AttachmentBarrierCmd,		name, srcState, dstState )
	DECL_SERIALIZER( CmdBuf_Bake::Draw_ClearAttachmentCmd,			name, clearValue, region, baseLayer, layerCount )

	// acceleration structure build commands
	DECL_SERIALIZER( CmdBuf_Bake::ASBuild_BuildGeometryCmd,			Ser_RTGeometryBuild( build ), dstId )
	DECL_SERIALIZER( CmdBuf_Bake::ASBuild_BuildSceneCmd,			Ser_RTSceneBuild( build ), dstId )
	DECL_SERIALIZER( CmdBuf_Bake::ASBuild_UpdateGeometryCmd,		Ser_RTGeometryBuild( build ), srcId, dstId )
	DECL_SERIALIZER( CmdBuf_Bake::ASBuild_UpdateSceneCmd,			Ser_RTSceneBuild( build ), srcId, dstId )
	DECL_SERIALIZER( CmdBuf_Bake::ASBuild_CopyGeometryCmd,			srcId, dstId, mode )
	DECL_SERIALIZER( CmdBuf_Bake::ASBuild_CopySceneCmd,				srcId, dstId, mode )
	DECL_SERIALIZER( CmdBuf_Bake::ASBuild_WriteGeometryPropertyCmd,	property, as, dstBuffer, offset, size )
	DECL_SERIALIZER( CmdBuf_Bake::ASBuild_WriteScenePropertyCmd,	property, as, dstBuffer, offset, size )

	// ray tracing commands
	DECL_SERIALIZER( CmdBuf_Bake::RayTracing_BindPipelineCmd,		ppln )
	DECL_SERIALIZER( CmdBuf_Bake::RayTracing_BindDescriptorSetCmd,	index, ds, dynamicOffsets )
	DECL_SERIALIZER( CmdBuf_Bake::RayTracing_PushConstantCmd,		pcIndex_offset, pcIndex_stage, data, typeName )
	DECL_SERIALIZER( CmdBuf_Bake::RayTracing_SetStackSizeCmd,		size )
	DECL_SERIALIZER( CmdBuf_Bake::RayTracing_TraceRaysCmd,			dim, sbt )
	DECL_SERIALIZER( CmdBuf_Bake::RayTracing_TraceRaysIndirectCmd,	sbt, indirectBuffer, indirectOffset )
	DECL_SERIALIZER( CmdBuf_Bake::RayTracing_TraceRaysIndirect2Cmd,	indirectBuffer, indirectOffset )
//-----------------------------------------------------------------------------

} // AE::RemoteGraphics::Msg
