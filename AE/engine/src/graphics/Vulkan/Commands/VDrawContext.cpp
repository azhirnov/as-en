// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VDrawContext.h"

namespace AE::Graphics::_hidden_
{

/*
=================================================
	constructor
=================================================
*/
	_VDirectDrawCtx::_VDirectDrawCtx (const DrawTask &task) __Th___ :
		_VBaseDirectContext{	// throw
			_ReuseOrCreateCommandBuffer( *task.GetDrawBatchPtr(), Default, DebugLabel{ task.DbgFullName(), task.DbgColor() })
		},
		_mngr{ task.GetDrawBatchPtr() }	// throw
	{
		CHECK_THROW( task.IsValid() );

		DBG_GRAPHICS_ONLY( GraphicsScheduler().DbgCheckFrameId( _mngr.GetFrameId(), task.DbgFullName() );)
	}

	_VDirectDrawCtx::_VDirectDrawCtx (const VPrimaryCmdBufState &state, VCommandBuffer cmdbuf) __Th___ :
		_VBaseDirectContext{ RVRef(cmdbuf) },	// throw
		_mngr{ state }							// throw
	{
		DBG_GRAPHICS_ONLY( GraphicsScheduler().DbgCheckFrameId( _mngr.GetFrameId(), "DrawTask" );)
	}

/*
=================================================
	destructor
=================================================
*/
	_VDirectDrawCtx::~_VDirectDrawCtx () __NE___
	{
		ASSERT( _NoPendingBarriers() );
	}

/*
=================================================
	EndCommandBuffer
=================================================
*/
	VkCommandBuffer  _VDirectDrawCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GCTX_CHECK( _mngr.IsSecondary() );
		return _VBaseDirectContext::_EndCommandBuffer();	// throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VCommandBuffer  _VDirectDrawCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GCTX_CHECK( not _mngr.IsSecondary() );
		return _VBaseDirectContext::_ReleaseCommandBuffer();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	_VIndirectDrawCtx::_VIndirectDrawCtx (const VPrimaryCmdBufState &state, VSoftwareCmdBufPtr cmdbuf) __Th___ :
		_VBaseIndirectContext{ RVRef(cmdbuf) },	// throw
		_mngr{ state }							// throw
	{
		DBG_GRAPHICS_ONLY( GraphicsScheduler().DbgCheckFrameId( _mngr.GetFrameId(), "DrawTask" );)
	}

	_VIndirectDrawCtx::_VIndirectDrawCtx (const DrawTask &task) __Th___ :
		_VBaseIndirectContext{ DebugLabel{ task.DbgFullName(), task.DbgColor() }},	// throw
		_mngr{ task.GetDrawBatchPtr() }												// throw
	{
		CHECK_THROW( task.IsValid() );

		DBG_GRAPHICS_ONLY( GraphicsScheduler().DbgCheckFrameId( _mngr.GetFrameId(), task.DbgFullName() );)
	}

/*
=================================================
	EndCommandBuffer
=================================================
*/
	VBakedCommands  _VIndirectDrawCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GCTX_CHECK( _mngr.IsSecondary() );
		return _VBaseIndirectContext::_EndCommandBuffer();	// throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VSoftwareCmdBufPtr  _VIndirectDrawCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GCTX_CHECK( not _mngr.IsSecondary() );
		return _VBaseIndirectContext::_ReleaseCommandBuffer();
	}

/*
=================================================
	_BindVertexBuffers
=================================================
*/
	void  _VIndirectDrawCtx::_BindVertexBuffers (uint						firstBinding,
												 ArrayView<VkBuffer>		buffers,
												 ArrayView<VkDeviceSize>	offsets) __Th___
	{
		auto&	cmd			= _cmdbuf->CreateCmd< BindVertexBuffersCmd, VkBuffer, VkDeviceSize >( buffers.size() );	// throw
		auto*	dst_buffers	= Cast<VkBuffer>( AlignUp( static_cast<void*>(&cmd + 1), AlignOf<VkBuffer> ));
		auto*	dst_offsets	= Cast<VkDeviceSize>( AlignUp( static_cast<void*>(dst_buffers + buffers.size()), AlignOf<VkDeviceSize> ));

		cmd.firstBinding	= ushort(firstBinding);
		cmd.count			= ushort(buffers.size());
		MemCopy( OUT dst_buffers, buffers.data(), ArraySizeOf(buffers) );
		MemCopy( OUT dst_offsets, offsets.data(), ArraySizeOf(offsets) );
	}

/*
=================================================
	_ClearAttachment
=================================================
*/
	void  _VIndirectDrawCtx::_ClearAttachment (const VkClearAttachment &clear, const VkClearRect &rect) __Th___
	{
		auto&	cmd	= _cmdbuf->CreateCmd< ClearAttachmentsCmd >();	// throw
		cmd.clear	= clear;
		cmd.rect	= rect;
	}

