// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	DrawCtx --> DirectDrawCtx   --> BarrierMngr --> Vulkan device
			\-> IndirectDrawCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics_rhi/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics_rhi/Vulkan/Commands/VAccumBarriers.h"

namespace AE::Graphics::_hidden_
{
	template <typename CtxImpl>
	class _VGraphicsContextImpl;


	//
	// Vulkan Direct Draw Context implementation
	//

	class _VDirectDrawCtx : public _VBaseDirectContext
	{
	// types
	private:
		using Validator_t	= DrawContextValidation;
	public:
		using DrawCoroRef	= _Coro_::DrawTaskImpl::UserApi;


	// variables
	private:
		struct {
			VkPipelineBindPoint		bindPoint		= VK_PIPELINE_BIND_POINT_MAX_ENUM;
			VkPipelineLayout		pplnLayout		= Default;
			EPipelineDynamicState	dynStates		= Default;
		}					_states;

	protected:
		VDrawBarrierManager	_mngr;


	// methods
	public:
		~_VDirectDrawCtx ()																							__NE_OV;

		void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;

		void  BindIndexBuffer (VkBuffer buffer, Bytes offset, EIndex indexType)										__Th___;

		void  DrawIndirect (VkBuffer	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride)																		__Th___;

		void  DrawIndexedIndirect (VkBuffer		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride)																__Th___;

		void  DrawIndirectCount (VkBuffer	indirectBuffer,
								 Bytes		indirectBufferOffset,
								 VkBuffer	countBuffer,
								 Bytes		countBufferOffset,
								 uint		maxDrawCount,
								 Bytes		stride)																	__Th___;

		void  DrawIndexedIndirectCount (VkBuffer	indirectBuffer,
										Bytes		indirectBufferOffset,
										VkBuffer	countBuffer,
										Bytes		countBufferOffset,
										uint		maxDrawCount,
										Bytes		stride)															__Th___;

		void  DrawMeshTasksIndirect (VkBuffer	indirectBuffer,
									 Bytes		indirectBufferOffset,
									 uint		drawCount,
									 Bytes		stride)																__Th___;

		void  DrawMeshTasksIndirectCount (VkBuffer	indirectBuffer,
										  Bytes		indirectBufferOffset,
										  VkBuffer	countBuffer,
										  Bytes		countBufferOffset,
										  uint		maxDrawCount,
										  Bytes		stride)															__Th___;

		void  ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &, bool isPreprocessed)					__Th___;

		void  AttachmentBarrier (AttachmentName::Ref name, EResourceState srcState, EResourceState dstState)		__Th___	{ _mngr.AttachmentBarrier( name, srcState, dstState ); }

