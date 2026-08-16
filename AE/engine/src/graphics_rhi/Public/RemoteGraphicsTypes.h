// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics_rhi/Public/IDs.h"
#include "graphics_rhi/Public/Queue.h"

namespace AE::Graphics
{

	using RmGraphicsPipelineID		= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  1 >;
	using RmMeshPipelineID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  2 >;
	using RmComputePipelineID		= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  3 >;
	using RmRayTracingPipelineID	= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  4 >;
	using RmTilePipelineID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  5 >;
	using RmPipelineCacheID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  6 >;
	using RmPipelinePackID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  7 >;
	using RmDescriptorSetID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  8 >;
	using RmDescriptorSetLayoutID	= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start +  9 >;

	using RmBufferID				= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 10 >;
	using RmImageID					= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 11 >;
	using RmBufferViewID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 12 >;
	using RmImageViewID				= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 13 >;
	using RmRTGeometryID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 14 >;		// bottom-level AS
	using RmRTSceneID				= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 15 >;		// top-level AS
	using RmRTShaderBindingID		= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 16 >;		// shader binding table
	using RmRTMicromapID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 17 >;

	using RmIndirectCommandsLayoutID = HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 18 >;

	// for internal usage
	using RmSamplerID				= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 30 >;
	using RmPipelineLayoutID		= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 31 >;
	using RmRenderPassID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 32 >;
	using RmMemoryID				= HandleTmpl< 32, 32, Graphics::_hidden_::RemoteIDs_Start + 33 >;
	using RmIndirectExecutionSetID	= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 34 >;

	using RmVideoSessionID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 40 >;
	using RmVideoBufferID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 41 >;
	using RmVideoImageID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 42 >;

	// for internal usage
	using RmGfxMemAllocatorID		= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 60 >;
	using RmDescriptorAllocatorID	= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 61 >;
	using RmRenderTechPipelinesID	= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 62 >;
	using RmCommandBufferID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 63 >;
	using RmDrawCommandBufferID		= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 64 >;
	using RmCommandBatchID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 65 >;
	using RmDrawCommandBatchID		= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 66 >;
	using RmSemaphoreID				= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 67 >;
	using RmQueryID					= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 68 >;


	enum class RmDevicePtr		: ulong { Unknown = 0 };	// pointer on device side
	enum class RmDeviceOffset	: ulong { Unknown = 0 };	// offset in bytes from 'Msg::UploadData::data' on device side

	enum class RmHostPtr		: ulong { Unknown = 0 };	// pointer on host side
	enum class RmHostOffset		: ulong { Unknown = 0 };	// offset in bytes from 'Msg::UploadData::data' on host side


	struct RemoteMemoryObjInfo
	{
		RmDevicePtr		devicePtr	= Default;
		void*			mappedPtr	= null;
	};


	struct RemoteCmdBatchDependency
	{
		RmSemaphoreID		semaphore;
		ulong				value		= 0;		// for timeline semaphore

		ND_ explicit operator bool ()	C_NE___	{ return bool{semaphore}; }
	};


	struct RemoteBufferDesc
	{
		RmBufferID		bufferId;
		BufferDesc		desc;
		RmDevicePtr		devicePtr	= Default;
		RC<SharedMem>	hostMem;
		DeviceAddress	address		= Default;
		bool			releaseRef	= false;
	};


	struct RemoteBufferViewDesc
	{
		RmBufferViewID	viewId;
		BufferViewDesc	desc;
		bool			releaseRef	= false;
	};


	struct RemoteImageDesc
	{
		RmImageID		imageId;
		ImageDesc		desc;
		bool			releaseRef	= false;
	};


	struct RemoteImageViewDesc
	{
		RmImageViewID	viewId;
		ImageViewDesc	desc;
		bool			releaseRef	= false;
	};

} // AE::Graphics
//-----------------------------------------------------------------------------


namespace AE::RemoteGraphics
{
	using namespace AE::Graphics;

/*
=================================================
	RmCast
=================================================
*/
	Nd__In RmBufferID				RmCast (BufferID				id)		{ return BitCast<RmBufferID>(id); }
	Nd__In BufferID					RmCast (RmBufferID				id)		{ return BitCast<BufferID>(id); }

	Nd__In RmImageID				RmCast (ImageID					id)		{ return BitCast<RmImageID>(id); }
	Nd__In ImageID					RmCast (RmImageID				id)		{ return BitCast<ImageID>(id); }

	Nd__In RmBufferViewID			RmCast (BufferViewID			id)		{ return BitCast<RmBufferViewID>(id); }
	Nd__In BufferViewID				RmCast (RmBufferViewID			id)		{ return BitCast<BufferViewID>(id); }

	Nd__In RmImageViewID			RmCast (ImageViewID				id)		{ return BitCast<RmImageViewID>(id); }
	Nd__In ImageViewID				RmCast (RmImageViewID			id)		{ return BitCast<ImageViewID>(id); }

	Nd__In RmRTGeometryID			RmCast (RTGeometryID			id)		{ return BitCast<RmRTGeometryID>(id); }
	Nd__In RTGeometryID				RmCast (RmRTGeometryID			id)		{ return BitCast<RTGeometryID>(id); }

	Nd__In RmRTSceneID				RmCast (RTSceneID				id)		{ return BitCast<RmRTSceneID>(id); }
	Nd__In RTSceneID				RmCast (RmRTSceneID				id)		{ return BitCast<RTSceneID>(id); }

	Nd__In RmRTMicromapID			RmCast (RTMicromapID			id)		{ return BitCast<RmRTMicromapID>(id); }
	Nd__In RTMicromapID				RmCast (RmRTMicromapID			id)		{ return BitCast<RTMicromapID>(id); }

