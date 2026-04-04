// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VDrawContext.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics::_hidden_
{

/*
=================================================
	constructor
=================================================
*/
	_VDirectDrawCtx::_VDirectDrawCtx (DrawCoroRef task) __Th___ :
		_VBaseDirectContext{	// throw
			_ReuseOrCreateCommandBuffer( *task.DrawBatchPtr(), Default, DebugLabel{ task.DbgFullName(), task.DbgColor() })
		},
		_mngr{ task.DrawBatchPtr() }	// throw
	{
		CHECK_THROW( task.IsValid() );

		GFX_DBG_ONLY( GraphicsScheduler().DbgCheckFrameId( _mngr.GetFrameId(), task.DbgFullName() );)
	}

	_VDirectDrawCtx::_VDirectDrawCtx (const VPrimaryCmdBufState &state, VCommandBuffer cmdbuf) __Th___ :
		_VBaseDirectContext{ RVRef(cmdbuf) },	// throw
		_mngr{ state }							// throw
	{
		GFX_DBG_ONLY( GraphicsScheduler().DbgCheckFrameId( _mngr.GetFrameId(), "DrawTask" );)
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

/*
=================================================
	_BindPipeline
=================================================
*/
	void  _VDirectDrawCtx::_BindPipeline (VkPipelineBindPoint bindPoint, VkPipeline ppln, VkPipelineLayout layout, EPipelineDynamicState dynStates)
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
	void  _VDirectDrawCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		VALIDATE_GCTX( BindDescriptorSet( _states.pplnLayout, index, ds ));

		vkCmdBindDescriptorSets( _cmdbuf.Get(), _states.bindPoint, _states.pplnLayout, index.vkIndex, 1, &ds, uint(dynamicOffsets.size()), dynamicOffsets.data() );
	}

/*
=================================================
	_PushGraphicsConstant
=================================================
*/
	void  _VDirectDrawCtx::_PushGraphicsConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages) __Th___
	{
		VALIDATE_GCTX( PushConstant( _states.pplnLayout, offset, size, values, stages ));

		vkCmdPushConstants( _cmdbuf.Get(), _states.pplnLayout, VEnumCast(stages), uint(offset), uint(size), values );
	}

/*
=================================================
	SetViewport
=================================================
*/
	void  _VDirectDrawCtx::SetViewport (uint first, ArrayView<VkViewport> viewports) __Th___
	{
		VALIDATE_GCTX( SetViewport( first, viewports ));

		vkCmdSetViewport( _cmdbuf.Get(), first, uint(viewports.size()), viewports.data() );
	}

/*
=================================================
	SetScissor
=================================================
*/
	void  _VDirectDrawCtx::SetScissor (uint first, ArrayView<VkRect2D> scissors)
	{
		VALIDATE_GCTX( SetScissor( first, scissors ));

		vkCmdSetScissor( _cmdbuf.Get(), first, uint(scissors.size()), scissors.data() );
	}

/*
=================================================
	_SetDepthBias
=================================================
*/
	void  _VDirectDrawCtx::_SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) __Th___
	{
		VALIDATE_GCTX( SetDepthBias( _GetDynamicStates(), depthBiasClamp ));

		vkCmdSetDepthBias( _cmdbuf.Get(), depthBiasConstantFactor, depthBiasClamp, depthBiasSlopeFactor );
	}

/*
=================================================
	_SetDepthBounds
=================================================
*/
	void  _VDirectDrawCtx::_SetDepthBounds (float minDepthBounds, float maxDepthBounds) __Th___
	{
		VALIDATE_GCTX( SetDepthBounds( _GetDynamicStates() ));

		vkCmdSetDepthBounds( _cmdbuf.Get(), minDepthBounds, maxDepthBounds );
	}

