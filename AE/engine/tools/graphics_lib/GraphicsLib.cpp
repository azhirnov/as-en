// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_lib/GraphicsLib.h"
#include "graphics/GraphicsImpl.h"

#ifdef AE_ENABLE_VULKAN
# include "VulkanSyncLog.h"
#endif
#define ENABLE_SYNC_LOG		0

#if defined(AE_ENABLE_VULKAN) or defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define VULKAN_ONLY( ... )	__VA_ARGS__
#else
#	define VULKAN_ONLY( ... )
#endif
#if defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define METAL_ONLY( ... )	__VA_ARGS__
#else
#	define METAL_ONLY( ... )
#endif

namespace AE::GraphicsLib
{
namespace
{
	using namespace AE::Threading;

	using CtxAlloc_t = LinearAllocator<>;



	//
	// Graphics Context
	//
	class GraphicsContext2Impl final : public IGraphicsContext2
	{
	// variables
	private:
		DirectCtx::Graphics		_ctx;
		CtxAlloc_t &			_alloc;

	// methods
	public:
		GraphicsContext2Impl (const RenderTask &task, DirectCtx::CommandBuffer cmdbuf, CtxAlloc_t &alloc) __NE___ : _ctx{ task, RVRef(cmdbuf) }, _alloc{alloc} {}

		ND_ DirectCtx::CommandBuffer	ReleaseCommandBuffer ()							__Th___	{ return _ctx.ReleaseCommandBuffer(); }
		ND_ auto						EndCommandBuffer ()								__Th___	{ return _ctx.EndCommandBuffer(); }


		// GLibBase //
		void  Destroy ()																__NE_OV	{ delete this; }


		// IGraphicsContext2 //
		DrawContext2	BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg)	__Th_OV;
		DrawContext2	NextSubpass (DrawContext2 &prevPassCtx, DebugLabel dbg)			__Th_OV;
		void			EndRenderPass (DrawContext2 &)									__Th_OV;


