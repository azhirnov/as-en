// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RBaseContext.h"
# include "graphics/Remote/Commands/RAccumBarriers.h"
# include "graphics/Remote/REnumCast.h"
# include "graphics/Remote/Commands/RDrawBarrierManager.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Draw Context implementation
	//

	class RDrawContext final : public Graphics::_hidden_::_RBaseContext, public IDrawContext
	{
		friend class RGraphicsContext;

	// types
	private:
		using AccumBar		= Graphics::_hidden_::RAccumDrawBarriers< RDrawContext >;
		using Validator_t	= Graphics::_hidden_::DrawContextValidation;


	// variables
	private:
		Graphics::_hidden_::RDrawBarrierManager		_mngr;
		EPipelineDynamicState						_dynStates		= Default;


	// methods
	public:
		RDrawContext (const RPrimaryCmdBufState &state, CmdBuf_t cmdbuf)													__Th___;
		explicit RDrawContext (const DrawTask &task)																		__Th___;
		explicit RDrawContext (RDrawContext &&other)																		__Th___;
		~RDrawContext ()																									__NE___;

		RDrawContext ()																										= delete;
		RDrawContext (const RDrawContext &)																					= delete;

		// pipeline and shader resources
		void  BindPipeline (GraphicsPipelineID ppln)																		__Th_OV;
		void  BindPipeline (MeshPipelineID ppln)																			__Th_OV;
		void  BindPipeline (TilePipelineID ppln)																			__Th_OV;

		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th_OV;
		void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th_OV;
		using IDrawContext::PushConstant;

		// dynamic states
		void  SetViewport (const Viewport &viewport)																		__Th_OV	{ SetViewports( ArrayView{ &viewport, 1 }); }
		void  SetViewports (ArrayView<Viewport> viewports)																	__Th_OV;
		void  SetScissor (const RectI &scissors)																			__Th_OV	{ SetScissors( ArrayView{ &scissors, 1 }); }
		void  SetScissors (ArrayView<RectI> scissors)																		__Th_OV;
		void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)				__Th_OV;
		void  SetStencilCompareMask (uint compareMask)																		__Th___	{ SetStencilCompareMask( compareMask, compareMask ); }
		void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask)											__Th___;
		void  SetStencilWriteMask (uint writeMask)																			__Th___	{ SetStencilWriteMask( writeMask, writeMask ); }
		void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask)													__Th___;
		void  SetStencilReference (uint reference)																			__Th_OV	{ SetStencilReference( reference, reference ); }
		void  SetStencilReference (uint frontReference, uint backReference)													__Th_OV;
		void  SetBlendConstants (const RGBA32f &color)																		__Th_OV;
		void  SetDepthBounds (float minDepthBounds, float maxDepthBounds)													__Th_OV;
		void  SetFragmentShadingRate (EShadingRate, EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp textureOp)	__Th_OV;

		// draw commands
		void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)												__Th_OV;
		void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)													__Th_OV;
		void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)					__Th_OV;
		bool  BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, BufferID buffer, Bytes offset)		__Th_OV;

		void  BindIndexBuffer (RmBufferID buffer, Bytes offset, EIndex indexType)											__Th___;
		void  BindVertexBuffer (uint index, RmBufferID buffer, Bytes offset)												__Th___;
		void  BindVertexBuffers (uint firstBinding, ArrayView<RmBufferID> buffers, ArrayView<Bytes> offsets)				__Th___;
		bool  BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, RmBufferID buffer, Bytes offset)		__Th___;

		using IDrawContext::Draw;
		using IDrawContext::DrawIndexed;
		using IDrawContext::DrawIndirect;
		using IDrawContext::DrawIndexedIndirect;
		using IDrawContext::DrawMeshTasksIndirect;
		using IDrawContext::DrawIndirectCount;
		using IDrawContext::DrawIndexedIndirectCount;
		using IDrawContext::DrawMeshTasksIndirectCount;

		void  Draw (uint vertexCount,
					uint instanceCount	= 1,
					uint firstVertex	= 0,
					uint firstInstance	= 0)																				__Th_OV;

		void  DrawIndexed (uint indexCount,
						   uint instanceCount	= 1,
						   uint firstIndex		= 0,
						   int  vertexOffset	= 0,
						   uint firstInstance	= 0)																		__Th_OV;

		void  DrawIndirect (BufferID	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride)																				__Th_OV;

		void  DrawIndirect (RmBufferID	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride)																				__Th___;

		void  DrawIndexedIndirect (BufferID		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride)																		__Th_OV;

		void  DrawIndexedIndirect (RmBufferID	indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride)																		__Th___;

		// mesh shader
		void  DrawMeshTasks (const uint3 &taskCount)																		__Th_OV;

		void  DrawMeshTasksIndirect (BufferID	indirectBuffer,
									 Bytes		indirectBufferOffset,
									 uint		drawCount,
									 Bytes		stride)																		__Th_OV;

		void  DrawMeshTasksIndirect (RmBufferID	indirectBuffer,
									 Bytes		indirectBufferOffset,
									 uint		drawCount,
									 Bytes		stride)																		__Th___;

		// extension
		void  DrawIndirectCount (BufferID	indirectBuffer,
								 Bytes		indirectBufferOffset,
								 BufferID	countBuffer,
								 Bytes		countBufferOffset,
								 uint		maxDrawCount,
								 Bytes		stride)																			__Th_OV;

		void  DrawIndirectCount (RmBufferID	indirectBuffer,
								 Bytes		indirectBufferOffset,
								 RmBufferID	countBuffer,
								 Bytes		countBufferOffset,
								 uint		maxDrawCount,
								 Bytes		stride)																			__Th___;

		void  DrawIndexedIndirectCount (BufferID	indirectBuffer,
										Bytes		indirectBufferOffset,
										BufferID	countBuffer,
										Bytes		countBufferOffset,
										uint		maxDrawCount,
										Bytes		stride)																	__Th_OV;

		void  DrawIndexedIndirectCount (RmBufferID	indirectBuffer,
										Bytes		indirectBufferOffset,
										RmBufferID	countBuffer,
										Bytes		countBufferOffset,
										uint		maxDrawCount,
										Bytes		stride)																	__Th___;

		void  DrawMeshTasksIndirectCount (BufferID	indirectBuffer,
										  Bytes		indirectBufferOffset,
										  BufferID	countBuffer,
										  Bytes		countBufferOffset,
										  uint		maxDrawCount,
										  Bytes		stride)																	__Th_OV;

		void  DrawMeshTasksIndirectCount (RmBufferID	indirectBuffer,
										  Bytes			indirectBufferOffset,
										  RmBufferID	countBuffer,
										  Bytes			countBufferOffset,
										  uint			maxDrawCount,
										  Bytes			stride)																__Th___;

		// tile shader
		void  DispatchTile ()																								__Th_OV;

		void  DebugMarker (DebugLabel dbg)																					__Th_OV	{ _DebugMarker( dbg ); }
		void  PushDebugGroup (DebugLabel dbg)																				__Th_OV	{ _PushDebugGroup( dbg ); }
		void  PopDebugGroup ()																								__Th_OV	{ _PopDebugGroup(); }

		void  WriteTimestamp (const IQueryManager::IQuery &q, uint index, EPipelineScope srcScope)							__Th_OV;

		void  AttachmentBarrier (AttachmentName::Ref, EResourceState srcState, EResourceState dstState)						__Th_OV;

		void  CommitBarriers ()																								__Th_OV;

		// clear //
		bool  ClearAttachment (AttachmentName::Ref, const RGBA32f &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		bool  ClearAttachment (AttachmentName::Ref, const RGBA32u &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		bool  ClearAttachment (AttachmentName::Ref, const RGBA32i &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		bool  ClearAttachment (AttachmentName::Ref, const DepthStencil &, const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		using IDrawContext::ClearAttachment;

		// vertex stream
		ND_ bool  AllocVStream (Bytes size, OUT VertexStream &result)														__Th_OV;

		ND_ RPrimaryCmdBufState const&	GetPrimaryCtxState ()																C_NE___	{ return this->_mngr.GetPrimaryCtxState(); }
		ND_ FrameUID					GetFrameId ()																		C_NE_OF	{ return this->_mngr.GetFrameId(); }

		ND_ RmDrawCommandBufferID	EndCommandBuffer ()																		__Th___;
		ND_ CmdBuf_t				ReleaseCommandBuffer ()																	__Th___;

	private:
		template <typename ...IDs>
		ND_ decltype(auto)	_GetResourcesOrThrow (IDs ...ids)																__Th___	{ return this->_mngr.Get( ids... ); }

		ND_ bool			_NoPendingBarriers ()																			C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto			_GetDynamicStates ()																			C_NE___	{ return _dynStates; }

		using EFeature = RDevice::EFeature;
		ND_ bool			_HasFeature (EFeature feat)																		C_NE___	{ return _mngr.GetDevice().HasFeature( feat ); }
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
