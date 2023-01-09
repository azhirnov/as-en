// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/RenderGraph/RGCommandBatch.h"

namespace AE::RG::_hidden_
{

#define RG_INHERIT_BARRIERS \
	private: \
		void  BufferBarrier (BufferID buffer, EResourceState srcState, EResourceState dstState)									 __Th_OV { return _ctx.BufferBarrier( buffer, srcState, dstState ); } \
		\
		void  BufferViewBarrier (BufferViewID view, EResourceState srcState, EResourceState dstState)							 __Th_OV { return _ctx.BufferViewBarrier( view, srcState, dstState ); } \
		\
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState)									 __Th_OV { return _ctx.ImageBarrier( image, srcState, dstState ); } \
		void  ImageBarrier (ImageID image, EResourceState srcState, EResourceState dstState, const ImageSubresourceRange &subRes)__Th_OV { return _ctx.ImageBarrier( image, srcState, dstState, subRes ); } \
		\
		void  ImageViewBarrier (ImageViewID view, EResourceState srcState, EResourceState dstState)								 __Th_OV { return _ctx.ImageViewBarrier( view, srcState, dstState ); } \
		\
		void  MemoryBarrier (EResourceState srcState, EResourceState dstState)													 __NE_OV { return _ctx.MemoryBarrier( srcState, dstState ); } \
		void  MemoryBarrier (EPipelineScope srcScope, EPipelineScope dstScope)													 __NE_OV { return _ctx.MemoryBarrier( srcScope, dstScope ); } \
		void  MemoryBarrier ()																									 __NE_OV { return _ctx.MemoryBarrier(); } \
		\
		void  ExecutionBarrier (EPipelineScope srcScope, EPipelineScope dstScope)												 __NE_OV { return _ctx.ExecutionBarrier( srcScope, dstScope ); } \
		void  ExecutionBarrier ()																								 __NE_OV { return _ctx.ExecutionBarrier(); } \
		\
		void  AcquireBufferOwnership (BufferID buffer, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)	 __Th_OV { return _ctx.AcquireBufferOwnership( buffer, srcQueue, srcState, dstState ); } \
		void  ReleaseBufferOwnership (BufferID buffer, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)	 __Th_OV { return _ctx.ReleaseBufferOwnership( buffer, srcState, dstState, dstQueue ); } \
		\
		void  AcquireImageOwnership (ImageID image, EQueueType srcQueue, EResourceState srcState, EResourceState dstState)		 __Th_OV { return _ctx.AcquireImageOwnership( image, srcQueue, srcState, dstState ); } \
		void  ReleaseImageOwnership (ImageID image, EResourceState srcState, EResourceState dstState, EQueueType dstQueue)		 __Th_OV { return _ctx.ReleaseImageOwnership( image, srcState, dstState, dstQueue ); } \
		\
		ND_ CommandBatch &					_Batch ()																			 __NE___ { return _ctx._GetBarrierMngr().GetBatch(); } \
		ND_ RGCommandBatchPtr::RGBatchData&	_RGBatch ()																			 __NE___ { return *Cast<RGCommandBatchPtr::RGBatchData>( _Batch().GetUserData() ); } \
		ND_ uint							_ExeIdx ()																			 __NE___ { return _ctx._GetBarrierMngr().GetRenderTask().GetExecutionIndex(); } \
		ND_ auto&							_ResMngr ()																			 __NE___ { return _ctx._GetBarrierMngr().GetResourceManager(); } \
		\
	public: \
		void  CommitBarriers ()																									 __Th_OV { return _ctx.CommitBarriers(); } \
		\
		void  DebugMarker (DebugLabel dbg)																						 __Th_OV { return _ctx.DebugMarker( dbg ); } \
		void  PushDebugGroup (DebugLabel dbg)																					 __Th_OV { return _ctx.PushDebugGroup( dbg ); } \
		void  PopDebugGroup ()																									 __Th_OV { return _ctx.PopDebugGroup(); } \
		\
		ND_ CommandBatch const&		GetCommandBatch ()																			 C_NE___ { return _ctx.GetCommandBatch(); } \
		\
		void  ResourceState (BufferID     id, EResourceState state)																 __Th___ { _RGBatch().ResourceState( _ExeIdx(), _ctx, id, state ); } \
		void  ResourceState (ImageID      id, EResourceState state)																 __Th___ { _RGBatch().ResourceState( _ExeIdx(), _ctx, id, state ); } \
		void  ResourceState (ImageViewID  id, EResourceState state)																 __Th___ { auto* v = _ResMngr().GetResourcesOrThrow( id );  _RGBatch().ResourceState( _ExeIdx(), _ctx, v->Image(),  state ); } \
		void  ResourceState (BufferViewID id, EResourceState state)																 __Th___ { auto* v = _ResMngr().GetResourcesOrThrow( id );  _RGBatch().ResourceState( _ExeIdx(), _ctx, v->Buffer(), state ); } \
		void  ResourceState (RTSceneID    id, EResourceState state)																 __Th___ { _RGBatch().ResourceState( _ExeIdx(), _ctx, id, state ); } \
		void  ResourceState (RTGeometryID id, EResourceState state)																 __Th___ { _RGBatch().ResourceState( _ExeIdx(), _ctx, id, state ); } \
		\
		ND_ auto  EndCommandBuffer ()																							 __Th___	\
		{																																	\
			if ( _ctx._GetBarrierMngr().GetRenderTask().IsLastInBatch() )																	\
				_RGBatch().FinalBarriers( _ExeIdx(), _ctx );																				\
																																			\
			return _ctx.EndCommandBuffer();																									\
		}																																	\


	
	//
	// Transfer Context
	//

	template <typename BaseCtx>
	class TransferContext final : public ITransferContext
	{
	// types
	public:
		STATIC_ASSERT( BaseCtx::IsTransferContext );

		static constexpr bool	IsIndirectContext	= BaseCtx::IsIndirectContext;
		static constexpr bool	IsTransferContext	= true;


	// variables
	private:
		BaseCtx		_ctx;

	// methods
	public:
		explicit TransferContext (const RenderTask &task)													__Th___ : _ctx{ task } {}
		
		template <typename RawCmdBufType>
		TransferContext (const RenderTask &task, RawCmdBufType cmdbuf)										__Th___ : _ctx{ task, RVRef(cmdbuf) } {}

		TransferContext ()																					= delete;
		TransferContext (const TransferContext &)															= delete;

		ND_ BaseCtx&  GetBaseContext ()																		__NE___	{ return _ctx; }

		void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data)								__Th_OV;
		void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)					__Th_OV;
		
		void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)				__Th_OV;
		void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)					__Th_OV;
		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)	__Th_OV;
		void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)	__Th_OV;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)	__Th_OV;
		void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)	__Th_OV;
		
		void  UploadBuffer (BufferID buffer, Bytes offset, Bytes requiredSize, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static)__Th_OV;
			
		void  UploadImage (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)			__Th_OV;

		Promise<BufferMemView>  ReadbackBuffer (BufferID buffer, Bytes offset, Bytes size, EStagingHeapType heapType = EStagingHeapType::Static)__Th_OV;
		Promise<ImageMemView>   ReadbackImage (ImageID image, const ReadbackImageDesc &desc)													__Th_OV;

		void  UploadBuffer (INOUT BufferStream &stream, OUT BufferMemView &memView, EStagingHeapType heapType = EStagingHeapType::Static)		__Th_OV;
		void  UploadImage (INOUT ImageStream &stream, OUT ImageMemView &memView, EStagingHeapType heapType = EStagingHeapType::Dynamic)			__Th_OV;

		bool  UpdateHostBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)				__Th_OV;
		
		Promise<ArrayView<ubyte>>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)				__Th_OV;
		
		uint3  MinImageTransferGranularity ()																C_NE_OV	{ return _ctx.MinImageTransferGranularity(); }

		void  GenerateMipmaps (ImageID image)																__Th_OV;
		
		using ITransferContext::UpdateHostBuffer;
		using ITransferContext::UploadBuffer;
		using ITransferContext::UploadImage;

		RG_INHERIT_BARRIERS
	};



	//
	// Compute Context
	//
	
	template <typename BaseCtx>
	class ComputeContext final : public IComputeContext
	{
	// types
	public:
		STATIC_ASSERT( BaseCtx::IsComputeContext );

		static constexpr bool	IsIndirectContext	= BaseCtx::IsIndirectContext;
		static constexpr bool	IsComputeContext	= true;


	// variables
	private:
		BaseCtx		_ctx;

	// methods
	public:
		explicit ComputeContext (const RenderTask &task)															__Th___ : _ctx{ task } {}

		template <typename RawCmdBufType>
		ComputeContext (const RenderTask &task, RawCmdBufType cmdbuf)												__Th___ : _ctx{ task, RVRef(cmdbuf) } {}

		ComputeContext ()																							= delete;
		ComputeContext (const ComputeContext &)																		= delete;
		
		ND_ BaseCtx&  GetBaseContext ()																				__NE___	{ return _ctx; }

		void  BindPipeline (ComputePipelineID ppln)																	__Th_OV	{ return _ctx.BindPipeline( ppln ); }
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)__Th_OV	{ return _ctx.BindDescriptorSet( index, ds, dynamicOffsets ); }
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)						__Th_OV	{ return _ctx.PushConstant( offset, size, values, stages ); }
		
		using IComputeContext::Dispatch;

		void  Dispatch (const uint3 &groupCount)																	__Th_OV	{ return _ctx.Dispatch( groupCount ); }
		void  DispatchIndirect (BufferID buffer, Bytes offset)														__Th_OV;

		RG_INHERIT_BARRIERS
	};



	//
	// Graphics Context
	//
	
	template <typename BaseCtx>
	class GraphicsContext final : public IGraphicsContext
	{
	// types
	public:
		STATIC_ASSERT( BaseCtx::IsGraphicsContext );

		static constexpr bool	IsIndirectContext	= BaseCtx::IsIndirectContext;
		static constexpr bool	IsGraphicsContext	= true;

		using DrawCtx = typename BaseCtx::DrawCtx;


	// variables
	private:
		BaseCtx		_ctx;

	// methods
	public:
		explicit GraphicsContext (const RenderTask &task)											__Th___ : _ctx{ task } {}
		
		template <typename RawCmdBufType>
		GraphicsContext (const RenderTask &task, RawCmdBufType cmdbuf)								__Th___ : _ctx{ task, RVRef(cmdbuf) } {}
		
		// continue render pass
		template <typename RawCmdBufType>
		GraphicsContext (const RenderTask &task, const DrawCommandBatch &batch, RawCmdBufType cmdbuf)__Th___ : _ctx{ task, batch, RVRef(cmdbuf) } {}

		GraphicsContext ()																			= delete;
		GraphicsContext (const GraphicsContext &)													= delete;
		
		ND_ BaseCtx&	GetBaseContext ()															__NE___	{ return _ctx; }

		ND_ auto const&	GetState ()																	C_NE___	{ return _ctx.GetState(); }
		ND_ bool		IsInsideRenderPass ()														C_NE___	{ return _ctx.IsInsideRenderPass(); }
		ND_ bool		IsSecondaryCmdbuf ()														C_NE___	{ return _ctx.IsSecondaryCmdbuf(); }

		ND_ DrawCtx	BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default)			__Th___;
		ND_ DrawCtx	NextSubpass (DrawCtx& prevPassCtx, DebugLabel dbg = Default)					__Th___	{ return _ctx.NextSubpass( prevPassCtx, dbg ); }
			void	EndRenderPass (DrawCtx& ctx)													__Th___;
			
		ND_ auto	BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg = Default)		__Th___;
		ND_ auto	NextMtSubpass (const DrawCommandBatch &prevPassBatch, DebugLabel dbg = Default)	__Th___	{ return _ctx.NextMtSubpass( prevPassBatch, dbg ); }
			void	EndMtRenderPass ()																__Th___;
			void	ExecuteSecondary (DrawCommandBatch &batch)										__Th___	{ return _ctx.ExecuteSecondary( batch ); }

		RG_INHERIT_BARRIERS
	};



	//
	// Ray Tracing Context
	//
	
	template <typename BaseCtx>
	class RayTracingContext final : public IRayTracingContext
	{
	// types
	public:
		STATIC_ASSERT( BaseCtx::IsRayTracingContext );

		static constexpr bool	IsIndirectContext	= BaseCtx::IsIndirectContext;
		static constexpr bool	IsRayTracingContext	= true;


	// variables
	private:
		BaseCtx		_ctx;

	// methods
	public:
		explicit RayTracingContext (const RenderTask &task)																__Th___	: _ctx{ task } {}
		
		template <typename RawCmdBufType>
		RayTracingContext (const RenderTask &task, RawCmdBufType cmdbuf)												__Th___	: _ctx{ task, cmdbuf } {}

		RayTracingContext ()																							= delete;
		RayTracingContext (const RayTracingContext &)																	= delete;
		
		ND_ BaseCtx&  GetBaseContext ()																					__NE___	{ return _ctx; }

		void  BindPipeline (RayTracingPipelineID ppln)																	__Th_OV	{ return _ctx.BindPipeline( ppln ); }
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)	__Th_OV	{ return _ctx.BindDescriptorSet( index, ds, dynamicOffsets ); }
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)							__Th_OV	{ return _ctx.PushConstant( offset, size, values, stages ); }

		void  SetStackSize (Bytes size)																					__Th_OV	{ return _ctx.SetStackSize( size ); }
		
		void  TraceRays (const uint2 dim, RTShaderBindingID sbt)														__Th_OV;
		void  TraceRays (const uint3 dim, RTShaderBindingID sbt)														__Th_OV;

		void  TraceRays (const uint2 dim, const RTShaderBindingTable &sbt)												__Th_OV;
		void  TraceRays (const uint3 dim, const RTShaderBindingTable &sbt)												__Th_OV;

		void  TraceRaysIndirect (RTShaderBindingID sbt, BufferID indirectBuffer, Bytes indirectBufferOffset)			__Th_OV;
		void  TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectBufferOffset)	__Th_OV;

		void  TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)									__Th_OV;

		RG_INHERIT_BARRIERS
	};



	//
	// Acceleration Structure Build Context
	//
	
	template <typename BaseCtx>
	class ASBuildContext final : public IASBuildContext
	{
	// types
	public:
		STATIC_ASSERT( BaseCtx::IsASBuildContext );

		static constexpr bool	IsIndirectContext	= BaseCtx::IsIndirectContext;
		static constexpr bool	IsASBuildContext	= true;


	// variables
	private:
		BaseCtx		_ctx;

	// methods
	public:
		explicit ASBuildContext (const RenderTask &task)																	__Th___	: _ctx{ task } {}
		
		template <typename RawCmdBufType>
		ASBuildContext (const RenderTask &task, RawCmdBufType cmdbuf)														__Th___ : _ctx{ task, RVRef(cmdbuf) } {}

		ASBuildContext ()																									= delete;
		ASBuildContext (const ASBuildContext &)																				= delete;
		
		ND_ BaseCtx&  GetBaseContext ()																						__NE___	{ return _ctx; }

		void  Build (const RTGeometryBuild &cmd, RTGeometryID dst)															__Th_OV;
		void  Build (const RTSceneBuild &cmd, RTSceneID dst)																__Th_OV;
		
		void  Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst)										__Th_OV;
		void  Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst)												__Th_OV;
		
		void  Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)							__Th_OV;
		void  Copy (RTSceneID src, RTSceneID dst, ERTASCopyMode mode = ERTASCopyMode::Clone)								__Th_OV;

		void  WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size = UMax)	__Th_OV;
		void  WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size = UMax)		__Th_OV;
		
		Promise<Bytes>  ReadProperty (ERTASProperty property, RTGeometryID as)												__Th_OV;
		Promise<Bytes>  ReadProperty (ERTASProperty property, RTSceneID as)													__Th_OV;

		RG_INHERIT_BARRIERS
	};
	


	//
	// Draw Context
	//
	/*
	template <typename BaseCtx>
	class DrawContext final : public IDrawContext
	{
	// types
	public:
		STATIC_ASSERT( BaseCtx::IsDrawBuildContext );

		static constexpr bool	IsIndirectContext	= BaseCtx::IsIndirectContext;
		static constexpr bool	IsDrawBuildContext	= true;


	// variables
	private:
		BaseCtx		_ctx;


	// methods
	public:
		DrawContext (const VPrimaryCmdBufState &state, CmdBuf_t cmdbuf, DebugLabel dbg)										__Th___;
		explicit DrawContext (const DrawTask &task)																			__Th___;
		
		DrawContext ()																										= delete;
		DrawContext (const DrawContext &)																					= delete;
		
		ND_ BaseCtx&  GetBaseContext ()																						__NE___	{ return _ctx; }

	// pipeline and shader resources //
		void  BindPipeline (GraphicsPipelineID ppln)																		__Th_OV;
		void  BindPipeline (MeshPipelineID ppln)																			__Th_OV;
		void  BindPipeline (TilePipelineID ppln)																			__Th_OV;
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th_OV;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages = EShaderStages::AllGraphics)__Th_OV;
		
	// dynamic states //
		void  SetViewport (const Viewport_t &viewport)																		__Th_OV;
		void  SetViewports (ArrayView<Viewport_t> viewports)																__Th_OV;
		void  SetScissor (const RectI &scissor)																				__Th_OV;
		void  SetScissors (ArrayView<RectI> scissors)																		__Th_OV;
		void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)				__Th_OV;
		void  SetStencilReference (uint reference)																			__Th_OV;
		void  SetStencilReference (uint frontReference, uint backReference)													__Th_OV;
		void  SetBlendConstants (const RGBA32f &color)																		__Th_OV;

	// draw commands //
		
		void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)												__Th_OV;
		void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)													__Th_OV;
		void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)					__Th_OV;
		bool  BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset)		__Th_OV;

		void  Draw (uint vertexCount,
					uint instanceCount	= 1,
					uint firstVertex	= 0,
					uint firstInstance	= 0)						__Th_OV;

		void  DrawIndexed (uint indexCount,
						   uint instanceCount	= 1,
						   uint firstIndex		= 0,
						   int  vertexOffset	= 0,
						   uint firstInstance	= 0)				__Th_OV;

		void  DrawIndirect (BufferID	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride)						__Th_OV;
		
		void  DrawIndexedIndirect (BufferID		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride)				__Th_OV;

		// tile shader //
		void  DispatchTile ()										__Th_OV;
		
		// mesh shader //
		void  DrawMeshTasks (const uint3 &taskCount)				__Th_OV;
		
		void  DrawMeshTasksIndirect (BufferID	indirectBuffer,
									 Bytes		indirectBufferOffset,
									 uint		drawCount,
									 Bytes		stride)				__Th_OV;

		// for debugging //
		void  DebugMarker (DebugLabel dbg)							__Th_OV;
		void  PushDebugGroup (DebugLabel dbg)						__Th_OV;
		void  PopDebugGroup ()										__Th_OV;
		
		// only for RW attachments //
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)	__Th_OV;
		void  CommitBarriers ()																			__Th_OV;
		
		// vertex stream //
		bool  AllocVStream (Bytes size, OUT VertexStream &result)										__Th_OV;
	};*/
