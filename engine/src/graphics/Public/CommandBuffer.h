// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Interfaces should be used only for some low CPU bound code!

	IDrawContext, ITransferContext, IComputeContext, IGraphicsContext, IRayTracingContext, IASBuildContext
		thread-safe:	no
*/

#pragma once

#include "graphics/Public/CommandBufferTypes.h"
#include "graphics/Public/RenderPassDesc.h"
#include "graphics/Public/VulkanTypes.h"
#include "graphics/Public/RayTracingDesc.h"
#include "graphics/Public/ShaderBindingTable.h"

namespace AE::Graphics
{

	//
	// Draw Context interface
	//

	class IDrawContext
	{
	// types
	public:
		using Viewport_t = RenderPassDesc::Viewport;


	// interface
	public:
		// pipeline and shader resources
		virtual void  BindPipeline (GraphicsPipelineID ppln) = 0;
		virtual void  BindPipeline (MeshPipelineID ppln) = 0;
		virtual void  BindPipeline (TilePipelineID ppln) = 0;
		virtual void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) = 0;
		virtual void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages = EShaderStages::AllGraphics) = 0;
		
		// dynamic states
		virtual void  SetViewport (const Viewport_t &viewport) = 0;
		virtual void  SetViewports (ArrayView<Viewport_t> viewports) = 0;
		virtual void  SetScissor (const RectI &scissor) = 0;
		virtual void  SetScissors (ArrayView<RectI> scissors) = 0;
		virtual void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) = 0;
		virtual void  SetDepthBounds (float minDepthBounds, float maxDepthBounds) = 0;
		//virtual void  SetStencilCompareMask (uint compareMask) = 0;								// Vulkan only
		//virtual void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask) = 0;	// Vulkan only
		//virtual void  SetStencilWriteMask (uint writeMask) = 0;									// Vulkan only
		//virtual void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask) = 0;			// Vulkan only
		virtual void  SetStencilReference (uint reference) = 0;
		virtual void  SetStencilReference (uint frontReference, uint backReference) = 0;
		virtual void  SetBlendConstants (const RGBA32f &color) = 0;

		// draw commands
		virtual void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType) = 0;
		virtual void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset) = 0;
		virtual void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets) = 0;
		virtual bool  BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset) = 0;

		virtual void  Draw (uint vertexCount,
							uint instanceCount	= 1,
							uint firstVertex	= 0,
							uint firstInstance	= 0) = 0;

		void  Draw (const DrawCmd &cmd)  { Draw( cmd.vertexCount, cmd.instanceCount, cmd.firstVertex, cmd.firstInstance ); }

		virtual void  DrawIndexed (uint indexCount,
								   uint instanceCount	= 1,
								   uint firstIndex		= 0,
								   int  vertexOffset	= 0,
								   uint firstInstance	= 0) = 0;

		void  DrawIndexed (const DrawIndexedCmd &cmd)  { DrawIndexed( cmd.indexCount, cmd.instanceCount, cmd.firstIndex, cmd.vertexOffset, cmd.firstInstance ); }

		virtual void  DrawIndirect (BufferID	indirectBuffer,
									Bytes		indirectBufferOffset,
									uint		drawCount,
									Bytes		stride) = 0;

		void  DrawIndirect (const DrawIndirectCmd &cmd)  { DrawIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }

		virtual void  DrawIndexedIndirect (BufferID		indirectBuffer,
										   Bytes		indirectBufferOffset,
										   uint			drawCount,
										   Bytes		stride) = 0;

		void  DrawIndexedIndirect (const DrawIndexedIndirectCmd &cmd)  { DrawIndexedIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }

		// tile shader
		virtual void  DispatchTile () = 0;
		
		// for debugging
		virtual void  DebugMarker (NtStringView text, RGBA8u color) = 0;
		virtual void  PushDebugGroup (NtStringView text, RGBA8u color) = 0;
		virtual void  PopDebugGroup () = 0;
		
		// only for RW attachments
		virtual void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState) = 0;
		virtual void  CommitBarriers () = 0;
		
		// vertex stream
		ND_ virtual bool  AllocVStream (Bytes size, OUT VertexStream &result) = 0;
	};



	//
	// Base Context interface
	//

	class IBaseContext
	{
	// interface
	public:
		virtual void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState) = 0;
		
		// internaly get 'BufferID' from 'BufferViewID'
		virtual void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState) = 0;

		virtual void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState) = 0;
		virtual void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes) = 0;
		
		// internaly get 'ImageID' from 'ImageViewID'
		virtual void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState) = 0;

		virtual void  MemoryBarrier (EResourceState srcState, EResourceState dstState) = 0;
		virtual void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope) = 0;
		virtual void  MemoryBarrier () = 0;

		virtual void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope) = 0;
		virtual void  ExecutionBarrier () = 0;

		virtual void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState) = 0;
		virtual void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue) = 0;
		
		virtual void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState) = 0;
		virtual void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue) = 0;
		
		virtual void  CommitBarriers () = 0;

		// for debugging
		virtual void  DebugMarker (NtStringView text, RGBA8u color) = 0;
		virtual void  PushDebugGroup (NtStringView text, RGBA8u color) = 0;
		virtual void  PopDebugGroup () = 0;
	};

	

	//
	// Transfer Context interface
	//

	class ITransferContext : public IBaseContext
	{
	// interface
	public:
		virtual void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data) = 0;
		//virtual void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data) = 0;		// Vulkan only
		
		virtual void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges) = 0;
		virtual void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges) = 0;
		virtual void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges) = 0;
		virtual void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges) = 0;
		virtual void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges) = 0;
		virtual void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges) = 0;
		
		// write to device local memory using staging buffer
		ND_			Bytes UploadBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data, EStagingHeapType heapType = EStagingHeapType::Static);
			virtual void  UploadBuffer (BufferID buffer, Bytes offset, Bytes size, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static) = 0;

		ND_			Bytes UploadImage (ImageID image, const UploadImageDesc &desc, const void* data, Bytes size);
			virtual void  UploadImage  (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView) = 0;

		// read from device local memory using staging buffer
		ND_ virtual Promise<BufferMemView>  ReadbackBuffer (BufferID buffer, Bytes offset, Bytes size, EStagingHeapType heapType = EStagingHeapType::Static) = 0;
		ND_ virtual Promise<ImageMemView>   ReadbackImage (ImageID image, const ReadbackImageDesc &desc) = 0;

		// partially upload
			virtual void  UploadBuffer (INOUT BufferStream &stream, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static) = 0;
			virtual void  UploadImage (INOUT ImageStream &stream, OUT ImageMemView &memView, EStagingHeapType heapType = EStagingHeapType::Dynamic) = 0;

		// partially read
		//ND_ virtual Promise<BufferMemView>  ReadbackBuffer (BufferStream &stream, EStagingHeapType heapType = EStagingHeapType::Static) = 0;
		//ND_ virtual Promise<ImageMemView>   ReadbackImage (ImageStream &stream, EStagingHeapType heapType = EStagingHeapType::Static) = 0;

		// only for host-visible memory
		ND_	virtual bool  UpdateHostBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data) = 0;
		ND_ virtual bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped) = 0;

		ND_ virtual Promise<BufferMemView>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size) = 0;
		

	// only in compute queue //
	
		//virtual void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges) = 0;		// Vulkan only
		//virtual void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges) = 0;		// Vulkan only
		//virtual void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges) = 0;		// Vulkan only
		
	// only in graphics queue //

		//virtual void  ClearDepthStencilImage (ImageID image, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges) = 0;	// Vulkan only
		//virtual void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions) = 0;					// Vulkan only
		//virtual void  ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions) = 0;;								// Vulkan only
		virtual void  GenerateMipmaps (ImageID image) = 0;


	public:
		// only for host-visible memory
		template <typename T>	ND_ bool  UpdateHostBuffer (BufferID buffer, Bytes offset, ArrayView<T> data)		{ return UpdateHostBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }
		template <typename T>	ND_ bool  UpdateHostBuffer (BufferID buffer, Bytes offset, const Array<T> &data)	{ return UpdateHostBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }
		
		// copy to device local memory using staging buffer
		template <typename T>	ND_ Bytes  UploadImage (ImageID image, const UploadImageDesc &desc, ArrayView<T> data)		{ return UploadImage( image, desc, data.data(), ArraySizeOf(data) ); }
		template <typename T>	ND_ Bytes  UploadImage (ImageID image, const UploadImageDesc &desc, const Array<T> &data)	{ return UploadImage( image, desc, data.data(), ArraySizeOf(data) ); }
		
		// copy to device local memory using staging buffer
		template <typename T>	ND_ Bytes  UploadBuffer (BufferID buffer, Bytes offset, ArrayView<T> data, EStagingHeapType heapType = EStagingHeapType::Static)	{ return UploadBuffer( buffer, offset, ArraySizeOf(data), data.data(), heapType ); }
		template <typename T>	ND_ Bytes  UploadBuffer (BufferID buffer, Bytes offset, const Array<T> &data, EStagingHeapType heapType = EStagingHeapType::Static)	{ return UploadBuffer( buffer, offset, ArraySizeOf(data), data.data(), heapType ); }
	};



	//
	// Compute Context interface
	//

	class IComputeContext : public IBaseContext
	{
	// interface
	public:
		virtual void  BindPipeline (ComputePipelineID ppln) = 0;
		virtual void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) = 0;
		virtual void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages) = 0;
		
		virtual void  Dispatch (const uint3 &groupCount) = 0;
		virtual void  DispatchIndirect (BufferID buffer, Bytes offset) = 0;
		virtual void  DispatchBase (const uint3 &baseGroup, const uint3 &groupCount) = 0;

		void  Dispatch (const uint2 &groupCount)								{ return Dispatch( uint3{ groupCount, 1u }); }
		void  DispatchBase (const uint2 &baseGroup, const uint2 &groupCount)	{ return DispatchBase( uint3{ baseGroup, 0u }, uint3{ groupCount, 1u }); }
	};



	//
	// Graphics Context interface
	//

	class IGraphicsContext : public IBaseContext
	{
	// interface
	public:
		//ND_ virtual IDrawContext*	BeginRenderPass (const RenderPassDesc &desc) = 0;
		//ND_ virtual IDrawContext*	NextSubpass () = 0;
		//	virtual void			EndRenderPass () = 0;

		//virtual VSecondaryBatch*  BeginMtRenderPass () = 0;
		//virtual void  ExecuteCommands () = 0;
		//virtual VSecondaryBatch*  NextMtSubpass () = 0;
	};



	//
	// Ray Tracing Context interface
	//

	class IRayTracingContext : public IBaseContext
	{
	// interface
	public:
		virtual void  BindPipeline (RayTracingPipelineID ppln) = 0;
		virtual void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) = 0;
		virtual void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages) = 0;
		//virtual void  StackSize () = 0;
		
		//virtual void  TraceRays (const uint2 dim, const ShaderBindingTable &sbt) = 0;
		//virtual void  TraceRays (const uint3 dim, const ShaderBindingTable &sbt) = 0;
		virtual void  TraceRaysIndirect () = 0;
	};



	//
	// Acceleration Structure Build Context interface
	//

	class IASBuildContext : public IBaseContext
	{
	// interface
	public:
		virtual void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst) = 0;
		virtual void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst) = 0;
		virtual void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone) = 0;
		
		virtual void  Build  (const RTSceneBuild &cmd, RTSceneID dst) = 0;
		virtual void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst) = 0;
		virtual void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone) = 0;

		// TODO:
		//	- build indirect

		//virtual void  WriteCompactedSize () = 0;
	};


}	// AE::Graphics