		// IBaseContext //
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)										__Th_OV	{ _ctx.BufferBarrier( buffer, srcState, dstState ); }
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)								__Th_OV	{ _ctx.BufferViewBarrier( view, srcState, dstState ); }

		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)										__Th_OV	{ _ctx.ImageBarrier( image, srcState, dstState ); }
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)	__Th_OV	{ _ctx.ImageBarrier( image, srcState, dstState, subRes ); }
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)									__Th_OV	{ _ctx.ImageViewBarrier( view, srcState, dstState ); }

		void  MemoryBarrier (EResourceState srcState, EResourceState dstState)														__Th_OV	{ _ctx.MemoryBarrier( srcState, dstState ); }
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__Th_OV	{ _ctx.MemoryBarrier( srcScope, dstScope ); }
		void  MemoryBarrier ()																										__Th_OV	{ _ctx.MemoryBarrier(); }

		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__Th_OV	{ _ctx.ExecutionBarrier( srcScope, dstScope ); }
		void  ExecutionBarrier ()																									__Th_OV	{ _ctx.ExecutionBarrier(); }

		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		__Th_OV	{ _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); }
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		__Th_OV	{ _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); }

		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__Th_OV	{ _ctx.AcquireImageOwnership( image, srcQueue, srcState, dstState ); }
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__Th_OV	{ _ctx.ReleaseImageOwnership( image, srcState, dstState, dstQueue ); }

		void  CommitBarriers ()																										__Th_OV	{ _ctx.CommitBarriers(); }

		void  DebugMarker (DebugLabel dbg)																							__Th_OV	{ _ctx.DebugMarker( dbg ); }
		void  PushDebugGroup (DebugLabel dbg)																						__Th_OV	{ _ctx.PushDebugGroup( dbg ); }
		void  PopDebugGroup ()																										__Th_OV	{ _ctx.PopDebugGroup(); }

		VULKAN_ONLY(
		void  WriteTimestamp (const IQueryManager::IQuery &q, uint index, EPipelineScope srcScope)									__Th_OV	{ _ctx.WriteTimestamp( q, index, srcScope ); }
		)

		FrameUID  GetFrameId ()																										C_NE_OV	{ return _ctx.GetFrameId(); }
	};


	//
	// Transfer Context
	//
	class TransferContext2Impl final : public ITransferContext2
	{
	// variables
	private:
		DirectCtx::Transfer		_ctx;

	// methods
	public:
		TransferContext2Impl (const RenderTask &task, DirectCtx::CommandBuffer cmdbuf)	__NE___ : _ctx{ task, RVRef(cmdbuf) } {}

		ND_ DirectCtx::CommandBuffer	ReleaseCommandBuffer ()							__Th___	{ return _ctx.ReleaseCommandBuffer(); }
		ND_ auto						EndCommandBuffer ()								__Th___	{ return _ctx.EndCommandBuffer(); }


		// GLibBase //
		void  Destroy ()																					__NE_OV	{ delete this; }


		// ITransferContext //
		void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data)								__Th_OV	{ _ctx.FillBuffer( buffer, offset, size, data ); }
		void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)					__Th_OV	{ _ctx.UpdateBuffer( buffer, offset, size, data ); }

		void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)				__Th_OV	{ _ctx.CopyBuffer( srcBuffer, dstBuffer, ranges ); }
		void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)					__Th_OV	{ _ctx.CopyImage( srcImage, dstImage, ranges ); }
		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)	__Th_OV	{ _ctx.CopyBufferToImage( srcBuffer, dstImage, ranges ); }
		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)	__Th_OV	{ _ctx.CopyBufferToImage( srcBuffer, dstImage, ranges ); }
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)	__Th_OV	{ _ctx.CopyImageToBuffer( srcImage, dstBuffer, ranges ); }
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)	__Th_OV	{ _ctx.CopyImageToBuffer( srcImage, dstBuffer, ranges ); }

		void  UploadBuffer (BufferID buffer, const UploadBufferDesc &desc, OUT BufferMemView &memView)		__Th_OV	{ _ctx.UploadBuffer( buffer, desc, OUT memView ); }

		void  UploadImage (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)			__Th_OV	{ _ctx.UploadImage( image, desc, OUT memView ); }
		void  UploadImage (VideoImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)		__Th_OV	{ _ctx.UploadImage( image, desc, OUT memView ); }

		ReadbackBufferResult  ReadbackBuffer (BufferID buffer, const ReadbackBufferDesc &desc)				__Th_OV	{ Unused( buffer, desc );  return Default; }
		ReadbackImageResult   ReadbackImage (ImageID image, const ReadbackImageDesc &desc)					__Th_OV	{ Unused( image, desc );  return Default; }
		ReadbackImageResult   ReadbackImage (VideoImageID image, const ReadbackImageDesc &desc)				__Th_OV	{ Unused( image, desc );  return Default; }

		void  UploadBuffer (INOUT BufferStream &stream, OUT BufferMemView &memView)							__Th_OV	{ _ctx.UploadBuffer( INOUT stream, OUT memView ); }
		void  UploadImage (INOUT ImageStream &stream, OUT ImageMemView &memView)							__Th_OV	{ _ctx.UploadImage( INOUT stream, OUT memView ); }
		void  UploadImage (INOUT VideoImageStream &stream, OUT ImageMemView &memView)						__Th_OV	{ _ctx.UploadImage( INOUT stream, OUT memView ); }

		Promise<BufferMemView>  ReadbackBuffer (INOUT BufferStream &stream)									__Th_OV	{ Unused( stream );  return Default; }
		Promise<ImageMemView>   ReadbackImage (INOUT ImageStream &stream)									__Th_OV	{ Unused( stream );  return Default; }
		Promise<ImageMemView>   ReadbackImage (INOUT VideoImageStream &stream)								__Th_OV	{ Unused( stream );  return Default; }

		bool  UpdateHostBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)				__Th_OV	{ return _ctx.UpdateHostBuffer( buffer, offset, size, data ); }
		VULKAN_ONLY(
		bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped)			__Th_OV	{ return _ctx.MapHostBuffer( buffer, offset, INOUT size, OUT mapped ); }
		)

		Promise<ArrayView<ubyte>>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)				__Th_OV	{ Unused( buffer, offset, size );  return Default; }

		uint3  MinImageTransferGranularity ()																C_NE_OV	{ return _ctx.MinImageTransferGranularity(); }

		VULKAN_ONLY(
		void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges)	__Th_OV	{ _ctx.ClearColorImage( image, color, ranges ); }
		void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges)	__Th_OV	{ _ctx.ClearColorImage( image, color, ranges ); }
		void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges)	__Th_OV	{ _ctx.ClearColorImage( image, color, ranges ); }
		)

		void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions)	__Th_OV	{ _ctx.BlitImage( srcImage, dstImage, filter, regions ); }

		VULKAN_ONLY(
		void  ClearDepthStencilImage (ImageID image, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges) __Th_OV { _ctx.ClearDepthStencilImage( image, depthStencil, ranges ); }
		void  ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions)				__Th_OV	{ _ctx.ResolveImage( srcImage, dstImage, regions ); }
		)

		void  GenerateMipmaps (ImageID image, EResourceState state)												__Th_OV	{ _ctx.GenerateMipmaps( image, state ); }
		void  GenerateMipmaps (ImageID image, ArrayView<ImageSubresourceRange> ranges, EResourceState state)	__Th_OV	{ _ctx.GenerateMipmaps( image, ranges, state ); }


		// IBaseContext //
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)										__Th_OV	{ _ctx.BufferBarrier( buffer, srcState, dstState ); }
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)								__Th_OV	{ _ctx.BufferViewBarrier( view, srcState, dstState ); }

		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)										__Th_OV	{ _ctx.ImageBarrier( image, srcState, dstState ); }
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)	__Th_OV	{ _ctx.ImageBarrier( image, srcState, dstState, subRes ); }
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)									__Th_OV	{ _ctx.ImageViewBarrier( view, srcState, dstState ); }

		void  MemoryBarrier (EResourceState srcState, EResourceState dstState)														__Th_OV	{ _ctx.MemoryBarrier( srcState, dstState ); }
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__Th_OV	{ _ctx.MemoryBarrier( srcScope, dstScope ); }
		void  MemoryBarrier ()																										__Th_OV	{ _ctx.MemoryBarrier(); }

		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__Th_OV	{ _ctx.ExecutionBarrier( srcScope, dstScope ); }
		void  ExecutionBarrier ()																									__Th_OV	{ _ctx.ExecutionBarrier(); }

		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		__Th_OV	{ _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); }
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		__Th_OV	{ _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); }

		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__Th_OV	{ _ctx.AcquireImageOwnership( image, srcQueue, srcState, dstState ); }
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__Th_OV	{ _ctx.ReleaseImageOwnership( image, srcState, dstState, dstQueue ); }

		void  CommitBarriers ()																										__Th_OV	{ _ctx.CommitBarriers(); }

		void  DebugMarker (DebugLabel dbg)																							__Th_OV	{ _ctx.DebugMarker( dbg ); }
		void  PushDebugGroup (DebugLabel dbg)																						__Th_OV	{ _ctx.PushDebugGroup( dbg ); }
		void  PopDebugGroup ()																										__Th_OV	{ _ctx.PopDebugGroup(); }

		VULKAN_ONLY(
		void  WriteTimestamp (const IQueryManager::IQuery &q, uint index, EPipelineScope srcScope)									__Th_OV	{ _ctx.WriteTimestamp( q, index, srcScope ); }
		)

		FrameUID  GetFrameId ()																										C_NE_OV	{ return _ctx.GetFrameId(); }
	};


	//
	// Compute Context
	//
	class ComputeContext2Impl final : public IComputeContext2
	{
	// variables
	private:
		DirectCtx::Compute		_ctx;

	// methods
	public:
		ComputeContext2Impl (const RenderTask &task, DirectCtx::CommandBuffer cmdbuf)	__NE___ : _ctx{ task, RVRef(cmdbuf) } {}

		ND_ DirectCtx::CommandBuffer	ReleaseCommandBuffer ()							__Th___	{ return _ctx.ReleaseCommandBuffer(); }
		ND_ auto						EndCommandBuffer ()								__Th___	{ return _ctx.EndCommandBuffer(); }


		// GLibBase //
		void  Destroy ()																											__NE_OV	{ delete this; }


		// IComputeContext //
		void  BindPipeline (ComputePipelineID ppln)																					__Th_OV	{ _ctx.BindPipeline( ppln ); }
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)				__Th_OV	{ _ctx.BindDescriptorSet( index, ds, dynamicOffsets ); }
		void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)			__Th_OV	{ _ctx.PushConstant( idx, size, values, typeName ); }

		void  Dispatch (const uint3 &groupCount)																					__Th_OV	{ _ctx.Dispatch( groupCount ); }
		void  DispatchIndirect (BufferID buffer, Bytes offset)																		__Th_OV	{ _ctx.DispatchIndirect( buffer, offset ); }


		// IBaseContext //
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)										__Th_OV	{ _ctx.BufferBarrier( buffer, srcState, dstState ); }
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)								__Th_OV	{ _ctx.BufferViewBarrier( view, srcState, dstState ); }

		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)										__Th_OV	{ _ctx.ImageBarrier( image, srcState, dstState ); }
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)	__Th_OV	{ _ctx.ImageBarrier( image, srcState, dstState, subRes ); }
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)									__Th_OV	{ _ctx.ImageViewBarrier( view, srcState, dstState ); }

		void  MemoryBarrier (EResourceState srcState, EResourceState dstState)														__Th_OV	{ _ctx.MemoryBarrier( srcState, dstState ); }
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__Th_OV	{ _ctx.MemoryBarrier( srcScope, dstScope ); }
		void  MemoryBarrier ()																										__Th_OV	{ _ctx.MemoryBarrier(); }

		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__Th_OV	{ _ctx.ExecutionBarrier( srcScope, dstScope ); }
		void  ExecutionBarrier ()																									__Th_OV	{ _ctx.ExecutionBarrier(); }

		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		__Th_OV	{ _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); }
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		__Th_OV	{ _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); }

		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__Th_OV	{ _ctx.AcquireImageOwnership( image, srcQueue, srcState, dstState ); }
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__Th_OV	{ _ctx.ReleaseImageOwnership( image, srcState, dstState, dstQueue ); }

		void  CommitBarriers ()																										__Th_OV	{ _ctx.CommitBarriers(); }

		void  DebugMarker (DebugLabel dbg)																							__Th_OV	{ _ctx.DebugMarker( dbg ); }
		void  PushDebugGroup (DebugLabel dbg)																						__Th_OV	{ _ctx.PushDebugGroup( dbg ); }
		void  PopDebugGroup ()																										__Th_OV	{ _ctx.PopDebugGroup(); }

		VULKAN_ONLY(
		void  WriteTimestamp (const IQueryManager::IQuery &q, uint index, EPipelineScope srcScope)									__Th_OV	{ _ctx.WriteTimestamp( q, index, srcScope ); }
		)

		FrameUID  GetFrameId ()																										C_NE_OV	{ return _ctx.GetFrameId(); }
	};


	//
	// Resource Manager
	//
	class ResourceManager2 final : public IResourceManager2
	{
	// variables
	private:
		Ptr<ResourceManager>	_rm;

	// methods
	public:
		ResourceManager2 ()									__NE___	{}
		explicit ResourceManager2 (Ptr<ResourceManager> rm)	__NE___ : _rm{rm} {}

		// IResourceManager //
		bool						IsSupported (EMemoryType memType)																	C_NE_OV	{ return _rm->IsSupported( memType ); }
		bool						IsSupported (const BufferDesc &desc)																C_NE_OV	{ return _rm->IsSupported( desc ); }
		bool						IsSupported (const ImageDesc &desc)																	C_NE_OV	{ return _rm->IsSupported( desc ); }
		bool						IsSupported (const VideoImageDesc &desc)															C_NE_OV	{ return _rm->IsSupported( desc ); }
		bool						IsSupported (const VideoBufferDesc &desc)															C_NE_OV	{ return _rm->IsSupported( desc ); }
		bool						IsSupported (const VideoSessionDesc &desc)															C_NE_OV	{ return _rm->IsSupported( desc ); }
		bool						IsSupported (BufferID buffer, const BufferViewDesc &desc)											C_NE_OV	{ return _rm->IsSupported( buffer, desc ); }
		bool						IsSupported (ImageID image, const ImageViewDesc &desc)												C_NE_OV	{ return _rm->IsSupported( image, desc ); }
		bool						IsSupported (const RTGeometryDesc &desc)															C_NE_OV	{ return _rm->IsSupported( desc ); }
		bool						IsSupported (const RTGeometryBuild &build)															C_NE_OV	{ return _rm->IsSupported( build ); }
		bool						IsSupported (const RTSceneDesc &desc)																C_NE_OV	{ return _rm->IsSupported( desc ); }
		bool						IsSupported (const RTSceneBuild &build)																C_NE_OV	{ return _rm->IsSupported( build ); }

		Strong<ImageID>				CreateImage (const ImageDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator)				__NE_OV	{ return _rm->CreateImage( desc, dbgName, RVRef(allocator) ); }
		Strong<BufferID>			CreateBuffer (const BufferDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator)				__NE_OV	{ return _rm->CreateBuffer( desc, dbgName, RVRef(allocator) ); }

		Strong<ImageID>				CreateImage (const NativeImageDesc_t &, StringView, GfxMemAllocatorPtr)								__NE_OV	{ CHECK_MSG( false, "not supported" );  return Default; }
		Strong<BufferID>			CreateBuffer (const NativeBufferDesc_t &, StringView, GfxMemAllocatorPtr)							__NE_OV	{ CHECK_MSG( false, "not supported" );  return Default; }

		Strong<ImageViewID>			CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName)						__NE_OV	{ return _rm->CreateImageView( desc, image, dbgName ); }
		Strong<BufferViewID>		CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName)					__NE_OV	{ return _rm->CreateBufferView( desc, buffer, dbgName ); }

		Strong<ImageViewID>			CreateImageView (const NativeImageViewDesc_t &, ImageID, StringView)								__NE_OV	{ CHECK_MSG( false, "not supported" );  return Default; }
		Strong<BufferViewID>		CreateBufferView (const NativeBufferViewDesc_t &, BufferID, StringView)								__NE_OV	{ CHECK_MSG( false, "not supported" );  return Default; }

		Strong<RTGeometryID>		CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator)		__NE_OV	{ return _rm->CreateRTGeometry( desc, dbgName, RVRef(allocator) ); }
		Strong<RTSceneID>			CreateRTScene (const RTSceneDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator)			__NE_OV	{ return _rm->CreateRTScene( desc, dbgName, RVRef(allocator) ); }

		Bytes						GetShaderGroupStackSize (RayTracingPipelineID ppln, ArrayView<RayTracingGroupName> names, ERTShaderGroup type) __NE_OV { return _rm->GetShaderGroupStackSize( ppln, names, type ); }
		RTASBuildSizes				GetRTGeometrySizes (const RTGeometryBuild &desc)													__NE_OV	{ return _rm->GetRTGeometrySizes( desc ); }
		RTASBuildSizes				GetRTSceneSizes (const RTSceneBuild &desc)															__NE_OV	{ return _rm->GetRTSceneSizes( desc ); }

		DeviceAddress				GetDeviceAddress (BufferID		id)																	C_NE_OV	{ return _rm->GetDeviceAddress( id ); }
		DeviceAddress				GetDeviceAddress (RTGeometryID	id)																	C_NE_OV	{ return _rm->GetDeviceAddress( id ); }

		bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, GraphicsPipelineID   ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName)	__NE_OV { return _rm->CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName ); }
		bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, MeshPipelineID       ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName)	__NE_OV { return _rm->CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName ); }
		bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, ComputePipelineID    ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName)	__NE_OV { return _rm->CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName ); }
		bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, RayTracingPipelineID ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName)	__NE_OV { return _rm->CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName ); }
		bool						CreateDescriptorSets (OUT DescSetBinding &binding, OUT Strong<DescriptorSetID> *dst, usize count, TilePipelineID       ppln, DescriptorSetName::Ref dsName, DescriptorAllocatorPtr allocator, StringView dbgName)	__NE_OV { return _rm->CreateDescriptorSets( OUT binding, OUT dst, count, ppln, dsName, RVRef(allocator), dbgName ); }
		bool						CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count, PipelinePackID packId, DSLayoutName::Ref dslName, DescriptorAllocatorPtr allocator, StringView dbgName)										__NE_OV { return _rm->CreateDescriptorSets( OUT dst, count, packId, dslName, RVRef(allocator), dbgName ); }
		bool						CreateDescriptorSets (OUT Strong<DescriptorSetID> *dst, usize count, DescriptorSetLayoutID layoutId, DescriptorAllocatorPtr allocator, StringView dbgName)															__NE_OV { return _rm->CreateDescriptorSets( OUT dst, count, layoutId, RVRef(allocator), dbgName ); }

		PushConstantIndex			GetPushConstantIndex (GraphicsPipelineID   ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize)		__NE_OV	{ return _rm->GetPushConstantIndex( ppln, pcName, typeName, dataSize ); }
		PushConstantIndex			GetPushConstantIndex (MeshPipelineID       ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize)		__NE_OV	{ return _rm->GetPushConstantIndex( ppln, pcName, typeName, dataSize ); }
		PushConstantIndex			GetPushConstantIndex (ComputePipelineID    ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize)		__NE_OV	{ return _rm->GetPushConstantIndex( ppln, pcName, typeName, dataSize ); }
		PushConstantIndex			GetPushConstantIndex (RayTracingPipelineID ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize)		__NE_OV	{ return _rm->GetPushConstantIndex( ppln, pcName, typeName, dataSize ); }
		PushConstantIndex			GetPushConstantIndex (TilePipelineID       ppln, PushConstantName::Ref pcName, ShaderStructName::Ref typeName, Bytes dataSize)		__NE_OV	{ return _rm->GetPushConstantIndex( ppln, pcName, typeName, dataSize ); }

		Strong<GraphicsPipelineID>	CreateGraphicsPipeline	(PipelinePackID packId, PipelineTmplName::Ref name, const GraphicsPipelineDesc	 &desc, PipelineCacheID cache)	__NE_OV	{ return _rm->CreateGraphicsPipeline( packId, name, desc, cache ); }
		Strong<MeshPipelineID>		CreateMeshPipeline		(PipelinePackID packId, PipelineTmplName::Ref name, const MeshPipelineDesc		 &desc, PipelineCacheID cache)	__NE_OV	{ return _rm->CreateMeshPipeline( packId, name, desc, cache ); }
		Strong<ComputePipelineID>	CreateComputePipeline	(PipelinePackID packId, PipelineTmplName::Ref name, const ComputePipelineDesc	 &desc, PipelineCacheID cache)	__NE_OV	{ return _rm->CreateComputePipeline( packId, name, desc, cache ); }
		Strong<RayTracingPipelineID>CreateRayTracingPipeline(PipelinePackID packId, PipelineTmplName::Ref name, const RayTracingPipelineDesc &desc, PipelineCacheID cache)	__NE_OV	{ return _rm->CreateRayTracingPipeline( packId, name, desc, cache ); }
		Strong<TilePipelineID>		CreateTilePipeline		(PipelinePackID packId, PipelineTmplName::Ref name, const TilePipelineDesc		 &desc, PipelineCacheID cache)	__NE_OV	{ return _rm->CreateTilePipeline( packId, name, desc, cache ); }

		Strong<VideoSessionID>		CreateVideoSession (const VideoSessionDesc &desc, StringView dbgName, GfxMemAllocatorPtr allocator)	__NE_OV	{ return _rm->CreateVideoSession( desc, dbgName, RVRef(allocator) ); }
		Strong<VideoBufferID>		CreateVideoBuffer  (const VideoBufferDesc  &desc, StringView dbgName, GfxMemAllocatorPtr allocator)	__NE_OV	{ return _rm->CreateVideoBuffer( desc, dbgName, RVRef(allocator) ); }
		Strong<VideoImageID>		CreateVideoImage   (const VideoImageDesc   &desc, StringView dbgName, GfxMemAllocatorPtr allocator)	__NE_OV	{ return _rm->CreateVideoImage( desc, dbgName, RVRef(allocator) ); }

		Strong<PipelineCacheID>		CreatePipelineCache ()												__NE_OV	{ return _rm->CreatePipelineCache(); }

		bool						InitializeResources (Strong<PipelinePackID> defaultPackId)			__NE_OV	{ return _rm->InitializeResources( RVRef(defaultPackId) ); }
		Strong<PipelinePackID>		LoadPipelinePack (const PipelinePackDesc &desc)						__NE_OV	{ return _rm->LoadPipelinePack( desc ); }
		Array<RenderTechName>		GetSupportedRenderTechs (PipelinePackID id)							C_NE_OV	{ return _rm->GetSupportedRenderTechs( id ); }

		BufferDesc const&			GetDescription (BufferID id)										C_NE_OV	{ return _rm->GetDescription( id ); }
		ImageDesc const&			GetDescription (ImageID id)											C_NE_OV	{ return _rm->GetDescription( id ); }
		BufferViewDesc const&		GetDescription (BufferViewID id)									C_NE_OV	{ return _rm->GetDescription( id ); }
		ImageViewDesc const&		GetDescription (ImageViewID id)										C_NE_OV	{ return _rm->GetDescription( id ); }
		RTShaderBindingDesc const&	GetDescription (RTShaderBindingID id)								C_NE_OV	{ return _rm->GetDescription( id ); }
		RTGeometryDesc const&		GetDescription (RTGeometryID id)									C_NE_OV	{ return _rm->GetDescription( id ); }
		RTSceneDesc const&			GetDescription (RTSceneID id)										C_NE_OV	{ return _rm->GetDescription( id ); }
		VideoImageDesc const&		GetDescription (VideoImageID id)									C_NE_OV	{ return _rm->GetDescription( id ); }
		VideoBufferDesc const&		GetDescription (VideoBufferID id)									C_NE_OV	{ return _rm->GetDescription( id ); }
		VideoSessionDesc const&		GetDescription (VideoSessionID id)									C_NE_OV	{ return _rm->GetDescription( id ); }

		bool						ForceReleaseResources ()											__NE_OV	{ return _rm->ForceReleaseResources(); }

		NativeBuffer_t				GetBufferHandle (BufferID)											C_NE_OV	{ CHECK_MSG( false, "not supported" );  return Default; }
		NativeImage_t				GetImageHandle (ImageID)											C_NE_OV	{ CHECK_MSG( false, "not supported" );  return Default; }
		NativeBufferView_t			GetBufferViewHandle (BufferViewID)									C_NE_OV	{ CHECK_MSG( false, "not supported" );  return Default; }
		NativeImageView_t			GetImageViewHandle (ImageViewID)									C_NE_OV	{ CHECK_MSG( false, "not supported" );  return Default; }

		bool						GetMemoryInfo (ImageID, OUT NativeMemObjInfo_t &)					C_NE_OV	{ CHECK_MSG( false, "not supported" );  return false; }
		bool						GetMemoryInfo (BufferID, OUT NativeMemObjInfo_t &)					C_NE_OV	{ CHECK_MSG( false, "not supported" );  return false; }

		AsyncRTechPipelines			LoadRenderTechAsync (PipelinePackID, RenderTechName::Ref, PipelineCacheID)				__NE_OV	{ CHECK_MSG( false, "not supported" );  return Default; }
		RenderTechPipelinesPtr		LoadRenderTech (PipelinePackID packId, RenderTechName::Ref name, PipelineCacheID cache)	__NE_OV	{ return _rm->LoadRenderTech( packId, name, cache ); }

		StagingBufferStat			GetStagingBufferFrameStat (FrameUID frameId)						C_NE_OV	{ return _rm->GetStagingBufferFrameStat( frameId ); }
		FeatureSet const&			GetFeatureSet ()													C_NE_OV	{ return _rm->GetFeatureSet(); }

		GfxMemAllocatorPtr			CreateLinearGfxMemAllocator (Bytes pageSize)						C_NE_OV	{ return _rm->CreateLinearGfxMemAllocator( pageSize ); }
		GfxMemAllocatorPtr			CreateBlockGfxMemAllocator (Bytes blockSize, Bytes pageSize)		C_NE_OV	{ return _rm->CreateBlockGfxMemAllocator( blockSize, pageSize ); }
		GfxMemAllocatorPtr			CreateUnifiedGfxMemAllocator (Bytes pageSize)						C_NE_OV	{ return _rm->CreateUnifiedGfxMemAllocator( pageSize ); }
		GfxMemAllocatorPtr			CreateLargeSizeGfxMemAllocator ()									C_NE_OV	{ return _rm->CreateLargeSizeGfxMemAllocator(); }
		GfxMemAllocatorPtr			GetDefaultGfxMemAllocator ()										C_NE_OV	{ return _rm->GetDefaultGfxMemAllocator(); }

		DescriptorAllocatorPtr		GetDefaultDescriptorAllocator ()									C_NE_OV	{ return _rm->GetDefaultDescriptorAllocator(); }


		// IResourceManager2 //
		IDevice const&  GetDevice () __NE_OV	{ return _rm->GetDevice(); }

		bool  _DelayedRelease (ulong id, uint type) __NE_OV
		{
			switch ( type )
			{
				case Types_t::Index<ImageID> :			{ Strong<ImageID>			s {BitCastRlx<ImageID>(id)};			return _rm->DelayedRelease( s ) == 0; }
				case Types_t::Index<ImageViewID> :		{ Strong<ImageViewID>		s {BitCastRlx<ImageViewID>(id)};		return _rm->DelayedRelease( s ) == 0; }
				case Types_t::Index<DescriptorSetID> :	{ Strong<DescriptorSetID>	s {BitCastRlx<DescriptorSetID>(id)};	return _rm->DelayedRelease( s ) == 0; }
			}
			StaticAssert( Types_t::Count == 3 );
			RETURN_ERR( "not supported" );
		}

		bool  _ImmediatelyRelease (ulong id, uint type) __NE_OV
		{
			switch ( type )
			{
				case Types_t::Index<ImageID> :			{ Strong<ImageID>			s {BitCastRlx<ImageID>(id)};			return _rm->ImmediatelyRelease( s ) == 0; }
				case Types_t::Index<ImageViewID> :		{ Strong<ImageViewID>		s {BitCastRlx<ImageViewID>(id)};		return _rm->ImmediatelyRelease( s ) == 0; }
				case Types_t::Index<DescriptorSetID> :	{ Strong<DescriptorSetID>	s {BitCastRlx<DescriptorSetID>(id)};	return _rm->ImmediatelyRelease( s ) == 0; }
			}
			StaticAssert( Types_t::Count == 3 );
			RETURN_ERR( "not supported" );
		}
	};


	//
	// Descriptor Updater
	//
	class DescriptorUpdater2 final : public IDescriptorUpdater2
	{
	// variables
	private:
		DescriptorUpdater	_du;

	// methods
	public:
		DescriptorUpdater2 () __NE___ {}

		// GLibBase //
		void  Destroy ()																												__NE_OV	{ delete this; }

		// IDescriptorUpdater //
		bool  Set (DescriptorSetID descrSetId, EDescUpdateMode mode)																	__NE_OV { return _du.Set( descrSetId, mode ); }
		bool  Flush ()																													__NE_OV { return _du.Flush(); }

		bool  BindImage (UniformName::Ref name, ImageViewID image, uint elementIndex)													__NE_OV { return _du.BindImage( name, image, elementIndex ); }
		bool  BindImages (UniformName::Ref name, ArrayView<ImageViewID> images, uint firstIndex)										__NE_OV { return _du.BindImages( name, images, firstIndex ); }
		uint  ImageCount (UniformName::Ref name)																						C_NE_OV { return _du.ImageCount( name ); }

		bool  BindVideoImage (UniformName::Ref name, VideoImageID image, uint elementIndex)												__NE_OV { return _du.BindVideoImage( name, image, elementIndex ); }

		bool  BindTexture (UniformName::Ref name, ImageViewID image, SamplerName::Ref sampler, uint elementIndex)						__NE_OV { return _du.BindTexture( name, image, sampler, elementIndex ); }
		bool  BindTextures (UniformName::Ref name, ArrayView<ImageViewID> images, SamplerName::Ref sampler, uint firstIndex)			__NE_OV { return _du.BindTextures( name, images, sampler, firstIndex ); }
		uint  TextureCount (UniformName::Ref name)																						C_NE_OV { return _du.TextureCount( name ); }

		bool  BindSampler (UniformName::Ref name, SamplerName::Ref sampler, uint elementIndex)											__NE_OV { return _du.BindSampler( name, sampler, elementIndex ); }
		bool  BindSamplers (UniformName::Ref name, ArrayView<SamplerName> samplers, uint firstIndex)									__NE_OV { return _du.BindSamplers( name, samplers, firstIndex ); }
		uint  SamplerCount (UniformName::Ref name)																						C_NE_OV { return _du.SamplerCount( name ); }

		bool  BindBuffer (UniformName::Ref name, ShaderStructName::Ref typeName, BufferID buffer, uint elementIndex)					__NE_OV { return _du.BindBuffer( name, typeName, buffer, elementIndex ); }
		bool  BindBuffer (UniformName::Ref name, ShaderStructName::Ref typeName, BufferID buffer, Bytes offset, Bytes size, uint idx)	__NE_OV { return _du.BindBuffer( name, typeName, buffer, offset, size, idx ); }
		bool  BindBuffers (UniformName::Ref name, ShaderStructName::Ref typeName, ArrayView<BufferID> buffers, uint firstIndex)			__NE_OV { return _du.BindBuffers( name, typeName, buffers, firstIndex ); }
		uint  BufferCount (UniformName::Ref name)																						C_NE_OV { return _du.BufferCount( name ); }

		ShaderStructName  GetBufferStructName (UniformName::Ref name)																	C_NE_OV { return _du.GetBufferStructName( name ); }

		bool  BindTexelBuffer (UniformName::Ref name, BufferViewID view, uint elementIndex)												__NE_OV { return _du.BindTexelBuffer( name, view, elementIndex ); }
		bool  BindTexelBuffers (UniformName::Ref name, ArrayView<BufferViewID> views, uint firstIndex)									__NE_OV { return _du.BindTexelBuffers( name, views, firstIndex ); }
		uint  TexelBufferCount (UniformName::Ref name)																					C_NE_OV { return _du.TexelBufferCount( name ); }

		bool  BindRayTracingScene (UniformName::Ref name, RTSceneID scene, uint elementIndex)											__NE_OV { return _du.BindRayTracingScene( name, scene, elementIndex ); }
		bool  BindRayTracingScenes (UniformName::Ref name, ArrayView<RTSceneID> scenes, uint firstIndex)								__NE_OV { return _du.BindRayTracingScenes( name, scenes, firstIndex ); }
		uint  RayTracingSceneCount (UniformName::Ref name)																				C_NE_OV { return _du.RayTracingSceneCount( name ); }
	};



	//
	// Graphics Library implementation
	//
	class GraphicsLibImpl final : public IGraphicsLib
	{
	// types
	private:
		class RenderTaskImpl final : public Graphics::RenderTask
		{
		public:
			GraphicsLibImpl&	_glib;

		public:
			RenderTaskImpl (GraphicsLibImpl &glib, CommandBatchPtr batch) __NE___ :
				RenderTask{ RVRef(batch), CmdBufExeIndex::Exact(0), Default },
				_glib{ glib }
			{}

				void  Run ()			__Th_OV {}
			ND_ bool  IsSubmitted ()	C_NE___	{ return not IsValid(); }
		};


	// variables
	private:
	  #if defined(AE_ENABLE_VULKAN)
		Graphics::VDeviceInitializer		_device;
		Graphics::VSwapchainInitializer		_swapchain;
		Graphics::VulkanSyncLog				_syncLog;

	  #elif defined(AE_ENABLE_METAL)
		Graphics::MDeviceInitializer		_device;
		Graphics::MSwapchainInitializer		_swapchain;

	  #else
	  #	error not implemented
	  #endif

		Ptr<Graphics::ResourceManager>		_resMngr;
		ResourceManager2					_resMngr2;

		RC<RenderTaskImpl>					_rtask;
		CommandBatchPtr						_cmdBatch;

		CtxAlloc_t							_ctxAllocator;

		Atomic<bool>						_recreateSwapchain	{false};

		const milliseconds					_timeout			{500};


	// methods
	public:
		GraphicsLibImpl ()																			__NE___;
		~GraphicsLibImpl ()																			__NE___;

		bool				Initialize (const GraphicsCreateInfo &,
										StringView	buildConfig,
										bool		optimizeIDs,
										Version3	engineVersion)									__NE_OV;
		void				Destroy ()																__NE_OV;

		bool				CreateSwapchain (const uint2 &, const SwapchainDesc &, NativeWindow)	__NE_OV;

		IDevice const*		GetDevice ()															__NE_OV	{ return &_device; }
		IResourceManager2*	GetResourceManager ()													__NE_OV	{ return &_resMngr2; }
		DescUpdaterPtr		CreateDescriptorUpdater ()												__NE_OV;

		TransferContextPtr	BeginTransferContext (IBaseContext*)									__NE_OV;
		ComputeContextPtr	BeginComputeContext (IBaseContext*)										__NE_OV;
		GraphicsContextPtr	BeginGraphicsContext (IBaseContext*)									__NE_OV;

		bool				BeginFrame ()															__NE_OV;
		bool				GetTargets (OUT App::IOutputSurface::RenderTargets_t &)					__NE_OV;
		bool				EndFrame (IBaseContext*)												__NE_OV;
		bool				WaitAll (nanoseconds timeout)											__NE_OV;

	private:
		ND_ bool  _AcquireSwapchainImage ()															__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	BeginRenderPass
=================================================
*/
	DrawContext2  GraphicsContext2Impl::BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg) __Th___
	{
		auto*	dctx = _alloc.Allocate< DirectCtx::Draw >();
		CHECK_THROW( dctx != null );

		PlacementNew<DirectCtx::Draw>( OUT dctx, _ctx.BeginRenderPass( desc, dbg ));
		return DrawContext2{ dctx };
	}

