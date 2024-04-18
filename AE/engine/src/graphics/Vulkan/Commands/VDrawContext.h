// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	DrawCtx --> DirectDrawCtx   --> BarrierMngr --> Vulkan device
			\-> IndirectDrawCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics/Vulkan/Commands/VAccumBarriers.h"
# include "graphics/Vulkan/VEnumCast.h"

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

		void  AttachmentBarrier (AttachmentName::Ref name, EResourceState srcState, EResourceState dstState)			__Th___	{ _mngr.AttachmentBarrier( name, srcState, dstState ); }

		void  SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask)									__Th___;
		void  SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference)									__Th___;
		void  SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask)								__Th___;

		void  SetViewport (uint first, ArrayView<VkViewport> viewports)												__Th___;
		void  SetScissor (uint first, ArrayView<VkRect2D> scissors)													__Th___;

		ND_ VkCommandBuffer	EndCommandBuffer ()																		__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()																	__Th___;

	protected:
		explicit _VDirectDrawCtx (const DrawTask &task)																__Th___;
		_VDirectDrawCtx (const VPrimaryCmdBufState &state, VCommandBuffer cmdbuf)									__Th___;

		void  _BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout, EPipelineDynamicState dynStates);
		void  _PushGraphicsConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages);

		void  _SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
		void  _SetDepthBounds (float minDepthBounds, float maxDepthBounds);
		void  _SetBlendConstants (const RGBA32f &color);

		void  _SetFragmentShadingRate (const VkExtent2D &fragSize, VkFragmentShadingRateCombinerOpKHR primitiveOp,
									   VkFragmentShadingRateCombinerOpKHR textureOp);

		void  _BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<VkDeviceSize> offsets);

		void  _Draw (uint vertexCount,
					 uint instanceCount,
					 uint firstVertex,
					 uint firstInstance);

		void  _DrawIndexed (uint indexCount,
							uint instanceCount,
							uint firstIndex,
							int  vertexOffset,
							uint firstInstance);

		void  _DrawMeshTasks (const uint3 &taskCount);

		void  _DispatchTile ()																						__Th___;

		void  _CommitBarriers ();

		void  _ClearAttachment (const VkClearAttachment &clear, const VkClearRect &rect);

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

		void  AttachmentBarrier (AttachmentName::Ref name, EResourceState srcState, EResourceState dstState)		__Th___	{ _mngr.AttachmentBarrier( name, srcState, dstState ); }

		void  SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask)									__Th___;
		void  SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference)									__Th___;
		void  SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask)								__Th___;

		void  SetViewport (uint first, ArrayView<VkViewport> viewports)												__Th___;
		void  SetScissor (uint first, ArrayView<VkRect2D> scissors)													__Th___;

		ND_ VBakedCommands		EndCommandBuffer ()																	__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()																__Th___;

	protected:
		explicit _VIndirectDrawCtx (const DrawTask &task)															__Th___;
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

		void  _CommitBarriers ();

		void  _ClearAttachment (const VkClearAttachment &clear, const VkClearRect &rect);

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
	class _VDrawContextImpl : public CtxImpl, public IDrawContext
	{
	// types
	public:
		using CmdBuf_t		= typename CtxImpl::CmdBuf_t;
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= VAccumDrawBarriers< _VDrawContextImpl< CtxImpl >>;
		using Validator_t	= DrawContextValidation;


	// methods
	public:
		_VDrawContextImpl (const VPrimaryCmdBufState &state, CmdBuf_t cmdbuf)												__Th___;
		explicit _VDrawContextImpl (const DrawTask &task)																	__Th___;
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
		void  SetViewport (const Viewport_t &viewport)																		__Th_OV;
		void  SetViewports (ArrayView<Viewport_t> viewports)																__Th_OV;
		void  SetScissor (const RectI &scissors)																			__Th_OV;
		void  SetScissors (ArrayView<RectI> scissors)																		__Th_OV;
		void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)				__Th_OV;
		void  SetStencilCompareMask (uint compareMask)																		__Th___;
		void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask)											__Th___;
		void  SetStencilWriteMask (uint writeMask)																			__Th___;
		void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask)													__Th___;
		void  SetStencilReference (uint reference)																			__Th_OV;
		void  SetStencilReference (uint frontReference, uint backReference)													__Th_OV;
		void  SetBlendConstants (const RGBA32f &color)																		__Th_OV	{ RawCtx::_SetBlendConstants( color ); }
		void  SetDepthBounds (float minDepthBounds, float maxDepthBounds)													__Th___;
		void  SetFragmentShadingRate (EShadingRate, EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp textureOp)	__Th___;

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

		using RawCtx::DrawIndirect;
		using RawCtx::DrawIndexedIndirect;

		using IDrawContext::Draw;
		using IDrawContext::DrawIndexed;
		using IDrawContext::DrawIndirect;
		using IDrawContext::DrawIndexedIndirect;
		using IDrawContext::DrawMeshTasksIndirect;

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
		using RawCtx::DrawIndirectCount;
		using RawCtx::DrawIndexedIndirectCount;

		void  DrawIndirectCount (const DrawIndirectCountCmd &cmd)															__Th___	{ DrawIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
		void  DrawIndirectCount (BufferID	indirectBuffer,
								 Bytes		indirectBufferOffset,
								 BufferID	countBuffer,
								 Bytes		countBufferOffset,
								 uint		maxDrawCount,
								 Bytes		stride)																			__Th___;

		void  DrawIndexedIndirectCount (const DrawIndexedIndirectCountCmd &cmd)												__Th___	{ DrawIndexedIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
		void  DrawIndexedIndirectCount (BufferID	indirectBuffer,
										Bytes		indirectBufferOffset,
										BufferID	countBuffer,
										Bytes		countBufferOffset,
										uint		maxDrawCount,
										Bytes		stride)																	__Th___;

		using RawCtx::DrawMeshTasksIndirect;
		using RawCtx::DrawMeshTasksIndirectCount;

		void  DrawMeshTasksIndirectCount (const DrawMeshTasksIndirectCountCmd &cmd)											__Th___	{ DrawMeshTasksIndirectCount( cmd.indirectBuffer, cmd.indirectBufferOffset, cmd.countBuffer, cmd.countBufferOffset, cmd.maxDrawCount, cmd.stride ); }
		void  DrawMeshTasksIndirectCount (BufferID	indirectBuffer,
										  Bytes		indirectBufferOffset,
										  BufferID	countBuffer,
										  Bytes		countBufferOffset,
										  uint		maxDrawCount,
										  Bytes		stride)																	__Th___;

		// tile shader
		void  DispatchTile ()																								__Th_OV	{ RawCtx::_DispatchTile(); }

		void  DebugMarker (DebugLabel dbg)																					__Th_OV	{ RawCtx::_DebugMarker( dbg ); }
		void  PushDebugGroup (DebugLabel dbg)																				__Th_OV	{ RawCtx::_PushDebugGroup( dbg ); }
		void  PopDebugGroup ()																								__Th_OV	{ RawCtx::_PopDebugGroup(); }

		void  AttachmentBarrier (AttachmentName::Ref name, EResourceState srcState, EResourceState dstState)				__Th_OV	{ RawCtx::AttachmentBarrier( name, srcState, dstState ); }

		void  CommitBarriers ()																								__Th_OV	{ return RawCtx::_CommitBarriers(); }

		// clear //
		bool  ClearAttachment (AttachmentName::Ref, const RGBA32f &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		bool  ClearAttachment (AttachmentName::Ref, const RGBA32u &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		bool  ClearAttachment (AttachmentName::Ref, const RGBA32i &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		bool  ClearAttachment (AttachmentName::Ref, const RGBA8u  &,      const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;
		bool  ClearAttachment (AttachmentName::Ref, const DepthStencil &, const RectI &, ImageLayer baseLayer = 0_layer, uint layerCount = 1) __Th_OV;

		// vertex stream
		ND_ bool  AllocVStream (Bytes size, OUT VertexStream &result)														__Th_OV;

		ND_ VPrimaryCmdBufState const&	GetPrimaryCtxState ()																C_NE___	{ return this->_mngr.GetPrimaryCtxState(); }
		ND_ FrameUID					GetFrameId ()																		C_NE_OF	{ return this->_mngr.GetFrameId(); }

	private:
		template <typename ...IDs>
		ND_ decltype(auto)	_GetResourcesOrThrow (IDs ...ids)																__Th___	{ return this->_mngr.Get( ids... ); }

		bool  _ClearAttachment (AttachmentName::Ref, VkClearAttachment &, const RectI &, ImageLayer baseLayer, uint layerCount) __Th___;
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectDrawContext	= Graphics::_hidden_::_VDrawContextImpl< Graphics::_hidden_::_VDirectDrawCtx >;
	using VIndirectDrawContext	= Graphics::_hidden_::_VDrawContextImpl< Graphics::_hidden_::_VIndirectDrawCtx >;

} // AE::Graphics


namespace AE::Graphics::_hidden_
{

/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_VDrawContextImpl<C>::_VDrawContextImpl (const VPrimaryCmdBufState &state, CmdBuf_t cmdbuf) __Th___ :
		RawCtx{ state, RVRef(cmdbuf) }	// throw
	{}

	template <typename C>
	_VDrawContextImpl<C>::_VDrawContextImpl (_VDrawContextImpl &&other) __Th___ :
		RawCtx{ other.GetPrimaryCtxState(), RVRef(other._cmdbuf) }	// throw
	{}

	template <typename C>
	_VDrawContextImpl<C>::_VDrawContextImpl (const DrawTask &task) __Th___ :
		RawCtx{ task }	// throw
	{
		if_likely( auto* batch = task.GetDrawBatchPtr() )
		{
			if_likely( not batch->GetViewports().empty() )
				RawCtx::SetViewport( 0, batch->GetViewports() );

			if_likely( not batch->GetScissors().empty() )
				RawCtx::SetScissor( 0, batch->GetScissors() );
		}
	}

/*
=================================================
	BindPipeline
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::BindPipeline (GraphicsPipelineID ppln) __Th___
	{
		auto&	gppln = _GetResourcesOrThrow( ppln );
		GCTX_CHECK( gppln.RenderPassSubpassIndex() == GetPrimaryCtxState().subpassIndex );

		RawCtx::_BindPipeline( gppln.BindPoint(), gppln.Handle(), gppln.Layout(), gppln.DynamicState() );
	}

	template <typename C>
	void  _VDrawContextImpl<C>::BindPipeline (MeshPipelineID ppln) __Th___
	{
		auto&	mppln = _GetResourcesOrThrow( ppln );
		GCTX_CHECK( mppln.RenderPassSubpassIndex() == GetPrimaryCtxState().subpassIndex );

		RawCtx::_BindPipeline( mppln.BindPoint(), mppln.Handle(), mppln.Layout(), mppln.DynamicState() );
	}

	template <typename C>
	void  _VDrawContextImpl<C>::BindPipeline (TilePipelineID ppln) __Th___
	{
		auto&	tppln = _GetResourcesOrThrow( ppln );
		GCTX_CHECK( tppln.RenderPassSubpassIndex() == GetPrimaryCtxState().subpassIndex );

		RawCtx::_BindPipeline( tppln.BindPoint(), tppln.Handle(), tppln.Layout(), tppln.DynamicState() );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		auto&	desc_set = _GetResourcesOrThrow( ds );

		BindDescriptorSet( index, desc_set.Handle(), dynamicOffsets );
	}

/*
=================================================
	PushConstant
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName) __Th___
	{
		VALIDATE_GCTX( PushConstant( idx, size, typeName ));
		Unused( typeName );

		RawCtx::_PushGraphicsConstant( idx.offset, size, values, EShaderStages(0) | idx.stage );
	}

/*
=================================================
	SetViewport
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetViewport (const Viewport_t &viewport) __Th___
	{
		return SetViewports( ArrayView<Viewport_t>{ &viewport, 1u });
	}

	template <typename C>
	void  _VDrawContextImpl<C>::SetViewports (ArrayView<Viewport_t> viewports) __Th___
	{
		GCTX_CHECK( viewports.size() <= GraphicsConfig::MaxViewports );

		StaticArray< VkViewport, GraphicsConfig::MaxViewports >	vk_viewports;

		for (usize i = 0; i < viewports.size(); ++i)
		{
			const auto&	src = viewports[i];
			auto&		dst = vk_viewports[i];

			dst.x			= src.rect.left;
			dst.y			= src.rect.top;
			dst.width		= src.rect.Width();
			dst.height		= src.rect.Height();
			dst.minDepth	= src.minDepth;
			dst.maxDepth	= src.maxDepth;
		}
		RawCtx::SetViewport( 0, ArrayView<VkViewport>{ vk_viewports.data(), viewports.size() });
	}

/*
=================================================
	SetScissor
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetScissor (const RectI &scissor) __Th___
	{
		return SetScissors( ArrayView<RectI>{ &scissor, 1u });
	}

	template <typename C>
	void  _VDrawContextImpl<C>::SetScissors (ArrayView<RectI> scissors) __Th___
	{
		GCTX_CHECK( scissors.size() <= GraphicsConfig::MaxViewports );

		StaticArray< VkRect2D, GraphicsConfig::MaxViewports >	vk_scissors;

		for (usize i = 0; i < scissors.size(); ++i)
		{
			auto&	src = scissors[i];
			auto&	dst = vk_scissors[i];

			dst.offset	= { src.left,          src.top };
			dst.extent	= { uint(src.Width()), uint(src.Height()) };
		}
		RawCtx::SetScissor( 0, ArrayView<VkRect2D>{ vk_scissors.data(), scissors.size() });
	}

/*
=================================================
	SetDepthBias
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) __Th___
	{
		RawCtx::_SetDepthBias( depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor );
	}

/*
=================================================
	SetDepthBounds
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetDepthBounds (float minDepthBounds, float maxDepthBounds) __Th___
	{
		RawCtx::_SetDepthBounds( minDepthBounds, maxDepthBounds );
	}

/*
=================================================
	SetFragmentShadingRate
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetFragmentShadingRate (EShadingRate rate, EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp textureOp) __Th___
	{
		VALIDATE_GCTX( SetFragmentShadingRate( this->_GetDynamicStates(), rate, primitiveOp, textureOp ));

		uint2	size = EShadingRate_Size( rate );

		RawCtx::_SetFragmentShadingRate( VkExtent2D{size.x, size.y}, VEnumCast(primitiveOp), VEnumCast(textureOp) );
	}

/*
=================================================
	SetStencilCompareMask
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetStencilCompareMask (uint compareMask) __Th___
	{
		RawCtx::SetStencilCompareMask( VK_STENCIL_FACE_FRONT_AND_BACK, compareMask );
	}

	template <typename C>
	void  _VDrawContextImpl<C>::SetStencilCompareMask (uint frontCompareMask, uint backCompareMask) __Th___
	{
		RawCtx::SetStencilCompareMask( VK_STENCIL_FACE_FRONT_BIT, frontCompareMask );
		RawCtx::SetStencilCompareMask( VK_STENCIL_FACE_BACK_BIT,  backCompareMask );
	}

/*
=================================================
	SetStencilWriteMask
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetStencilWriteMask (uint writeMask) __Th___
	{
		RawCtx::SetStencilWriteMask( VK_STENCIL_FACE_FRONT_AND_BACK, writeMask );
	}

	template <typename C>
	void  _VDrawContextImpl<C>::SetStencilWriteMask (uint frontWriteMask, uint backWriteMask) __Th___
	{
		RawCtx::SetStencilWriteMask( VK_STENCIL_FACE_FRONT_BIT, frontWriteMask );
		RawCtx::SetStencilWriteMask( VK_STENCIL_FACE_BACK_BIT,  backWriteMask );
	}

/*
=================================================
	SetStencilReference
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetStencilReference (uint reference) __Th___
	{
		RawCtx::SetStencilReference( VK_STENCIL_FACE_FRONT_AND_BACK, reference );
	}

	template <typename C>
	void  _VDrawContextImpl<C>::SetStencilReference (uint frontReference, uint backReference) __Th___
	{
		RawCtx::SetStencilReference( VK_STENCIL_FACE_FRONT_BIT, frontReference );
		RawCtx::SetStencilReference( VK_STENCIL_FACE_BACK_BIT,  backReference );
	}

/*
=================================================
	BindIndexBuffer
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( buffer );

		return BindIndexBuffer( buf.Handle(), offset, indexType );
	}

/*
=================================================
	BindVertexBuffer
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::BindVertexBuffer (uint index, BufferID buffer, Bytes offset) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( buffer );

		BindVertexBuffer( index, buf.Handle(), offset );
	}

	template <typename C>
	void  _VDrawContextImpl<C>::BindVertexBuffer (uint index, VkBuffer buffer, Bytes offset) __Th___
	{
		VALIDATE_GCTX( BindVertexBuffers( index, {buffer}, {offset} ));

		const VkDeviceSize	off	= VkDeviceSize(offset);
		RawCtx::_BindVertexBuffers( index, ArrayView<VkBuffer>{&buffer, 1}, ArrayView<VkDeviceSize>{&off, 1} );
	}

	template <typename C>
	bool  _VDrawContextImpl<C>::BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, BufferID buffer, Bytes offset) __Th___
	{
		auto  [ppln, buf] = _GetResourcesOrThrow( pplnId, buffer );

		uint	idx = ppln.GetVertexBufferIndex( name );
		CHECK_ERR( idx != UMax );

		BindVertexBuffer( idx, buf.Handle(), offset );
		return true;
	}

/*
=================================================
	BindVertexBuffers
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets) __Th___
	{
		StaticAssert( sizeof(Bytes) == sizeof(VkDeviceSize) );
		VALIDATE_GCTX( BindVertexBuffers( firstBinding, buffers, offsets ));

		StaticArray< VkBuffer, GraphicsConfig::MaxVertexBuffers >	dst_buffers;

		for (uint i = 0; i < buffers.size(); ++i)
		{
			auto&	buffer	= _GetResourcesOrThrow( buffers[i] );
			dst_buffers[i]	= buffer.Handle();
		}

		RawCtx::_BindVertexBuffers( firstBinding, dst_buffers, offsets.Cast<VkDeviceSize>() );
	}

	template <typename C>
	void  _VDrawContextImpl<C>::BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<Bytes> offsets) __Th___
	{
		VALIDATE_GCTX( BindVertexBuffers( firstBinding, buffers, offsets ));
		RawCtx::_BindVertexBuffers( firstBinding, buffers, offsets.Cast<VkDeviceSize>() );
	}

/*
=================================================
	Draw
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::Draw (uint vertexCount,
									  uint instanceCount,
									  uint firstVertex,
									  uint firstInstance) __Th___
	{
		RawCtx::_Draw( vertexCount, instanceCount, firstVertex, firstInstance );
	}

/*
=================================================
	DrawIndexed
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::DrawIndexed (uint indexCount,
											 uint instanceCount,
											 uint firstIndex,
											 int  vertexOffset,
											 uint firstInstance) __Th___
	{
		RawCtx::_DrawIndexed( indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
	}

/*
=================================================
	DrawIndirect
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::DrawIndirect (BufferID	indirectBuffer,
											  Bytes		indirectBufferOffset,
											  uint		drawCount,
											  Bytes		stride) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( DrawIndirect( buf.Description(), indirectBufferOffset, drawCount, stride ));

		RawCtx::DrawIndirect( buf.Handle(), indirectBufferOffset, drawCount, stride );
	}

/*
=================================================
	DrawIndexedIndirect
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::DrawIndexedIndirect (BufferID	indirectBuffer,
													 Bytes		indirectBufferOffset,
													 uint		drawCount,
													 Bytes		stride) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( DrawIndexedIndirect( buf.Description(), indirectBufferOffset, drawCount, stride ));

		RawCtx::DrawIndexedIndirect( buf.Handle(), indirectBufferOffset, drawCount, stride );
	}

/*
=================================================
	DrawIndirectCount
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::DrawIndirectCount (BufferID	indirectBuffer,
												   Bytes	indirectBufferOffset,
												   BufferID	countBuffer,
												   Bytes	countBufferOffset,
												   uint		maxDrawCount,
												   Bytes	stride) __Th___
	{
		auto  [ibuf, cbuf] = _GetResourcesOrThrow( indirectBuffer, countBuffer );
		VALIDATE_GCTX( DrawIndirectCount( ibuf.Description(), indirectBufferOffset,
										  cbuf.Description(), countBufferOffset,
										  maxDrawCount, stride ));

		RawCtx::DrawIndirectCount( ibuf.Handle(), indirectBufferOffset, cbuf.Handle(), countBufferOffset, maxDrawCount, stride );
	}

/*
=================================================
	DrawIndexedIndirectCount
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::DrawIndexedIndirectCount (BufferID	indirectBuffer,
														  Bytes		indirectBufferOffset,
														  BufferID	countBuffer,
														  Bytes		countBufferOffset,
														  uint		maxDrawCount,
														  Bytes		stride) __Th___
	{
		auto  [ibuf, cbuf] = _GetResourcesOrThrow( indirectBuffer, countBuffer );
		VALIDATE_GCTX( DrawIndexedIndirectCount( ibuf.Description(), indirectBufferOffset,
												 cbuf.Description(), countBufferOffset,
												 maxDrawCount, stride ));

		RawCtx::DrawIndexedIndirectCount( ibuf.Handle(), indirectBufferOffset, cbuf.Handle(), countBufferOffset, maxDrawCount, stride );
	}

/*
=================================================
	DrawMeshTasksIndirect
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::DrawMeshTasksIndirect (BufferID	indirectBuffer,
													   Bytes	indirectBufferOffset,
													   uint		drawCount,
													   Bytes	stride) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( DrawMeshTasksIndirect( buf.Description(), indirectBufferOffset, drawCount, stride ));

		RawCtx::DrawMeshTasksIndirect( buf.Handle(), indirectBufferOffset, drawCount, stride );
	}

/*
=================================================
	DrawMeshTasksIndirectCount
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::DrawMeshTasksIndirectCount (BufferID	indirectBuffer,
															Bytes		indirectBufferOffset,
															BufferID	countBuffer,
															Bytes		countBufferOffset,
															uint		maxDrawCount,
															Bytes		stride) __Th___
	{
		auto  [ibuf, cbuf] = _GetResourcesOrThrow( indirectBuffer, countBuffer );
		VALIDATE_GCTX( DrawMeshTasksIndirectCount( ibuf.Description(), indirectBufferOffset,
												   cbuf.Description(), countBufferOffset,
												   maxDrawCount, stride ));

		RawCtx::DrawMeshTasksIndirectCount( ibuf.Handle(), indirectBufferOffset, cbuf.Handle(), countBufferOffset, maxDrawCount, stride );
	}

/*
=================================================
	AllocVStream
=================================================
*/
	template <typename C>
	bool  _VDrawContextImpl<C>::AllocVStream (Bytes size, OUT VertexStream &result)
	{
		return this->_mngr.GetResourceManager().GetStagingManager().AllocVStream( GetFrameId(), size, OUT result );
	}

/*
=================================================
	ClearAttachment
=================================================
*/
	template <typename C>
	bool  _VDrawContextImpl<C>::ClearAttachment (AttachmentName::Ref name, const RGBA32f &color, const RectI &rect, ImageLayer baseLayer, uint layerCount) __Th___
	{
		VkClearAttachment	clear_att	= {};
		clear_att.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
		MemCopy( OUT clear_att.clearValue.color.int32, color );

		return _ClearAttachment( name, clear_att, rect, baseLayer, layerCount );
	}

	template <typename C>
	bool  _VDrawContextImpl<C>::ClearAttachment (AttachmentName::Ref name, const RGBA32u &color, const RectI &rect, ImageLayer baseLayer, uint layerCount) __Th___
	{
		VkClearAttachment	clear_att	= {};
		clear_att.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
		MemCopy( OUT clear_att.clearValue.color.int32, color );

		return _ClearAttachment( name, clear_att, rect, baseLayer, layerCount );
	}

	template <typename C>
	bool  _VDrawContextImpl<C>::ClearAttachment (AttachmentName::Ref name, const RGBA32i &color, const RectI &rect, ImageLayer baseLayer, uint layerCount) __Th___
	{
		VkClearAttachment	clear_att	= {};
		clear_att.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
		MemCopy( OUT clear_att.clearValue.color.int32, color );

		return _ClearAttachment( name, clear_att, rect, baseLayer, layerCount );
	}

	template <typename C>
	bool  _VDrawContextImpl<C>::ClearAttachment (AttachmentName::Ref name, const RGBA8u &color, const RectI &rect, ImageLayer baseLayer, uint layerCount) __Th___
	{
		return ClearAttachment( name, RGBA32f{color}, rect, baseLayer, layerCount );
	}

	template <typename C>
	bool  _VDrawContextImpl<C>::ClearAttachment (AttachmentName::Ref name, const DepthStencil &dsClear, const RectI &rect, ImageLayer baseLayer, uint layerCount) __Th___
	{
		VkClearAttachment	clear_att		= {};
		clear_att.aspectMask				= VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		clear_att.clearValue.depthStencil	= { dsClear.depth, dsClear.stencil };

		return _ClearAttachment( name, clear_att, rect, baseLayer, layerCount );
	}

	template <typename C>
	bool  _VDrawContextImpl<C>::_ClearAttachment (AttachmentName::Ref name, VkClearAttachment &clearAtt, const RectI &rect, ImageLayer baseLayer, uint layerCount) __Th___
	{
		VALIDATE_GCTX( ClearAttachment( rect ));

		const uint	idx = this->_mngr.GetAttachmentIndex( name );
		CHECK_ERR( idx != UMax );

		clearAtt.colorAttachment = idx;

		VkClearRect		vk_rect	= {};
		vk_rect.rect.offset		= { rect.left, rect.top };
		vk_rect.rect.extent		= { uint(rect.Width()), uint(rect.Height()) };
		vk_rect.baseArrayLayer	= baseLayer.Get();
		vk_rect.layerCount		= layerCount;

		RawCtx::_ClearAttachment( clearAtt, vk_rect );
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_BindPipeline
=================================================
*/
	inline void  _VDirectDrawCtx::_BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout, EPipelineDynamicState dynStates)
	{
		_states.bindPoint	= bindPoint;
		_states.pplnLayout	= layout;
		_states.dynStates	= dynStates;

		vkCmdBindPipeline( _cmdbuf.Get(), bindPoint, ppln );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	inline void  _VDirectDrawCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		VALIDATE_GCTX( BindDescriptorSet( _states.pplnLayout, index, ds ));

		vkCmdBindDescriptorSets( _cmdbuf.Get(), _states.bindPoint, _states.pplnLayout, index.vkIndex, 1, &ds, uint(dynamicOffsets.size()), dynamicOffsets.data() );
	}

/*
=================================================
	_PushGraphicsConstant
=================================================
*/
	inline void  _VDirectDrawCtx::_PushGraphicsConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages) __Th___
	{
		VALIDATE_GCTX( PushConstant( _states.pplnLayout, offset, size, values, stages ));

		vkCmdPushConstants( _cmdbuf.Get(), _states.pplnLayout, VEnumCast(stages), uint(offset), uint(size), values );
	}

/*
=================================================
	SetViewport
=================================================
*/
	inline void  _VDirectDrawCtx::SetViewport (uint first, ArrayView<VkViewport> viewports) __Th___
	{
		VALIDATE_GCTX( SetViewport( first, viewports ));

		vkCmdSetViewport( _cmdbuf.Get(), first, uint(viewports.size()), viewports.data() );
	}

/*
=================================================
	SetScissor
=================================================
*/
	inline void  _VDirectDrawCtx::SetScissor (uint first, ArrayView<VkRect2D> scissors)
	{
		VALIDATE_GCTX( SetScissor( first, scissors ));

		vkCmdSetScissor( _cmdbuf.Get(), first, uint(scissors.size()), scissors.data() );
	}

/*
=================================================
	_SetDepthBias
=================================================
*/
	inline void  _VDirectDrawCtx::_SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) __Th___
	{
		VALIDATE_GCTX( SetDepthBias( _GetDynamicStates(), depthBiasClamp ));

		vkCmdSetDepthBias( _cmdbuf.Get(), depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor );
	}

/*
=================================================
	_SetDepthBounds
=================================================
*/
	inline void  _VDirectDrawCtx::_SetDepthBounds (float minDepthBounds, float maxDepthBounds) __Th___
	{
		VALIDATE_GCTX( SetDepthBounds( _GetDynamicStates() ));

		vkCmdSetDepthBounds( _cmdbuf.Get(), minDepthBounds, maxDepthBounds );
	}

/*
=================================================
	_SetFragmentShadingRate
=================================================
*/
	inline void  _VDirectDrawCtx::_SetFragmentShadingRate (const VkExtent2D &fragSize, VkFragmentShadingRateCombinerOpKHR primitiveOp,
														   VkFragmentShadingRateCombinerOpKHR textureOp)
	{
		ASSERT( fragSize.width <= 4 and fragSize.height <= 4 );

		VkFragmentShadingRateCombinerOpKHR	combiner_ops[2] = { primitiveOp, textureOp };

		vkCmdSetFragmentShadingRateKHR( _cmdbuf.Get(), &fragSize, combiner_ops );
	}

/*
=================================================
	SetStencilCompareMask
=================================================
*/
	inline void  _VDirectDrawCtx::SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask) __Th___
	{
		VALIDATE_GCTX( SetStencilCompareMask( _GetDynamicStates() ));

		vkCmdSetStencilCompareMask( _cmdbuf.Get(), faceMask, compareMask );
	}

/*
=================================================
	SetStencilWriteMask
=================================================
*/
	inline void  _VDirectDrawCtx::SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask) __Th___
	{
		VALIDATE_GCTX( SetStencilWriteMask( _GetDynamicStates() ));

		vkCmdSetStencilWriteMask( _cmdbuf.Get(), faceMask, writeMask );
	}

/*
=================================================
	SetStencilReference
=================================================
*/
	inline void  _VDirectDrawCtx::SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference) __Th___
	{
		VALIDATE_GCTX( SetStencilReference( _GetDynamicStates() ));

		vkCmdSetStencilReference( _cmdbuf.Get(), faceMask, reference );
	}

/*
=================================================
	_SetBlendConstants
=================================================
*/
	inline void  _VDirectDrawCtx::_SetBlendConstants (const RGBA32f &color) __Th___
	{
		VALIDATE_GCTX( SetBlendConstants( _GetDynamicStates() ));

		vkCmdSetBlendConstants( _cmdbuf.Get(), color.data() );
	}

/*
=================================================
	BindIndexBuffer
=================================================
*/
	inline void  _VDirectDrawCtx::BindIndexBuffer (VkBuffer buffer, Bytes offset, EIndex indexType) __Th___
	{
		VALIDATE_GCTX( BindIndexBuffer( buffer, indexType ));

		vkCmdBindIndexBuffer( _cmdbuf.Get(), buffer, VkDeviceSize(offset), VEnumCast(indexType) );

		// TODO: vkCmdBindIndexBuffer2KHR (VK_KHR_maintenance5)
	}

/*
=================================================
	_BindVertexBuffers
=================================================
*/
	inline void  _VDirectDrawCtx::_BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<VkDeviceSize> offsets)
	{
		vkCmdBindVertexBuffers( _cmdbuf.Get(), firstBinding, uint(buffers.size()), buffers.data(), offsets.data() );

		// TODO: vkCmdBindVertexBuffers2
	}