//-----------------------------------------------------------------------------
	

	
	template <typename C>
	void  TransferContext<C>::FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data) __Th___
	{
		ResourceState( buffer, EResourceState::CopyDst );
		_ctx.CommitBarriers();
		_ctx.FillBuffer( buffer, offset, size, data );
	}
	
	template <typename C>
	void  TransferContext<C>::UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data) __Th___
	{
		ResourceState( buffer, EResourceState::CopyDst );
		_ctx.CommitBarriers();
		_ctx.UpdateBuffer( buffer, offset, size, data );
	}
		
	template <typename C>
	void  TransferContext<C>::CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges) __Th___
	{
		ResourceState( srcBuffer, EResourceState::CopySrc );
		ResourceState( dstBuffer, EResourceState::CopyDst );
		_ctx.CommitBarriers();
		_ctx.CopyBuffer( srcBuffer, dstBuffer, ranges );
	}
	
	template <typename C>
	void  TransferContext<C>::CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges) __Th___
	{
		ResourceState( srcImage, EResourceState::CopySrc );
		ResourceState( dstImage, EResourceState::CopyDst );
		_ctx.CommitBarriers();
		_ctx.CopyImage( srcImage, dstImage, ranges );
	}
	
	template <typename C>
	void  TransferContext<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges) __Th___
	{
		ResourceState( srcBuffer, EResourceState::CopySrc );
		ResourceState( dstImage,  EResourceState::CopyDst );
		_ctx.CommitBarriers();
		_ctx.CopyBufferToImage( srcBuffer, dstImage, ranges );
	}
	
	template <typename C>
	void  TransferContext<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges) __Th___
	{
		ResourceState( srcBuffer, EResourceState::CopySrc );
		ResourceState( dstImage,  EResourceState::CopyDst );
		_ctx.CommitBarriers();
		_ctx.CopyBufferToImage( srcBuffer, dstImage, ranges );
	}
	
	template <typename C>
	void  TransferContext<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges) __Th___
	{
		ResourceState( srcImage,  EResourceState::CopySrc );
		ResourceState( dstBuffer, EResourceState::CopyDst );
		_ctx.CommitBarriers();
		_ctx.CopyImageToBuffer( srcImage, dstBuffer, ranges );
	}
	
	template <typename C>
	void  TransferContext<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges) __Th___
	{
		ResourceState( srcImage,  EResourceState::CopySrc );
		ResourceState( dstBuffer, EResourceState::CopyDst );
		_ctx.CommitBarriers();
		_ctx.CopyImageToBuffer( srcImage, dstBuffer, ranges );
	}
		
	template <typename C>
	void  TransferContext<C>::UploadBuffer (BufferID buffer, Bytes offset, Bytes requiredSize, OUT BufferMemView &memView, EStagingHeapType heapType) __Th___
	{
		ResourceState( buffer, EResourceState::CopyDst );
		// TODO: check for UploadMemory() barrier in batch
		_ctx.CommitBarriers();
		_ctx.UploadBuffer( buffer, offset, requiredSize, OUT memView, heapType );
	}
			
	template <typename C>
	void  TransferContext<C>::UploadImage (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView) __Th___
	{
		ResourceState( image, EResourceState::CopyDst );
		// TODO: check for UploadMemory() barrier in batch
		_ctx.CommitBarriers();
		_ctx.UploadImage( image, desc, OUT memView );
	}
	
	template <typename C>
	Promise<BufferMemView>  TransferContext<C>::ReadbackBuffer (BufferID buffer, Bytes offset, Bytes size, EStagingHeapType heapType) __Th___
	{
		ResourceState( buffer, EResourceState::CopySrc );
		// TODO: check for ReadbackMemory() barrier in batch
		_ctx.CommitBarriers();
		return _ctx.ReadbackBuffer( buffer, offset, size, heapType );
	}
	
	template <typename C>
	Promise<ImageMemView>  TransferContext<C>::ReadbackImage (ImageID image, const ReadbackImageDesc &desc) __Th___
	{
		ResourceState( image, EResourceState::CopySrc );
		// TODO: check for ReadbackMemory() barrier in batch
		_ctx.CommitBarriers();
		return _ctx.ReadbackImage( image, desc );
	}

	template <typename C>
	void  TransferContext<C>::UploadBuffer (INOUT BufferStream &stream, OUT BufferMemView &memView, EStagingHeapType heapType) __Th___
	{
		ResourceState( stream.Buffer(), EResourceState::CopyDst );
		// TODO: check for UploadMemory() barrier in batch
		_ctx.CommitBarriers();
		_ctx.UploadBuffer( INOUT stream, OUT memView, heapType );
	}
	
	template <typename C>
	void  TransferContext<C>::UploadImage (INOUT ImageStream &stream, OUT ImageMemView &memView, EStagingHeapType heapType) __Th___
	{
		ResourceState( stream.Image(), EResourceState::CopyDst );
		// TODO: check for UploadMemory() barrier in batch
		_ctx.CommitBarriers();
		_ctx.UploadImage( INOUT stream, OUT memView, heapType );
	}
	
	template <typename C>
	bool  TransferContext<C>::UpdateHostBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data) __Th___
	{
		ResourceState( buffer, EResourceState::Host_Write );
		return _ctx.UpdateHostBuffer( buffer, offset, size, data );
	}
		
	template <typename C>
	Promise<ArrayView<ubyte>>  TransferContext<C>::ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size) __Th___
	{
		ResourceState( buffer, EResourceState::Host_Read );
		_ctx.CommitBarriers();
		return _ctx.ReadHostBuffer( buffer, offset, size );
	}
	
	template <typename C>
	void  TransferContext<C>::GenerateMipmaps (ImageID image) __Th___
	{
		ResourceState( image, EResourceState::BlitSrc );
		_ctx.CommitBarriers();
		_ctx.GenerateMipmaps( image );
	}
