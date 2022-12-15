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
	public:
		using CmdBuf_t = VCommandBuffer;


	// variables
	private:
		struct {
			VkPipelineBindPoint		bindPoint		= VK_PIPELINE_BIND_POINT_MAX_ENUM;
			VkPipelineLayout		pplnLayout		= Default;
			EPipelineDynamicState	flags			= Default;
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
		
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)				__Th___	{ _mngr.AttachmentBarrier( name, srcState, dstState ); }

		void  SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask)									__Th___;
		void  SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference)									__Th___;
		void  SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask)								__Th___;

		void  SetViewport (uint first, ArrayView<VkViewport> viewports)												__Th___;
		void  SetScissor (uint first, ArrayView<VkRect2D> scissors)													__Th___;
		
		ND_ VkCommandBuffer	EndCommandBuffer ()																		__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()																	__Th___;
		
	private:
		_VDirectDrawCtx (const DrawTask &task, CmdBuf_t cmdbuf)														__Th___;
	protected:
		explicit _VDirectDrawCtx (const DrawTask &task)																__Th___;
		_VDirectDrawCtx (const VPrimaryCmdBufState &state, CmdBuf_t cmdbuf, DebugLabel dbg)							__Th___;

		void  _BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout, EPipelineDynamicState flags);
		void  _PushGraphicsConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages);
		
		void  _SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
		void  _SetDepthBounds (float minDepthBounds, float maxDepthBounds);
		void  _SetBlendConstants (const RGBA32f &color);
		
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
		
		void  _DispatchTile ();

		void  _CommitBarriers ();

		ND_ bool	_NoPendingBarriers ()	C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetExtensions ()		C_NE___	{ return _mngr.GetDevice().GetExtensions(); }
		ND_ auto&	_GetFeatures ()			C_NE___	{ return _mngr.GetDevice().GetProperties().features; }
		ND_ auto&	_GetDeviceProperties ()	C_NE___	{ return _mngr.GetDevice().GetDeviceProperties(); }

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
	public:
		using CmdBuf_t = VSoftwareCmdBufPtr;


	// variables
	private:
		struct {
			VkPipelineBindPoint		bindPoint		= VK_PIPELINE_BIND_POINT_MAX_ENUM;
			VkPipelineLayout		pplnLayout		= Default;
			EPipelineDynamicState	flags			= Default;
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
		
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)				__Th___	{ _mngr.AttachmentBarrier( name, srcState, dstState ); }
		
		void  SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask)									__Th___;
		void  SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference)									__Th___;
		void  SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask)								__Th___;

		void  SetViewport (uint first, ArrayView<VkViewport> viewports)												__Th___;
		void  SetScissor (uint first, ArrayView<VkRect2D> scissors)													__Th___;
		
		ND_ VBakedCommands		EndCommandBuffer ()																	__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()																__Th___;

	protected:
		explicit _VIndirectDrawCtx (const DrawTask &task)															__Th___;
		_VIndirectDrawCtx (const VPrimaryCmdBufState &state, CmdBuf_t cmdbuf, DebugLabel dbg)						__Th___;
		
		void  _BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout, EPipelineDynamicState flags);
		void  _PushGraphicsConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages);
		
		void  _SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor);
		void  _SetDepthBounds (float minDepthBounds, float maxDepthBounds);
		void  _SetBlendConstants (const RGBA32f &color);
		
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
		
		void  _DispatchTile ();

		void  _CommitBarriers ();

		ND_ bool	_NoPendingBarriers ()	C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetExtensions ()		C_NE___	{ return _mngr.GetDevice().GetExtensions(); }
		ND_ auto&	_GetFeatures ()			C_NE___	{ return _mngr.GetDevice().GetProperties().features; }
		ND_ auto&	_GetDeviceProperties ()	C_NE___	{ return _mngr.GetDevice().GetDeviceProperties(); }
		
	private:
		template <typename C> friend class _VGraphicsContextImpl;
		ND_ VSoftwareCmdBuf&  _RawCmdBuf ()		{ return *_cmdbuf; }
	};



	//
	// Vulkan Draw Context implementation
	//

	template <typename CtxImpl>
	class _VDrawContextImpl final : public CtxImpl, public IDrawContext
	{
	// types
	public:
		static constexpr bool	IsDrawContext		= true;
		static constexpr bool	IsVulkanDrawContext	= true;
		
	private:
		using RawCtx	= CtxImpl;
		using AccumBar	= VAccumDrawBarriers< _VDrawContextImpl< CtxImpl >>;
		using CmdBuf_t	= typename CtxImpl::CmdBuf_t;

		
	// methods
	public:
		_VDrawContextImpl (const VPrimaryCmdBufState &state, CmdBuf_t cmdbuf, DebugLabel dbg)						__Th___;
		explicit _VDrawContextImpl (const DrawTask &task)															__Th___;
		
		_VDrawContextImpl ()																						= delete;
		_VDrawContextImpl (const _VDrawContextImpl &)																= delete;

		// pipeline and shader resources
		void  BindPipeline (GraphicsPipelineID ppln)																__Th_OV;
		void  BindPipeline (MeshPipelineID ppln)																	__Th_OV;
		void  BindPipeline (TilePipelineID ppln)																	__Th_OV;

		using RawCtx::BindDescriptorSet;

		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)__Th_OV;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)						__Th_OV;
		
		// dynamic states
		void  SetViewport (const Viewport_t &viewport)																__Th_OV;
		void  SetViewports (ArrayView<Viewport_t> viewports)														__Th_OV;
		void  SetScissor (const RectI &scissors)																	__Th_OV;
		void  SetScissors (ArrayView<RectI> scissors)																__Th_OV;
		void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)		__Th_OV;
		void  SetStencilCompareMask (uint compareMask)																__Th___;
		void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask)									__Th___;
		void  SetStencilWriteMask (uint writeMask)																	__Th___;
		void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask)											__Th___;
		void  SetStencilReference (uint reference)																	__Th_OV;
		void  SetStencilReference (uint frontReference, uint backReference)											__Th_OV;
		void  SetBlendConstants (const RGBA32f &color)																__Th_OV	{ RawCtx::_SetBlendConstants( color ); }
		void  SetDepthBounds (float minDepthBounds, float maxDepthBounds)											__Th___;
		
		using RawCtx::SetViewport;
		using RawCtx::SetScissor;
		using RawCtx::SetStencilWriteMask;
		using RawCtx::SetStencilReference;
		using RawCtx::SetStencilCompareMask;

		// draw commands
		using RawCtx::BindIndexBuffer;

		void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)											__Th_OV;
		void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset)												__Th_OV;
		void  BindVertexBuffer (uint index, VkBuffer buffer, Bytes offset)												__Th___;
		void  BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<Bytes> offsets)				__Th___;
		void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)				__Th_OV;
		bool  BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset)	__Th_OV;

		using RawCtx::DrawIndirect;
		using RawCtx::DrawIndexedIndirect;

		using IDrawContext::Draw;
		using IDrawContext::DrawIndexed;
		using IDrawContext::DrawIndirect;
		using IDrawContext::DrawIndexedIndirect;

		void  Draw (uint vertexCount,
					uint instanceCount	= 1,
					uint firstVertex	= 0,
					uint firstInstance	= 0)																	__Th_OV;

		void  DrawIndexed (uint indexCount,
						   uint instanceCount	= 1,
						   uint firstIndex		= 0,
						   int  vertexOffset	= 0,
						   uint firstInstance	= 0)															__Th_OV;

		void  DrawIndirect (BufferID	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride)																	__Th_OV;

		void  DrawIndexedIndirect (BufferID		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride)															__Th_OV;

		// mesh shader
		void  DrawMeshTasks (const uint3 &taskCount)															__Th_OV	{ RawCtx::_DrawMeshTasks( taskCount ); }

		void  DrawMeshTasksIndirect (BufferID	indirectBuffer,
									 Bytes		indirectBufferOffset,
									 uint		drawCount,
									 Bytes		stride)															__Th_OV;

		// extension
		using RawCtx::DrawIndirectCount;
		using RawCtx::DrawIndexedIndirectCount;

		void  DrawIndirectCount (BufferID	indirectBuffer,
								 Bytes		indirectBufferOffset,
								 BufferID	countBuffer,
								 Bytes		countBufferOffset,
								 uint		maxDrawCount,
								 Bytes		stride)																__Th___;

		void  DrawIndexedIndirectCount (BufferID	indirectBuffer,
										Bytes		indirectBufferOffset,
										BufferID	countBuffer,
										Bytes		countBufferOffset,
										uint		maxDrawCount,
										Bytes		stride)														__Th___;

		using RawCtx::DrawMeshTasksIndirect;
		using RawCtx::DrawMeshTasksIndirectCount;

		void  DrawMeshTasksIndirectCount (BufferID	indirectBuffer,
										  Bytes		indirectBufferOffset,
										  BufferID	countBuffer,
										  Bytes		countBufferOffset,
										  uint		maxDrawCount,
										  Bytes		stride)														__Th___;
		
		// tile shader
		void  DispatchTile ()																					__Th_OV	{ RawCtx::_DispatchTile(); }

		void  DebugMarker (DebugLabel dbg)																		__Th_OV	{ RawCtx::_DebugMarker( dbg ); }
		void  PushDebugGroup (DebugLabel dbg)																	__Th_OV	{ RawCtx::_PushDebugGroup( dbg ); }
		void  PopDebugGroup ()																					__Th_OV	{ RawCtx::_PopDebugGroup(); }
		
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)			__Th_OV	{ RawCtx::AttachmentBarrier( name, srcState, dstState ); }
		
		void  CommitBarriers ()																					__Th_OV	{ return RawCtx::_CommitBarriers(); }

		// vertex stream
		ND_ bool  AllocVStream (Bytes size, OUT VertexStream &result)											__Th_OV;
		
		ND_ VPrimaryCmdBufState const&	GetPrimaryCtxState ()													C_NE___	{ return this->_mngr.GetPrimaryCtxState(); }
		
	private:
		template <typename ...IDs>
		ND_ decltype(auto)	_GetResourcesOrThrow (IDs ...ids)													__Th___	{ return this->_mngr.Get( ids... ); }
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
	_VDrawContextImpl<C>::_VDrawContextImpl (const VPrimaryCmdBufState &state, CmdBuf_t cmdbuf, DebugLabel dbg) :
		RawCtx{ state, RVRef(cmdbuf), dbg }	// throw
	{}
	
	template <typename C>
	_VDrawContextImpl<C>::_VDrawContextImpl (const DrawTask &task) :
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
	void  _VDrawContextImpl<C>::BindPipeline (GraphicsPipelineID ppln)
	{
		auto&	gppln = _GetResourcesOrThrow( ppln );
		ASSERT( gppln.RenderPassSubpassIndex() == GetPrimaryCtxState().subpassIndex );

		RawCtx::_BindPipeline( gppln.BindPoint(), gppln.Handle(), gppln.Layout(), gppln.DynamicState() );
	}
	
	template <typename C>
	void  _VDrawContextImpl<C>::BindPipeline (MeshPipelineID ppln)
	{
		auto&	mppln = _GetResourcesOrThrow( ppln );
		ASSERT( mppln.RenderPassSubpassIndex() == GetPrimaryCtxState().subpassIndex );
		
		RawCtx::_BindPipeline( mppln.BindPoint(), mppln.Handle(), mppln.Layout(), mppln.DynamicState() );
	}
	
	template <typename C>
	void  _VDrawContextImpl<C>::BindPipeline (TilePipelineID ppln)
	{
		auto&	tppln = _GetResourcesOrThrow( ppln );
		ASSERT( tppln.RenderPassSubpassIndex() == GetPrimaryCtxState().subpassIndex );
		
		RawCtx::_BindPipeline( tppln.BindPoint(), tppln.Handle(), tppln.Layout(), tppln.DynamicState() );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets)
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
	void  _VDrawContextImpl<C>::PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		ASSERT( IsAligned( size, sizeof(uint) ));

		RawCtx::_PushGraphicsConstant( offset, size, values, stages );
	}
		
