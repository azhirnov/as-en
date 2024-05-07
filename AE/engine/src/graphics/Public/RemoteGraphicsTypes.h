// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/Queue.h"

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
	using RmMemoryID				= HandleTmpl< 32, 32, Graphics::_hidden_::RemoteIDs_Start + 17 >;

	// for internal usage
	using RmSamplerID				= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 18 >;
	using RmPipelineLayoutID		= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 19 >;
	using RmRenderPassID			= HandleTmpl< 16, 16, Graphics::_hidden_::RemoteIDs_Start + 20 >;

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
	ND_ inline RmBufferID				RmCast (BufferID				id)		{ return BitCast<RmBufferID>(id); }
	ND_ inline BufferID					RmCast (RmBufferID				id)		{ return BitCast<BufferID>(id); }

	ND_ inline RmImageID				RmCast (ImageID					id)		{ return BitCast<RmImageID>(id); }
	ND_ inline ImageID					RmCast (RmImageID				id)		{ return BitCast<ImageID>(id); }

	ND_ inline RmBufferViewID			RmCast (BufferViewID			id)		{ return BitCast<RmBufferViewID>(id); }
	ND_ inline BufferViewID				RmCast (RmBufferViewID			id)		{ return BitCast<BufferViewID>(id); }

	ND_ inline RmImageViewID			RmCast (ImageViewID				id)		{ return BitCast<RmImageViewID>(id); }
	ND_ inline ImageViewID				RmCast (RmImageViewID			id)		{ return BitCast<ImageViewID>(id); }

	ND_ inline RmRTGeometryID			RmCast (RTGeometryID			id)		{ return BitCast<RmRTGeometryID>(id); }
	ND_ inline RTGeometryID				RmCast (RmRTGeometryID			id)		{ return BitCast<RTGeometryID>(id); }

	ND_ inline RmRTSceneID				RmCast (RTSceneID				id)		{ return BitCast<RmRTSceneID>(id); }
	ND_ inline RTSceneID				RmCast (RmRTSceneID				id)		{ return BitCast<RTSceneID>(id); }

	ND_ inline RmRTShaderBindingID		RmCast (RTShaderBindingID		id)		{ return BitCast<RmRTShaderBindingID>(id); }
	ND_ inline RTShaderBindingID		RmCast (RmRTShaderBindingID		id)		{ return BitCast<RTShaderBindingID>(id); }

	ND_ inline RmMemoryID				RmCast (MemoryID				id)		{ return BitCast<RmMemoryID>(id); }
	ND_ inline MemoryID					RmCast (RmMemoryID				id)		{ return BitCast<MemoryID>(id); }

	ND_ inline RmPipelineCacheID		RmCast (PipelineCacheID			id)		{ return BitCast<RmPipelineCacheID>(id); }
	ND_ inline PipelineCacheID			RmCast (RmPipelineCacheID		id)		{ return BitCast<PipelineCacheID>(id); }

	ND_ inline RmDescriptorSetID		RmCast (DescriptorSetID			id)		{ return BitCast<RmDescriptorSetID>(id); }
	ND_ inline DescriptorSetID			RmCast (RmDescriptorSetID		id)		{ return BitCast<DescriptorSetID>(id); }

	ND_ inline RmPipelinePackID			RmCast (PipelinePackID			id)		{ return BitCast<RmPipelinePackID>(id); }
	ND_ inline PipelinePackID			RmCast (RmPipelinePackID		id)		{ return BitCast<PipelinePackID>(id); }

	ND_ inline RmGraphicsPipelineID		RmCast (GraphicsPipelineID		id)		{ return BitCast<RmGraphicsPipelineID>(id); }
	ND_ inline GraphicsPipelineID		RmCast (RmGraphicsPipelineID	id)		{ return BitCast<GraphicsPipelineID>(id); }

	ND_ inline RmMeshPipelineID			RmCast (MeshPipelineID			id)		{ return BitCast<RmMeshPipelineID>(id); }
	ND_ inline MeshPipelineID			RmCast (RmMeshPipelineID		id)		{ return BitCast<MeshPipelineID>(id); }

	ND_ inline RmComputePipelineID		RmCast (ComputePipelineID		id)		{ return BitCast<RmComputePipelineID>(id); }
	ND_ inline ComputePipelineID		RmCast (RmComputePipelineID		id)		{ return BitCast<ComputePipelineID>(id); }

	ND_ inline RmRayTracingPipelineID	RmCast (RayTracingPipelineID	id)		{ return BitCast<RmRayTracingPipelineID>(id); }
	ND_ inline RayTracingPipelineID		RmCast (RmRayTracingPipelineID	id)		{ return BitCast<RayTracingPipelineID>(id); }

	ND_ inline RmTilePipelineID			RmCast (TilePipelineID			id)		{ return BitCast<RmTilePipelineID>(id); }
	ND_ inline TilePipelineID			RmCast (RmTilePipelineID		id)		{ return BitCast<TilePipelineID>(id); }

	ND_ inline RmDescriptorSetLayoutID	RmCast (DescriptorSetLayoutID	id)		{ return BitCast<RmDescriptorSetLayoutID>(id); }
	ND_ inline DescriptorSetLayoutID	RmCast (RmDescriptorSetLayoutID	id)		{ return BitCast<DescriptorSetLayoutID>(id); }

	ND_ inline RmVideoSessionID			RmCast (VideoSessionID			id)		{ return BitCast<RmVideoSessionID>(id); }
	ND_ inline VideoSessionID			RmCast (RmVideoSessionID		id)		{ return BitCast<VideoSessionID>(id); }

	ND_ inline RmVideoBufferID			RmCast (VideoBufferID			id)		{ return BitCast<RmVideoBufferID>(id); }
	ND_ inline VideoBufferID			RmCast (RmVideoBufferID			id)		{ return BitCast<VideoBufferID>(id); }

	ND_ inline RmVideoImageID			RmCast (VideoImageID			id)		{ return BitCast<RmVideoImageID>(id); }
	ND_ inline VideoImageID				RmCast (RmVideoImageID			id)		{ return BitCast<VideoImageID>(id); }

	ND_ inline SamplerID				RmCast (RmSamplerID				id)		{ return BitCast<SamplerID>(id); }
	ND_ inline RmSamplerID				RmCast (SamplerID				id)		{ return BitCast<RmSamplerID>(id); }

	ND_ inline PipelineLayoutID			RmCast (RmPipelineLayoutID		id)		{ return BitCast<PipelineLayoutID>(id); }
	ND_ inline RmPipelineLayoutID		RmCast (PipelineLayoutID		id)		{ return BitCast<RmPipelineLayoutID>(id); }

	ND_ inline Bytes					RmCast (RmDeviceOffset		offset)		{ return BitCast<Bytes>(offset); }
	ND_ inline void*					RmCast (RmDevicePtr			ptr)		{ return BitCastRlx<void*>(ptr); }
	ND_ inline Bytes					RmCast (RmHostOffset		offset)		{ return BitCast<Bytes>(offset); }
	ND_ inline void*					RmCast (RmHostPtr			ptr)		{ return BitCastRlx<void*>(ptr); }

	template <typename T>
	ND_ inline auto  RmCast (ArrayView<T> arr)
	{
		using R = decltype(RmCast( T{} ));
		StaticAssert( sizeof(R) == sizeof(T) );
		return ArrayView<R>{ Cast<R>(arr.data()), arr.size() };
	}

	template <typename T>
	ND_ inline auto  RmCast (const Array<T> &arr)
	{
		return RmCast( ArrayView<T>{arr} );
	}


} // AE::RemoteGraphics