//-----------------------------------------------------------------------------


	
	template <typename C>
	void  ComputeContext<C>::DispatchIndirect (BufferID buffer, Bytes offset) __Th___
	{
		ResourceState( buffer, EResourceState::IndirectBuffer );
		_ctx.CommitBarriers();
		return _ctx.DispatchIndirect( buffer, offset );
	}
//-----------------------------------------------------------------------------



	template <typename C>
	typename GraphicsContext<C>::DrawCtx  GraphicsContext<C>::BeginRenderPass (const RenderPassDesc &desc, DebugLabel dbg) __Th___
	{
		for (auto [name, att] : desc.attachments)
		{
			CHECK( att.initial == Default );
			CHECK( att.final == Default );
		}
		return _ctx.BeginRenderPass( desc, dbg );
	}
	
	template <typename C>
	void  GraphicsContext<C>::EndRenderPass (DrawCtx& ctx) __Th___
	{
		_RGBatch().SetRenderPassFinalStates( _ExeIdx(), _ctx.GetState() );
		return _ctx.EndRenderPass( ctx );
	}
			
	template <typename C>
	auto  GraphicsContext<C>::BeginMtRenderPass (const RenderPassDesc &desc, DebugLabel dbg) __Th___
	{
		for (auto [name, att] : desc.attachments)
		{
			CHECK( att.initial == Default );
			CHECK( att.final == Default );
		}
		return _ctx.BeginMtRenderPass( desc, dbg );
	}
			
	template <typename C>
	void  GraphicsContext<C>::EndMtRenderPass () __Th___
	{
		_RGBatch().SetRenderPassFinalStates( _ExeIdx(), _ctx.GetState() );
		return _ctx.EndMtRenderPass();
	}