/*
=================================================
	SetViewport
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetViewport (const Viewport_t &viewport)
	{
		return SetViewports( ArrayView<Viewport_t>{ &viewport, 1u });
	}
	
	template <typename C>
	void  _VDrawContextImpl<C>::SetViewports (ArrayView<Viewport_t> viewports)
	{
		ASSERT( not viewports.empty() );
		
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
	void  _VDrawContextImpl<C>::SetScissor (const RectI &scissor)
	{
		return SetScissors( ArrayView<RectI>{ &scissor, 1u });
	}

	template <typename C>
	void  _VDrawContextImpl<C>::SetScissors (ArrayView<RectI> scissors)
	{
		ASSERT( not scissors.empty() );

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
	void  _VDrawContextImpl<C>::SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
	{
		RawCtx::_SetDepthBias( depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor );
	}
	
/*
=================================================
	SetDepthBounds
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetDepthBounds (float minDepthBounds, float maxDepthBounds)
	{
		RawCtx::_SetDepthBounds( minDepthBounds, maxDepthBounds );
	}
	
/*
=================================================
	SetStencilCompareMask
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetStencilCompareMask (uint compareMask)
	{
		RawCtx::SetStencilCompareMask( VK_STENCIL_FACE_FRONT_AND_BACK, compareMask );
	}
	
	template <typename C>
	void  _VDrawContextImpl<C>::SetStencilCompareMask (uint frontCompareMask, uint backCompareMask)
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
	void  _VDrawContextImpl<C>::SetStencilWriteMask (uint writeMask)
	{
		RawCtx::SetStencilWriteMask( VK_STENCIL_FACE_FRONT_AND_BACK, writeMask );
	}
	
	template <typename C>
	void  _VDrawContextImpl<C>::SetStencilWriteMask (uint frontWriteMask, uint backWriteMask)
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
	void  _VDrawContextImpl<C>::SetStencilReference (uint reference)
	{
		RawCtx::SetStencilReference( VK_STENCIL_FACE_FRONT_AND_BACK, reference );
	}
	
	template <typename C>
	void  _VDrawContextImpl<C>::SetStencilReference (uint frontReference, uint backReference)
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
	void  _VDrawContextImpl<C>::BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType)
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
	void  _VDrawContextImpl<C>::BindVertexBuffer (uint index, BufferID buffer, Bytes offset)
	{
		auto&	buf = _GetResourcesOrThrow( buffer );

		BindVertexBuffer( index, buf.Handle(), offset );
	}
	
	template <typename C>
	void  _VDrawContextImpl<C>::BindVertexBuffer (uint index, VkBuffer buffer, Bytes offset)
	{
		const VkDeviceSize	off	= VkDeviceSize(offset);
		RawCtx::_BindVertexBuffers( index, ArrayView<VkBuffer>{&buffer, 1}, ArrayView<VkDeviceSize>{&off, 1} );
	}
	
	template <typename C>
	bool  _VDrawContextImpl<C>::BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset)
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
	void  _VDrawContextImpl<C>::BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets)
	{
		ASSERT( buffers.size() == offsets.size() );
		ASSERT( buffers.size() <= GraphicsConfig::MaxVertexBuffers );
		STATIC_ASSERT( sizeof(Bytes) == sizeof(VkDeviceSize) );

		StaticArray< VkBuffer, GraphicsConfig::MaxVertexBuffers >	dst_buffers;

		for (uint i = 0; i < buffers.size(); ++i)
		{
			auto&	buffer	= _GetResourcesOrThrow( buffers[i] );
			
			dst_buffers[i]	= buffer.Handle();
		}

		RawCtx::_BindVertexBuffers( firstBinding, dst_buffers, offsets.Cast<VkDeviceSize>() );
	}
	
	template <typename C>
	void  _VDrawContextImpl<C>::BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<Bytes> offsets)
	{
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
									  uint firstInstance)
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
											 uint firstInstance)
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
											  Bytes		stride)
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );

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
													 Bytes		stride)
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );

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
												   Bytes	stride)
	{
		auto  [ibuf, cbuf] = _GetResourcesOrThrow( indirectBuffer, countBuffer );

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
														  Bytes		stride)
	{
		auto  [ibuf, cbuf] = _GetResourcesOrThrow( indirectBuffer, countBuffer );
		
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
													   Bytes	stride)
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );

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
															Bytes		stride)
	{
		auto  [ibuf, cbuf] = _GetResourcesOrThrow( indirectBuffer, countBuffer );
		
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
		return this->_mngr.GetResourceManager().GetStagingManager().AllocVStream( this->_mngr.GetFrameId(), size, OUT result );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_BindPipeline
=================================================
*/
	inline void  _VDirectDrawCtx::_BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout, EPipelineDynamicState flags)
	{
		_states.bindPoint	= bindPoint;
		_states.pplnLayout	= layout;
		_states.flags		= flags;

		vkCmdBindPipeline( _cmdbuf.Get(), bindPoint, ppln );
	}
	