/*
=================================================
	_Draw
=================================================
*/
	inline void  _VDirectDrawCtx::_Draw (uint vertexCount,
										 uint instanceCount,
										 uint firstVertex,
										 uint firstInstance) __Th___
	{
		VALIDATE_GCTX( Draw( _states.pplnLayout ));

		vkCmdDraw( _cmdbuf.Get(), vertexCount, instanceCount, firstVertex, firstInstance );
	}

/*
=================================================
	_DrawIndexed
=================================================
*/
	inline void  _VDirectDrawCtx::_DrawIndexed (uint indexCount,
												uint instanceCount,
												uint firstIndex,
												int  vertexOffset,
												uint firstInstance) __Th___
	{
		VALIDATE_GCTX( DrawIndexed( _states.pplnLayout ));

		vkCmdDrawIndexed( _cmdbuf.Get(), indexCount, instanceCount, firstIndex, vertexOffset, firstInstance );
	}

/*
=================================================
	DrawIndirect
=================================================
*/
	inline void  _VDirectDrawCtx::DrawIndirect (VkBuffer indirectBuffer,
												Bytes	 indirectBufferOffset,
												uint	 drawCount,
												Bytes	 stride) __Th___
	{
		VALIDATE_GCTX( DrawIndirect( _states.pplnLayout, indirectBuffer, drawCount, stride ));

		vkCmdDrawIndirect( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset), drawCount, CheckCast<uint>(stride) );
	}

