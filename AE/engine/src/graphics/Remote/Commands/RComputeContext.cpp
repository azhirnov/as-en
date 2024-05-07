// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RComputeContext.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	constructor
=================================================
*/
	RComputeContext::RComputeContext (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
		RBaseContext{ task, RVRef(cmdbuf), dbg, ECtxType::Compute }
	{
		Validator_t::CtxInit( task.GetQueueMask() );
	}

/*
=================================================
	BindPipeline
=================================================
*/
	void  RComputeContext::BindPipeline (ComputePipelineID pplnId) __Th___
	{
		auto&	ppln = _GetResourcesOrThrow( pplnId );

		Msg::CmdBuf_Bake::Compute_BindPipelineCmd  cmd;
		cmd.ppln = ppln.Handle();
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	void  RComputeContext::BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		auto&	desc_set = _GetResourcesOrThrow( ds );
		GCTX_CHECK( index );

		Msg::CmdBuf_Bake::Compute_BindDescriptorSetCmd  cmd;
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
	void  RComputeContext::PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName) __Th___
	{
		VALIDATE_GCTX( PushConstant( idx, size, typeName ));

		Msg::CmdBuf_Bake::Compute_PushConstantCmd  cmd;
		cmd.pcIndex_offset	= idx.bufferId;
		cmd.pcIndex_stage	= idx.stage;
		cmd.data			= ArrayView<ubyte>{ Cast<ubyte>(values), usize{size} };
		cmd.typeName		= typeName;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	Dispatch
=================================================
*/
	void  RComputeContext::Dispatch (const uint3 &groupCount) __Th___
	{
		Msg::CmdBuf_Bake::Compute_Dispatch  cmd;
		cmd.groupCount = groupCount;
		_cmdbuf->AddCommand( cmd );
	}

/*
=================================================
	DispatchIndirect
=================================================
*/
	void  RComputeContext::DispatchIndirect (BufferID buffer, Bytes offset) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( buffer );

		Msg::CmdBuf_Bake::Compute_DispatchIndirect  cmd;
		cmd.buffer	= buf.Handle();
		cmd.offset	= offset;
		_cmdbuf->AddCommand( cmd );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