	Nd__In RmRTShaderBindingID		RmCast (RTShaderBindingID		id)		{ return BitCast<RmRTShaderBindingID>(id); }
	Nd__In RTShaderBindingID		RmCast (RmRTShaderBindingID		id)		{ return BitCast<RTShaderBindingID>(id); }

	Nd__In RmMemoryID				RmCast (MemoryID				id)		{ return BitCast<RmMemoryID>(id); }
	Nd__In MemoryID					RmCast (RmMemoryID				id)		{ return BitCast<MemoryID>(id); }

	Nd__In RmPipelineCacheID		RmCast (PipelineCacheID			id)		{ return BitCast<RmPipelineCacheID>(id); }
	Nd__In PipelineCacheID			RmCast (RmPipelineCacheID		id)		{ return BitCast<PipelineCacheID>(id); }

	Nd__In RmDescriptorSetID		RmCast (DescriptorSetID			id)		{ return BitCast<RmDescriptorSetID>(id); }
	Nd__In DescriptorSetID			RmCast (RmDescriptorSetID		id)		{ return BitCast<DescriptorSetID>(id); }

	Nd__In RmPipelinePackID			RmCast (PipelinePackID			id)		{ return BitCast<RmPipelinePackID>(id); }
	Nd__In PipelinePackID			RmCast (RmPipelinePackID		id)		{ return BitCast<PipelinePackID>(id); }

	Nd__In RmGraphicsPipelineID		RmCast (GraphicsPipelineID		id)		{ return BitCast<RmGraphicsPipelineID>(id); }
	Nd__In GraphicsPipelineID		RmCast (RmGraphicsPipelineID	id)		{ return BitCast<GraphicsPipelineID>(id); }

	Nd__In RmMeshPipelineID			RmCast (MeshPipelineID			id)		{ return BitCast<RmMeshPipelineID>(id); }
	Nd__In MeshPipelineID			RmCast (RmMeshPipelineID		id)		{ return BitCast<MeshPipelineID>(id); }

	Nd__In RmComputePipelineID		RmCast (ComputePipelineID		id)		{ return BitCast<RmComputePipelineID>(id); }
	Nd__In ComputePipelineID		RmCast (RmComputePipelineID		id)		{ return BitCast<ComputePipelineID>(id); }

	Nd__In RmRayTracingPipelineID	RmCast (RayTracingPipelineID	id)		{ return BitCast<RmRayTracingPipelineID>(id); }
	Nd__In RayTracingPipelineID		RmCast (RmRayTracingPipelineID	id)		{ return BitCast<RayTracingPipelineID>(id); }

	Nd__In RmTilePipelineID			RmCast (TilePipelineID			id)		{ return BitCast<RmTilePipelineID>(id); }
	Nd__In TilePipelineID			RmCast (RmTilePipelineID		id)		{ return BitCast<TilePipelineID>(id); }

	Nd__In RmDescriptorSetLayoutID	RmCast (DescriptorSetLayoutID	id)		{ return BitCast<RmDescriptorSetLayoutID>(id); }
	Nd__In DescriptorSetLayoutID	RmCast (RmDescriptorSetLayoutID	id)		{ return BitCast<DescriptorSetLayoutID>(id); }

	Nd__In RmVideoSessionID			RmCast (VideoSessionID			id)		{ return BitCast<RmVideoSessionID>(id); }
	Nd__In VideoSessionID			RmCast (RmVideoSessionID		id)		{ return BitCast<VideoSessionID>(id); }

	Nd__In RmVideoBufferID			RmCast (VideoBufferID			id)		{ return BitCast<RmVideoBufferID>(id); }
	Nd__In VideoBufferID			RmCast (RmVideoBufferID			id)		{ return BitCast<VideoBufferID>(id); }

	Nd__In RmVideoImageID			RmCast (VideoImageID			id)		{ return BitCast<RmVideoImageID>(id); }
	Nd__In VideoImageID				RmCast (RmVideoImageID			id)		{ return BitCast<VideoImageID>(id); }

	Nd__In SamplerID				RmCast (RmSamplerID				id)		{ return BitCast<SamplerID>(id); }
	Nd__In RmSamplerID				RmCast (SamplerID				id)		{ return BitCast<RmSamplerID>(id); }

	Nd__In PipelineLayoutID			RmCast (RmPipelineLayoutID		id)		{ return BitCast<PipelineLayoutID>(id); }
	Nd__In RmPipelineLayoutID		RmCast (PipelineLayoutID		id)		{ return BitCast<RmPipelineLayoutID>(id); }

	Nd__In Bytes					RmCast (RmDeviceOffset		offset)		{ return BitCast<Bytes>(offset); }
	Nd__In void*					RmCast (RmDevicePtr			ptr)		{ return BitCastRlx<void*>(ptr); }
	Nd__In Bytes					RmCast (RmHostOffset		offset)		{ return BitCast<Bytes>(offset); }
	Nd__In void*					RmCast (RmHostPtr			ptr)		{ return BitCastRlx<void*>(ptr); }

	template <typename T>
	Nd__In auto  RmCast (ArrayView<T> arr)
	{
		using R = decltype(RmCast( T{} ));
		StaticAssert( sizeof(R) == sizeof(T) );
		return ArrayView<R>{ Cast<R>(arr.data()), arr.size() };
	}

	template <typename T>
	Nd__In auto  RmCast (const Array<T> &arr)
	{
		return RmCast( ArrayView<T>{arr} );
	}


} // AE::RemoteGraphics