/*
=================================================
	_SetFragmentShadingRate
=================================================
*/
	void  _VDirectDrawCtx::_SetFragmentShadingRate (const VkExtent2D &fragSize, VkFragmentShadingRateCombinerOpKHR primitiveOp, VkFragmentShadingRateCombinerOpKHR textureOp) __Th___
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
	void  _VDirectDrawCtx::SetStencilCompareMask (VkStencilFaceFlagBits faceMask, uint compareMask) __Th___
	{
		VALIDATE_GCTX( SetStencilCompareMask( _GetDynamicStates() ));

		vkCmdSetStencilCompareMask( _cmdbuf.Get(), faceMask, compareMask );
	}

/*
=================================================
	SetStencilWriteMask
=================================================
*/
	void  _VDirectDrawCtx::SetStencilWriteMask (VkStencilFaceFlagBits faceMask, uint writeMask) __Th___
	{
		VALIDATE_GCTX( SetStencilWriteMask( _GetDynamicStates() ));

		vkCmdSetStencilWriteMask( _cmdbuf.Get(), faceMask, writeMask );
	}

/*
=================================================
	SetStencilReference
=================================================
*/
	void  _VDirectDrawCtx::SetStencilReference (VkStencilFaceFlagBits faceMask, uint reference) __Th___
	{
		VALIDATE_GCTX( SetStencilReference( _GetDynamicStates() ));

		vkCmdSetStencilReference( _cmdbuf.Get(), faceMask, reference );
	}

/*
=================================================
	_SetBlendConstants
=================================================
*/
	void  _VDirectDrawCtx::_SetBlendConstants (const RGBA32f &color) __Th___
	{
		VALIDATE_GCTX( SetBlendConstants( _GetDynamicStates() ));

		vkCmdSetBlendConstants( _cmdbuf.Get(), color.data() );
	}

/*
=================================================
	BindIndexBuffer
=================================================
*/
	void  _VDirectDrawCtx::BindIndexBuffer (VkBuffer buffer, Bytes offset, EIndex indexType) __Th___
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
	void  _VDirectDrawCtx::_BindVertexBuffers (uint firstBinding, ArrayView<VkBuffer> buffers, ArrayView<VkDeviceSize> offsets)
	{
		vkCmdBindVertexBuffers( _cmdbuf.Get(), firstBinding, uint(buffers.size()), buffers.data(), offsets.data() );

		// TODO: vkCmdBindVertexBuffers2
	}

/*
=================================================
	_Draw
=================================================
*/
	void  _VDirectDrawCtx::_Draw (uint vertexCount,
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
	void  _VDirectDrawCtx::_DrawIndexed (uint indexCount,
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
	void  _VDirectDrawCtx::DrawIndirect (VkBuffer indirectBuffer,
										 Bytes	  indirectBufferOffset,
										 uint	  drawCount,
										 Bytes	  stride) __Th___
	{
		VALIDATE_GCTX( DrawIndirect( _states.pplnLayout, indirectBuffer, drawCount, stride ));

		vkCmdDrawIndirect( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset), drawCount, CheckCast{stride} );
	}

/*
=================================================
	DrawIndexedIndirect
=================================================
*/
	void  _VDirectDrawCtx::DrawIndexedIndirect (VkBuffer	indirectBuffer,
												Bytes		indirectBufferOffset,
												uint		drawCount,
												Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawIndexedIndirect( _states.pplnLayout, indirectBuffer, drawCount, stride ));

		vkCmdDrawIndexedIndirect( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset), drawCount, CheckCast{stride} );
	}

/*
=================================================
	DrawIndirectCount
=================================================
*/
	void  _VDirectDrawCtx::DrawIndirectCount (VkBuffer	indirectBuffer,
											  Bytes		indirectBufferOffset,
											  VkBuffer	countBuffer,
											  Bytes		countBufferOffset,
											  uint		maxDrawCount,
											  Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawIndirectCount( _states.pplnLayout, indirectBuffer, countBuffer, maxDrawCount, stride ));

		vkCmdDrawIndirectCountKHR( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset), countBuffer, VkDeviceSize(countBufferOffset), maxDrawCount, CheckCast{stride} );
	}