/*
=================================================
	BindDescriptorSet
=================================================
*/
	inline void  _VDirectDrawCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets)
	{
		ASSERT( _states.pplnLayout != Default );
		ASSERT( ds != Default );

		vkCmdBindDescriptorSets( _cmdbuf.Get(), _states.bindPoint, _states.pplnLayout, index.vkIndex, 1, &ds, uint(dynamicOffsets.size()), dynamicOffsets.data() );
	}
	
/*
=================================================
	_PushGraphicsConstant
=================================================
*/
	inline void  _VDirectDrawCtx::_PushGraphicsConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		ASSERT( IsAligned( size, sizeof(uint) ));
		ASSERT( _states.pplnLayout != Default );

		vkCmdPushConstants( _cmdbuf.Get(), _states.pplnLayout, VEnumCast(stages), uint(offset), uint(size), values );
	}
	
/*
=================================================
	SetViewport
=================================================
*/
	inline void  _VDirectDrawCtx::SetViewport (uint first, ArrayView<VkViewport> viewports)
	{
		ASSERT( not viewports.empty() );
		ASSERT( _GetFeatures().multiViewport or (first + viewports.size() == 0) );

		vkCmdSetViewport( _cmdbuf.Get(), first, uint(viewports.size()), viewports.data() );
	}