//-----------------------------------------------------------------------------


	
	template <typename C>
	void  RayTracingContext<C>::TraceRays (const uint2 dim, RTShaderBindingID sbt) __Th___
	{
		_ctx.CommitBarriers();
		_ctx.TraceRays( dim, sbt );
	}
	
	template <typename C>
	void  RayTracingContext<C>::TraceRays (const uint3 dim, RTShaderBindingID sbt) __Th___
	{
		_ctx.CommitBarriers();
		_ctx.TraceRays( dim, sbt );
	}
	
	template <typename C>
	void  RayTracingContext<C>::TraceRays (const uint2 dim, const RTShaderBindingTable &sbt) __Th___
	{
		_ctx.CommitBarriers();
		_ctx.TraceRays( dim, sbt );
	}
	
	template <typename C>
	void  RayTracingContext<C>::TraceRays (const uint3 dim, const RTShaderBindingTable &sbt) __Th___
	{
		_ctx.CommitBarriers();
		_ctx.TraceRays( dim, sbt );
	}
	
	template <typename C>
	void  RayTracingContext<C>::TraceRaysIndirect (RTShaderBindingID sbt, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		ResourceState( indirectBuffer, EResourceState::IndirectBuffer );
		_ctx.CommitBarriers();
		_ctx.TraceRaysIndirect( sbt, indirectBuffer, indirectBufferOffset );
	}
	
	template <typename C>
	void  RayTracingContext<C>::TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		ResourceState( indirectBuffer, EResourceState::IndirectBuffer );
		_ctx.CommitBarriers();
		_ctx.TraceRaysIndirect( sbt, indirectBuffer, indirectBufferOffset );
	}
	
	template <typename C>
	void  RayTracingContext<C>::TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		ResourceState( indirectBuffer, EResourceState::IndirectBuffer );
		_ctx.CommitBarriers();
		_ctx.TraceRaysIndirect2( indirectBuffer, indirectBufferOffset );
	}