		void  SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask)									__Th___;
		void  SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference)									__Th___;
		void  SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask)								__Th___;

		void  SetViewport (uint first, ArrayView<VkViewport> viewports)												__Th___;
		void  SetScissor (uint first, ArrayView<VkRect2D> scissors)													__Th___;

		ND_ VkCommandBuffer	EndCommandBuffer ()																		__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()																	__Th___;

	protected:
		explicit _VDirectDrawCtx (DrawCoroRef task)																	__Th___;
		_VDirectDrawCtx (const VPrimaryCmdBufState &state, VCommandBuffer cmdbuf)									__Th___;

		void  _BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline, VkPipelineLayout, EPipelineDynamicState)	__Th___;
		void  _PushGraphicsConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages)			__Th___;

		void  _SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)		__Th___;
		void  _SetDepthBounds (float minDepthBounds, float maxDepthBounds)											__Th___;
		void  _SetBlendConstants (const RGBA32f &color)																__Th___;

		void  _SetFragmentShadingRate (const VkExtent2D &fragSize, VkFragmentShadingRateCombinerOpKHR primitiveOp,
									   VkFragmentShadingRateCombinerOpKHR textureOp)								__Th___;

		void  _BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<VkDeviceSize> offsets)	__Th___;

		void  _Draw (uint vertexCount,
					 uint instanceCount,
					 uint firstVertex,
					 uint firstInstance)																			__Th___;

		void  _DrawIndexed (uint indexCount,
							uint instanceCount,
							uint firstIndex,
							int  vertexOffset,
							uint firstInstance)																		__Th___;

		void  _DrawMeshTasks (const uint3 &taskCount)																__Th___;

		void  _DispatchTile ()																						__Th___;

		void  _CommitBarriers ()																					__Th___;

		void  _ClearAttachment (const VkClearAttachment &clear, const VkClearRect &rect)							__Th___;

		ND_ bool	_NoPendingBarriers ()	C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto	_GetDynamicStates ()	C_NE___	{ return _states.dynStates; }

	private:
		template <typename C> friend class _VGraphicsContextImpl;
		ND_ VkCommandBuffer  _RawCmdBuf ()	C_NE___	{ return _cmdbuf.Get(); }
	};



	//
	// Vulkan Indirect Draw Context implementation
	//

	class _VIndirectDrawCtx : public _VBaseIndirectContext
	{
	// types
	private:
		using Validator_t	= DrawContextValidation;
	public:
		using DrawCoroRef	= _Coro_::DrawTaskImpl::UserApi;


	// variables
	private:
		struct {
			VkPipelineBindPoint		bindPoint		= VK_PIPELINE_BIND_POINT_MAX_ENUM;
			VkPipelineLayout		pplnLayout		= Default;
			EPipelineDynamicState	dynStates		= Default;
		}					_states;
	protected:
		VDrawBarrierManager	_mngr;


	// methods
	public:
		void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;

		void  BindIndexBuffer (VkBuffer buffer, Bytes offset, EIndex indexType)										__Th___;

		void  DrawIndirect (VkBuffer	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride)																		__Th___;

		void  DrawIndexedIndirect (VkBuffer		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride)																__Th___;

		void  DrawIndirectCount (VkBuffer	indirectBuffer,
								 Bytes		indirectBufferOffset,
								 VkBuffer	countBuffer,
								 Bytes		countBufferOffset,
								 uint		maxDrawCount,
								 Bytes		stride)																	__Th___;

		void  DrawIndexedIndirectCount (VkBuffer	indirectBuffer,
										Bytes		indirectBufferOffset,
										VkBuffer	countBuffer,
										Bytes		countBufferOffset,
										uint		maxDrawCount,
										Bytes		stride)															__Th___;

		void  DrawMeshTasksIndirect (VkBuffer	indirectBuffer,
									 Bytes		indirectBufferOffset,
									 uint		drawCount,
									 Bytes		stride)																__Th___;

		void  DrawMeshTasksIndirectCount (VkBuffer	indirectBuffer,
										  Bytes		indirectBufferOffset,
										  VkBuffer	countBuffer,
										  Bytes		countBufferOffset,
										  uint		maxDrawCount,
										  Bytes		stride)															__Th___;

		void  ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &, bool isPreprocessed)					__Th___;

		void  AttachmentBarrier (AttachmentName::Ref name, EResourceState srcState, EResourceState dstState)		__Th___	{ _mngr.AttachmentBarrier( name, srcState, dstState ); }

		void  SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask)									__Th___;
		void  SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference)									__Th___;
		void  SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask)								__Th___;

		void  SetViewport (uint first, ArrayView<VkViewport> viewports)												__Th___;
		void  SetScissor (uint first, ArrayView<VkRect2D> scissors)													__Th___;

		ND_ VBakedCommands		EndCommandBuffer ()																	__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()																__Th___;

	protected:
		explicit _VIndirectDrawCtx (DrawCoroRef task)																__Th___;
		_VIndirectDrawCtx (const VPrimaryCmdBufState &state, VSoftwareCmdBufPtr cmdbuf)								__Th___;

		void  _BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout, EPipelineDynamicState dynStates);
		void  _PushGraphicsConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages);

		void  _SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)		__Th___;
		void  _SetDepthBounds (float minDepthBounds, float maxDepthBounds)											__Th___;
		void  _SetBlendConstants (const RGBA32f &color)																__Th___;

		void  _SetFragmentShadingRate (const VkExtent2D &fragSize, VkFragmentShadingRateCombinerOpKHR primitiveOp,
									   VkFragmentShadingRateCombinerOpKHR textureOp)								__Th___;

		void  _BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<VkDeviceSize> offsets)	__Th___;

		void  _Draw (uint vertexCount,
					 uint instanceCount,
					 uint firstVertex,
					 uint firstInstance)																			__Th___;

		void  _DrawIndexed (uint indexCount,
							uint instanceCount,
							uint firstIndex,
							int  vertexOffset,
							uint firstInstance)																		__Th___;

		void  _DrawMeshTasks (const uint3 &taskCount)																__Th___;

		void  _DispatchTile ()																						__Th___;

		void  _CommitBarriers ()																					__Th___;

		void  _ClearAttachment (const VkClearAttachment &clear, const VkClearRect &rect)							__Th___;

		ND_ bool	_NoPendingBarriers ()	C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto	_GetDynamicStates ()	C_NE___	{ return _states.dynStates; }

	private:
		template <typename C> friend class _VGraphicsContextImpl;
		ND_ VSoftwareCmdBuf&  _RawCmdBuf ()			{ return *_cmdbuf; }
	};



	//
	// Vulkan Draw Context implementation
	//

	template <typename CtxImpl>
	class _VDrawContextImpl final : public CtxImpl, public IDrawContextVk
	{
	// types
	public:
		using CmdBuf_t		= typename CtxImpl::CmdBuf_t;
		using DrawCoroRef	= typename CtxImpl::DrawCoroRef;
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= AccumDrawBarriers< _VDrawContextImpl< CtxImpl >>;
		using Validator_t	= DrawContextValidation;


	// methods
	public:
		_VDrawContextImpl (const VPrimaryCmdBufState &state, CmdBuf_t cmdbuf)												__Th___;
		explicit _VDrawContextImpl (DrawCoroRef task)																		__Th___;
		explicit _VDrawContextImpl (_VDrawContextImpl &&other)																__Th___;

		_VDrawContextImpl ()																								= delete;
		_VDrawContextImpl (const _VDrawContextImpl &)																		= delete;

		// pipeline and shader resources
		void  BindPipeline (GraphicsPipelineID ppln)																		__Th_OV;
		void  BindPipeline (MeshPipelineID ppln)																			__Th_OV;
		void  BindPipeline (TilePipelineID ppln)																			__Th_OV;

		using RawCtx::BindDescriptorSet;

		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th_OV;
		void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th_OV;
		using IDrawContext::PushConstant;

		// dynamic states
		void  SetViewport (const Viewport &viewport)																		__Th_OV;
		void  SetViewports (ArrayView<Viewport> viewports)																	__Th_OV;
		void  SetScissor (const RectI &scissors)																			__Th_OV;
		void  SetScissors (ArrayView<RectI> scissors)																		__Th_OV;
		void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)				__Th_OV;
		void  SetStencilCompareMask (uint compareMask)																		__Th_OV;
		void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask)											__Th_OV;
		void  SetStencilWriteMask (uint writeMask)																			__Th_OV;
		void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask)													__Th_OV;
		void  SetStencilReference (uint reference)																			__Th_OV;
		void  SetStencilReference (uint frontReference, uint backReference)													__Th_OV;
		void  SetBlendConstants (const RGBA32f &color)																		__Th_OV	{ RawCtx::_SetBlendConstants( color ); }
		void  SetDepthBounds (float minDepthBounds, float maxDepthBounds)													__Th_OV;
		void  SetFragmentShadingRate (EShadingRate, EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp textureOp)	__Th_OV;

		using RawCtx::SetViewport;
		using RawCtx::SetScissor;
		using RawCtx::SetStencilWriteMask;
		using RawCtx::SetStencilReference;
		using RawCtx::SetStencilCompareMask;

		// draw commands
		using RawCtx::BindIndexBuffer;

		void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)												__Th_OV;
		void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)													__Th_OV;
		void  BindVertexBuffer (uint index, VkBuffer buffer, Bytes offset)													__Th___;
		void  BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<Bytes> offsets)					__Th___;
		void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)					__Th_OV;
		bool  BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, BufferID buffer, Bytes offset)		__Th_OV;
		bool  BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, VkBuffer buffer, Bytes offset)		__Th___;

		using RawCtx::DrawIndirect;
		using RawCtx::DrawIndexedIndirect;
		using RawCtx::DrawMeshTasksIndirect;
		using RawCtx::DrawIndirectCount;
		using RawCtx::DrawIndexedIndirectCount;
		using RawCtx::DrawMeshTasksIndirectCount;

		using IDrawContext::Draw;
		using IDrawContext::DrawIndexed;
		using IDrawContext::DrawIndirect;
		using IDrawContext::DrawIndexedIndirect;

		using IDrawContextVk::DrawMeshTasksIndirect;
		using IDrawContextVk::DrawIndirectCount;
		using IDrawContextVk::DrawIndexedIndirectCount;
		using IDrawContextVk::DrawMeshTasksIndirectCount;

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

		void  DrawIndexedIndirect (BufferID		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride)																		__Th_OV;

		// mesh shader
		void  DrawMeshTasks (const uint3 &taskCount)																		__Th_OV	{ RawCtx::_DrawMeshTasks( taskCount ); }

		void  DrawMeshTasksIndirect (BufferID	indirectBuffer,
									 Bytes		indirectBufferOffset,
									 uint		drawCount,
									 Bytes		stride)																		__Th_OV;

		// extension
		void  DrawIndirectCount (BufferID	indirectBuffer,
								 Bytes		indirectBufferOffset,
								 BufferID	countBuffer,
								 Bytes		countBufferOffset,
								 uint		maxDrawCount,
								 Bytes		stride)																			__Th_OV;

		void  DrawIndexedIndirectCount (BufferID	indirectBuffer,
										Bytes		indirectBufferOffset,
										BufferID	countBuffer,
										Bytes		countBufferOffset,
										uint		maxDrawCount,
										Bytes		stride)																	__Th_OV;

		void  DrawMeshTasksIndirectCount (BufferID	indirectBuffer,
										  Bytes		indirectBufferOffset,
										  BufferID	countBuffer,
										  Bytes		countBufferOffset,
										  uint		maxDrawCount,
										  Bytes		stride)																	__Th_OV;

		// tile shader
		void  DispatchTile ()																								__Th_OV	{ RawCtx::_DispatchTile(); }

		// indirect commands //
		void  BindInitialPipeline (IndirectExecutionSetID)																	__Th_OV;

		void  ExecuteGeneratedCommands (const ExecuteGeneratedCommandsCmd &)												__Th_OV;
		void  ExecuteGeneratedCommands (const ExecuteGeneratedCommands2Cmd &)												__Th_OV;

		void  DebugMarker (DebugLabel dbg)																					__Th_OV	{ RawCtx::_DebugMarker( dbg ); }
		void  PushDebugGroup (DebugLabel dbg)																				__Th_OV	{ RawCtx::_PushDebugGroup( dbg ); }
		void  PopDebugGroup ()																								__Th_OV	{ RawCtx::_PopDebugGroup(); }

		void  WriteTimestamp (const IQueryManager::IQuery &q, uint index, EPipelineScope srcScope)							__Th_OV	{ RawCtx::_WriteTimestamp( static_cast<VQueryManager::Query const&>(q), index, srcScope, this->_mngr.GetSupportedStages() ); }

		void  AttachmentBarrier (AttachmentName::Ref name, EResourceState srcState, EResourceState dstState)				__Th_OV	{ RawCtx::AttachmentBarrier( name, srcState, dstState ); }

		void  CommitBarriers ()																								__Th_OV	{ return RawCtx::_CommitBarriers(); }

		// clear //
		bool  ClearAttachment (AttachmentName::Ref, const RGBA32f &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		bool  ClearAttachment (AttachmentName::Ref, const RGBA32u &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		bool  ClearAttachment (AttachmentName::Ref, const RGBA32i &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		bool  ClearAttachment (AttachmentName::Ref, const DepthStencil &, const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		using IDrawContext::ClearAttachment;

		// vertex stream
		ND_ bool  AllocVStream (Bytes size, OUT VertexStream &result)														__Th_OV;

		ND_ VPrimaryCmdBufState const&	GetPrimaryCtxState ()																C_NE___	{ return this->_mngr.GetPrimaryCtxState(); }
		ND_ FrameUID					GetFrameId ()																		C_NE_OF	{ return this->_mngr.GetFrameId(); }

	private:
		template <typename ...IDs>
		ND_ decltype(auto)	_GetResourcesOrThrow (IDs ...ids)																__Th___	{ return this->_mngr.Get( ids... ); }

		bool  _ClearAttachment (AttachmentName::Ref, VkClearAttachment &, const RectI &, ImageLayer baseLayer, uint layerCount) __Th___;
	};

    extern template class _VDrawContextImpl< _VDirectDrawCtx >;
    extern template class _VDrawContextImpl< _VIndirectDrawCtx >;

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectDrawContext	= Graphics::_hidden_::_VDrawContextImpl< Graphics::_hidden_::_VDirectDrawCtx >;
	using VIndirectDrawContext	= Graphics::_hidden_::_VDrawContextImpl< Graphics::_hidden_::_VIndirectDrawCtx >;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