/*
=================================================
	SetScissor
=================================================
*/
	inline void  _VDirectDrawCtx::SetScissor (uint first, ArrayView<VkRect2D> scissors)
	{
		ASSERT( not scissors.empty() );
		ASSERT( _GetFeatures().multiViewport or (first + scissors.size() == 0) );

		vkCmdSetScissor( _cmdbuf.Get(), first, uint(scissors.size()), scissors.data() );
	}
	
/*
=================================================
	_SetDepthBias
=================================================
*/
	inline void  _VDirectDrawCtx::_SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
	{
		ASSERT( AllBits( _states.flags, EPipelineDynamicState::DepthBias ));
		ASSERT( _GetFeatures().depthBiasClamp or IsZero( depthBiasClamp ));

		vkCmdSetDepthBias( _cmdbuf.Get(), depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor );
	}
	
/*
=================================================
	_SetDepthBounds
=================================================
*/
	inline void  _VDirectDrawCtx::_SetDepthBounds (float minDepthBounds, float maxDepthBounds)
	{
		ASSERT( _GetFeatures().depthBounds );
		//ASSERT( AllBits( _states.flags, EPipelineDynamicState::DepthBounds ));

		vkCmdSetDepthBounds( _cmdbuf.Get(), minDepthBounds, maxDepthBounds );
	}
	