/*
=================================================
	_BindPipeline
=================================================
*/
	void  _VIndirectDrawCtx::_BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout, EPipelineDynamicState dynStates) __Th___
	{
		_states.bindPoint	= bindPoint;
		_states.pplnLayout	= layout;
		_states.dynStates	= dynStates;

		_cmdbuf->BindPipeline( bindPoint, ppln, layout );	// throw
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	void  _VIndirectDrawCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		VALIDATE_GCTX( BindDescriptorSet( _states.pplnLayout, index, ds ));

		_cmdbuf->BindDescriptorSet( _states.bindPoint, _states.pplnLayout, index.vkIndex, ds, dynamicOffsets );	// throw
	}

/*
=================================================
	_PushGraphicsConstant
=================================================
*/
	void  _VIndirectDrawCtx::_PushGraphicsConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages) __Th___
	{
		VALIDATE_GCTX( PushConstant( _states.pplnLayout, offset, size, values, stages ));

		_cmdbuf->PushConstant( _states.pplnLayout, offset, size, values, stages );	// throw
	}

/*
=================================================
	SetViewport
=================================================
*/
	void  _VIndirectDrawCtx::SetViewport (uint first, ArrayView<VkViewport> viewports) __Th___
	{
		VALIDATE_GCTX( SetViewport( first, viewports ));

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
	void  _VIndirectDrawCtx::SetScissor (uint first, ArrayView<VkRect2D> scissors) __Th___
	{
		VALIDATE_GCTX( SetScissor( first, scissors ));

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
	void  _VIndirectDrawCtx::_SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) __Th___
	{
		VALIDATE_GCTX( SetDepthBias( _GetDynamicStates(), depthBiasClamp ));

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
	void  _VIndirectDrawCtx::_SetDepthBounds (float minDepthBounds, float maxDepthBounds) __Th___
	{
		VALIDATE_GCTX( SetDepthBounds( _GetDynamicStates() ));

		auto&	cmd	= _cmdbuf->CreateCmd< SetDepthBoundsCmd >();	// throw
		cmd.minDepthBounds	= minDepthBounds;
		cmd.maxDepthBounds	= maxDepthBounds;
	}

/*
=================================================
	_SetFragmentShadingRate
=================================================
*/
	void  _VIndirectDrawCtx::_SetFragmentShadingRate (const VkExtent2D &fragSize, VkFragmentShadingRateCombinerOpKHR primitiveOp, VkFragmentShadingRateCombinerOpKHR textureOp) __Th___
	{
		// validated in 'SetFragmentShadingRate()'
		ASSERT( fragSize.width <= 4 and fragSize.height <= 4 );

		auto&	cmd = _cmdbuf->CreateCmd< SetFragmentShadingRateCmd >();	// throw
		cmd.fragSize.x	= ubyte(fragSize.width);
		cmd.fragSize.y	= ubyte(fragSize.height);
		cmd.primitiveOp	= primitiveOp;
		cmd.textureOp	= textureOp;
	}

/*
=================================================
	SetStencilCompareMask
=================================================
*/
	void  _VIndirectDrawCtx::SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask) __Th___
	{
		VALIDATE_GCTX( SetStencilCompareMask( _GetDynamicStates() ));

		auto&	cmd	= _cmdbuf->CreateCmd< SetStencilCompareMaskCmd >();	// throw
		cmd.faceMask	= faceMask;
		cmd.compareMask	= compareMask;
	}

/*
=================================================
	SetStencilWriteMask
=================================================
*/
	void  _VIndirectDrawCtx::SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask) __Th___
	{
		VALIDATE_GCTX( SetStencilWriteMask( _GetDynamicStates() ));

		auto&	cmd	= _cmdbuf->CreateCmd< SetStencilWriteMaskCmd >();	// throw
		cmd.faceMask	= faceMask;
		cmd.writeMask	= writeMask;
	}

/*
=================================================
	SetStencilReference
=================================================
*/
	void  _VIndirectDrawCtx::SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference) __Th___
	{
		VALIDATE_GCTX( SetStencilReference( _GetDynamicStates() ));

		auto&	cmd	= _cmdbuf->CreateCmd< SetStencilReferenceCmd >();	// throw
		cmd.faceMask	= faceMask;
		cmd.reference	= reference;
	}

/*
=================================================
	_SetBlendConstants
=================================================
*/
	void  _VIndirectDrawCtx::_SetBlendConstants (const RGBA32f &color) __Th___
	{
		VALIDATE_GCTX( SetBlendConstants( _GetDynamicStates() ));

		auto&	cmd	= _cmdbuf->CreateCmd< SetBlendConstantsCmd >();	// throw
		cmd.color	= color;
	}

