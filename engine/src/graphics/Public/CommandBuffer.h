// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Interfaces should be used only for some low CPU bound code!

	IDrawContext, ITransferContext, IComputeContext, IGraphicsContext, IRayTracingContext, IASBuildContext
		thread-safe:	no

		exceptions:		yes			- for indirect command buffer
						optional	- for non-portable: 'EndCommandBuffer()'

		Resource state tracking:
			- only manual state transition.
			- required state in method description.
			- supported states for DescriptorSetID:
				buffers:	ShaderUniform, ShaderStorage_RW
				image:		ShaderSample,  ShaderStorage_RW, InputColorAttachment_RW, InputDepthStencilAttachment_RW, DepthStencilTest_ShaderSample
				as:			ShaderRTAS_Read

*/

#pragma once

#include "graphics/Public/CommandBufferTypes.h"
#include "graphics/Public/RenderPassDesc.h"
#include "graphics/Public/VulkanTypes.h"
#include "graphics/Public/RayTracingDesc.h"

#define VULKAN_ONLY( ... )
#define METAL_ONLY( ... )
#define UNIMPLEMENTED( ... )

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

	// pipeline and shader resources //
		virtual void  BindPipeline (GraphicsPipelineID ppln)																		__Th___	= 0;
		virtual void  BindPipeline (MeshPipelineID ppln)																			__Th___	= 0;
		virtual void  BindPipeline (TilePipelineID ppln)																			__Th___	= 0;
		virtual void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th___ = 0;

		virtual void  PushConstant (const PushConstantIndex &idx, Bytes size, const void *values, const ShaderStructName &typeName)	__Th___	= 0;
		template <typename T> void  PushConstant (const PushConstantIndex &idx, const T &data)										__Th___	{ return PushConstant( idx, Sizeof(data), &data, T::TypeName ); }
		
	// dynamic states //
		virtual void  SetViewport (const Viewport_t &viewport)																		__Th___	= 0;
		virtual void  SetViewports (ArrayView<Viewport_t> viewports)																__Th___	= 0;
		virtual void  SetScissor (const RectI &scissor)																				__Th___	= 0;
		virtual void  SetScissors (ArrayView<RectI> scissors)																		__Th___	= 0;
		//		requires: EPipelineDynamicState::DepthBias
		virtual void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)				__Th___ = 0;
		//		requires: EPipelineDynamicState::StencilReference
		virtual void  SetStencilReference (uint reference)																			__Th___	= 0;
		virtual void  SetStencilReference (uint frontReference, uint backReference)													__Th___	= 0;
		//		requires: EPipelineDynamicState::BlendConstants
		virtual void  SetBlendConstants (const RGBA32f &color)																		__Th___	= 0;

		VULKAN_ONLY(
		//		requires: EPipelineDynamicState::DepthBounds
				void  SetDepthBounds (float minDepthBounds, float maxDepthBounds)													__Th___;
		//		requires: EPipelineDynamicState::StencilCompareMask
				void  SetStencilCompareMask (uint compareMask)																		__Th___;
				void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask)											__Th___;
		//		requires: EPipelineDynamicState::StencilWriteMask
				void  SetStencilWriteMask (uint writeMask)																			__Th___;
				void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask)													__Th___;
		)

	// draw commands //
		
		//		buffer:  EResourceState::IndexBuffer
		virtual void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)												__Th___	= 0;
		//		buffer:  EResourceState::VertexBuffer
		virtual void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)													__Th___	= 0;
		virtual void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)					__Th___	= 0;
		virtual bool  BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset)		__Th___ = 0;

				void  Draw (const DrawCmd &cmd)																						__Th___	{ Draw( cmd.vertexCount, cmd.instanceCount, cmd.firstVertex, cmd.firstInstance ); }
		virtual void  Draw (uint vertexCount,
							uint instanceCount	= 1,
							uint firstVertex	= 0,
							uint firstInstance	= 0)																				__Th___	= 0;

				void  DrawIndexed (const DrawIndexedCmd &cmd)																		__Th___	{ DrawIndexed( cmd.indexCount, cmd.instanceCount, cmd.firstIndex, cmd.vertexOffset, cmd.firstInstance ); }
		virtual void  DrawIndexed (uint indexCount,
								   uint instanceCount	= 1,
								   uint firstIndex		= 0,
								   int  vertexOffset	= 0,
								   uint firstInstance	= 0)																		__Th___	= 0;

		//		indirectBuffer: EResourceState::IndirectBuffer
				void  DrawIndirect (const DrawIndirectCmd &cmd)																		__Th___	{ DrawIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }
		virtual void  DrawIndirect (BufferID	indirectBuffer,
									Bytes		indirectBufferOffset,
									uint		drawCount,
									Bytes		stride)																				__Th___	= 0;
		
		//		indirectBuffer: EResourceState::IndirectBuffer
				void  DrawIndexedIndirect (const DrawIndexedIndirectCmd &cmd)														__Th___ { DrawIndexedIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }
		virtual void  DrawIndexedIndirect (BufferID		indirectBuffer,
										   Bytes		indirectBufferOffset,
										   uint			drawCount,
										   Bytes		stride)																		__Th___ = 0;

		// tile shader //
		virtual void  DispatchTile ()																								__Th___ = 0;
		
		// mesh shader //
		virtual void  DrawMeshTasks (const uint3 &taskCount)																		__Th___	= 0;
		
		//		indirectBuffer: EResourceState::IndirectBuffer
				void  DrawMeshTasksIndirect (const DrawMeshTasksIndirectCmd &cmd)													__Th___	{ DrawMeshTasksIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }
		virtual void  DrawMeshTasksIndirect (BufferID	indirectBuffer,
											 Bytes		indirectBufferOffset,
											 uint		drawCount,
											 Bytes		stride)																		__Th___	= 0;

		// extension
		VULKAN_ONLY(
				void  DrawIndirectCount (const DrawIndirectCountCmd &cmd)															__Th___;
				void  DrawIndirectCount (BufferID	indirectBuffer,
										 Bytes		indirectBufferOffset,
										 BufferID	countBuffer,
										 Bytes		countBufferOffset,
										 uint		maxDrawCount,
										 Bytes		stride)																			__Th___;
				
				void  DrawIndexedIndirectCount (const DrawIndexedIndirectCountCmd &cmd)												__Th___;
				void  DrawIndexedIndirectCount (BufferID	indirectBuffer,
												Bytes		indirectBufferOffset,
												BufferID	countBuffer,
												Bytes		countBufferOffset,
												uint		maxDrawCount,
												Bytes		stride)																	__Th___;
				
				void  DrawMeshTasksIndirectCount (const DrawMeshTasksIndirectCountCmd &cmd)											__Th___;
				void  DrawMeshTasksIndirectCount (BufferID	indirectBuffer,
												  Bytes		indirectBufferOffset,
												  BufferID	countBuffer,
												  Bytes		countBufferOffset,
												  uint		maxDrawCount,
												  Bytes		stride)																	__Th___;
		)

		// for debugging //
		virtual void  DebugMarker (DebugLabel dbg)																					__Th___	= 0;
		virtual void  PushDebugGroup (DebugLabel dbg)																				__Th___	= 0;
		virtual void  PopDebugGroup ()																								__Th___	= 0;
		
		// only for RW attachments //
		virtual void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)						__Th___	= 0;
		virtual void  CommitBarriers ()																								__Th___ = 0;
		
		// clear //
		virtual bool  ClearAttachment (AttachmentName, const RGBA32f &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th___ = 0;
		virtual bool  ClearAttachment (AttachmentName, const RGBA32u &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th___ = 0;
		virtual bool  ClearAttachment (AttachmentName, const RGBA32i &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th___ = 0;
		virtual bool  ClearAttachment (AttachmentName, const RGBA8u  &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th___ = 0;
		virtual bool  ClearAttachment (AttachmentName, const DepthStencil &, const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th___ = 0;

		// vertex stream //
		ND_ virtual bool  AllocVStream (Bytes size, OUT VertexStream &result)														__Th___ = 0;

		ND_ virtual FrameUID  GetFrameId ()																							C_NE___	= 0;
		
		UNIMPLEMENTED(
			PrimaryCmdBufState const&	GetPrimaryCtxState ()																		C_NE___;
			DrawCommandBatch const*		GetCommandBatch ()																			C_NE___;	// can be null
		)
	};



	//
	// Base Context interface
	//

	class IBaseContext
	{
	// interface
	public:
		virtual void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)										__Th___	= 0;
		
		// internally get 'BufferID' from 'BufferViewID'
		virtual void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)								__Th___	= 0;

		virtual void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)										__Th___	= 0;
		virtual void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)	__Th___	= 0;
		
		// internally get 'ImageID' from 'ImageViewID'
		virtual void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)									__Th___	= 0;

		virtual void  MemoryBarrier (EResourceState srcState, EResourceState dstState)														__Th___	= 0;
		virtual void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__Th___	= 0;
		virtual void  MemoryBarrier ()																										__Th___	= 0;

		virtual void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__Th___	= 0;
		virtual void  ExecutionBarrier ()																									__Th___	= 0;

		virtual void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		__Th___	= 0;
		virtual void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		__Th___	= 0;
		
		virtual void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)			__Th___	= 0;
		virtual void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)			__Th___	= 0;
		
		virtual void  CommitBarriers ()																										__Th___	= 0;

		// for debugging
		virtual void  DebugMarker (DebugLabel dbg)																							__Th___	= 0;
		virtual void  PushDebugGroup (DebugLabel dbg)																						__Th___	= 0;
		virtual void  PopDebugGroup ()																										__Th___	= 0;

		ND_ virtual FrameUID  GetFrameId ()																									C_NE___	= 0;

		UNIMPLEMENTED(
			CommandBatch const&		GetCommandBatch ()																						C_NE___;
			CommandBatchPtr			GetCommandBatchRC ()																					C_NE___;
			AccumBar				AccumBarriers ()																						__NE___;
			DeferredBar				DeferredBarriers ()																						__NE___;
			IResourceManager &		GetResourceManager ()																					C_NE___;
		)
	};

	

	//
	// Transfer Context interface
	//

	class ITransferContext : public IBaseContext
	{
	// interface
	public:
		//		buffer: EResourceState::CopyDst
		virtual void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data)								__Th___	= 0;
		virtual void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)					__Th___	= 0; // Vulkan - native, Metal - used UploadBuffer()
		
		//		srcBuffer, srcImage: EResourceState::CopySrc
		//		dstBuffer, dstImage: EResourceState::CopyDst
		virtual void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)				__Th___	= 0;
		virtual void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)					__Th___	= 0;
		virtual void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)	__Th___	= 0;
		virtual void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)	__Th___	= 0;
		virtual void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)	__Th___	= 0;
		virtual void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)	__Th___	= 0;
		
	// write to device local memory using staging buffer //
		//		buffer: EResourceState::CopyDst
		ND_			bool  UploadBuffer (BufferID buffer, Bytes offset, Bytes dataSize, const void* data, EStagingHeapType heapType = EStagingHeapType::Static)				__Th___;
			virtual void  UploadBuffer (BufferID buffer, Bytes offset, Bytes requiredSize, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static)__Th___	= 0;
			
		//		image: EResourceState::CopyDst
		ND_			Bytes UploadImage (ImageID image, const UploadImageDesc &desc, const void* data, Bytes size)	__Th___;
			virtual void  UploadImage (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)		__Th___	= 0;

	// read from device local memory using staging buffer //
		//		buffer, image: EResourceState::CopySrc
		ND_ virtual Promise<BufferMemView>  ReadbackBuffer (BufferID buffer, Bytes offset, Bytes size, EStagingHeapType heapType = EStagingHeapType::Static)__Th___ = 0;
		ND_ virtual Promise<ImageMemView>   ReadbackImage (ImageID image, const ReadbackImageDesc &desc)													__Th___	= 0;

	// partially upload //
		//		stream.buffer, stream.image: EResourceState::CopyDst
			virtual void  UploadBuffer (INOUT BufferStream &stream, OUT BufferMemView &memView)						__Th___ = 0;
			virtual void  UploadImage (INOUT ImageStream &stream, OUT ImageMemView &memView)						__Th___ = 0;

	// partially read //
		//		stream.buffer, stream.image: EResourceState::CopySrc
		//ND_ Promise<BufferMemView>  ReadbackBuffer (BufferStream &stream)											__Th___;
		//ND_ Promise<ImageMemView>   ReadbackImage (ImageStream &stream)											__Th___;

	// only for host-visible memory //
		//		buffer: EResourceState::Host_Write
		ND_	virtual bool  UpdateHostBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)			__Th___ = 0;
		VULKAN_ONLY(
					bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped);
		)
		
		//		buffer: EResourceState::Host_Read
		ND_ virtual Promise<ArrayView<ubyte>>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)			__Th___ = 0;
		
		ND_ virtual uint3  MinImageTransferGranularity ()															C_NE___ = 0;


	// only in compute queue //
	
		VULKAN_ONLY(
				void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges);
				void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges);
				void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges);
		)
		

	// only in graphics queue //
	
		//		srcImage: EResourceState::BlitSrc
		//		dstImage: EResourceState::BlitDst
		virtual void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions)	__Th___ = 0;
			
		VULKAN_ONLY(
				void  ClearDepthStencilImage (ImageID image, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges);
				void  ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions);
		)

		//		image:
		//			in: { src level: EResourceState::BlitSrc, dst levels: EResourceState::Unknown }
		//			out: EResourceState::BlitSrc
		virtual void  GenerateMipmaps (ImageID image)																	__Th___	= 0;
		virtual void  GenerateMipmaps (ImageID image, ArrayView<ImageSubresourceRange> ranges)							__Th___	= 0;


	public:
		// only for host-visible memory
		template <typename T>	ND_ bool  UpdateHostBuffer (BufferID buffer, Bytes offset, ArrayView<T> data)		__Th___	{ return UpdateHostBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }
		template <typename T>	ND_ bool  UpdateHostBuffer (BufferID buffer, Bytes offset, const Array<T> &data)	__Th___	{ return UpdateHostBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }
		
		// copy to device local memory using staging buffer
		template <typename T>	ND_ Bytes  UploadImage (ImageID image, const UploadImageDesc &desc, ArrayView<T> data)		__Th___	{ return UploadImage( image, desc, data.data(), ArraySizeOf(data) ); }
		template <typename T>	ND_ Bytes  UploadImage (ImageID image, const UploadImageDesc &desc, const Array<T> &data)	__Th___	{ return UploadImage( image, desc, data.data(), ArraySizeOf(data) ); }
		
		// copy to device local memory using staging buffer
		template <typename T>	ND_ bool  UploadBuffer (BufferID buffer, Bytes offset, ArrayView<T> data, EStagingHeapType heapType = EStagingHeapType::Static)		__Th___	{ return UploadBuffer( buffer, offset, ArraySizeOf(data), data.data(), heapType ); }
		template <typename T>	ND_ bool  UploadBuffer (BufferID buffer, Bytes offset, const Array<T> &data, EStagingHeapType heapType = EStagingHeapType::Static)	__Th___	{ return UploadBuffer( buffer, offset, ArraySizeOf(data), data.data(), heapType ); }
	};



	//
	// Compute Context interface
	//

	class IComputeContext : public IBaseContext
	{
	// interface
	public:
		virtual void  BindPipeline (ComputePipelineID ppln)																			__Th___	= 0;
		virtual void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th___	= 0;

		virtual void  PushConstant (const PushConstantIndex &idx, Bytes size, const void *values, const ShaderStructName &typeName)	__Th___	= 0;
		template <typename T> void  PushConstant (const PushConstantIndex &idx, const T &data)										__Th___	{ return PushConstant( idx, Sizeof(data), &data, T::TypeName ); }
		
		virtual void  Dispatch (const uint3 &groupCount)																			__Th___	= 0;

		//		buffer: EResourceState::IndirectBuffer
		virtual void  DispatchIndirect (BufferID buffer, Bytes offset)																__Th___	= 0;
		
				void  Dispatch (const uint   groupCount)																			__Th___	{ return Dispatch( uint3{ groupCount, 1u, 1u }); }
				void  Dispatch (const uint2 &groupCount)																			__Th___	{ return Dispatch( uint3{ groupCount, 1u }); }
	};



	//
	// Graphics Context interface
	//

	class IGraphicsContext : public IBaseContext
	{
	// interface
	public:
	//	ND_ virtual IDrawContext		BeginRenderPass (const RenderPassDesc &desc)	__Th___	= 0;
	//	ND_ virtual IDrawContext		NextSubpass (IDrawContext &prevPassCtx)			__Th___	= 0;
	//		virtual void				EndRenderPass (IDrawContext &)					__Th___	= 0;

	//	ND_	virtual RC<DrawCommandBatch> BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg)			__Th___	= 0;
	//	ND_ virtual RC<DrawCommandBatch> NextMtSubpass (const DrawCommandBatch &prevPassBatch, DebugLabel dbg)	__Th___	= 0;
	//		virtual void				 ExecuteCommands (DrawCommandBatch &)									__Th___	= 0;
	//		virtual void				 EndMtRenderPass ()
	};



	//
	// Ray Tracing Context interface
	//

	class IRayTracingContext : public IBaseContext
	{
	// interface
	public:
		virtual void  BindPipeline (RayTracingPipelineID ppln)																		__Th___	= 0;
		virtual void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th___	= 0;

		virtual void  PushConstant (const PushConstantIndex &idx, Bytes size, const void *values, const ShaderStructName &typeName)	__Th___	= 0;
		template <typename T> void  PushConstant (const PushConstantIndex &idx, const T &data)										__Th___	{ return PushConstant( idx, Sizeof(data), &data, T::TypeName ); }
		

		//		requires: EPipelineDynamicState::RTStackSize
		virtual void  SetStackSize (Bytes size)																						__Th___	= 0;
		
		//		sbt: EResourceState::RTShaderBindingTable
		virtual void  TraceRays (const uint2 dim, RTShaderBindingID sbt)															__Th___	= 0;
		virtual void  TraceRays (const uint3 dim, RTShaderBindingID sbt)															__Th___	= 0;

		virtual void  TraceRays (const uint2 dim, const RTShaderBindingTable &sbt)													__Th___	= 0;
		virtual void  TraceRays (const uint3 dim, const RTShaderBindingTable &sbt)													__Th___	= 0;
		
		//		sbt:            EResourceState::RTShaderBindingTable
		//		indirectBuffer: EResourceState::IndirectBuffer
		virtual void  TraceRaysIndirect (RTShaderBindingID sbt,
										 BufferID indirectBuffer, Bytes indirectBufferOffset)										__Th___	= 0;
		virtual void  TraceRaysIndirect (const RTShaderBindingTable &sbt,
										 BufferID indirectBuffer, Bytes indirectBufferOffset)										__Th___	= 0;

		virtual void  TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)										__Th___	= 0;
		
		VULKAN_ONLY(
				void  TraceRaysIndirect (const RTShaderBindingTable &sbt, VDeviceAddress address);
				void  TraceRaysIndirect2 (VDeviceAddress address);
		)
	};



	//
	// Acceleration Structure Build Context interface
	//

	class IASBuildContext : public IBaseContext
	{
	// interface
	public:

	// Vulkan: AS build stage //

		//		dst: EResourceState::BuildRTAS_Write
		virtual void  Build (const RTGeometryBuild &cmd, RTGeometryID dst)															__Th___	= 0;
		virtual void  Build (const RTSceneBuild &cmd, RTSceneID dst)																__Th___	= 0;
		
		//		src: EResourceState::BuildRTAS_Read
		//		dst: EResourceState::BuildRTAS_ReadWrite
		virtual void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)										__Th___	= 0;
		virtual void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)												__Th___	= 0;
		
		
	// Vulkan: AS copy stage //
	
		// If used ERTASCopyMode::Compaction 'dst' must have at least 'size = ReadCompactedSize( dst )'.
		//		src: EResourceState::CopyRTAS_Read
		//		dst: EResourceState::CopyRTAS_Write
		virtual void  Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)							__Th___	= 0;
		virtual void  Copy (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)								__Th___	= 0;

		//		as:        EResourceState::CopyRTAS_Read
		//		dstBuffer: EResourceState::CopyDst
		virtual void  WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size = UMax)	__Th___	= 0;
		virtual void  WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size = UMax)		__Th___	= 0;
		
		//		as: EResourceState::CopyRTAS_Read
		ND_ virtual Promise<Bytes>  ReadProperty (ERTASProperty property, RTGeometryID as)											__Th___	= 0;
		ND_ virtual Promise<Bytes>  ReadProperty (ERTASProperty property, RTSceneID as)												__Th___	= 0;
		
		VULKAN_ONLY(
				//		src: EResourceState::CopyRTAS_Read
				//		dst: EResourceState::CopyRTAS_Write
				void  SerializeToMemory (RTGeometryID src, VDeviceAddress dst);
				void  SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset);
		
				void  SerializeToMemory (RTSceneID src, VDeviceAddress dst);
				void  SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset);

				void  DeserializeFromMemory (VDeviceAddress src, RTGeometryID dst);
				void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst);
		
				void  DeserializeFromMemory (VDeviceAddress src, RTSceneID dst);
				void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst);
				
				//		dst: EResourceState::BuildRTAS_Write
				//		requires 'rayTracingPipelineTraceRaysIndirect2' feature flag
			//	void  BuildIndirect (VDeviceAddress indirectMem);
			//	void  BuildIndirect (Bytes indirectBuffer, Bytes indirectBufferOffset);
		)
	};



	//
	// Video Decode
	//

	class IVideoDecodeContext : public IBaseContext
	{
	// interface
	public:
		virtual void  Decode (const VideoDecodeCmd &)	__Th___ = 0;
	};



	//
	// Video Encode
	//

	class IVideoEncodeContext : public IBaseContext
	{
	// interface
	public:
		virtual void  Encode (const VideoEncodeCmd &)	__Th___ = 0;
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	UploadImage
=================================================
*/
	inline Bytes  ITransferContext::UploadImage (ImageID imageId, const UploadImageDesc &uploadDesc, const void* data, Bytes size) __Th___
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
	inline bool  ITransferContext::UploadBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data, EStagingHeapType heapType) __Th___
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

#undef VULKAN_ONLY
#undef METAL_ONLY
#undef UNIMPLEMENTED