/*
=================================================
	SetStencilCompareMask
=================================================
*/
	inline void  _VDirectDrawCtx::SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask)
	{
		ASSERT( AllBits( _states.flags, EPipelineDynamicState::StencilCompareMask ));
		vkCmdSetStencilCompareMask( _cmdbuf.Get(), faceMask, compareMask );
	}
	
/*
=================================================
	SetStencilWriteMask
=================================================
*/
	inline void  _VDirectDrawCtx::SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask)
	{
		ASSERT( AllBits( _states.flags, EPipelineDynamicState::StencilWriteMask ));
		vkCmdSetStencilWriteMask( _cmdbuf.Get(), faceMask, writeMask );
	}
	
/*
=================================================
	SetStencilReference
=================================================
*/
	inline void  _VDirectDrawCtx::SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference)
	{
		ASSERT( AllBits( _states.flags, EPipelineDynamicState::StencilReference ));
		vkCmdSetStencilReference( _cmdbuf.Get(), faceMask, reference );
	}
	
/*
=================================================
	_SetBlendConstants
=================================================
*/
	inline void  _VDirectDrawCtx::_SetBlendConstants (const RGBA32f &color)
	{
		ASSERT( AllBits( _states.flags, EPipelineDynamicState::BlendConstants ));
		vkCmdSetBlendConstants( _cmdbuf.Get(), color.data() );
	}

/*
=================================================
	BindIndexBuffer
=================================================
*/
	inline void  _VDirectDrawCtx::BindIndexBuffer (VkBuffer buffer, Bytes offset, EIndex indexType)
	{
		vkCmdBindIndexBuffer( _cmdbuf.Get(), buffer, VkDeviceSize(offset), VEnumCast(indexType) );
	}
	
/*
=================================================
	_BindVertexBuffers
=================================================
*/
	inline void  _VDirectDrawCtx::_BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<VkDeviceSize> offsets)
	{
		ASSERT( buffers.size() );
		ASSERT( buffers.size() == offsets.size() );

		DEBUG_ONLY(
			const auto	align = VkDeviceSize(_GetDeviceProperties().res.minVertexBufferOffsetAlign);
			for (auto off : offsets) {
				ASSERT( off % align == 0 );
			})

		vkCmdBindVertexBuffers( _cmdbuf.Get(), firstBinding, uint(buffers.size()), buffers.data(), offsets.data() );
	}
	
/*
=================================================
	_Draw
=================================================
*/
	inline void  _VDirectDrawCtx::_Draw (uint vertexCount,
										 uint instanceCount,
										 uint firstVertex,
										 uint firstInstance)
	{
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
												uint firstInstance)
	{
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
												Bytes	 stride)
	{
		ASSERT( drawCount > 0 );
		ASSERT( stride >= sizeof(VkDrawIndirectCommand ) );
		
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
													   Bytes	stride)
	{
		ASSERT( drawCount > 0 );
		ASSERT( stride >= sizeof(VkDrawIndexedIndirectCommand ) );

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
													 Bytes		stride)
	{
		ASSERT( _GetExtensions().drawIndirectCount );
		ASSERT( maxDrawCount > 0 );
		ASSERT( stride >= sizeof(VkDrawIndirectCommand ) );

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
															Bytes		stride)
	{
		ASSERT( _GetExtensions().drawIndirectCount );
		ASSERT( maxDrawCount > 0 );
		ASSERT( stride >= sizeof(VkDrawIndexedIndirectCommand ) );

		vkCmdDrawIndexedIndirectCountKHR( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset),
										  countBuffer, VkDeviceSize(countBufferOffset),
										  maxDrawCount, CheckCast<uint>(stride) );
	}
	
