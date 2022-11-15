// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Interfaces should be used only for some low CPU bound code!

	IDrawContext, ITransferContext, IComputeContext, IGraphicsContext, IRayTracingContext, IASBuildContext
		thread-safe:	no

		exceptions:		yes			- for indirect command buffer
						optional	- for non-portable: 'EndCommandBuffer()'
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
		virtual void  BindPipeline (GraphicsPipelineID ppln)																		__TH___	= 0;
		virtual void  BindPipeline (MeshPipelineID ppln)																			__TH___	= 0;
		virtual void  BindPipeline (TilePipelineID ppln)																			__TH___	= 0;
		virtual void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)					__TH___ = 0;
		virtual void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages = EShaderStages::AllGraphics)__TH___	= 0;
		
		// dynamic states
		virtual void  SetViewport (const Viewport_t &viewport)																		__TH___	= 0;
		virtual void  SetViewports (ArrayView<Viewport_t> viewports)																__TH___	= 0;
		virtual void  SetScissor (const RectI &scissor)																				__TH___	= 0;
		virtual void  SetScissors (ArrayView<RectI> scissors)																		__TH___	= 0;
		virtual void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)				__TH___ = 0;
		virtual void  SetStencilReference (uint reference)																			__TH___	= 0;
		virtual void  SetStencilReference (uint frontReference, uint backReference)													__TH___	= 0;
		virtual void  SetBlendConstants (const RGBA32f &color)																		__TH___	= 0;
		//      void  SetDepthBounds (float minDepthBounds, float maxDepthBounds);									// Vulkan only
		//      void  SetStencilCompareMask (uint compareMask);														// Vulkan only
		//      void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask);							// Vulkan only
		//      void  SetStencilWriteMask (uint writeMask);															// Vulkan only
		//      void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask);								// Vulkan only

		// draw commands
		virtual void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)												__TH___	= 0;
		virtual void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)													__TH___	= 0;
		virtual void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)					__TH___	= 0;
		virtual bool  BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset)		__TH___ = 0;

		virtual void  Draw (uint vertexCount,
							uint instanceCount	= 1,
							uint firstVertex	= 0,
							uint firstInstance	= 0)						__TH___	= 0;

		void  Draw (const DrawCmd &cmd)										__TH___	{ Draw( cmd.vertexCount, cmd.instanceCount, cmd.firstVertex, cmd.firstInstance ); }

		virtual void  DrawIndexed (uint indexCount,
								   uint instanceCount	= 1,
								   uint firstIndex		= 0,
								   int  vertexOffset	= 0,
								   uint firstInstance	= 0)				__TH___	= 0;

		void  DrawIndexed (const DrawIndexedCmd &cmd)						__TH___	{ DrawIndexed( cmd.indexCount, cmd.instanceCount, cmd.firstIndex, cmd.vertexOffset, cmd.firstInstance ); }

		virtual void  DrawIndirect (BufferID	indirectBuffer,
									Bytes		indirectBufferOffset,
									uint		drawCount,
									Bytes		stride)						__TH___	= 0;

		void  DrawIndirect (const DrawIndirectCmd &cmd)						__TH___	{ DrawIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }

		virtual void  DrawIndexedIndirect (BufferID		indirectBuffer,
										   Bytes		indirectBufferOffset,
										   uint			drawCount,
										   Bytes		stride)				__TH___ = 0;

		void  DrawIndexedIndirect (const DrawIndexedIndirectCmd &cmd)		__TH___ { DrawIndexedIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }

		// tile shader
		virtual void  DispatchTile ()										__TH___ = 0;
		
		// mesh shader
		virtual void  DrawMeshTasks (const uint3 &taskCount)				__TH___	= 0;

		virtual void  DrawMeshTasksIndirect (BufferID	indirectBuffer,
											 Bytes		indirectBufferOffset,
											 uint		drawCount,
											 Bytes		stride)				__TH___	= 0;

		// for debugging
		virtual void  DebugMarker (NtStringView text, RGBA8u color)			__TH___	= 0;
		virtual void  PushDebugGroup (NtStringView text, RGBA8u color)		__TH___	= 0;
		virtual void  PopDebugGroup ()										__TH___	= 0;
		
		// only for RW attachments
		virtual void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)	__TH___	= 0;
		virtual void  CommitBarriers ()																			__TH___ = 0;
		
		// vertex stream
		ND_ virtual bool  AllocVStream (Bytes size, OUT VertexStream &result)									__TH___ = 0;
	};



	//
	// Base Context interface
	//

	class IBaseContext
	{
	// interface
	public:
		virtual void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)										__TH___	= 0;
		
		// internaly get 'BufferID' from 'BufferViewID'
		virtual void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)								__TH___	= 0;

		virtual void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)										__TH___	= 0;
		virtual void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)	__TH___	= 0;
		
		// internaly get 'ImageID' from 'ImageViewID'
		virtual void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)									__TH___	= 0;

		virtual void  MemoryBarrier (EResourceState srcState, EResourceState dstState)														__TH___	= 0;
		virtual void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__TH___	= 0;
		virtual void  MemoryBarrier ()																										__TH___	= 0;

		virtual void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__TH___	= 0;
		virtual void  ExecutionBarrier ()																									__TH___	= 0;

		virtual void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		__TH___	= 0;
		virtual void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		__TH___	= 0;
		
		virtual void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__TH___	= 0;
		virtual void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__TH___	= 0;
		
		virtual void  CommitBarriers ()																										__TH___	= 0;

		// for debugging
		virtual void  DebugMarker (NtStringView text, RGBA8u color)																			__TH___	= 0;
		virtual void  PushDebugGroup (NtStringView text, RGBA8u color)																		__TH___	= 0;
		virtual void  PopDebugGroup ()																										__TH___	= 0;
	};

	

	//
	// Transfer Context interface
	//

	class ITransferContext : public IBaseContext
	{
	// interface
	public:
		virtual void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data)								__TH___	= 0;
		virtual void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)					__TH___	= 0; // Vulkan - native, Metal - used UploadBuffer()
		
		virtual void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)				__TH___	= 0;
		virtual void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)					__TH___	= 0;
		virtual void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)	__TH___	= 0;
		virtual void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)	__TH___	= 0;
		virtual void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)	__TH___	= 0;
		virtual void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)	__TH___	= 0;
		
		// write to device local memory using staging buffer
		ND_			bool  UploadBuffer (BufferID buffer, Bytes offset, Bytes dataSize, const void* data, EStagingHeapType heapType = EStagingHeapType::Static)				__TH___;
			virtual void  UploadBuffer (BufferID buffer, Bytes offset, Bytes requiredSize, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static)__TH___	= 0;

		ND_			Bytes UploadImage (ImageID image, const UploadImageDesc &desc, const void* data, Bytes size)	__TH___;
			virtual void  UploadImage  (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)		__TH___	= 0;

		// read from device local memory using staging buffer
		ND_ virtual Promise<BufferMemView>  ReadbackBuffer (BufferID buffer, Bytes offset, Bytes size, EStagingHeapType heapType = EStagingHeapType::Static)__TH___ = 0;
		ND_ virtual Promise<ImageMemView>   ReadbackImage (ImageID image, const ReadbackImageDesc &desc)													__TH___	= 0;

		// partially upload
			virtual void  UploadBuffer (INOUT BufferStream &stream, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static)		__TH___ = 0;
			virtual void  UploadImage (INOUT ImageStream &stream, OUT ImageMemView &memView, EStagingHeapType heapType = EStagingHeapType::Dynamic)			__TH___ = 0;

		// partially read
		//ND_ Promise<BufferMemView>  ReadbackBuffer (BufferStream &stream, EStagingHeapType heapType = EStagingHeapType::Static)					__TH___;
		//ND_ Promise<ImageMemView>   ReadbackImage (ImageStream &stream, EStagingHeapType heapType = EStagingHeapType::Static)						__TH___;

		// only for host-visible memory
		ND_	virtual bool  UpdateHostBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)			__TH___ = 0;
		//          bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped);					// Vulkan only

		ND_ virtual Promise<ArrayView<ubyte>>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)			__TH___ = 0;
		
		ND_ virtual uint3  MinImageTransferGranularity ()															C_NE___ = 0;

	// only in compute queue //
	
		// void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges);		// Vulkan only
		// void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges);		// Vulkan only
		// void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges);		// Vulkan only
		
	// only in graphics queue //

		// void  ClearDepthStencilImage (ImageID image, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges) = 0;	// Vulkan only
		// void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions) = 0;					// Vulkan only
		// void  ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions) = 0;;								// Vulkan only

		virtual void  GenerateMipmaps (ImageID image)																__TH___	= 0;


	public:
		// only for host-visible memory
		template <typename T>	ND_ bool  UpdateHostBuffer (BufferID buffer, Bytes offset, ArrayView<T> data)		__TH___	{ return UpdateHostBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }
		template <typename T>	ND_ bool  UpdateHostBuffer (BufferID buffer, Bytes offset, const Array<T> &data)	__TH___	{ return UpdateHostBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }
		
		// copy to device local memory using staging buffer
		template <typename T>	ND_ Bytes  UploadImage (ImageID image, const UploadImageDesc &desc, ArrayView<T> data)		__TH___	{ return UploadImage( image, desc, data.data(), ArraySizeOf(data) ); }
		template <typename T>	ND_ Bytes  UploadImage (ImageID image, const UploadImageDesc &desc, const Array<T> &data)	__TH___	{ return UploadImage( image, desc, data.data(), ArraySizeOf(data) ); }
		
		// copy to device local memory using staging buffer
		template <typename T>	ND_ bool  UploadBuffer (BufferID buffer, Bytes offset, ArrayView<T> data, EStagingHeapType heapType = EStagingHeapType::Static)		__TH___	{ return UploadBuffer( buffer, offset, ArraySizeOf(data), data.data(), heapType ); }
		template <typename T>	ND_ bool  UploadBuffer (BufferID buffer, Bytes offset, const Array<T> &data, EStagingHeapType heapType = EStagingHeapType::Static)	__TH___	{ return UploadBuffer( buffer, offset, ArraySizeOf(data), data.data(), heapType ); }
	};



	//
	// Compute Context interface
	//

	class IComputeContext : public IBaseContext
	{
	// interface
	public:
		virtual void  BindPipeline (ComputePipelineID ppln)															__TH___	= 0;
		virtual void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)	__TH___	= 0;
		virtual void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)				__TH___	= 0;
		
		virtual void  Dispatch (const uint3 &groupCount)															__TH___	= 0;
		virtual void  DispatchIndirect (BufferID buffer, Bytes offset)												__TH___	= 0;

				void  Dispatch (const uint2 &groupCount)															__TH___	{ return Dispatch( uint3{ groupCount, 1u }); }
	};



	//
	// Graphics Context interface
	//

	class IGraphicsContext : public IBaseContext
	{
	// interface
	public:
		//ND_ virtual IDrawContext*	BeginRenderPass (const RenderPassDesc &desc)	__TH___	= 0;
		//ND_ virtual IDrawContext*	NextSubpass ()									__TH___	= 0;
		//	virtual void			EndRenderPass ()								__TH___	= 0;

		//virtual VSecondaryBatch*  BeginMtRenderPass ()							__TH___	= 0;
		//virtual void				ExecuteCommands ()								__TH___	= 0;
		//virtual VSecondaryBatch*  NextMtSubpass ()								__TH___	= 0;
	};



	//
	// Ray Tracing Context interface
	//

	class IRayTracingContext : public IBaseContext
	{
	// interface
	public:
		virtual void  BindPipeline (RayTracingPipelineID ppln)														__TH___	= 0;
		virtual void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)	__TH___	= 0;
		virtual void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)				__TH___	= 0;
		//virtual void  SetStackSize ()																				__TH___	= 0;
		
		//virtual void  TraceRays (const uint2 dim, const ShaderBindingTable &sbt)	__TH___	= 0;
		//virtual void  TraceRays (const uint3 dim, const ShaderBindingTable &sbt)	__TH___	= 0;
		virtual void  TraceRaysIndirect ()											__TH___	= 0;
	};



	//
	// Acceleration Structure Build Context interface
	//

	class IASBuildContext : public IBaseContext
	{
	// interface
	public:
		virtual void  Build  (const RTGeometryBuild &cmd, RTGeometryID dst)										__TH___	= 0;
		virtual void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)					__TH___	= 0;
		virtual void  Copy   (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)	__TH___	= 0;
		
		virtual void  Build  (const RTSceneBuild &cmd, RTSceneID dst)											__TH___	= 0;
		virtual void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)							__TH___	= 0;
		virtual void  Copy   (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)			__TH___	= 0;

		// TODO:
		//	- build indirect

		// 'dstBuffer' must be in EREsourceState::CopyDst
		virtual void  WriteCompactedSize (RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size)		__TH___	= 0;
		virtual void  WriteCompactedSize (RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size)			__TH___	= 0;

		ND_ virtual Promise<Bytes>  ReadCompactedSize (RTGeometryID as)											__TH___	= 0;
		ND_ virtual Promise<Bytes>  ReadCompactedSize (RTSceneID as)											__TH___	= 0;
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	UploadImage
=================================================
*/
	inline Bytes  ITransferContext::UploadImage (ImageID imageId, const UploadImageDesc &uploadDesc, const void* data, Bytes size) __TH___
	{
		ImageMemView	mem_view;
		UploadImage( imageId, uploadDesc, OUT mem_view );

		Bytes	written = 0_b;
		for (auto& dst : mem_view.Parts())
		{
			MemCopy( OUT dst.ptr, data + written, dst.size );
			written += dst.size;
		}

		ASSERT( written <= size );
		Unused( size );

		return written;
	}
		
/*
=================================================
	UploadBuffer
=================================================
*/
	inline bool  ITransferContext::UploadBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data, EStagingHeapType heapType) __TH___
	{
		BufferMemView	ranges;
		UploadBuffer( buffer, offset, size, OUT ranges, heapType );

		Bytes	written = 0_b;
		for (auto& dst : ranges)
		{
			MemCopy( OUT dst.ptr, data + written, dst.size );
			written += dst.size;
		}
		ASSERT( written <= size );
		return written == size;
	}


} // AE::Graphics