/*
=================================================
	BindIndexBuffer
=================================================
*/
	void  _VIndirectDrawCtx::BindIndexBuffer (VkBuffer buffer, Bytes offset, EIndex indexType) __Th___
	{
		VALIDATE_GCTX( BindIndexBuffer( buffer, indexType ));

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
	void  _VIndirectDrawCtx::_Draw (uint vertexCount,
									uint instanceCount,
									uint firstVertex,
									uint firstInstance) __Th___
	{
		VALIDATE_GCTX( Draw( _states.pplnLayout ));

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
	void  _VIndirectDrawCtx::_DrawIndexed (uint indexCount,
										   uint instanceCount,
										   uint firstIndex,
										   int  vertexOffset,
										   uint firstInstance) __Th___
	{
		VALIDATE_GCTX( DrawIndexed( _states.pplnLayout ));

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
	void  _VIndirectDrawCtx::DrawIndirect (VkBuffer	indirectBuffer,
										   Bytes	indirectBufferOffset,
										   uint		drawCount,
										   Bytes	stride) __Th___
	{
		VALIDATE_GCTX( DrawIndirect( _states.pplnLayout, indirectBuffer, drawCount, stride ));

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
	void  _VIndirectDrawCtx::DrawIndexedIndirect (VkBuffer	indirectBuffer,
												  Bytes		indirectBufferOffset,
												  uint		drawCount,
												  Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawIndexedIndirect( _states.pplnLayout, indirectBuffer, drawCount, stride ));

		auto&	cmd	= _cmdbuf->CreateCmd< DrawIndexedIndirectCmd >();	// throw
		cmd.indirectBuffer		= indirectBuffer;
		cmd.indirectBufferOffset= VkDeviceSize(indirectBufferOffset);
		cmd.drawCount			= drawCount;
		cmd.stride				= uint(stride);
	}

/*
=================================================
	DrawIndirectCount
=================================================
*/
	void  _VIndirectDrawCtx::DrawIndirectCount (VkBuffer	indirectBuffer,
												Bytes		indirectBufferOffset,
												VkBuffer	countBuffer,
												Bytes		countBufferOffset,
												uint		maxDrawCount,
												Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawIndirectCount( _states.pplnLayout, indirectBuffer, countBuffer, maxDrawCount, stride ));

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
	DrawIndexedIndirectCount
=================================================
*/
	void  _VIndirectDrawCtx::DrawIndexedIndirectCount (VkBuffer	indirectBuffer,
													   Bytes	indirectBufferOffset,
													   VkBuffer	countBuffer,
													   Bytes	countBufferOffset,
													   uint		maxDrawCount,
													   Bytes	stride) __Th___
	{
		VALIDATE_GCTX( DrawIndexedIndirectCount( _states.pplnLayout, indirectBuffer, countBuffer, maxDrawCount, stride ));

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
	void  _VIndirectDrawCtx::_DrawMeshTasks (const uint3 &taskCount) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasks( _states.pplnLayout, taskCount ));

		auto&	cmd	= _cmdbuf->CreateCmd< DrawMeshTasksCmd >();	// throw
		cmd.taskCount	= taskCount;
	}

/*
=================================================
	DrawMeshTasksIndirect
=================================================
*/
	void  _VIndirectDrawCtx::DrawMeshTasksIndirect (VkBuffer	indirectBuffer,
													Bytes		indirectBufferOffset,
													uint		drawCount,
													Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasksIndirect( _states.pplnLayout, indirectBuffer, drawCount, stride ));

		auto&	cmd	= _cmdbuf->CreateCmd< DrawMeshTasksIndirectCmd >();	// throw
		cmd.indirectBuffer		= indirectBuffer;
		cmd.indirectBufferOffset= VkDeviceSize(indirectBufferOffset);
		cmd.drawCount			= drawCount;
		cmd.stride				= uint(stride);
	}

/*
=================================================
	DrawMeshTasksIndirectCount
=================================================
*/
	void  _VIndirectDrawCtx::DrawMeshTasksIndirectCount (VkBuffer	indirectBuffer,
														 Bytes		indirectBufferOffset,
														 VkBuffer	countBuffer,
														 Bytes		countBufferOffset,
														 uint		maxDrawCount,
														 Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasksIndirectCount( _states.pplnLayout, indirectBuffer, countBuffer, maxDrawCount, stride ));

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
	void  _VIndirectDrawCtx::_DispatchTile () __Th___
	{
		VALIDATE_GCTX( DispatchTile( _states.pplnLayout ));

		Unused( _cmdbuf->CreateCmd< DispatchTileCmd >());	// throw
	}

/*
=================================================
	_CommitBarriers
=================================================
*/
	void  _VIndirectDrawCtx::_CommitBarriers () __Th___
	{
	    auto* bar = _mngr.GetBarriers();
		if_unlikely( bar != null )
		{
			_cmdbuf->PipelineBarrier( *bar );	// throw
			_mngr.ClearBarriers();
		}
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
