// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Interfaces should be used only for some low CPU bound code!

	IDrawContext, ITransferContext, IComputeContext, IGraphicsContext, IRayTracingContext, IASBuildContext
		thread-safe:	no

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
				as:			ShaderRTAS

*/

#pragma once

#include "graphics/Public/CommandBufferTypes.h"
#include "graphics/Public/RenderPassDesc.h"
#include "graphics/Public/VulkanTypes.h"
#include "graphics/Public/RayTracingDesc.h"
#include "graphics/Public/QueryManager.h"

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
#define GAPI_DEPENDENT( ... )

namespace AE::Graphics
{

	//
	// Draw Context interface
	//

	class NO_VTABLE IDrawContext
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

		VULKAN_ONLY(
		//	requires: EPipelineDynamicState::DepthBounds
		virtual	void  SetDepthBounds (float minDepthBounds, float maxDepthBounds)													__Th___ = 0;
		//	requires: EPipelineDynamicState::StencilCompareMask
		virtual	void  SetStencilCompareMask (uint compareMask)																		__Th___ = 0;
		virtual	void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask)											__Th___ = 0;
		//	requires: EPipelineDynamicState::StencilWriteMask
		virtual	void  SetStencilWriteMask (uint writeMask)																			__Th___ = 0;
		virtual	void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask)													__Th___ = 0;
		//	requires: EPipelineDynamicState::FragmentShadingRate
		virtual	void  SetFragmentShadingRate (EShadingRate, EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp textureOp)	__Th___ = 0;
		//	requires: EPipelineDynamicState::ViewportWScaling
		virtual void  SetViewportWScaling (ArrayView<packed_float2> scaling)														__Th___ = 0;
		)

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

				void  DrawIndirect (const DrawIndirectCmd &cmd)																		__Th___	{ DrawIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }
		virtual void  DrawIndirect (BufferID	indirectBuffer,
									Bytes		indirectBufferOffset,
									uint		drawCount,
									Bytes		stride)																				__Th___	= 0;

				void  DrawIndexedIndirect (const DrawIndexedIndirectCmd &cmd)														__Th___ { DrawIndexedIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }
		virtual void  DrawIndexedIndirect (BufferID		indirectBuffer,
										   Bytes		indirectBufferOffset,
										   uint			drawCount,
										   Bytes		stride)																		__Th___ = 0;

		// tile shader //
		virtual void  DispatchTile ()																								__Th___ = 0;

		// mesh shader //
		virtual void  DrawMeshTasks (const uint3 &taskCount)																		__Th___	= 0;

				void  DrawMeshTasksIndirect (const DrawMeshTasksIndirectCmd &cmd)													__Th___	{ DrawMeshTasksIndirect( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.drawCount, cmd.stride ); }
		virtual void  DrawMeshTasksIndirect (BufferID	indirectBuffer,
											 Bytes		indirectBufferOffset,
											 uint		drawCount,
											 Bytes		stride)																		__Th___	= 0;

		// extension //
		VULKAN_ONLY(
				void  DrawIndirectCount (const DrawIndirectCountCmd &cmd)															__Th___	{ DrawIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
		virtual	void  DrawIndirectCount (BufferID	indirectBuffer,
										 Bytes		indirectBufferOffset,
										 BufferID	countBuffer,
										 Bytes		countBufferOffset,
										 uint		maxDrawCount,
										 Bytes		stride)																			__Th___ = 0;

				void  DrawIndexedIndirectCount (const DrawIndexedIndirectCountCmd &cmd)												__Th___	{ DrawIndexedIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
		virtual	void  DrawIndexedIndirectCount (BufferID	indirectBuffer,
												Bytes		indirectBufferOffset,
												BufferID	countBuffer,
												Bytes		countBufferOffset,
												uint		maxDrawCount,
												Bytes		stride)																	__Th___ = 0;

				void  DrawMeshTasksIndirectCount (const DrawMeshTasksIndirectCountCmd &cmd)											__Th___	{ DrawMeshTasksIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
		virtual	void  DrawMeshTasksIndirectCount (BufferID	indirectBuffer,
												  Bytes		indirectBufferOffset,
												  BufferID	countBuffer,
												  Bytes		countBufferOffset,
												  uint		maxDrawCount,
												  Bytes		stride)																	__Th___ = 0;
		)

		// for debugging //
		virtual void  DebugMarker (DebugLabel dbg)																					__Th___	= 0;
		virtual void  PushDebugGroup (DebugLabel dbg)																				__Th___	= 0;
		virtual void  PopDebugGroup ()																								__Th___	= 0;

		VULKAN_ONLY(
		virtual void  WriteTimestamp (const IQueryManager::IQuery &, uint index, EPipelineScope srcScope)							__Th___ = 0;
		)

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

	class NO_VTABLE IBaseContext
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

		VULKAN_ONLY(
		virtual void  WriteTimestamp (const IQueryManager::IQuery &, uint index, EPipelineScope srcScope)									__Th___ = 0;
		)

		ND_ virtual FrameUID  GetFrameId ()																									C_NE___	= 0;

		GAPI_DEPENDENT(
			CommandBatch const&		GetCommandBatch ()																						C_NE___;
			CommandBatchPtr			GetCommandBatchRC ()																					C_NE___;
			AccumBar				AccumBarriers ()																						__NE___;
			DeferredBar				DeferredBarriers ()																						__NE___;
			IResourceManager &		GetResourceManager ()																					C_NE___;
			IDevice const&			GetDevice ()																							C_NE___;
			RenderTask const&		GetRenderTask ()																						C_NE___;
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
			void  _AddNextCycleEndDeps (AsyncTask task)												__NE___;
		};

		struct ReadbackBufferResult : _ReadbackResult
		{
			Promise<BufferMemView>	readOp;

			ReadbackBufferResult ()																	__NE___ = default;
			explicit ReadbackBufferResult (Promise<BufferMemView> readOp)							__NE___ : readOp{RVRef(readOp)} {}

			template <typename Fn>
			auto  Then (Fn &&fn, StringView dbgName = Default, ETaskQueue queueType = Default)		__NE___ { auto p = readOp.Then( FwdArg<Fn>(fn), dbgName, queueType );  _AddNextCycleEndDeps( AsyncTask{p} );  return p; }

			template <typename Fn>
			auto  Except (Fn &&fn, StringView dbgName = Default, ETaskQueue queueType = Default)	__NE___ { return readOp.Except( FwdArg<Fn>(fn), dbgName, queueType ); }
		};


		struct ReadbackImageResult : _ReadbackResult
		{
			Promise<ImageMemView>	readOp;

			ReadbackImageResult ()																	__NE___ = default;
			explicit ReadbackImageResult (Promise<ImageMemView> readOp)								__NE___ : readOp{RVRef(readOp)} {}

			template <typename Fn>
			auto  Then (Fn &&fn, StringView dbgName = Default, ETaskQueue queueType = Default)		__NE___ { auto p = readOp.Then( FwdArg<Fn>(fn), dbgName, queueType );  _AddNextCycleEndDeps( AsyncTask{p} );  return p; }

			template <typename Fn>
			auto  Except (Fn &&fn, StringView dbgName = Default, ETaskQueue queueType = Default)	__NE___ { return readOp.Except( FwdArg<Fn>(fn), dbgName, queueType ); }
		};


		struct ReadbackBufferResult2 : ReadbackBufferResult
		{
			Bytes		remain;		// non-zero if not enough space to read whole buffer

			ReadbackBufferResult2 ()																__NE___ = default;
			ReadbackBufferResult2 (Promise<BufferMemView> readOp, Bytes remain)						__NE___ : ReadbackBufferResult{RVRef(readOp)}, remain{remain} {}

			ND_ bool  IsCompleted ()																C_NE___	{ return remain == 0; }
		};


		struct ReadbackImageResult2 : ReadbackImageResult
		{
			uint3		remain;		// non-zero if not enough space to read whole image

			ReadbackImageResult2 ()																	__NE___ = default;
			ReadbackImageResult2 (Promise<ImageMemView> readOp, uint3 remain)						__NE___ : ReadbackImageResult{RVRef(readOp)}, remain{remain} {}

			ND_ bool  IsCompleted ()																C_NE___	{ return All( IsZero( remain )); }
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
			virtual void  UploadImage (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)			__Th___	= 0;
			virtual void  UploadImage (VideoImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)		__Th___	= 0;

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
		VULKAN_ONLY(
		ND_	virtual	bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped)			__Th___ = 0;
		)

		//		buffer: EResourceState::Host_Read
		ND_ virtual Promise<ArrayView<ubyte>>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)				__Th___ = 0;

		ND_ virtual uint3  MinImageTransferGranularity ()																C_NE___ = 0;


	// only in compute/graphics queue //

		VULKAN_ONLY(
		//		image: EResourceState::ClearDst
				void  ClearColorImage (ImageID image, const RGBA8u  &color, ArrayView<ImageSubresourceRange> ranges)	__Th___;
		virtual	void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges)	__Th___ = 0;
		virtual	void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges)	__Th___ = 0;
		virtual	void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges)	__Th___ = 0;
		)


	// only in graphics queue //

		//		srcImage: EResourceState::BlitSrc
		//		dstImage: EResourceState::BlitDst
		virtual void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions)	__Th___ = 0;

		VULKAN_ONLY(
		//		image: EResourceState::ClearDst
		virtual	void  ClearDepthStencilImage (ImageID image, const DepthStencil &depthStencil,
											  ArrayView<ImageSubresourceRange> ranges)									__Th___ = 0;

		//		srcImage: EResourceState::BlitSrc
		//		dstImage: EResourceState::BlitDst
		virtual	void  ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions)				__Th___ = 0;
		)

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

	class IComputeContext : public IBaseContext
	{
	// interface
	public:
		virtual void  BindPipeline (ComputePipelineID ppln)																			__Th___	= 0;
		virtual void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th___	= 0;

		virtual void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th___	= 0;
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

	class IRayTracingContext : public IBaseContext
	{
	// interface
	public:
		virtual void  BindPipeline (RayTracingPipelineID ppln)																		__Th___	= 0;
		virtual void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th___	= 0;

		virtual void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th___	= 0;
		template <typename T> void  PushConstant (const PushConstantIndex &idx, const T &data)										__Th___	{ return PushConstant( idx, Sizeof(data), &data, T::TypeName ); }


		//	requires: EPipelineDynamicState::RTStackSize
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

		//	requires 'rayTracingPipelineTraceRaysIndirect2' feature flag
		virtual void  TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)										__Th___	= 0;

		VULKAN_ONLY(
		//		address: EResourceState::BuildRTAS_IndirectBuffer  (TraceRayIndirectCommand)
		virtual	void  TraceRaysIndirect (const RTShaderBindingTable &sbt, DeviceAddress address)									__Th___ = 0;

		//	requires 'rayTracingPipelineTraceRaysIndirect2' feature flag
		//		address: EResourceState::BuildRTAS_IndirectBuffer  (TraceRayIndirectCommand2)
		virtual	void  TraceRaysIndirect2 (DeviceAddress address)																	__Th___ = 0;
		)
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

		VULKAN_ONLY(
		//	requires 'accelerationStructureIndirectBuild' feature flag

		//		indirectBuffer:		EResourceState::BuildRTAS_IndirectBuffer
		//		dst:				EResourceState::BuildRTAS_Read
		//		all buffers in cmd:	EResourceState::BuildRTAS_Read

		//		'indirectBuffer' for geometry build: array of 'ASBuildIndirectCommand [TriangleCount + AABBsCount]'
		virtual	void  BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst, DeviceAddress indirectBuffer,
									 Bytes indirectStride = SizeOf<ASBuildIndirectCommand>)											__Th___ = 0;
		virtual	void  BuildIndirect (const RTGeometryBuild &cmd, RTGeometryID dst,
									 BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b,
									 Bytes indirectStride = SizeOf<ASBuildIndirectCommand>)											__Th___ = 0;

		//		'indirectBuffer' for scene build: single 'ASBuildIndirectCommand'
		virtual	void  BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst, DeviceAddress indirectBuffer)							__Th___ = 0;
		virtual	void  BuildIndirect (const RTSceneBuild &cmd, RTSceneID dst,
									 BufferID indirectBuffer, Bytes indirectBufferOffset = 0_b)										__Th___ = 0;
		)


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

		VULKAN_ONLY(
		//		src: EResourceState::CopyRTAS_Read
		//		dst: EResourceState::CopyRTAS_Write
		virtual	void  SerializeToMemory (RTGeometryID src, DeviceAddress dst)														__Th___ = 0;
		virtual	void  SerializeToMemory (RTGeometryID src, BufferID dst, Bytes dstOffset)											__Th___ = 0;

		virtual	void  SerializeToMemory (RTSceneID src, DeviceAddress dst)															__Th___ = 0;
		virtual	void  SerializeToMemory (RTSceneID src, BufferID dst, Bytes dstOffset)												__Th___ = 0;

		virtual	void  DeserializeFromMemory (DeviceAddress src, RTGeometryID dst)													__Th___ = 0;
		virtual	void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTGeometryID dst)										__Th___ = 0;

		virtual	void  DeserializeFromMemory (DeviceAddress src, RTSceneID dst)														__Th___ = 0;
		virtual	void  DeserializeFromMemory (BufferID src, Bytes srcOffset, RTSceneID dst)											__Th___ = 0;
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
	ClearColorImage
=================================================
*/
	inline void  ITransferContext::ClearColorImage (ImageID image, const RGBA8u &color, ArrayView<ImageSubresourceRange> ranges) __Th___
	{
		ClearColorImage( image, RGBA32f{color}, ranges );
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

#undef VULKAN_ONLY
#undef METAL_ONLY
#undef GAPI_DEPENDENT