/*
=================================================
	DrawIndexedIndirect
=================================================
*/
	inline void  _VDirectDrawCtx::DrawIndexedIndirect (VkBuffer	indirectBuffer,
													   Bytes	indirectBufferOffset,
													   uint		drawCount,
													   Bytes	stride) __Th___
	{
		VALIDATE_GCTX( DrawIndexedIndirect( _states.pplnLayout, indirectBuffer, drawCount, stride ));

		vkCmdDrawIndexedIndirect( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset), drawCount, CheckCast<uint>(stride) );
	}

/*
=================================================
	DrawIndirectCount
=================================================
*/
	inline void  _VDirectDrawCtx::DrawIndirectCount (VkBuffer	indirectBuffer,
													 Bytes		indirectBufferOffset,
													 VkBuffer	countBuffer,
													 Bytes		countBufferOffset,
													 uint		maxDrawCount,
													 Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawIndirectCount( _states.pplnLayout, indirectBuffer, countBuffer, maxDrawCount, stride ));

		vkCmdDrawIndirectCountKHR( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset), countBuffer, VkDeviceSize(countBufferOffset), maxDrawCount, CheckCast<uint>(stride) );
	}

/*
=================================================
	DrawIndexedIndirectCount
=================================================
*/
	inline void  _VDirectDrawCtx::DrawIndexedIndirectCount (VkBuffer	indirectBuffer,
															Bytes		indirectBufferOffset,
															VkBuffer	countBuffer,
															Bytes		countBufferOffset,
															uint		maxDrawCount,
															Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawIndexedIndirectCount( _states.pplnLayout, indirectBuffer, countBuffer, maxDrawCount, stride ));

		vkCmdDrawIndexedIndirectCountKHR( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset),
										  countBuffer, VkDeviceSize(countBufferOffset),
										  maxDrawCount, CheckCast<uint>(stride) );
	}