//-----------------------------------------------------------------------------

	

	template <typename C>
	void  ASBuildContext<C>::Build (const RTGeometryBuild &cmd, RTGeometryID dst) __Th___
	{
		ResourceState( dst, EResourceState::CopyRTAS_Write );
		_ctx.CommitBarriers();
		_ctx.Build( cmd, dst );
	}
	
	template <typename C>
	void  ASBuildContext<C>::Build (const RTSceneBuild &cmd, RTSceneID dst) __Th___
	{
		ResourceState( dst, EResourceState::CopyRTAS_Write );
		_ctx.CommitBarriers();
		_ctx.Build( cmd, dst );
	}
		
	template <typename C>
	void  ASBuildContext<C>::Update (const RTGeometryBuild &cmd, RTGeometryID src, RTGeometryID dst) __Th___
	{
		ResourceState( src, EResourceState::CopyRTAS_Read );
		ResourceState( dst, EResourceState::BuildRTAS_ReadWrite );
		_ctx.CommitBarriers();
		_ctx.Update( cmd, src, dst );
	}
	
	template <typename C>
	void  ASBuildContext<C>::Update (const RTSceneBuild &cmd, RTSceneID src, RTSceneID dst) __Th___
	{
		ResourceState( src, EResourceState::CopyRTAS_Read );
		ResourceState( dst, EResourceState::BuildRTAS_ReadWrite );
		_ctx.CommitBarriers();
		_ctx.Update( cmd, src, dst );
	}
		
	template <typename C>
	void  ASBuildContext<C>::Copy (RTGeometryID src, RTGeometryID dst, ERTASCopyMode mode) __Th___
	{
		ResourceState( src, EResourceState::CopyRTAS_Read );
		ResourceState( dst, EResourceState::CopyRTAS_Write );
		_ctx.CommitBarriers();
		_ctx.Copy( src, dst, mode );
	}
	
	template <typename C>
	void  ASBuildContext<C>::Copy (RTSceneID src, RTSceneID dst, ERTASCopyMode mode) __Th___
	{
		ResourceState( src, EResourceState::CopyRTAS_Read );
		ResourceState( dst, EResourceState::CopyRTAS_Write );
		_ctx.CommitBarriers();
		_ctx.Copy( src, dst, mode );
	}
	
	template <typename C>
	void  ASBuildContext<C>::WriteProperty (ERTASProperty property, RTGeometryID as, BufferID dstBuffer, Bytes offset, Bytes size) __Th___
	{
		ResourceState( as,        EResourceState::CopyRTAS_Read );
		ResourceState( dstBuffer, EResourceState::CopyDst );
		_ctx.CommitBarriers();
		_ctx.WriteProperty( property, as, dstBuffer, offset, size );
	}
	
	template <typename C>
	void  ASBuildContext<C>::WriteProperty (ERTASProperty property, RTSceneID as, BufferID dstBuffer, Bytes offset, Bytes size) __Th___
	{
		ResourceState( as,        EResourceState::CopyRTAS_Read );
		ResourceState( dstBuffer, EResourceState::CopyDst );
		_ctx.CommitBarriers();
		_ctx.WriteProperty( property, as, dstBuffer, offset, size );
	}

	template <typename C>
	Promise<Bytes>  ASBuildContext<C>::ReadProperty (ERTASProperty property, RTGeometryID as) __Th___
	{
		ResourceState( as, EResourceState::CopyRTAS_Read );
		_ctx.CommitBarriers();
		return _ctx.ReadProperty( property, as );
	}
	
	template <typename C>
	Promise<Bytes>  ASBuildContext<C>::ReadProperty (ERTASProperty property, RTSceneID as) __Th___
	{
		ResourceState( as, EResourceState::CopyRTAS_Read );
		_ctx.CommitBarriers();
		return _ctx.ReadProperty( property, as );
	}
//-----------------------------------------------------------------------------


} // AE::RG::_hidden_
