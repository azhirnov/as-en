// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RDrawContext.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	constructor
=================================================
*/
	RDrawContext::RDrawContext (const RPrimaryCmdBufState &state, CmdBuf_t cmdbuf) __Th___ :
		_RBaseContext{ RVRef(cmdbuf) },
		_mngr{ state }
	{}

	RDrawContext::RDrawContext (const DrawTask &task) __Th___ :
		_RBaseContext{
			_ReuseOrCreateCommandBuffer( Default, DebugLabel{ task.DbgFullName(), task.DbgColor() })
		},
		_mngr{ task.GetDrawBatchPtr(), task.GetDrawOrderIndex() }
	{
		CHECK_THROW( task.IsValid() );
	}

	RDrawContext::RDrawContext (RDrawContext &&other) __Th___ :
		RDrawContext{ other.GetPrimaryCtxState(), RVRef(other._cmdbuf) }
	{}

/*
=================================================
	destructor
=================================================
*/
	RDrawContext::~RDrawContext () __NE___
	{
		ASSERT( _mngr.NoPendingBarriers() );
	}

/*
=================================================
	EndCommandBuffer
=================================================
*/
	RmDrawCommandBufferID  RDrawContext::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GCTX_CHECK( _mngr.IsSecondary() );

		auto	batch = _mngr.GetBatchPtr();
		CHECK_THROW( batch );

		return _EndCommandBuffer( _mngr.GetDrawOrderIndex(), batch->Handle() );
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	RDrawContext::CmdBuf_t  RDrawContext::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GCTX_CHECK( not _mngr.IsSecondary() );
		return _ReleaseCommandBuffer();
	}