/*
=================================================
	_DrawMeshTasks
=================================================
*/
	inline void  _VDirectDrawCtx::_DrawMeshTasks (const uint3 &taskCount)
	{
		ASSERT( _GetExtensions().meshShader );
		
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
														 Bytes		stride)
	{
		ASSERT( _GetExtensions().meshShader );
		ASSERT( drawCount > 0 );
		ASSERT( stride >= sizeof(VkDrawMeshTasksIndirectCommandEXT) );
		
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
															  Bytes		stride)
	{
		ASSERT( _GetExtensions().meshShader );
		ASSERT( maxDrawCount > 0 );
		ASSERT( stride >= sizeof(VkDrawMeshTasksIndirectCommandEXT) );
		
		vkCmdDrawMeshTasksIndirectCountEXT( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset),
											countBuffer, VkDeviceSize(countBufferOffset),
											maxDrawCount, CheckCast<uint>(stride) );
	}
	
/*
=================================================
	_DispatchTile
=================================================
*/
	inline void  _VDirectDrawCtx::_DispatchTile ()
	{
		ASSERT( _GetExtensions().subpassShadingHW );

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
//-----------------------------------------------------------------------------


	
/*
=================================================
	_BindPipeline
=================================================
*/
	inline void  _VIndirectDrawCtx::_BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout, EPipelineDynamicState flags)
	{
		_states.bindPoint	= bindPoint;
		_states.pplnLayout	= layout;
		_states.flags		= flags;

		_cmdbuf->BindPipeline( bindPoint, ppln, layout );
	}
	
/*
=================================================
	BindDescriptorSet
=================================================
*/
	inline void  _VIndirectDrawCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets)
	{
		ASSERT( _states.pplnLayout != Default );

		_cmdbuf->BindDescriptorSet( _states.bindPoint, _states.pplnLayout, index.vkIndex, ds, dynamicOffsets );
	}

/*
=================================================
	_PushGraphicsConstant
=================================================
*/
	inline void  _VIndirectDrawCtx::_PushGraphicsConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		ASSERT( _states.pplnLayout != Default );
		ASSERT( IsAligned( size, sizeof(uint) ));

		_cmdbuf->PushConstant( _states.pplnLayout, offset, size, values, stages );
	}

/*
=================================================
	SetViewport
=================================================
*/
	inline void  _VIndirectDrawCtx::SetViewport (uint first, ArrayView<VkViewport> viewports)
	{
		ASSERT( not viewports.empty() );
		ASSERT( _GetFeatures().multiViewport or (first + viewports.size() == 0) );

		auto&	cmd = _cmdbuf->CreateCmd< SetViewportCmd, VkViewport >( viewports.size() );	// throw
		auto*	dst	= Cast<VkViewport>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkViewport> ));
			
		cmd.first	= ushort(first);
		cmd.count	= ushort(viewports.size());
		MemCopy( OUT dst, viewports.data(), ArraySizeOf(viewports) );
	}

/*
=================================================
	SetScissor
=================================================
*/
	inline void  _VIndirectDrawCtx::SetScissor (uint first, ArrayView<VkRect2D> scissors)
	{
		ASSERT( not scissors.empty() );
		ASSERT( _GetFeatures().multiViewport or (first + scissors.size() == 0) );

		auto&	cmd	= _cmdbuf->CreateCmd< SetScissorCmd, VkRect2D >( scissors.size() );	// throw
		auto*	dst	= Cast<VkRect2D>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkRect2D> ));

		cmd.first	= ushort(first);
		cmd.count	= ushort(scissors.size());
		MemCopy( OUT dst, scissors.data(), ArraySizeOf(scissors) );
	}
	
/*
=================================================
	_SetDepthBias
=================================================
*/
	inline void  _VIndirectDrawCtx::_SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
	{
		ASSERT( AllBits( _states.flags, EPipelineDynamicState::DepthBias ));
		ASSERT( _GetFeatures().depthBiasClamp or IsZero( depthBiasClamp ));

		auto&	cmd	= _cmdbuf->CreateCmd< SetDepthBiasCmd >();	// throw
		cmd.depthBiasConstantFactor	= depthBiasConstantFactor;
		cmd.depthBiasClamp			= depthBiasClamp;
		cmd.depthBiasSlopeFactor	= depthBiasSlopeFactor;
	}
	
/*
=================================================
	_SetDepthBounds
=================================================
*/
	inline void  _VIndirectDrawCtx::_SetDepthBounds (float minDepthBounds, float maxDepthBounds)
	{
		ASSERT( _GetFeatures().depthBounds );
		//ASSERT( AllBits( _states.flags, EPipelineDynamicState::DepthBounds ));

		auto&	cmd	= _cmdbuf->CreateCmd< SetDepthBoundsCmd >();	// throw
		cmd.minDepthBounds	= minDepthBounds;
		cmd.maxDepthBounds	= maxDepthBounds;
	}
	
/*
=================================================
	SetStencilCompareMask
=================================================
*/
	inline void  _VIndirectDrawCtx::SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask)
	{
		ASSERT( AllBits( _states.flags, EPipelineDynamicState::StencilCompareMask ));
		auto&	cmd	= _cmdbuf->CreateCmd< SetStencilCompareMaskCmd >();	// throw
		cmd.faceMask	= faceMask;
		cmd.compareMask	= compareMask;
	}
	
