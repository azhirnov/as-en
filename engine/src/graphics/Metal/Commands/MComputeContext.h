// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	ComputeCtx  --> DirectComputeCtx   --> BarrierMngr --> Metal device 
				\-> IndirectComputeCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MBakedCommands.h"
# include "graphics/Metal/Commands/MBaseIndirectContext.h"
# include "graphics/Metal/Commands/MBaseDirectContext.h"
# include "graphics/Metal/Commands/MAccumBarriers.h"
# include "graphics/Metal/Commands/MArgumentSetter.h"
# include "graphics/Metal/MRenderTaskScheduler.h"

namespace AE::Graphics::_hidden_
{
	
	//
	// Metal Direct Compute Context implementation
	//

	class _MDirectComputeCtx : public MBaseDirectContext
	{
	// types
	private:
		using ArgSet_t = MArgumentSetter< EShader::Compute, true >;


	// variables
	private:
		MetalComputeCommandEncoderRC	_encoder;
	protected:
		struct {
			ushort3							localSize;
		}								_states;

		
	// methods
	public:
		void  DispatchThreadgroups (const uint3 &threadgroupsPerGrid, const uint3 &threadsPerThreadgroup);
		void  DispatchThreads (const uint3 &threadsPerGrid, const uint3 &threadsPerThreadgroup);
		void  DispatchThreadgroupsIndirect (MetalBuffer indirectBuffer, Bytes offset, const uint3 &threadsPerThreadgroup);

		void  SetImageblock (const uint2 &dimInPix);

		ND_ ArgSet_t  Arguments ()									{ return ArgSet_t{ _encoder }; }
		
		ND_ MetalCommandBufferRC	EndCommandBuffer ();
		ND_ MCommandBuffer		 	ReleaseCommandBuffer ();

	protected:
		_MDirectComputeCtx (const RenderTask &task);
		_MDirectComputeCtx (const RenderTask &task, MCommandBuffer cmdbuf);

		ND_ MetalCommandEncoder  _BaseEncoder ()					{ return MetalCommandEncoder{ _encoder.Ptr() }; }

		void  _Dispatch (const uint3 &groupCount)					{ DispatchThreadgroups( groupCount, uint3{_states.localSize} ); }
		void  _DispatchIndirect (MetalBuffer buffer, Bytes offset)	{ DispatchThreadgroupsIndirect( buffer, offset, uint3{_states.localSize} ); }

		void  _BindPipeline (MetalComputePipeline ppln, const uint3 &localSize);
		void  _BindDescriptorSet (uint index, const MDescriptorSet &ds, ArrayView<uint> dynamicOffsets);

		void  _DebugMarker (NtStringView text, RGBA8u)				{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_DebugMarker( _BaseEncoder(), text ); }
		void  _PushDebugGroup (NtStringView text, RGBA8u)			{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PushDebugGroup( _BaseEncoder(), text ); }
		void  _PopDebugGroup ()										{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PopDebugGroup( _BaseEncoder() ); }
	};
	


	//
	// Metal Indirect Compute Context implementation
	//
	
	class _MIndirectComputeCtx : public MBaseIndirectContext
	{
	// types
	private:
		using ArgSet_t = MArgumentSetter< EShader::Compute, false >;


	// variables
	protected:
		struct {
			ushort3		localSize;
		}			_states;


	// methods
	public:
		void  DispatchThreadgroups (const uint3 &threadgroupsPerGrid, const uint3 &threadsPerThreadgroup);
		void  DispatchThreads (const uint3 &threadsPerGrid, const uint3 &threadsPerThreadgroup);
		void  DispatchThreadgroupsIndirect (MetalBuffer indirectBuffer, Bytes offset, const uint3 &threadsPerThreadgroup);

		void  SetImageblock (const uint2 &dimInPix);
		
		ND_ ArgSet_t  Arguments ()									{ return ArgSet_t{ this->_cmdbuf.get() }; }
		
		ND_ MBakedCommands		EndCommandBuffer ();
		ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ();