/*
=================================================
	BindPipeline
=================================================
*/
	void  RDrawContext::BindPipeline (GraphicsPipelineID pplnId) __Th___
	{
		auto&	ppln = _GetResourcesOrThrow( pplnId );
		GCTX_CHECK( ppln.RenderPassSubpassIndex() == GetPrimaryCtxState().subpassIndex );
		_dynStates = ppln.DynamicState();

		Msg::CmdBuf_Bake::Draw_BindGraphicsPipelineCmd  cmd;
		cmd.ppln = ppln.Handle();
		_cmdbuf->AddCommand( cmd );
	}

	void  RDrawContext::BindPipeline (MeshPipelineID pplnId) __Th___
	{
		auto&	ppln = _GetResourcesOrThrow( pplnId );
		GCTX_CHECK( ppln.RenderPassSubpassIndex() == GetPrimaryCtxState().subpassIndex );
		_dynStates = ppln.DynamicState();

		Msg::CmdBuf_Bake::Draw_BindMeshPipelineCmd  cmd;
		cmd.ppln = ppln.Handle();
		_cmdbuf->AddCommand( cmd );
	}

	void  RDrawContext::BindPipeline (TilePipelineID pplnId) __Th___
	{
		auto&	ppln = _GetResourcesOrThrow( pplnId );
		GCTX_CHECK( ppln.RenderPassSubpassIndex() == GetPrimaryCtxState().subpassIndex );
		_dynStates = ppln.DynamicState();

		Msg::CmdBuf_Bake::Draw_BindTilePipelineCmd  cmd;
		cmd.ppln = ppln.Handle();
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	void  RDrawContext::BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		auto&	desc_set = _GetResourcesOrThrow( ds );
		GCTX_CHECK( index );

		Msg::CmdBuf_Bake::Draw_BindDescriptorSetCmd  cmd;
		cmd.index			= index;
		cmd.ds				= desc_set.Handle();
		cmd.dynamicOffsets	= dynamicOffsets;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	PushConstant
=================================================
*/
	void  RDrawContext::PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName) __Th___
	{
		VALIDATE_GCTX( PushConstant( idx, size, typeName ));

		Msg::CmdBuf_Bake::Draw_PushConstantCmd  cmd;
		cmd.pcIndex_offset	= idx.bufferId;
		cmd.pcIndex_stage	= idx.stage;
		cmd.data			= ArrayView<ubyte>{ Cast<ubyte>(values), usize{size} };
		cmd.typeName		= typeName;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	SetViewports
=================================================
*/
	void  RDrawContext::SetViewports (ArrayView<Viewport> viewports) __Th___
	{
		GCTX_CHECK( viewports.size() <= GraphicsConfig::MaxViewports );

		Msg::CmdBuf_Bake::Draw_SetViewportsCmd  cmd;
		cmd.viewports = viewports;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	SetScissors
=================================================
*/
	void  RDrawContext::SetScissors (ArrayView<RectI> scissors) __Th___
	{
		GCTX_CHECK( scissors.size() <= GraphicsConfig::MaxViewports );

		Msg::CmdBuf_Bake::Draw_SetScissorsCmd  cmd;
		cmd.scissors = scissors;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	SetDepthBias
=================================================
*/
	void  RDrawContext::SetDepthBias (float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor) __Th___
	{
		VALIDATE_GCTX( SetDepthBias( _GetDynamicStates(), depthBiasClamp ));

		Msg::CmdBuf_Bake::Draw_SetDepthBiasCmd  cmd;
		cmd.depthBiasConstantFactor	= depthBiasConstantFactor;
		cmd.depthBiasClamp			= depthBiasClamp;
		cmd.depthBiasSlopeFactor	= depthBiasSlopeFactor;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	SetStencilCompareMask
=================================================
*/
	void  RDrawContext::SetStencilCompareMask (uint frontCompareMask, uint backCompareMask) __Th___
	{
		VALIDATE_GCTX( SetStencilCompareMask( _GetDynamicStates() ));
		GCTX_CHECK( _HasFeature( EFeature::StencilCompareMask ));

		Msg::CmdBuf_Bake::Draw_SetStencilCompareMaskCmd  cmd;
		cmd.frontCompareMask	= frontCompareMask;
		cmd.backCompareMask		= backCompareMask;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	SetStencilWriteMask
=================================================
*/
	void  RDrawContext::SetStencilWriteMask (uint frontWriteMask, uint backWriteMask) __Th___
	{
		VALIDATE_GCTX( SetStencilWriteMask( _GetDynamicStates() ));
		GCTX_CHECK( _HasFeature( EFeature::StencilWriteMask ));

		Msg::CmdBuf_Bake::Draw_SetStencilWriteMaskCmd  cmd;
		cmd.frontWriteMask	= frontWriteMask;
		cmd.backWriteMask	= backWriteMask;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	SetStencilReference
=================================================
*/
	void  RDrawContext::SetStencilReference (uint frontReference, uint backReference) __Th___
	{
		VALIDATE_GCTX( SetStencilReference( _GetDynamicStates() ));

		Msg::CmdBuf_Bake::Draw_SetStencilReferenceCmd  cmd;
		cmd.frontReference	= frontReference;
		cmd.backReference	= backReference;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	SetBlendConstants
=================================================
*/
	void  RDrawContext::SetBlendConstants (const RGBA32f &color) __Th___
	{
		VALIDATE_GCTX( SetBlendConstants( _GetDynamicStates() ));

		Msg::CmdBuf_Bake::Draw_SetBlendConstantsCmd  cmd;
		cmd.color = color;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	SetDepthBounds
=================================================
*/
	void  RDrawContext::SetDepthBounds (float minDepthBounds, float maxDepthBounds) __Th___
	{
		VALIDATE_GCTX( SetDepthBounds( _GetDynamicStates() ));
		GCTX_CHECK( _HasFeature( EFeature::DepthBounds ));

		Msg::CmdBuf_Bake::Draw_SetDepthBoundsCmd  cmd;
		cmd.minDepthBounds = minDepthBounds;
		cmd.maxDepthBounds = maxDepthBounds;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	SetFragmentShadingRate
=================================================
*/
	void  RDrawContext::SetFragmentShadingRate (EShadingRate rate, EShadingRateCombinerOp primitiveOp, EShadingRateCombinerOp textureOp) __Th___
	{
		VALIDATE_GCTX( SetFragmentShadingRate( _GetDynamicStates(), rate, primitiveOp, textureOp ));
		GCTX_CHECK( _HasFeature( EFeature::FragmentShadingRate ));

		Msg::CmdBuf_Bake::Draw_SetFragmentShadingRateCmd  cmd;
		cmd.rate		= rate;
		cmd.primitiveOp	= primitiveOp;
		cmd.textureOp	= textureOp;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	BindIndexBuffer
=================================================
*/
	void  RDrawContext::BindIndexBuffer (BufferID buffer, Bytes offset, EIndex indexType) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( buffer );
		BindIndexBuffer( buf.Handle(), offset, indexType );
	}

	void  RDrawContext::BindIndexBuffer (RmBufferID buffer, Bytes offset, EIndex indexType) __Th___
	{
		Msg::CmdBuf_Bake::Draw_BindIndexBufferCmd  cmd;
		cmd.buffer		= buffer;
		cmd.offset		= offset;
		cmd.indexType	= indexType;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	BindVertexBuffer
=================================================
*/
	void  RDrawContext::BindVertexBuffer (uint index, BufferID buffer, Bytes offset) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( buffer );
		BindVertexBuffer( index, buf.Handle(), offset );
	}

	void  RDrawContext::BindVertexBuffer (uint index, RmBufferID buffer, Bytes offset) __Th___
	{
		BindVertexBuffers( index, ArrayView<RmBufferID>{ &buffer, 1 }, ArrayView<Bytes>{ &offset, 1 });
	}

/*
=================================================
	BindVertexBuffers
=================================================
*/
	void  RDrawContext::BindVertexBuffers (uint firstBinding, ArrayView<BufferID> buffers, ArrayView<Bytes> offsets) __Th___
	{
		VALIDATE_GCTX( BindVertexBuffers( firstBinding, buffers, offsets ));

		StaticArray< RmBufferID, GraphicsConfig::MaxVertexBuffers >	dst_buffers;

		for (usize i = 0; i < buffers.size(); ++i)
		{
			auto&	buf = _GetResourcesOrThrow( buffers[i] );
			dst_buffers[i] = buf.Handle();
		}
		BindVertexBuffers( firstBinding, dst_buffers, offsets );
	}

	void  RDrawContext::BindVertexBuffers (uint firstBinding, ArrayView<RmBufferID> buffers, ArrayView<Bytes> offsets) __Th___
	{
		Msg::CmdBuf_Bake::Draw_BindVertexBuffersCmd  cmd;
		cmd.firstBinding	= firstBinding;
		cmd.buffers			= buffers;
		cmd.offsets			= offsets;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	BindVertexBuffer
=================================================
*/
	bool  RDrawContext::BindVertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name, BufferID buffer, Bytes offset) __Th___
	{
		auto	[ppln, buf] = _GetResourcesOrThrow( pplnId, buffer );

		uint	idx = ppln.GetVertexBufferIndex( name );
		CHECK_ERR( idx != UMax );

		BindVertexBuffer( idx, buf.Handle(), offset );
		return true;
	}

/*
=================================================
	Draw
=================================================
*/
	void  RDrawContext::Draw (uint vertexCount,
							  uint instanceCount,
							  uint firstVertex,
							  uint firstInstance) __Th___
	{
		Msg::CmdBuf_Bake::DrawCmd  cmd;
		cmd.vertexCount		= vertexCount;
		cmd.instanceCount	= instanceCount;
		cmd.firstVertex		= firstVertex;
		cmd.firstInstance	= firstInstance;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	DrawIndexed
=================================================
*/
	void  RDrawContext::DrawIndexed (uint indexCount,
									 uint instanceCount,
									 uint firstIndex,
									 int  vertexOffset,
									 uint firstInstance) __Th___
	{
		Msg::CmdBuf_Bake::DrawIndexedCmd  cmd;
		cmd.indexCount		= indexCount;
		cmd.instanceCount	= instanceCount;
		cmd.firstIndex		= firstIndex;
		cmd.vertexOffset	= vertexOffset;
		cmd.firstInstance	= firstInstance;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	DrawIndirect
=================================================
*/
	void  RDrawContext::DrawIndirect (BufferID	indirectBuffer,
									  Bytes		indirectBufferOffset,
									  uint		drawCount,
									  Bytes		stride) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( DrawIndirect( buf.Description(), indirectBufferOffset, drawCount, stride ));
		DrawIndirect( buf.Handle(), indirectBufferOffset, drawCount, stride );
	}

	void  RDrawContext::DrawIndirect (RmBufferID	indirectBuffer,
									  Bytes			indirectBufferOffset,
									  uint			drawCount,
									  Bytes			stride) __Th___
	{
		VALIDATE_GCTX( DrawIndirect( drawCount, stride ));

		Msg::CmdBuf_Bake::DrawIndirectCmd  cmd;
		cmd.indirectBuffer			= indirectBuffer;
		cmd.indirectBufferOffset	= indirectBufferOffset;
		cmd.drawCount				= drawCount;
		cmd.stride					= stride;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	DrawIndexedIndirect
=================================================
*/
	void  RDrawContext::DrawIndexedIndirect (BufferID	indirectBuffer,
											 Bytes		indirectBufferOffset,
											 uint		drawCount,
											 Bytes		stride) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( DrawIndexedIndirect( buf.Description(), indirectBufferOffset, drawCount, stride ));
		DrawIndexedIndirect( buf.Handle(), indirectBufferOffset, drawCount, stride );
	}

	void  RDrawContext::DrawIndexedIndirect (RmBufferID	indirectBuffer,
											 Bytes		indirectBufferOffset,
											 uint		drawCount,
											 Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawIndexedIndirect( drawCount, stride ));

		Msg::CmdBuf_Bake::DrawIndexedIndirectCmd  cmd;
		cmd.indirectBuffer			= indirectBuffer;
		cmd.indirectBufferOffset	= indirectBufferOffset;
		cmd.drawCount				= drawCount;
		cmd.stride					= stride;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	DrawMeshTasks
=================================================
*/
	void  RDrawContext::DrawMeshTasks (const uint3 &taskCount) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasks( taskCount ));

		Msg::CmdBuf_Bake::DrawMeshTasksCmd  cmd;
		cmd.taskCount = taskCount;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	DrawMeshTasksIndirect
=================================================
*/
	void  RDrawContext::DrawMeshTasksIndirect (BufferID	indirectBuffer,
											   Bytes	indirectBufferOffset,
											   uint		drawCount,
											   Bytes	stride) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( DrawMeshTasksIndirect( buf.Description(), indirectBufferOffset, drawCount, stride ));
		DrawMeshTasksIndirect( buf.Handle(), indirectBufferOffset, drawCount, stride );
	}

	void  RDrawContext::DrawMeshTasksIndirect (RmBufferID	indirectBuffer,
											   Bytes		indirectBufferOffset,
											   uint			drawCount,
											   Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasksIndirect( drawCount, stride ));

		Msg::CmdBuf_Bake::DrawMeshTasksIndirectCmd  cmd;
		cmd.indirectBuffer			= indirectBuffer;
		cmd.indirectBufferOffset	= indirectBufferOffset;
		cmd.drawCount				= drawCount;
		cmd.stride					= stride;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	DrawIndirectCount
=================================================
*/
	void  RDrawContext::DrawIndirectCount (BufferID	indirectBuffer,
										   Bytes	indirectBufferOffset,
										   BufferID	countBuffer,
										   Bytes	countBufferOffset,
										   uint		maxDrawCount,
										   Bytes	stride) __Th___
	{
		auto	[ibuf, cbuf] = _GetResourcesOrThrow( indirectBuffer, countBuffer );
		VALIDATE_GCTX( DrawIndirectCount( ibuf.Description(), indirectBufferOffset,
										  cbuf.Description(), countBufferOffset,
										  maxDrawCount, stride ));
		DrawIndirectCount( ibuf.Handle(), indirectBufferOffset, cbuf.Handle(), countBufferOffset, maxDrawCount, stride );
	}

	void  RDrawContext::DrawIndirectCount (RmBufferID	indirectBuffer,
										   Bytes		indirectBufferOffset,
										   RmBufferID	countBuffer,
										   Bytes		countBufferOffset,
										   uint			maxDrawCount,
										   Bytes		stride) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::DrawIndirectCount ));
		VALIDATE_GCTX( DrawIndirectCount( maxDrawCount, stride ));

		Msg::CmdBuf_Bake::DrawIndirectCountCmd  cmd;
		cmd.indirectBuffer			= indirectBuffer;
		cmd.indirectBufferOffset	= indirectBufferOffset;
		cmd.countBuffer				= countBuffer;
		cmd.countBufferOffset		= countBufferOffset;
		cmd.maxDrawCount			= maxDrawCount;
		cmd.stride					= stride;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	DrawIndexedIndirectCount
=================================================
*/
	void  RDrawContext::DrawIndexedIndirectCount (BufferID	indirectBuffer,
												  Bytes		indirectBufferOffset,
												  BufferID	countBuffer,
												  Bytes		countBufferOffset,
												  uint		maxDrawCount,
												  Bytes		stride) __Th___
	{
		auto	[ibuf, cbuf] = _GetResourcesOrThrow( indirectBuffer, countBuffer );
		VALIDATE_GCTX( DrawIndexedIndirectCount( ibuf.Description(), indirectBufferOffset,
												 cbuf.Description(), countBufferOffset,
												 maxDrawCount, stride ));
		DrawIndexedIndirectCount( ibuf.Handle(), indirectBufferOffset, cbuf.Handle(), countBufferOffset, maxDrawCount, stride );
	}

	void  RDrawContext::DrawIndexedIndirectCount (RmBufferID	indirectBuffer,
												  Bytes			indirectBufferOffset,
												  RmBufferID	countBuffer,
												  Bytes			countBufferOffset,
												  uint			maxDrawCount,
												  Bytes			stride) __Th___
	{
		VALIDATE_GCTX( DrawIndexedIndirectCount( maxDrawCount, stride ));
		GCTX_CHECK( _HasFeature( EFeature::DrawIndexedIndirectCount ));

		Msg::CmdBuf_Bake::DrawIndexedIndirectCountCmd  cmd;
		cmd.indirectBuffer			= indirectBuffer;
		cmd.indirectBufferOffset	= indirectBufferOffset;
		cmd.countBuffer				= countBuffer;
		cmd.countBufferOffset		= countBufferOffset;
		cmd.maxDrawCount			= maxDrawCount;
		cmd.stride					= stride;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	DrawMeshTasksIndirectCount
=================================================
*/
	void  RDrawContext::DrawMeshTasksIndirectCount (BufferID	indirectBuffer,
													Bytes		indirectBufferOffset,
													BufferID	countBuffer,
													Bytes		countBufferOffset,
													uint		maxDrawCount,
													Bytes		stride) __Th___
	{
		auto	[ibuf, cbuf] = _GetResourcesOrThrow( indirectBuffer, countBuffer );
		VALIDATE_GCTX( DrawMeshTasksIndirectCount( ibuf.Description(), indirectBufferOffset,
												   cbuf.Description(), countBufferOffset,
												   maxDrawCount, stride ));
		DrawMeshTasksIndirectCount( ibuf.Handle(), indirectBufferOffset, cbuf.Handle(), countBufferOffset, maxDrawCount, stride );
	}

	void  RDrawContext::DrawMeshTasksIndirectCount (RmBufferID	indirectBuffer,
													Bytes		indirectBufferOffset,
													RmBufferID	countBuffer,
													Bytes		countBufferOffset,
													uint		maxDrawCount,
													Bytes		stride) __Th___
	{
		VALIDATE_GCTX( DrawMeshTasksIndirectCount( maxDrawCount, stride ));
		GCTX_CHECK( _HasFeature( EFeature::DrawMeshTasksIndirectCount ));

		Msg::CmdBuf_Bake::DrawMeshTasksIndirectCountCmd  cmd;
		cmd.indirectBuffer			= indirectBuffer;
		cmd.indirectBufferOffset	= indirectBufferOffset;
		cmd.countBuffer				= countBuffer;
		cmd.countBufferOffset		= countBufferOffset;
		cmd.maxDrawCount			= maxDrawCount;
		cmd.stride					= stride;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	DispatchTile
=================================================
*/
	void  RDrawContext::DispatchTile () __Th___
	{
		Msg::CmdBuf_Bake::DispatchTileCmd  cmd;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	AttachmentBarrier
=================================================
*/
	void  RDrawContext::AttachmentBarrier (AttachmentName::Ref name, EResourceState srcState, EResourceState dstState) __Th___
	{
		Msg::CmdBuf_Bake::Draw_AttachmentBarrierCmd  cmd;
		cmd.name		= name;
		cmd.srcState	= srcState;
		cmd.dstState	= dstState;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	ClearAttachment
=================================================
*/
	bool  RDrawContext::ClearAttachment (AttachmentName::Ref name, const RGBA32f &color, const RectI &region, ImageLayer baseLayer, uint layerCount) __Th___
	{
		VALIDATE_GCTX( ClearAttachment( region ));

		Msg::CmdBuf_Bake::Draw_ClearAttachmentCmd  cmd;
		cmd.name		= name;
		cmd.clearValue	= color;
		cmd.region		= region;
		cmd.baseLayer	= baseLayer;
		cmd.layerCount	= layerCount;
		_cmdbuf->AddCommand( cmd );
		return true;
	}

	bool  RDrawContext::ClearAttachment (AttachmentName::Ref name, const RGBA32u &color, const RectI &region, ImageLayer baseLayer, uint layerCount) __Th___
	{
		VALIDATE_GCTX( ClearAttachment( region ));

		Msg::CmdBuf_Bake::Draw_ClearAttachmentCmd  cmd;
		cmd.name		= name;
		cmd.clearValue	= color;
		cmd.region		= region;
		cmd.baseLayer	= baseLayer;
		cmd.layerCount	= layerCount;
		_cmdbuf->AddCommand( cmd );
		return true;
	}

	bool  RDrawContext::ClearAttachment (AttachmentName::Ref name, const RGBA32i &color, const RectI &region, ImageLayer baseLayer, uint layerCount) __Th___
	{
		VALIDATE_GCTX( ClearAttachment( region ));

		Msg::CmdBuf_Bake::Draw_ClearAttachmentCmd  cmd;
		cmd.name		= name;
		cmd.clearValue	= color;
		cmd.region		= region;
		cmd.baseLayer	= baseLayer;
		cmd.layerCount	= layerCount;
		_cmdbuf->AddCommand( cmd );
		return true;
	}

	bool  RDrawContext::ClearAttachment (AttachmentName::Ref name, const DepthStencil &depthStencil, const RectI &region, ImageLayer baseLayer, uint layerCount) __Th___
	{
		VALIDATE_GCTX( ClearAttachment( region ));

		Msg::CmdBuf_Bake::Draw_ClearAttachmentCmd  cmd;
		cmd.name		= name;
		cmd.clearValue	= depthStencil;
		cmd.region		= region;
		cmd.baseLayer	= baseLayer;
		cmd.layerCount	= layerCount;
		_cmdbuf->AddCommand( cmd );
		return true;
	}

/*
=================================================
	AllocVStream
=================================================
*/
	bool  RDrawContext::AllocVStream (Bytes size, OUT VertexStream &result) __Th___
	{
		bool	res = _mngr.GetResourceManager().GetStagingManager().AllocVStream( GetFrameId(), size, OUT result );

		if_likely( res )
			_cmdbuf->CopyHostToDev( result.devicePtr, result.mappedPtr, result.offset, result.size );

		return res;
	}

/*
=================================================
	CommitBarriers
=================================================
*/
	void  RDrawContext::CommitBarriers ()
	{
		auto	bar = _mngr.GetBarriers();
		if_unlikely( bar )
		{
			_cmdbuf->PipelineBarrier( bar );
			_mngr.ClearBarriers();
		}
	}

/*
=================================================
	WriteTimestamp
=================================================
*/
	void  RDrawContext::WriteTimestamp (const IQueryManager::IQuery &q, uint index, EPipelineScope srcScope) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::WriteTimestamp ));

		_cmdbuf->WriteTimestamp( static_cast<RQueryManager::Query const&>(q), index, srcScope );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