/*
=================================================
	DrawIndexedIndirectCount
=================================================
*/
	void  _VDirectDrawCtx::DrawIndexedIndirectCount (VkBuffer	indirectBuffer,
													 Bytes		indirectBufferOffset,
													 VkBuffer	countBuffer,
													 Bytes		countBufferOffset,
													 uint		maxDrawCount,
													 Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawIndexedIndirectCount( _states.pplnLayout, indirectBuffer, countBuffer, maxDrawCount, stride ));

		vkCmdDrawIndexedIndirectCountKHR( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset),
										  countBuffer, VkDeviceSize(countBufferOffset),
										  maxDrawCount, CheckCast{stride} );
	}

/*
=================================================
	_DrawMeshTasks
=================================================
*/
	void  _VDirectDrawCtx::_DrawMeshTasks (const uint3 &taskCount) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasks( _states.pplnLayout, taskCount ));

		vkCmdDrawMeshTasksEXT( _cmdbuf.Get(), taskCount.x, taskCount.y, taskCount.z );
	}

/*
=================================================
	DrawMeshTasksIndirect
=================================================
*/
	void  _VDirectDrawCtx::DrawMeshTasksIndirect (VkBuffer	indirectBuffer,
												  Bytes		indirectBufferOffset,
												  uint		drawCount,
												  Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasksIndirect( _states.pplnLayout, indirectBuffer, drawCount, stride ));

		vkCmdDrawMeshTasksIndirectEXT( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset), drawCount, CheckCast{stride} );
	}

/*
=================================================
	DrawMeshTasksIndirectCount
=================================================
*/
	void  _VDirectDrawCtx::DrawMeshTasksIndirectCount (VkBuffer		indirectBuffer,
													   Bytes		indirectBufferOffset,
													   VkBuffer		countBuffer,
													   Bytes		countBufferOffset,
													   uint			maxDrawCount,
													   Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasksIndirectCount( _states.pplnLayout, indirectBuffer, countBuffer, maxDrawCount, stride ));

		vkCmdDrawMeshTasksIndirectCountEXT( _cmdbuf.Get(), indirectBuffer, VkDeviceSize(indirectBufferOffset),
											countBuffer, VkDeviceSize(countBufferOffset),
											maxDrawCount, CheckCast{stride} );
	}

/*
=================================================
	_DispatchTile
=================================================
*/
	void  _VDirectDrawCtx::_DispatchTile () __Th___
	{
		GCTX_CHECK( _states.pplnLayout != Default );

		vkCmdSubpassShadingHUAWEI( _cmdbuf.Get() );
	}

/*
=================================================
	_CommitBarriers
=================================================
*/
	void  _VDirectDrawCtx::_CommitBarriers ()
	{
		auto	bar = _mngr.GetBarriers();
		if_unlikely( bar )
		{
			vkCmdPipelineBarrier2KHR( _cmdbuf.Get(), bar.get() );
			_mngr.ClearBarriers();
		}
	}

/*
=================================================
	_ClearAttachment
=================================================
*/
	void  _VDirectDrawCtx::_ClearAttachment (const VkClearAttachment &clear, const VkClearRect &rect)
	{
		ASSERT( _NoPendingBarriers() );
		vkCmdClearAttachments( _cmdbuf.Get(), 1, &clear, 1, &rect );
	}

