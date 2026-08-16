// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Interfaces should be used only for some low CPU bound code!

	IDrawContext, ITransferContext, IComputeContext, IGraphicsContext, IRayTracingContext, IASBuildContext
		Thread-safe:	no

		exceptions:
			- for direct/indirect command buffers, if resource is not alive.
			- for direct/indirect command buffers, if AE_GRAPHICS_STRONG_VALIDATION are enabled.
			- for indirect command buffer, if mem allocation failed.
			- for non-portable: 'EndCommandBuffer()', if failed to end command buffer recording.

		Resource state tracking:
			- only manual state transition.
			- required state in method description.
			- supported states for resources in DescriptorSet:
				buffers:	ShaderUniform, ShaderStorage_RW
				image:		ShaderSample,  ShaderStorage_RW, InputColorAttachment, InputColorAttachment_RW,
							InputDepthStencilAttachment, InputDepthStencilAttachment_RW, DepthStencilTest_ShaderSample
				rtas:		ShaderRTAS

*/

#pragma once

#include "graphics_rhi/Public/CommandBufferTypes.h"
#include "graphics_rhi/Public/RenderPassDesc.h"
#include "graphics_rhi/Public/VulkanTypes.h"
#include "graphics_rhi/Public/RayTracingDesc.h"
#include "graphics_rhi/Public/QueryManager.h"

#define GAPI_DEPENDENT( ... )

namespace AE::Graphics
{

	//
	// Indirect Command Execution Context interface
	//

	class IIndirectCommandExecutionContext
	{
	// interface
	public:

		// indirect commands //
		virtual void  BindInitialPipeline (IndirectExecutionSetID)																	__Th___ = 0;

		virtual void  ExecuteGeneratedCommands (const ExecuteGeneratedCommandsCmd &)												__Th___ = 0;
		virtual void  ExecuteGeneratedCommands (const ExecuteGeneratedCommands2Cmd &)												__Th___ = 0;
	};



	//
	// Draw Context interface
	//

	class IDrawContext : public IIndirectCommandExecutionContext
	{
	// interface
	public:

	// pipeline and shader resources //
		virtual void  BindPipeline (GraphicsPipelineID ppln)																		__Th___	= 0;
		virtual void  BindPipeline (MeshPipelineID ppln)																			__Th___	= 0;
		virtual void  BindPipeline (TilePipelineID ppln)																			__Th___	= 0;

		virtual void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th___ = 0;

		virtual void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th___	= 0;
		template <typename T> void  PushConstant (const PushConstantIndex &idx, const T &data)										__Th___	{ return PushConstant( idx, Sizeof(data), &data, T::TypeName ); }

	// dynamic states //
		virtual void  SetViewport (const Viewport &viewport)																		__Th___	= 0;
		virtual void  SetViewports (ArrayView<Viewport> viewports)																	__Th___	= 0;
		virtual void  SetScissor (const RectI &scissor)																				__Th___	= 0;
		virtual void  SetScissors (ArrayView<RectI> scissors)																		__Th___	= 0;
		//	requires: EPipelineDynamicState::DepthBias
		virtual void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)				__Th___ = 0;
		//	requires: EPipelineDynamicState::StencilReference
		virtual void  SetStencilReference (uint reference)																			__Th___	= 0;
		virtual void  SetStencilReference (uint frontReference, uint backReference)													__Th___	= 0;
		//	requires: EPipelineDynamicState::BlendConstants
		virtual void  SetBlendConstants (const RGBA32f &color)																		__Th___	= 0;

	// draw commands //