/*
=================================================
	SetStencilWriteMask
=================================================
*/
	inline void  _VIndirectDrawCtx::SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask)
	{
		ASSERT( AllBits( _states.flags, EPipelineDynamicState::StencilWriteMask ));
		auto&	cmd	= _cmdbuf->CreateCmd< SetStencilWriteMaskCmd >();	// throw
		cmd.faceMask	= faceMask;
		cmd.writeMask	= writeMask;
	}
	
/*
=================================================
	SetStencilReference
=================================================
*/
	inline void  _VIndirectDrawCtx::SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference)
	{
		ASSERT( AllBits( _states.flags, EPipelineDynamicState::StencilReference ));
		auto&	cmd	= _cmdbuf->CreateCmd< SetStencilReferenceCmd >();	// throw
		cmd.faceMask	= faceMask;
		cmd.reference	= reference;
	}
	
/*
=================================================
	_SetBlendConstants
=================================================
*/
	inline void  _VIndirectDrawCtx::_SetBlendConstants (const RGBA32f &color)
	{
		ASSERT( AllBits( _states.flags, EPipelineDynamicState::BlendConstants ));
		auto&	cmd	= _cmdbuf->CreateCmd< SetBlendConstantsCmd >();	// throw
		cmd.color	= color;
	}
	
/*
=================================================
	BindIndexBuffer
=================================================
*/
	inline void  _VIndirectDrawCtx::BindIndexBuffer (VkBuffer buffer, Bytes offset, EIndex indexType)
	{
		auto&	cmd	= _cmdbuf->CreateCmd< BindIndexBufferCmd >();	// throw
		cmd.buffer		= buffer;
		cmd.offset		= VkDeviceSize(offset);
		cmd.indexType	= VEnumCast(indexType);
	}

/*
=================================================
	_Draw
=================================================
*/
	inline void  _VIndirectDrawCtx::_Draw (uint vertexCount,
										   uint instanceCount,
										   uint firstVertex,
										   uint firstInstance)
	{
		auto&	cmd	= _cmdbuf->CreateCmd< DrawCmd >();	// throw
		cmd.vertexCount		= vertexCount;
		cmd.instanceCount	= instanceCount;
		cmd.firstVertex		= firstVertex;
		cmd.firstInstance	= firstInstance;
	}
	
/*
=================================================
	_DrawIndexed
=================================================
*/
	inline void  _VIndirectDrawCtx::_DrawIndexed (uint indexCount,
												  uint instanceCount,
												  uint firstIndex,
												  int  vertexOffset,
												  uint firstInstance)
	{
		auto&	cmd	= _cmdbuf->CreateCmd< DrawIndexedCmd >();	// throw
		cmd.indexCount		= indexCount;
		cmd.instanceCount	= instanceCount;
		cmd.firstIndex		= firstIndex;
		cmd.vertexOffset	= vertexOffset;
		cmd.firstInstance	= firstInstance;
	}
	
/*
=================================================
	DrawIndirect
=================================================
*/
	inline void  _VIndirectDrawCtx::DrawIndirect (VkBuffer	indirectBuffer,
												  Bytes		indirectBufferOffset,
												  uint		drawCount,
												  Bytes		stride)
	{
		ASSERT( stride >= sizeof(VkDrawIndirectCommand ) );

		auto&	cmd	= _cmdbuf->CreateCmd< DrawIndirectCmd >();	// throw
		cmd.indirectBuffer		= indirectBuffer;
		cmd.indirectBufferOffset= VkDeviceSize(indirectBufferOffset);
		cmd.drawCount			= drawCount;
		cmd.stride				= uint(stride);
	}
	
/*
=================================================
	DrawIndexedIndirect
=================================================
*/
	inline void  _VIndirectDrawCtx::DrawIndexedIndirect (VkBuffer	indirectBuffer,
														 Bytes		indirectBufferOffset,
														 uint		drawCount,
														 Bytes		stride)
	{
		ASSERT( stride >= sizeof(VkDrawIndexedIndirectCommand ) );

		auto&	cmd	= _cmdbuf->CreateCmd< DrawIndexedIndirectCmd >();	// throw
		cmd.indirectBuffer		= indirectBuffer;
		cmd.indirectBufferOffset= VkDeviceSize(indirectBufferOffset);
		cmd.drawCount			= drawCount;
		cmd.stride				= uint(stride);
	}
	