/*
=================================================
	ExecuteGeneratedCommands
=================================================
*/
	void  _VDirectDrawCtx::ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &info, bool isPreprocessed) __Th___
	{
		vkCmdExecuteGeneratedCommandsEXT( _cmdbuf.Get(), isPreprocessed, &info );
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
		GFX_DBG_ONLY( GraphicsScheduler().DbgCheckFrameId( _mngr.GetFrameId(), "DrawTask" );)
	}

	_VIndirectDrawCtx::_VIndirectDrawCtx (DrawCoroRef task) __Th___ :
		_VBaseIndirectContext{ DebugLabel{ task.DbgFullName(), task.DbgColor() }},	// throw
		_mngr{ task.DrawBatchPtr() }												// throw
	{
		CHECK_THROW( task.IsValid() );

		GFX_DBG_ONLY( GraphicsScheduler().DbgCheckFrameId( _mngr.GetFrameId(), task.DbgFullName() );)
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
	ExecuteGeneratedCommands
=================================================
*/
	void  _VIndirectDrawCtx::ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &info, bool isPreprocessed) __Th___
	{
		auto&	cmd	= _cmdbuf->CreateCmd< ExecuteIndirectCommandsCmd >();	// throw
		cmd.info			= info;
		cmd.isPreprocessed	= isPreprocessed;
	}

/*
=================================================
	_CommitBarriers
=================================================
*/
	void  _VIndirectDrawCtx::_CommitBarriers () __Th___
	{
		auto	bar = _mngr.GetBarriers();
		if_unlikely( bar )
		{
			_cmdbuf->PipelineBarrier( *bar );	// throw
			_mngr.ClearBarriers();
		}
	}
//-----------------------------------------------------------------------------



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
	_VDrawContextImpl<C>::_VDrawContextImpl (DrawCoroRef task) __Th___ :
		RawCtx{ task }	// throw
	{
		if_likely( auto* batch = task.DrawBatchPtr() )
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

		RawCtx::_PushGraphicsConstant( idx.vulkanOffset, size, values, EShaderStages(0) | idx.stage );
	}

/*
=================================================
	SetViewport
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::SetViewport (const Viewport &viewport) __Th___
	{
		return SetViewports( ArrayView<Viewport>{ &viewport, 1u });
	}

	template <typename C>
	void  _VDrawContextImpl<C>::SetViewports (ArrayView<Viewport> viewports) __Th___
	{
		GCTX_CHECK( viewports.size() <= GraphicsConfig::MaxViewports );
		//VALIDATE_GCTX( SetViewports( viewports ));	// TODO

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
		//VALIDATE_GCTX( SetScissor( scissors ));	// TODO

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

		VkDeviceSize	off	= VkDeviceSize(offset);
		RawCtx::_BindVertexBuffers( index, {buffer}, {off} );
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

	template <typename C>
	bool  _VDrawContextImpl<C>::BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, VkBuffer buffer, Bytes offset) __Th___
	{
		auto&	ppln = _GetResourcesOrThrow( pplnId );

		uint	idx = ppln.GetVertexBufferIndex( name );
		CHECK_ERR( idx != UMax );

		BindVertexBuffer( idx, buffer, offset );
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

		for (usize i = 0; i < buffers.size(); ++i)
		{
			auto&	buffer	= _GetResourcesOrThrow( buffers[i] );
			dst_buffers[i]	= buffer.Handle();
		}

		RawCtx::_BindVertexBuffers( firstBinding, ArrayView<VkBuffer>{ dst_buffers.data(), buffers.size() }, offsets.Cast<VkDeviceSize>() );
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

/*
=================================================
	BindInitialPipeline
=================================================
*/
	template <typename C>
	void  _VDrawContextImpl<C>::BindInitialPipeline (IndirectExecutionSetID id) __Th___
	{
		auto&	exec_set = _GetResourcesOrThrow( id );
		auto	state	 = exec_set.GetInitialState();

		RawCtx::_BindPipeline( state.bindPoint, state.pipeline, state.layout, state.dynamicState );
	}

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------

# define VCTX_TYPE	_VDrawContextImpl
# define DISABLE_PREPROCESS
# include "graphics_rhi/Vulkan/Commands/VIndirectCommandExecutorImpl.cpp.h"
# undef DISABLE_PREPROCESS
# undef VCTX_TYPE

//-----------------------------------------------------------------------------
namespace AE::Graphics::_hidden_
{
    template class _VDrawContextImpl< _VDirectDrawCtx >;
    template class _VDrawContextImpl< _VIndirectDrawCtx >;

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