		//		buffer:  EResourceState::IndexBuffer
		virtual void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)												__Th___	= 0;
		//		buffer:  EResourceState::VertexBuffer
		virtual void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)													__Th___	= 0;
		virtual void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)					__Th___	= 0;
		virtual bool  BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, BufferID buffer, Bytes offset)		__Th___ = 0;

		// for Draw*() :
		//   VertexIndex   range:  [firstVertex,   firstVertex   + vertexCount]
		//   InstanceIndex range:  [firstInstance, firstInstance + instanceCount]

		// for DrawIndexed*() :
		//   VertexIndex   value:  indexBuffer[firstIndex + i] + vertexOffset
		//   InstanceIndex range:  [firstInstance, firstInstance + instanceCount]

		// for Draw*Indirect*() :
		//		indirectBuffer: EResourceState::IndirectBuffer

		// for Draw*Count() :
		//		countBuffer:	EResourceState::IndirectBuffer

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

		//	'indirectBuffer' at 'indirectBufferOffset' must contains array of 'drawCount' elements of type 'DrawIndirectCommand' with stride 'stride'.
				void  DrawIndirect (const DrawIndirectCmd &cmd)																		__Th___	{ DrawIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }
		virtual void  DrawIndirect (BufferID	indirectBuffer,
									Bytes		indirectBufferOffset,
									uint		drawCount,
									Bytes		stride)																				__Th___	= 0;

		//	'indirectBuffer' at 'indirectBufferOffset' must contains array of 'drawCount' elements of type 'DrawIndexedIndirectCommand' with stride 'stride'.
				void  DrawIndexedIndirect (const DrawIndexedIndirectCmd &cmd)														__Th___ { DrawIndexedIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }
		virtual void  DrawIndexedIndirect (BufferID		indirectBuffer,
										   Bytes		indirectBufferOffset,
										   uint			drawCount,
										   Bytes		stride)																		__Th___ = 0;

		// tile shader //
		virtual void  DispatchTile ()																								__Th___ = 0;

		// mesh shader //
		//	'taskCount' must be <= 'DeviceProperties::ComputeProperties::taskGroupCount'.
		//	'taskCount' total count must be <= 'DeviceProperties::ComputeProperties::taskTotalGroups'.
		virtual void  DrawMeshTasks (const uint3 &taskCount)																		__Th___	= 0;

		//	'indirectBuffer' at 'indirectBufferOffset' must contains array of 'drawCount' elements of type 'DrawMeshTasksIndirectCommand' with stride 'stride'.
				void  DrawMeshTasksIndirect (const DrawMeshTasksIndirectCmd &cmd)													__Th___	{ DrawMeshTasksIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }
		virtual void  DrawMeshTasksIndirect (BufferID	indirectBuffer,
											 Bytes		indirectBufferOffset,
											 uint		drawCount,
											 Bytes		stride)																		__Th___	= 0;

		// for debugging //
		virtual void  DebugMarker (DebugLabel dbg)																					__Th___	= 0;
		virtual void  PushDebugGroup (DebugLabel dbg)																				__Th___	= 0;
		virtual void  PopDebugGroup ()																								__Th___	= 0;

		// only for RW attachments //
		virtual void  AttachmentBarrier (AttachmentName::Ref, EResourceState srcState, EResourceState dstState)						__Th___	= 0;
		virtual void  CommitBarriers ()																								__Th___ = 0;

		// clear //
				bool  ClearAttachment (AttachmentName::Ref, const RGBA8u  &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th___;
		virtual bool  ClearAttachment (AttachmentName::Ref, const RGBA32f &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th___ = 0;
		virtual bool  ClearAttachment (AttachmentName::Ref, const RGBA32u &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th___ = 0;
		virtual bool  ClearAttachment (AttachmentName::Ref, const RGBA32i &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th___ = 0;
		virtual bool  ClearAttachment (AttachmentName::Ref, const DepthStencil &, const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th___ = 0;

		// vertex stream //
		ND_ virtual bool  AllocVStream (Bytes size, OUT VertexStream &result)														__Th___ = 0;

		ND_ virtual FrameUID  GetFrameId ()																							C_NE___	= 0;

		GAPI_DEPENDENT(
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
		virtual void  ResourceBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)									__Th___	= 0;

		// internally get 'BufferID' from 'BufferViewID'
		virtual void  ResourceBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)									__Th___	= 0;

		virtual void  ResourceBarrier (ImageID image, EResourceState srcState, EResourceState dstState)										__Th___	= 0;
		virtual void  ResourceBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)__Th___	= 0;

		// internally get 'ImageID' from 'ImageViewID'
		virtual void  ResourceBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)									__Th___	= 0;

		// internally get buffer storage
		virtual void  ResourceBarrier (RTGeometryID id, EResourceState srcState, EResourceState dstState)									__Th___	= 0;
		virtual void  ResourceBarrier (RTSceneID    id, EResourceState srcState, EResourceState dstState)									__Th___	= 0;
		virtual void  ResourceBarrier (RTMicromapID id, EResourceState srcState, EResourceState dstState)									__Th___	= 0;

		virtual void  ResourceBarrier (VideoImageID  id, EResourceState srcState, EResourceState dstState)									__Th___	= 0;
		virtual void  ResourceBarrier (VideoBufferID id, EResourceState srcState, EResourceState dstState)									__Th___	= 0;

		// may be slow on GPU side
		virtual void  MemoryBarrier (EResourceState srcState, EResourceState dstState)														__Th___	= 0;
		virtual void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)														__Th___	= 0;
		virtual void  MemoryBarrier ()																										__Th___	= 0;

		virtual void  ExecutionBarrier (EResourceState srcState, EResourceState dstState)													__Th___ = 0;
		virtual void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													__Th___	= 0;
		virtual void  ExecutionBarrier ()																									__Th___	= 0;

		// only for exclusive ownership
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

		GAPI_DEPENDENT(
			CommandBatch const&		GetCommandBatch ()																						C_NE___;
			CommandBatchPtr			GetCommandBatchRC ()																					C_NE___;
			AccumBar				AccumBarriers ()																						__NE___;
			DeferredBar				DeferredBarriers ()																						__NE___;
			IResourceManager &		GetResourceManager ()																					C_NE___;
			IDevice const&			GetDevice ()																							C_NE___;
			RenderCoroRef			GetRenderTask ()																						C_NE___;
		)
	};



	//
	// Transfer Context interface
	//

	class ITransferContext : public IBaseContext
	{
	// types
	public:
		struct _ReadbackResult
		{
		protected:
			template <typename ...Args>		auto  _Then (Args&& ...args)		__NE___ -> CoroCtorResult<Args...>;
		};

		struct ReadbackBufferResult : _ReadbackResult
		{
			Promise<BufferMemView>		readOp;

			ReadbackBufferResult ()												__NE___ = default;
			explicit ReadbackBufferResult (Promise<BufferMemView> readOp)		__NE___ : readOp{RVRef(readOp)} {}

			template <typename ...Args>		auto  Then (Args&& ...args)			__NE___	{ return _Then( readOp, FwdArg<Args>(args)... ); }
		};


		struct ReadbackImageResult : _ReadbackResult
		{
			Promise<ImageMemView>		readOp;

			ReadbackImageResult ()												__NE___ = default;
			explicit ReadbackImageResult (Promise<ImageMemView> readOp)			__NE___ : readOp{RVRef(readOp)} {}

			template <typename ...Args>		auto  Then (Args&& ...args)			__NE___	{ return _Then( readOp, FwdArg<Args>(args)... ); }
		};


		struct ReadbackBufferResult2 : ReadbackBufferResult
		{
			Bytes						remain;		// non-zero if not enough space to read whole buffer
			DEBUG_ONLY( mutable bool	_isFullyReadChecked = false;)

			ReadbackBufferResult2 ()											__NE___ = default;
			ReadbackBufferResult2 (Promise<BufferMemView> readOp, Bytes remain)	__NE___ : ReadbackBufferResult{RVRef(readOp)}, remain{remain} {}

			ND_ bool  IsFullyRead ()											C_NE___	{ DEBUG_ONLY( _isFullyReadChecked = true; )  return remain == 0; }

			template <typename ...Args>		auto  Then (Args&& ...args)			__NE___	{ ASSERT( _isFullyReadChecked );  return _Then( readOp, FwdArg<Args>(args)... ); }
			template <typename ...Args>		auto  IfFullyRead (Args&& ...args)	__NE___	-> CoroCtorResult<Args...>;
		};


		struct ReadbackImageResult2 : ReadbackImageResult
		{
			uint3						remain;		// non-zero if not enough space to read whole image
			DEBUG_ONLY( mutable bool	_isFullyReadChecked = false;)

			ReadbackImageResult2 ()												__NE___ = default;
			ReadbackImageResult2 (Promise<ImageMemView> readOp, uint3 remain)	__NE___ : ReadbackImageResult{RVRef(readOp)}, remain{remain} {}

			ND_ bool  IsFullyRead ()											C_NE___	{ DEBUG_ONLY( _isFullyReadChecked = true; )  return All( IsZero( remain )); }

			template <typename ...Args>		auto  Then (Args&& ...args)			__NE___	{ ASSERT( _isFullyReadChecked );  return _Then( readOp, FwdArg<Args>(args)... ); }
			template <typename ...Args>		auto  IfFullyRead (Args&& ...args)	__NE___	-> CoroCtorResult<Args...>;
		};

		struct ReadHostBufferResult : _ReadbackResult
		{
			Promise<ArrayView<ubyte>>	readOp;

			ReadHostBufferResult ()												__NE___ = default;
			explicit ReadHostBufferResult (Promise<ArrayView<ubyte>> readOp)	__NE___ : readOp{RVRef(readOp)} {}

			template <typename ...Args>		auto  Then (Args&& ...args)			__NE___	{ return _Then( readOp, FwdArg<Args>(args)... ); }
		};


	// interface
	public:
		//		buffer: EResourceState::ClearDst
		virtual void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data)									__Th___	= 0;
		//		buffer: EResourceState::ClearDst
		virtual void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)						__Th___	= 0; // Vulkan - native, Metal - used UploadBuffer()

		//		srcBuffer, srcImage: EResourceState::CopySrc
		//		dstBuffer, dstImage: EResourceState::CopyDst
		virtual void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)					__Th___	= 0;
		virtual void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)						__Th___	= 0;
		virtual void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)		__Th___	= 0;
		virtual void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)		__Th___	= 0;
		virtual void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)		__Th___	= 0;
		virtual void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)		__Th___	= 0;

	// write to device local memory using staging buffer //
		//		buffer: EResourceState::CopyDst
		ND_			bool  UploadBuffer (BufferID buffer, Bytes offset, Bytes dataSize, const void* data,
										EStagingHeapType heapType = EStagingHeapType::Static)							__Th___;
		// 'memView' data size may be <= 'desc.size' but multiple of 'desc.blockSize'.
			virtual void  UploadBuffer (BufferID buffer, const UploadBufferDesc &desc, OUT BufferMemView &memView)		__Th___	= 0;

		//		image: EResourceState::CopyDst
		// 'memView' dimension may be <= 'desc.imageDim' but multiple of texel block size.
			virtual void  UploadImage (ImageID image, const UploadImageDesc &, Bytes maxSize, OUT ImageMemView &)		__Th___	= 0;
			virtual void  UploadImage (VideoImageID image, const UploadImageDesc &, Bytes maxSize, OUT ImageMemView &)	__Th___	= 0;
					void  UploadImage (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)			__Th___;
					void  UploadImage (VideoImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)		__Th___;

	// read from device local memory using staging buffer //
		//		buffer, image: EResourceState::CopySrc
		ND_ virtual ReadbackBufferResult2	ReadbackBuffer (BufferID buffer, const ReadbackBufferDesc &)				__Th___ = 0;
		ND_ virtual ReadbackImageResult2	ReadbackImage (ImageID image, const ReadbackImageDesc &)					__Th___ = 0;
		ND_ virtual ReadbackImageResult2	ReadbackImage (VideoImageID image, const ReadbackImageDesc &)				__Th___ = 0;

	// partially upload //
		//		stream.buffer, stream.image: EResourceState::CopyDst
			virtual void  UploadBuffer (INOUT BufferStream &stream, OUT BufferMemView &memView)							__Th___ = 0;
			virtual void  UploadImage (INOUT ImageStream &stream, OUT ImageMemView &memView)							__Th___ = 0;
			virtual void  UploadImage (INOUT VideoImageStream &stream, OUT ImageMemView &memView)						__Th___ = 0;

	// partially read //
		//		stream.buffer, stream.image: EResourceState::CopySrc
		ND_ virtual ReadbackBufferResult	ReadbackBuffer (INOUT BufferStream &stream)									__Th___ = 0;
		ND_ virtual ReadbackImageResult		ReadbackImage (INOUT ImageStream &stream)									__Th___ = 0;
		ND_ virtual ReadbackImageResult		ReadbackImage (INOUT VideoImageStream &stream)								__Th___ = 0;

	// only for host-visible memory //
		ND_	virtual bool  UpdateHostBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)				__Th___ = 0;

		//		buffer: EResourceState::Host_Read
		ND_ virtual ReadHostBufferResult	ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)					__Th___ = 0;

		ND_ virtual uint3  MinImageTransferGranularity ()																C_NE___ = 0;


	// only in graphics queue //

		//		srcImage: EResourceState::BlitSrc
		//		dstImage: EResourceState::BlitDst
		virtual void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions)	__Th___ = 0;

		//		image:
		//			in:  { src level: EResourceState::BlitSrc, dst levels: EResourceState::Unknown }
		//			out: EResourceState::BlitSrc
		virtual void  GenerateMipmaps (ImageID image, EResourceState state = Default)									__Th___	= 0;
		virtual void  GenerateMipmaps (ImageID image, ArrayView<ImageSubresourceRange> ranges,
									   EResourceState state = Default)													__Th___	= 0;


	public:
		template <typename T>	void  UpdateBuffer (BufferID buffer, Bytes offset, ArrayView<T> data)					__Th___	{ return UpdateBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }
		template <typename T>	void  UpdateBuffer (BufferID buffer, Bytes offset, const Array<T> &data)				__Th___	{ return UpdateBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }

		// only for host-visible memory
		template <typename T>	ND_ bool  UpdateHostBuffer (BufferID buffer, Bytes offset, ArrayView<T> data)			__Th___	{ return UpdateHostBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }
		template <typename T>	ND_ bool  UpdateHostBuffer (BufferID buffer, Bytes offset, const Array<T> &data)		__Th___	{ return UpdateHostBuffer( buffer, offset, ArraySizeOf(data), data.data() ); }

		// copy to device local memory using staging buffer
								ND_ Bytes  UploadImage (ImageID image, const UploadImageDesc &, const void* data, Bytes size)	__Th___;
		template <typename T>	ND_ Bytes  UploadImage (ImageID image, const UploadImageDesc &desc, ArrayView<T> data)			__Th___	{ return UploadImage( image, desc, data.data(), ArraySizeOf(data) ); }
		template <typename T>	ND_ Bytes  UploadImage (ImageID image, const UploadImageDesc &desc, const Array<T> &data)		__Th___ { return UploadImage( image, desc, data.data(), ArraySizeOf(data) ); }

								ND_ Bytes  UploadImage (VideoImageID id, const UploadImageDesc &, const void* data, Bytes size)	__Th___;
		template <typename T>	ND_ Bytes  UploadImage (VideoImageID id, const UploadImageDesc &desc, ArrayView<T> data)		__Th___	{ return UploadImage( id, desc, data.data(), ArraySizeOf(data) ); }
		template <typename T>	ND_ Bytes  UploadImage (VideoImageID id, const UploadImageDesc &desc, const Array<T> &data)		__Th___ { return UploadImage( id, desc, data.data(), ArraySizeOf(data) ); }

		// copy to device local memory using staging buffer
		template <typename T>	ND_ bool  UploadBuffer (BufferID buffer, Bytes offset, ArrayView<T> data,
														EStagingHeapType heapType = EStagingHeapType::Static)			__Th___	{ return UploadBuffer( buffer, offset, ArraySizeOf(data), data.data(), heapType ); }
		template <typename T>	ND_ bool  UploadBuffer (BufferID buffer, Bytes offset, const Array<T> &data,
														EStagingHeapType heapType = EStagingHeapType::Static)			__Th___	{ return UploadBuffer( buffer, offset, ArraySizeOf(data), data.data(), heapType ); }
	};



	//
	// Compute Context interface
	//

	class IComputeContext : public IBaseContext, public IIndirectCommandExecutionContext
	{
	// interface
	public:
		virtual void  BindPipeline (ComputePipelineID ppln)																			__Th___	= 0;
		virtual void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th___	= 0;

		virtual void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th___	= 0;
		template <typename T> void  PushConstant (const PushConstantIndex &idx, const T &data)										__Th___	{ return PushConstant( idx, Sizeof(data), &data, T::TypeName ); }

		//	'groupCount' must be <= 'DeviceProperties::ComputeProperties::computeGroupCount'.
		virtual void  Dispatch (const uint3 &groupCount)																			__Th___	= 0;
				void  Dispatch (const uint2 &groupCount)																			__Th___	{ return Dispatch( uint3{ groupCount, 1u }); }
				void  Dispatch (const uint   groupCount)																			__Th___	{ return Dispatch( uint3{ groupCount, 1u, 1u }); }

		//	'buffer' at 'offset' must contains single objects with 'DispatchIndirectCommand' type.
		//		buffer: EResourceState::IndirectBuffer
		virtual void  DispatchIndirect (BufferID buffer, Bytes offset)																__Th___	= 0;
	};



	//
	// Graphics Context interface
	//

	class IGraphicsContext : public IBaseContext
	{
	// interface
	public:
	GAPI_DEPENDENT(
		// Record draw commands into the same command buffer.
		ND_ virtual IDrawContext		BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg)			__Th___	= 0;
		ND_ virtual IDrawContext		NextSubpass (IDrawContext &prevPassCtx, DebugLabel dbg)					__Th___	= 0;
			virtual void				EndRenderPass (IDrawContext &)											__Th___	= 0;

		// Record draw commands into multiple secondary command buffers asynchronously.
		ND_	virtual RC<DrawCommandBatch> BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg)			__Th___	= 0;	// non-null result or throw exception
		ND_ virtual RC<DrawCommandBatch> NextMtSubpass (const DrawCommandBatch &prevPassBatch, DebugLabel dbg)	__Th___	= 0;	// non-null result or throw exception
			virtual void				 ExecuteCommands (DrawCommandBatch &)									__Th___	= 0;
			virtual void				 ExecuteCommands (DrawCommandBatch &)									__Th___	= 0;
			virtual void				 EndMtRenderPass ()														__Th___	= 0;
	)
	};



	//
	// Ray Tracing Context interface
	//

	class IRayTracingContext : public IBaseContext, public IIndirectCommandExecutionContext
	{
	// interface
	public:
		virtual void  BindPipeline (RayTracingPipelineID ppln)																		__Th___	= 0;
		virtual void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th___	= 0;

		virtual void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th___	= 0;
		template <typename T> void  PushConstant (const PushConstantIndex &idx, const T &data)										__Th___	{ return PushConstant( idx, Sizeof(data), &data, T::TypeName ); }


		//	requires: EPipelineDynamicState::RTStackSize
		virtual void  SetStackSize (Bytes size)																						__Th___	= 0;

		//	'dim' must be <= 'DeviceProperties::RayTracingProperties::maxThreadCount'.
		//	'dim' total count must be <= 'DeviceProperties::RayTracingProperties::maxDispatchInvocations'.
		//		sbt: EResourceState::RTShaderBindingTable
		virtual void  TraceRays (const uint2 dim, RTShaderBindingID sbt)															__Th___	= 0;
		virtual void  TraceRays (const uint3 dim, RTShaderBindingID sbt)															__Th___	= 0;

		virtual void  TraceRays (const uint2 dim, const RTShaderBindingTable &sbt)													__Th___	= 0;
		virtual void  TraceRays (const uint3 dim, const RTShaderBindingTable &sbt)													__Th___	= 0;

		//	'indirectBuffer' at 'indirectBufferOffset' must contains single object with 'TraceRayIndirectCommand' type.
		//		sbt:            EResourceState::RTShaderBindingTable
		//		indirectBuffer: EResourceState::IndirectBuffer
		virtual void  TraceRaysIndirect (RTShaderBindingID sbt,
										 BufferID indirectBuffer, Bytes indirectBufferOffset)										__Th___	= 0;
		virtual void  TraceRaysIndirect (const RTShaderBindingTable &sbt,
										 BufferID indirectBuffer, Bytes indirectBufferOffset)										__Th___	= 0;

		//	'indirectBuffer' at 'indirectBufferOffset' must contains single object with 'TraceRayIndirectCommand2' type.
		//	requires 'rayTracingPipelineTraceRaysIndirect2' feature flag
		virtual void  TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)										__Th___	= 0;
	};



	//
	// Acceleration Structure Build Context interface
	//

	class IASBuildContext : public IBaseContext
	{
	// interface
	public:

	// Vulkan: AS build stage - batch commands for parallel execution

		//		dst: EResourceState::BuildRTAS_Write
		//		all buffers in cmd: EResourceState::BuildRTAS_Read
		virtual void  Build (const RTGeometryBuild &cmd, RTGeometryID dst)															__Th___	= 0;
		virtual void  Build (const RTSceneBuild &cmd, RTSceneID dst)																__Th___	= 0;

		//		src:				EResourceState::BuildRTAS_Read
		//		dst:				EResourceState::BuildRTAS_Write
		//		all buffers in cmd:	EResourceState::BuildRTAS_Read

		// - inactive primitives (vert, AABB, instance) must not be made active
		// - active primitives must not be made inactive
		virtual void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)										__Th___	= 0;
		virtual void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)												__Th___	= 0;


	// Vulkan: AS copy stage - batch commands for parallel execution

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
	IfFullyRead
