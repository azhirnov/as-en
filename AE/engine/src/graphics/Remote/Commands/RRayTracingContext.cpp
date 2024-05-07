// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RRayTracingContext.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	constructor
=================================================
*/
	RRayTracingContext::RRayTracingContext (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
		RBaseContext{ task, RVRef(cmdbuf), dbg, ECtxType::RayTracing }
	{
		Validator_t::CtxInit( task.GetQueueMask() );
	}

/*
=================================================
	BindPipeline
=================================================
*/
	void  RRayTracingContext::BindPipeline (RayTracingPipelineID pplnId) __Th___
	{
		auto&	ppln = _GetResourcesOrThrow( pplnId );

		Msg::CmdBuf_Bake::RayTracing_BindPipelineCmd  cmd;
		cmd.ppln = ppln.Handle();
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	void  RRayTracingContext::BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		auto&	desc_set = _GetResourcesOrThrow( ds );
		GCTX_CHECK( index );

		Msg::CmdBuf_Bake::RayTracing_BindDescriptorSetCmd  cmd;
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
	void  RRayTracingContext::PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName) __Th___
	{
		VALIDATE_GCTX( PushConstant( idx, size, typeName ));

		Msg::CmdBuf_Bake::RayTracing_PushConstantCmd  cmd;
		cmd.pcIndex_offset	= idx.bufferId;
		cmd.pcIndex_stage	= idx.stage;
		cmd.data			= ArrayView<ubyte>{ Cast<ubyte>(values), usize{size} };
		cmd.typeName		= typeName;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	SetStackSize
=================================================
*/
	void  RRayTracingContext::SetStackSize (Bytes size) __Th___
	{
		Msg::CmdBuf_Bake::RayTracing_SetStackSizeCmd  cmd;
		cmd.size = size;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	TraceRays
=================================================
*/
	void  RRayTracingContext::TraceRays (const uint3 dim, RTShaderBindingID sbtId) __Th___
	{
		auto&	sbt = _GetResourcesOrThrow( sbtId );
		TraceRays( dim, sbt.Handle() );
	}

	void  RRayTracingContext::TraceRays (const uint3 dim, RmRTShaderBindingID sbt) __Th___
	{
		VALIDATE_GCTX( TraceRays( dim ));

		Msg::CmdBuf_Bake::RayTracing_TraceRaysCmd  cmd;
		cmd.dim = dim;
		cmd.sbt = sbt;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	TraceRaysIndirect
=================================================
*/
	void  RRayTracingContext::TraceRaysIndirect (const RTShaderBindingTable &sbt, DeviceAddress address) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::TraceRaysIndirect_DevAddr ));

		UNTESTED
	}

	void  RRayTracingContext::TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( TraceRaysIndirect( buf.Description(), indirectBufferOffset ));
		TraceRaysIndirect( sbt.id, buf.Handle(), indirectBufferOffset );
	}

	void  RRayTracingContext::TraceRaysIndirect (RTShaderBindingID sbtId, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		auto	[buf, sbt] = _GetResourcesOrThrow( indirectBuffer, sbtId );
		VALIDATE_GCTX( TraceRaysIndirect( buf.Description(), indirectBufferOffset ));
		TraceRaysIndirect( sbt.Handle(), buf.Handle(), indirectBufferOffset );
	}

	void  RRayTracingContext::TraceRaysIndirect (RmRTShaderBindingID sbt, RmBufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		Msg::CmdBuf_Bake::RayTracing_TraceRaysIndirectCmd  cmd;
		cmd.sbt				= sbt;
		cmd.indirectBuffer	= indirectBuffer;
		cmd.indirectOffset	= indirectBufferOffset;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	TraceRaysIndirect2
=================================================
*/
	void  RRayTracingContext::TraceRaysIndirect2 (DeviceAddress address) __Th___
	{
		GCTX_CHECK( _HasFeature( EFeature::TraceRaysIndirect2_DevAddr ));

		UNTESTED
	}

	void  RRayTracingContext::TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( TraceRaysIndirect2( buf.Description(), indirectBufferOffset ));
		TraceRaysIndirect2( buf.Handle(), indirectBufferOffset );
	}

	void  RRayTracingContext::TraceRaysIndirect2 (RmBufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		Msg::CmdBuf_Bake::RayTracing_TraceRaysIndirect2Cmd  cmd;
		cmd.indirectBuffer	= indirectBuffer;
		cmd.indirectOffset	= indirectBufferOffset;
		_cmdbuf->AddCommand( cmd );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
