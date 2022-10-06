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
		~_VDirectDrawCtx () override;

		void  BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default);

		void  BindIndexBuffer (VkBuffer buffer, Bytes offset, EIndex indexType);

		void  DrawIndirect (VkBuffer	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride);

		void  DrawIndexedIndirect (VkBuffer		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride);
		
		void  DrawIndirectCount (VkBuffer	indirectBuffer,
								 Bytes		indirectBufferOffset,
								 VkBuffer	countBuffer,
								 Bytes		countBufferOffset,
								 uint		maxDrawCount,
								 Bytes		stride);
		
		void  DrawIndexedIndirectCount (VkBuffer	indirectBuffer,
										Bytes		indirectBufferOffset,
										VkBuffer	countBuffer,
										Bytes		countBufferOffset,
										uint		maxDrawCount,
										Bytes		stride);
		
		void  DrawMeshTasksIndirectNV (VkBuffer		indirectBuffer,
									   Bytes		indirectBufferOffset,
									   uint			drawCount,
									   Bytes		stride);
		
		void  DrawMeshTasksIndirectCountNV (VkBuffer	indirectBuffer,
											Bytes		indirectBufferOffset,
											VkBuffer	countBuffer,
											Bytes		countBufferOffset,
											uint		maxDrawCount,
											Bytes		stride);
		
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)		{ _mngr.AttachmentBarrier( name, srcState, dstState ); }
		
		ND_ VkCommandBuffer	EndCommandBuffer ()			{ ASSERT( _NoPendingBarriers() );  ASSERT( _mngr.IsSecondary() );   return _VBaseDirectContext::EndCommandBuffer(); }
		ND_ VCommandBuffer  ReleaseCommandBuffer ()		{ ASSERT( _NoPendingBarriers() );  return _VBaseDirectContext::ReleaseCommandBuffer(); }

		void  SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask);
		void  SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference);
		void  SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask);

		void  SetViewport (uint first, ArrayView<VkViewport> viewports);
		void  SetScissor (uint first, ArrayView<VkRect2D> scissors);

	protected:
		explicit _VDirectDrawCtx (Ptr<VDrawCommandBatch> batch);
		_VDirectDrawCtx (Ptr<VDrawCommandBatch> batch, CmdBuf_t cmdbuf);
		_VDirectDrawCtx (const VPrimaryCmdBufState &state, CmdBuf_t cmdbuf, NtStringView dbgName);

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

		void  _DrawMeshTasksNV (uint taskCount, uint firstTask);
		
		void  _DispatchTile ();

		void  _CommitBarriers ();

		ND_ bool	_NoPendingBarriers ()	const	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetExtensions ()		const	{ return _mngr.GetDevice().GetExtensions(); }

	private:
		template <typename C> friend class _VGraphicsContextImpl;
		ND_ VkCommandBuffer  _RawCmdBuf ()	const	{ return _cmdbuf.Get(); }
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
		void  BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default);

		void  BindIndexBuffer (VkBuffer buffer, Bytes offset, EIndex indexType);

		void  DrawIndirect (VkBuffer	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride);

		void  DrawIndexedIndirect (VkBuffer		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride);
		
		void  DrawIndirectCount (VkBuffer	indirectBuffer,
								 Bytes		indirectBufferOffset,
								 VkBuffer	countBuffer,
								 Bytes		countBufferOffset,
								 uint		maxDrawCount,
								 Bytes		stride);
		
		void  DrawIndexedIndirectCount (VkBuffer	indirectBuffer,
										Bytes		indirectBufferOffset,
										VkBuffer	countBuffer,
										Bytes		countBufferOffset,
										uint		maxDrawCount,
										Bytes		stride);
		
		void  DrawMeshTasksIndirectNV (VkBuffer		indirectBuffer,
									   Bytes		indirectBufferOffset,
									   uint			drawCount,
									   Bytes		stride);
		
		void  DrawMeshTasksIndirectCountNV (VkBuffer	indirectBuffer,
											Bytes		indirectBufferOffset,
											VkBuffer	countBuffer,
											Bytes		countBufferOffset,
											uint		maxDrawCount,
											Bytes		stride);
		
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)		{ _mngr.AttachmentBarrier( name, srcState, dstState ); }
		
		ND_ VBakedCommands		EndCommandBuffer ()			{ ASSERT( _NoPendingBarriers() );  ASSERT( _mngr.IsSecondary() );   return _VBaseIndirectContext::EndCommandBuffer(); }
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()		{ ASSERT( _NoPendingBarriers() );  return _VBaseIndirectContext::ReleaseCommandBuffer(); }
		
		void  SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask);
		void  SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference);
		void  SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask);

		void  SetViewport (uint first, ArrayView<VkViewport> viewports);
		void  SetScissor (uint first, ArrayView<VkRect2D> scissors);

	protected:
		explicit _VIndirectDrawCtx (Ptr<VDrawCommandBatch> batch);
		_VIndirectDrawCtx (Ptr<VDrawCommandBatch> batch, CmdBuf_t cmdbuf);
		_VIndirectDrawCtx (const VPrimaryCmdBufState &state, CmdBuf_t cmdbuf, NtStringView dbgName);
		
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

		void  _DrawMeshTasksNV (uint taskCount, uint firstTask);
		
		void  _DispatchTile ();

		void  _CommitBarriers ();

		ND_ bool	_NoPendingBarriers ()	const	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetExtensions ()		const	{ return _mngr.GetDevice().GetExtensions(); }
		
	private:
		template <typename C> friend class _VGraphicsContextImpl;
		ND_ VSoftwareCmdBuf&  _RawCmdBuf ()		{ return *_cmdbuf; }
	};



	//
	// Vulkan Draw Context implementation
	//

	template <typename CtxImpl>
	class _VDrawContextImpl : public CtxImpl, public IDrawContext
	{
		friend class _VIndirectGraphicsCtx;

	// types
	public:
		static constexpr bool	IsDrawContext		= true;
		static constexpr bool	IsVulkanDrawContext	= true;
		
	private:
		using RawCtx	= CtxImpl;
		using AccumBar	= VAccumDrawBarriers< _VDrawContextImpl< CtxImpl >>;
		using CmdBuf_t	= typename CtxImpl::CmdBuf_t;

		
	// methods
	private:
		explicit _VDrawContextImpl (CmdBuf_t cmdbuf);

	public:
		explicit _VDrawContextImpl (const VPrimaryCmdBufState &state, CmdBuf_t cmdbuf, NtStringView dbgName = Default);
		explicit _VDrawContextImpl (Ptr<VDrawCommandBatch> batch);
		
		_VDrawContextImpl () = delete;
		_VDrawContextImpl (const _VDrawContextImpl &) = delete;
		~_VDrawContextImpl () override {}

		// pipeline and shader resources
		void  BindPipeline (GraphicsPipelineID ppln) override final;
		void  BindPipeline (MeshPipelineID ppln) override final;
		void  BindPipeline (TilePipelineID ppln) override final;

		using RawCtx::BindDescriptorSet;

		void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) override final;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages) override final;
		
		// dynamic states
		void  SetViewport (const Viewport_t &viewport) override final;
		void  SetViewports (ArrayView<Viewport_t> viewports) override final;
		void  SetScissor (const RectI &scissors) override final;
		void  SetScissors (ArrayView<RectI> scissors) override final;
		void  SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) override final;
		void  SetDepthBounds (float minDepthBounds, float maxDepthBounds) override final;
		void  SetStencilCompareMask (uint compareMask);
		void  SetStencilCompareMask (uint frontCompareMask, uint backCompareMask);
		void  SetStencilWriteMask (uint writeMask);
		void  SetStencilWriteMask (uint frontWriteMask, uint backWriteMask);
		void  SetStencilReference (uint reference) override final;
		void  SetStencilReference (uint frontReference, uint backReference) override final;
		void  SetBlendConstants (const RGBA32f &color) override final	{ RawCtx::_SetBlendConstants( color ); }
		
		using RawCtx::SetViewport;
		using RawCtx::SetScissor;
		using RawCtx::SetStencilWriteMask;
		using RawCtx::SetStencilReference;
		using RawCtx::SetStencilCompareMask;

		// draw commands
		using RawCtx::BindIndexBuffer;

		void  BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType) override final;
		void  BindVertexBuffer (uint index, BufferID buffer, Bytes offset) override final;
		void  BindVertexBuffer (uint index, VkBuffer buffer, Bytes offset);
		void  BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<Bytes> offsets);
		void  BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets) override final;
		bool  BindVertexBuffer (GraphicsPipelineID pplnId, const VertexBufferName &name, BufferID buffer, Bytes offset) override final;

		using RawCtx::DrawIndirect;
		using RawCtx::DrawIndexedIndirect;

		using IDrawContext::Draw;
		using IDrawContext::DrawIndexed;
		using IDrawContext::DrawIndirect;
		using IDrawContext::DrawIndexedIndirect;

		void  Draw (uint vertexCount,
					uint instanceCount	= 1,
					uint firstVertex	= 0,
					uint firstInstance	= 0) override final;

		void  DrawIndexed (uint indexCount,
						   uint instanceCount	= 1,
						   uint firstIndex		= 0,
						   int  vertexOffset	= 0,
						   uint firstInstance	= 0) override final;

		void  DrawIndirect (BufferID	indirectBuffer,
							Bytes		indirectBufferOffset,
							uint		drawCount,
							Bytes		stride) override final;

		void  DrawIndexedIndirect (BufferID		indirectBuffer,
								   Bytes		indirectBufferOffset,
								   uint			drawCount,
								   Bytes		stride) override final;

		// extension
		using RawCtx::DrawIndirectCount;
		using RawCtx::DrawIndexedIndirectCount;

		void  DrawIndirectCount (BufferID	indirectBuffer,
								 Bytes		indirectBufferOffset,
								 BufferID	countBuffer,
								 Bytes		countBufferOffset,
								 uint		maxDrawCount,
								 Bytes		stride);

		void  DrawIndexedIndirectCount (BufferID	indirectBuffer,
										Bytes		indirectBufferOffset,
										BufferID	countBuffer,
										Bytes		countBufferOffset,
										uint		maxDrawCount,
										Bytes		stride);

		// mesh draw commands (extension)
		using RawCtx::DrawMeshTasksIndirectNV;
		using RawCtx::DrawMeshTasksIndirectCountNV;

		void  DrawMeshTasksNV (uint taskCount);

		void  DrawMeshTasksIndirectNV (BufferID		indirectBuffer,
									   Bytes		indirectBufferOffset,
									   uint			drawCount,
									   Bytes		stride);

		void  DrawMeshTasksIndirectCountNV (BufferID	indirectBuffer,
											Bytes		indirectBufferOffset,
											BufferID	countBuffer,
											Bytes		countBufferOffset,
											uint		maxDrawCount,
											Bytes		stride);
		
		// tile shader
		void  DispatchTile () override final;

		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }

		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }
		
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState)	override final	{ RawCtx::AttachmentBarrier( name, srcState, dstState ); }
		
		ND_ AccumBar  AccumBarriers ()		{ return AccumBar{ *this }; }
		
		// vertex stream
		ND_ bool  AllocVStream (Bytes size, OUT VertexStream &result) override final;
		
		ND_ VPrimaryCmdBufState const&	GetPrimaryCtxState ()	const	{ return this->_mngr.GetPrimaryCtxState(); }


	private:
		using RawCtx::_GetExtensions;
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectDrawContext	= _hidden_::_VDrawContextImpl< _hidden_::_VDirectDrawCtx >;
	using VIndirectDrawContext	= _hidden_::_VDrawContextImpl< _hidden_::_VIndirectDrawCtx >;

} // AE::Graphics

#endif	// AE_ENABLE_VULKAN