/*
=================================================
	DrawIndexed
=================================================
*/
	inline void  _VIndirectDrawCtx::DrawIndirectCount (VkBuffer	indirectBuffer,
													   Bytes	indirectBufferOffset,
													   VkBuffer	countBuffer,
													   Bytes	countBufferOffset,
													   uint		maxDrawCount,
													   Bytes	stride)
	{
		ASSERT( _GetExtensions().drawIndirectCount );
		ASSERT( stride >= sizeof(VkDrawIndirectCommand ) );

		auto&	cmd	= _cmdbuf->CreateCmd< DrawIndirectCountCmd >();	// throw
		cmd.indirectBuffer		= indirectBuffer;
		cmd.indirectBufferOffset= VkDeviceSize(indirectBufferOffset);
		cmd.countBuffer			= countBuffer;
		cmd.countBufferOffset	= VkDeviceSize(countBufferOffset);
		cmd.maxDrawCount		= maxDrawCount;
		cmd.stride				= uint(stride);
	}
	
/*
=================================================
	DrawIndexed
=================================================
*/
	inline void  _VIndirectDrawCtx::DrawIndexedIndirectCount (VkBuffer	indirectBuffer,
															  Bytes		indirectBufferOffset,
															  VkBuffer	countBuffer,
															  Bytes		countBufferOffset,
															  uint		maxDrawCount,
															  Bytes		stride)
	{
		ASSERT( _GetExtensions().drawIndirectCount );
		ASSERT( stride >= sizeof(VkDrawIndexedIndirectCommand ) );

		auto&	cmd	= _cmdbuf->CreateCmd< DrawIndexedIndirectCountCmd >();	// throw
		cmd.indirectBuffer		= indirectBuffer;
		cmd.indirectBufferOffset= VkDeviceSize(indirectBufferOffset);
		cmd.countBuffer			= countBuffer;
		cmd.countBufferOffset	= VkDeviceSize(countBufferOffset);
		cmd.maxDrawCount		= maxDrawCount;
		cmd.stride				= uint(stride);
	}

/*
=================================================
	_DrawMeshTasks
=================================================
*/
	inline void  _VIndirectDrawCtx::_DrawMeshTasks (const uint3 &taskCount)
	{
		ASSERT( _GetExtensions().meshShader );

		auto&	cmd	= _cmdbuf->CreateCmd< DrawMeshTasksCmd >();	// throw
		cmd.taskCount	= taskCount;
	}
	
/*
=================================================
	DrawMeshTasksIndirect
=================================================
*/
	inline void  _VIndirectDrawCtx::DrawMeshTasksIndirect (VkBuffer	indirectBuffer,
														   Bytes	indirectBufferOffset,
														   uint		drawCount,
														   Bytes	stride)
	{
		ASSERT( _GetExtensions().meshShader );
		ASSERT( drawCount > 0 );
		ASSERT( stride >= sizeof(VkDrawMeshTasksIndirectCommandEXT) );

		auto&	cmd	= _cmdbuf->CreateCmd< DrawMeshTasksIndirectCmd >();	// throw
		cmd.indirectBuffer		= indirectBuffer;
		cmd.indirectBufferOffset= VkDeviceSize(indirectBufferOffset);
		cmd.drawCount			= drawCount;
		cmd.stride				= uint(stride);
	}
	
/*
=================================================
	DrawMeshTasksIndirectCountNV
=================================================
*/
	inline void  _VIndirectDrawCtx::DrawMeshTasksIndirectCount (VkBuffer	indirectBuffer,
																Bytes		indirectBufferOffset,
																VkBuffer	countBuffer,
																Bytes		countBufferOffset,
																uint		maxDrawCount,
																Bytes		stride)
	{
		ASSERT( _GetExtensions().meshShader );
		ASSERT( maxDrawCount > 0 );
		ASSERT( stride >= sizeof(VkDrawMeshTasksIndirectCommandEXT) );

		auto&	cmd	= _cmdbuf->CreateCmd< DrawMeshTasksIndirectCountCmd >();	// throw
		cmd.indirectBuffer		= indirectBuffer;
		cmd.indirectBufferOffset= VkDeviceSize(indirectBufferOffset);
		cmd.countBuffer			= countBuffer;
		cmd.countBufferOffset	= VkDeviceSize(countBufferOffset);
		cmd.maxDrawCount		= maxDrawCount;
		cmd.stride				= uint(stride);
	}
	
/*
=================================================
	_DispatchTile
=================================================
*/
	inline void  _VIndirectDrawCtx::_DispatchTile ()
	{
		ASSERT( _GetExtensions().subpassShadingHW );

		Unused( _cmdbuf->CreateCmd< DispatchTileCmd >());	// throw
	}
	
/*
=================================================
	_CommitBarriers
=================================================
*/
	inline void  _VIndirectDrawCtx::_CommitBarriers ()
	{
	    auto* bar = _mngr.GetBarriers();
		if_unlikely( bar != null )
		{
			_cmdbuf->CommitBarriers( *bar );
			_mngr.ClearBarriers();
		}
	}

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