/*
=================================================
	NextSubpass
=================================================
*/
	DrawContext2  GraphicsContext2Impl::NextSubpass (DrawContext2 &prevPassCtx, DebugLabel dbg) __Th___
	{
		auto*	dctx = _alloc.Allocate< DirectCtx::Draw >();
		CHECK_THROW( dctx != null );

		auto&	prev = *Cast<DirectCtx::Draw>(prevPassCtx._ctx);
		PlacementNew<DirectCtx::Draw>( OUT dctx, _ctx.NextSubpass( prev, dbg ));

		PlacementDelete( prev );
		_alloc.Deallocate( &prev );
		prevPassCtx._ctx = null;

		return DrawContext2{ dctx };
	}

/*
=================================================
	EndRenderPass
=================================================
*/
	void  GraphicsContext2Impl::EndRenderPass (DrawContext2 &inCtx) __Th___
	{
		auto&	dctx = *Cast<DirectCtx::Draw>(inCtx._ctx);
		_ctx.EndRenderPass( INOUT dctx );

		PlacementDelete( dctx );
		_alloc.Deallocate( &dctx );
		inCtx._ctx = null;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	GraphicsLibImpl::GraphicsLibImpl () __NE___
	{
		StaticLogger::InitDefault();
	}

	GraphicsLibImpl::~GraphicsLibImpl () __NE___
	{
		StaticLogger::Deinitialize( true );
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  GraphicsLibImpl::Initialize (const GraphicsCreateInfo &ci, StringView buildConfig, const bool optimizeIDs, const Version3 engineVersion) __NE___
	{
		CHECK_ERR( optimizeIDs == AE_OPTIMIZE_IDS );

		if ( not buildConfig.empty() )
		{
		#ifdef CMAKE_INTDIR
			CHECK_ERR( buildConfig == CMAKE_INTDIR );
		#else
		# ifdef AE_CFG_DEBUG
			CHECK_ERR( buildConfig == "Debug" );
		# elif defined(AE_CFG_DEVELOP)
			CHECK_ERR( buildConfig == "Develop" );
		# elif defined(AE_CFG_PROFILE)
			CHECK_ERR( buildConfig == "Profile" );
		# elif defined(AE_CFG_RELEASE)
			CHECK_ERR( buildConfig == "Release" );
		# else
		#	error unknown config!
		# endif
		#endif
		}
		if ( engineVersion != Default )
		{
			CHECK_ERR( uint(engineVersion.major) == uint(AE_VERSION.Get<0>()) );
			CHECK_ERR( uint(engineVersion.minor) == uint(AE_VERSION.Get<1>()) );
			CHECK_ERR( uint(engineVersion.patch) == uint(AE_VERSION.Get<2>()) );
		}
		CHECK_ERR( not _device.IsInitialized() );

		TaskScheduler::InstanceCtor::Create();

		TaskScheduler::Config	cfg;
		CHECK_FATAL( Scheduler().Setup( cfg ));

	  #if defined(AE_ENABLE_VULKAN)
		CHECK_ERR( _device.Init( ci, VSwapchain::GetInstanceExtensions() ));

		#if ENABLE_SYNC_LOG
		{
			FlatHashMap<VkQueue, String>	qnames;
			for (auto& q : _device.GetQueues()) {
				qnames.emplace( q.handle, String{q.debugName} );
			}
			_syncLog.Initialize( INOUT _device.EditDeviceFnTable(), RVRef(qnames) );
			_syncLog.Enable();
		}
		#endif

	  #elif defined(AE_ENABLE_METAL)
		CHECK_ERR( _device.Init( msg.info ));

	  #endif

		CHECK_ERR( _device.IsInitialized() );
		RenderTaskScheduler::InstanceCtor::Create( _device );
		CHECK_ERR( GraphicsScheduler().Initialize( ci ));

		_resMngr  = &GraphicsScheduler().GetResourceManager();
		_resMngr2 = ResourceManager2{_resMngr};

		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  GraphicsLibImpl::Destroy () __NE___
	{
		_swapchain.Destroy();
		_swapchain.DestroySurface();

		GraphicsScheduler().Deinitialize();
		RenderTaskScheduler::InstanceCtor::Destroy();
		_resMngr  = null;
		_resMngr2 = Default;

	  #if ENABLE_SYNC_LOG
		_syncLog.Deinitialize( INOUT _device.EditDeviceFnTable() );
		_syncLog.Disable();
	  #endif

		_device.DestroyLogicalDevice();
		_device.DestroyInstance();

		Scheduler().Release();
		TaskScheduler::InstanceCtor::Destroy();
	}

/*
=================================================
	CreateSwapchain
=================================================
*/
	bool  GraphicsLibImpl::CreateSwapchain (const uint2 &viewSize, const SwapchainDesc &desc, NativeWindow nativeWnd) __NE___
	{
		if_unlikely( not _swapchain.IsSurfaceInitialized() )
		{
			CHECK_ERR( nativeWnd );
			CHECK_ERR( _swapchain.CreateSurface( nativeWnd ));
			CHECK_ERR( _resMngr->OnSurfaceCreated( _swapchain ));
		}

		CHECK_ERR( _swapchain.Create( viewSize, desc ));
		return true;
	}

/*
=================================================
	ReleaseCmdBuf
=================================================
*/
	ND_ static DirectCtx::CommandBuffer  ReleaseCmdBuf (IBaseContext* ctx)
	{
		if ( ctx != null )
		{
			if ( auto* gctx = dynamic_cast<GraphicsContext2Impl *>(ctx) )	return gctx->ReleaseCommandBuffer();
			if ( auto* tctx = dynamic_cast<TransferContext2Impl *>(ctx) )	return tctx->ReleaseCommandBuffer();
			if ( auto* cctx = dynamic_cast<ComputeContext2Impl *>(ctx) )	return cctx->ReleaseCommandBuffer();

			DBG_WARNING( "unknown context type" );
		}
		return Default;
	}

/*
=================================================
	CreateDescriptorUpdater
=================================================
*/
	DescUpdaterPtr  GraphicsLibImpl::CreateDescriptorUpdater () __NE___
	{
		return new DescriptorUpdater2{};
	}

/*
=================================================
	BeginTransferContext
=================================================
*/
	TransferContextPtr  GraphicsLibImpl::BeginTransferContext (IBaseContext* prev) __NE___
	{
		CHECK_ERR( _rtask );
		return new TransferContext2Impl{ *_rtask, ReleaseCmdBuf(prev) };
	}

/*
=================================================
	BeginComputeContext
=================================================
*/
	ComputeContextPtr  GraphicsLibImpl::BeginComputeContext (IBaseContext* prev) __NE___
	{
		CHECK_ERR( _rtask );
		return new ComputeContext2Impl{ *_rtask, ReleaseCmdBuf(prev) };
	}

/*
=================================================
	BeginGraphicsContext
=================================================
*/
	GraphicsContextPtr  GraphicsLibImpl::BeginGraphicsContext (IBaseContext* prev) __NE___
	{
		CHECK_ERR( _rtask );
		return new GraphicsContext2Impl{ *_rtask, ReleaseCmdBuf(prev), _ctxAllocator };
	}

/*
=================================================
	BeginFrame
=================================================
*/
	bool  GraphicsLibImpl::BeginFrame () __NE___
	{
		auto&	rts = GraphicsScheduler();

		CHECK_ERR( rts.WaitNextFrame( EThreadArray{ ETaskQueue::Renderer }, _timeout ));
		CHECK_ERR( rts.BeginFrame() );

		_cmdBatch = rts.BeginCmdBatch( EQueueType::Graphics, 0 );
		CHECK_ERR( _cmdBatch );

		if_unlikely( not _AcquireSwapchainImage() )
		{
			CHECK_ERR( _AcquireSwapchainImage() );
		}

		CHECK_ERR( _cmdBatch->AddInputSemaphore(  _swapchain.GetImageAvailableSemaphore(), 0 ));
		CHECK_ERR( _cmdBatch->AddOutputSemaphore( _swapchain.GetRenderFinishedSemaphore(), 0 ));

		_rtask = MakeRC<RenderTaskImpl>( ArgRef(*this), _cmdBatch );

		_cmdBatch->RunTask( _rtask, Tuple{}, null, null, False{} );
		CHECK_ERR( not _rtask->IsInterrupted() );

		_ctxAllocator.Discard();

		return true;
	}

/*
=================================================
	_AcquireSwapchainImage
=================================================
*/
	bool  GraphicsLibImpl::_AcquireSwapchainImage () __NE___
	{
		if ( _recreateSwapchain.load() )
		{
			auto	desc = _swapchain.GetDescription();
			CHECK_ERR( _swapchain.Create( uint2{}, desc ));
			_recreateSwapchain.store( false );
		}

	  #if defined(AE_ENABLE_VULKAN)
		VkResult	err = _swapchain.AcquireNextImage();
		switch ( err )
		{
			case_likely VK_SUCCESS :
				return true;

			case VK_SUBOPTIMAL_KHR :
				_recreateSwapchain.store( true );
				return true;

			case VK_ERROR_OUT_OF_DATE_KHR :
			case VK_TIMEOUT :
			case VK_NOT_READY :
			case VK_ERROR_SURFACE_LOST_KHR :
			default :
				_recreateSwapchain.store( true );
				return false;
		}

	  #elif defined(AE_ENABLE_METAL)
		UNTESTED;

	  #endif
	}

/*
=================================================
	GetTargets
=================================================
*/
	bool  GraphicsLibImpl::GetTargets (OUT App::IOutputSurface::RenderTargets_t &targets) __NE___
	{
		targets.clear();
		CHECK_ERR( _swapchain.IsImageAcquired() );	// must be between 'Begin()' / 'End()'

		auto	image_and_view	= _swapchain.GetCurrentImageAndViewID();
		auto&	dst				= targets.emplace_back();
		auto	sw_desc			= _swapchain.GetDescription();

		dst.imageId			= image_and_view.image;
		dst.viewId			= image_and_view.view;
		dst.region			= RectI{ int2{0}, int2{_swapchain.GetSurfaceSize()} };

		dst.pixToMm			= 0.f;
		dst.format			= sw_desc.colorFormat;
		dst.colorSpace		= sw_desc.colorSpace;

		dst.initialState	= EResourceState::PresentImage;
		dst.finalState		= EResourceState::PresentImage;

		return true;
	}

/*
=================================================
	EndFrame
=================================================
*/
	bool  GraphicsLibImpl::EndFrame (IBaseContext* ctx) __NE___
	{
		CHECK_ERR( ctx != null );
		CHECK_ERR( _rtask );
		CHECK_ERR( _cmdBatch );

		if ( auto* gctx = dynamic_cast<GraphicsContext2Impl *>(ctx) )	_rtask->Execute( *gctx );	else
		if ( auto* tctx = dynamic_cast<TransferContext2Impl *>(ctx) )	_rtask->Execute( *tctx );	else
		if ( auto* cctx = dynamic_cast<ComputeContext2Impl *>(ctx) )	_rtask->Execute( *cctx );	else
																		DBG_WARNING( "unknown context type" );
		CHECK_ERR( _cmdBatch->EndRecordingAndSubmit() );

		auto&	rts = GraphicsScheduler();
		auto	end = rts.EndFrame( Tuple{_rtask} );
		CHECK_ERR( Scheduler().Wait( {end}, EThreadArray{ETaskQueue::Renderer}, _timeout ));
		CHECK_ERR( end->IsCompleted() );

		_rtask		= null;
		_cmdBatch	= null;

	  #if defined(AE_ENABLE_VULKAN)
		auto		q	= _device.GetQueue( EQueueType::Graphics );
		VkResult	err = _swapchain.Present( q );
		switch ( err )
		{
			case_likely VK_SUCCESS :
				break;

			case VK_SUBOPTIMAL_KHR :
				_recreateSwapchain.store( true );	break;

			case VK_ERROR_OUT_OF_DATE_KHR :
			case VK_TIMEOUT :
			case VK_NOT_READY :
			case VK_ERROR_SURFACE_LOST_KHR :
			default :
				_recreateSwapchain.store( true );	break;
		}

	  #elif defined(AE_ENABLE_METAL)
		UNTESTED;

	  #endif

	  #if ENABLE_SYNC_LOG
		String	log;
		_syncLog.GetLog( OUT log );
		Unused( log );
	  #endif

		return true;
	}

/*
=================================================
	WaitAll
=================================================
*/
	bool  GraphicsLibImpl::WaitAll (nanoseconds timeout) __NE___
	{
		return GraphicsScheduler().WaitAll( timeout );
	}

} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
	CreateGraphicsLib
=================================================
*/
	extern "C" AE_GLIB_API IGraphicsLib*  CreateGraphicsLib ()
	{
		return new GraphicsLibImpl{};
	}


} // AE::GraphicsLib