=================================================
*/
	template <typename ...Args>
	auto  ITransferContext::ReadbackBufferResult2::IfFullyRead (Args&& ...args) __NE___ -> CoroCtorResult<Args...>
	{
		if ( IsFullyRead() )
			return _Then( readOp, FwdArg<Args>(args)... );
		else
			return Default;
	}

	template <typename ...Args>
	auto  ITransferContext::ReadbackImageResult2::IfFullyRead (Args&& ...args) __NE___ -> CoroCtorResult<Args...>
	{
		if ( IsFullyRead() )
			return _Then( readOp, FwdArg<Args>(args)... );
		else
			return Default;
	}

/*
=================================================
	UploadImage (ImageID)
=================================================
*/
	inline Bytes  ITransferContext::UploadImage (ImageID imageId, const UploadImageDesc &uploadDesc, const void* data, Bytes size) __Th___
	{
		ImageMemView	mem_view;
		UploadImage( imageId, uploadDesc, OUT mem_view );

		Bytes	written;
		for (auto& dst : mem_view.Parts())
		{
			MemCopy( OUT dst.ptr, data + written, dst.size );
			written += dst.size;
		}

		ASSERT( written <= size );
		Unused( size );

		return written;
	}

	inline void  ITransferContext::UploadImage (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView) __Th___
	{
		return UploadImage( image, desc, UMax, OUT memView );
	}