/*
=================================================
	_DrawMeshTasks
=================================================
*/
	inline void  _VDirectDrawCtx::_DrawMeshTasks (const uint3 &taskCount) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasks( _states.pplnLayout, taskCount ));

		vkCmdDrawMeshTasksEXT( _cmdbuf.Get(), taskCount.x, taskCount.y, taskCount.z );
	}

/*
=================================================
	DrawMeshTasksIndirect
=================================================
*/
	inline void  _VDirectDrawCtx::DrawMeshTasksIndirect (VkBuffer	indirectBuffer,
														 Bytes		indirectBufferOffset,
														 uint		drawCount,
														 Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasksIndirect( _states.pplnLayout, indirectBuffer, drawCount, stride ));

		vkCmdDrawMeshTasksIndirectEXT( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset), drawCount, CheckCast<uint>(stride) );
	}

/*
=================================================
	DrawMeshTasksIndirectCount
=================================================
*/
	inline void  _VDirectDrawCtx::DrawMeshTasksIndirectCount (VkBuffer	indirectBuffer,
															  Bytes		indirectBufferOffset,
															  VkBuffer	countBuffer,
															  Bytes		countBufferOffset,
															  uint		maxDrawCount,
															  Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasksIndirectCount( _states.pplnLayout, indirectBuffer, countBuffer, maxDrawCount, stride ));

		vkCmdDrawMeshTasksIndirectCountEXT( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset),
											countBuffer, VkDeviceSize(countBufferOffset),
											maxDrawCount, CheckCast<uint>(stride) );
	}

/*
=================================================
	_DispatchTile
=================================================
*/
	inline void  _VDirectDrawCtx::_DispatchTile () __Th___
	{
		GCTX_CHECK( _states.pplnLayout != Default );
		//ASSERT( _GetExtensions().subpassShadingHW );	// TODO: remove

		vkCmdSubpassShadingHUAWEI( _cmdbuf.Get() );
	}

/*
=================================================
	_CommitBarriers
=================================================
*/
	inline void  _VDirectDrawCtx::_CommitBarriers ()
	{
	    auto* bar = _mngr.GetBarriers();
		if_unlikely( bar != null )
		{
			vkCmdPipelineBarrier2KHR( _cmdbuf.Get(), bar );
			_mngr.ClearBarriers();
		}
	}

/*
=================================================
	_ClearAttachment
=================================================
*/
	inline void  _VDirectDrawCtx::_ClearAttachment (const VkClearAttachment &clear, const VkClearRect &rect)
	{
		ASSERT( _NoPendingBarriers() );
		vkCmdClearAttachments( _cmdbuf.Get(), 1, &clear, 1, &rect );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