	protected:
		_MIndirectComputeCtx (const RenderTask &task);
		_MIndirectComputeCtx (const RenderTask &task, MSoftwareCmdBufPtr cmdbuf);

		void  _Dispatch (const uint3 &groupCount)					{ DispatchThreadgroups( uint3{_states.localSize}, groupCount ); }
		void  _DispatchIndirect (MetalBuffer buffer, Bytes offset)	{ DispatchThreadgroupsIndirect( buffer, offset, uint3{_states.localSize} ); }

		void  _BindPipeline (MetalComputePipeline ppln, const uint3 &localSize);
		void  _BindDescriptorSet (uint index, const MDescriptorSet &ds, ArrayView<uint> dynamicOffsets);
	};



	//
	// Metal Compute Context implementation
	//
	
	template <typename CtxImpl>
	class _MComputeContextImpl : public CtxImpl, public IComputeContext
	{
	// types
	public:
		static constexpr bool	IsComputeContext		= true;
		static constexpr bool	IsMetalComputeContext	= true;
	private:
		using RawCtx	= CtxImpl;
		using AccumBar	= MAccumBarriers< _MComputeContextImpl< CtxImpl >>;


	// methods
	public:
		explicit _MComputeContextImpl (const RenderTask &task) : RawCtx{ task } {}

		template <typename RawCmdBufType>
		_MComputeContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) : RawCtx{ task, RVRef(cmdbuf) } {}

		_MComputeContextImpl () = delete;
		_MComputeContextImpl (const _MComputeContextImpl &) = delete;
		~_MComputeContextImpl () override {}

		void  BindPipeline (ComputePipelineID ppln) override final;
		void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) override final;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages) override final;
		
		using IComputeContext::Dispatch;
		void  Dispatch (const uint3 &groupCount)				override final	{ RawCtx::_Dispatch( groupCount ); }

		void  DispatchIndirect (MetalBuffer buffer, Bytes offset)				{ RawCtx::DispatchThreadgroupsWithIndirectBuffer( buffer, offset, this->_states.localSize ); }
		void  DispatchIndirect (BufferID buffer, Bytes offset)	override final;
		
		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }
		
		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }
		
		ND_ AccumBar  AccumBarriers ()											{ return AccumBar{ *this }; }

		MBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectComputeContext		= _hidden_::_MComputeContextImpl< _hidden_::_MDirectComputeCtx >;
	using MIndirectComputeContext	= _hidden_::_MComputeContextImpl< _hidden_::_MIndirectComputeCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
/*
=================================================
	BindPipeline
=================================================
*/
	template <typename C>
	void  _MComputeContextImpl<C>::BindPipeline (ComputePipelineID ppln)
	{
		auto*	cppln = this->_mngr.Get( ppln );
		CHECK_ERRV( cppln );

		RawCtx::_BindPipeline( cppln->Handle(), cppln->LocalSize() );
	}
	
/*
=================================================
	BindDescriptorSet
=================================================
*/
	template <typename C>
	void  _MComputeContextImpl<C>::BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets)
	{
		auto*	desc_set = this->_mngr.Get( ds );
		CHECK_ERRV( desc_set );

		RawCtx::_BindDescriptorSet( index, *desc_set, dynamicOffsets );
	}

/*
=================================================
	PushConstant
=================================================
*/
	template <typename C>
	void  _MComputeContextImpl<C>::PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		// TODO
		//RawCtx::_PushComputeConstant( offset, size, values, stages );
	}
	
/*
=================================================
	DispatchIndirect
=================================================
*/
	template <typename C>
	void  _MComputeContextImpl<C>::DispatchIndirect (BufferID bufferid, Bytes offset)
	{
		auto*	buf = this->_mngr.Get( bufferid );
		CHECK_ERRV( buf );
		ASSERT( buf->Size() >= offset + sizeof(DispatchIndirectCommand) );

		RawCtx::_DispatchIndirect( buf->Handle(), offset );
	}
//-----------------------------------------------------------------------------
	


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