/*
=================================================
	UploadImage (VideoImageID)
=================================================
*/
	inline Bytes  ITransferContext::UploadImage (VideoImageID imageId, const UploadImageDesc &uploadDesc, const void* data, Bytes size) __Th___
	{
		ImageMemView	mem_view;
		UploadImage( imageId, uploadDesc, OUT mem_view );

		Bytes	written;
		for (auto& dst : mem_view.Parts())
		{
			MemCopy( OUT dst.ptr, data + written, dst.size );
			written += dst.size;
		}

		ASSERT( written <= size );
		Unused( size );

		return written;
	}

	inline void  ITransferContext::UploadImage (VideoImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView) __Th___
	{
		return UploadImage( image, desc, UMax, OUT memView );
	}

/*
=================================================
	UploadBuffer
=================================================
*/
	inline bool  ITransferContext::UploadBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data, EStagingHeapType heapType) __Th___
	{
		UploadBufferDesc	desc;
		desc.offset		= offset;
		desc.size		= size;
		desc.heapType	= heapType;

		BufferMemView	ranges;
		UploadBuffer( buffer, desc, OUT ranges );

		Bytes	written;
		for (auto& dst : ranges)
		{
			MemCopy( OUT dst.ptr, data + written, dst.size );
			written += dst.size;
		}
		ASSERT( written <= desc.size );
		return written == desc.size;
	}

/*
=================================================
	ClearAttachment
=================================================
*/
	inline bool  IDrawContext::ClearAttachment (AttachmentName::Ref name, const RGBA8u &color, const RectI &region, ImageLayer baseLayer, uint layerCount) __Th___
	{
		return ClearAttachment( name, RGBA32f{color}, region, baseLayer, layerCount );
	}


} // AE::Graphics

#undef GAPI_DEPENDENT
