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
	protected:
		struct {
			ushort3		localSize;
		}			_states;

		
	// methods
	public:
		void  DispatchThreadgroups (const uint3 &threadgroupsPerGrid, const uint3 &threadsPerThreadgroup)					__Th___;
		void  DispatchThreads (const uint3 &threadsPerGrid, const uint3 &threadsPerThreadgroup)								__Th___;
		void  DispatchThreadgroupsIndirect (MetalBuffer indirectBuffer, Bytes offset, const uint3 &threadsPerThreadgroup)	__Th___;

		void  SetImageblock (const uint2 &dimInPix)																			__Th___;

		ND_ ArgSet_t  Arguments ()																							__NE___	{ return ArgSet_t{ _Encoder2() }; }
		
		ND_ MetalCommandBufferRC	EndCommandBuffer ()																		__Th___;
		ND_ MCommandBuffer		 	ReleaseCommandBuffer ()																	__Th___;

		MBARRIERMNGR_INHERIT_MBARRIERS

	protected:
		_MDirectComputeCtx (const RenderTask &task)																			__Th___;
		_MDirectComputeCtx (const RenderTask &task, MCommandBuffer cmdbuf)													__Th___;
		
		ND_ auto  						_Encoder ()			__NE___;
		ND_ MetalComputeCommandEncoder	_Encoder2 ()		__NE___	{ return MetalComputeCommandEncoder{ _cmdbuf.GetEncoder().Ptr() }; }

		void  _Dispatch (const uint3 &groupCount)					{ DispatchThreadgroups( groupCount, uint3{_states.localSize} ); }
		void  _DispatchIndirect (MetalBuffer buffer, Bytes offset)	{ DispatchThreadgroupsIndirect( buffer, offset, uint3{_states.localSize} ); }

		void  _BindPipeline (MetalComputePipeline ppln, const uint3 &localSize);

		void  _DebugMarker (DebugLabel dbg)							{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_DebugMarker( dbg ); }
		void  _PushDebugGroup (DebugLabel dbg)						{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PushDebugGroup( dbg ); }
		void  _PopDebugGroup ()										{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PopDebugGroup(); }
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
		void  DispatchThreadgroups (const uint3 &threadgroupsPerGrid, const uint3 &threadsPerThreadgroup)					__Th___;
		void  DispatchThreads (const uint3 &threadsPerGrid, const uint3 &threadsPerThreadgroup)								__Th___;
		void  DispatchThreadgroupsIndirect (MetalBuffer indirectBuffer, Bytes offset, const uint3 &threadsPerThreadgroup)	__Th___;

		void  SetImageblock (const uint2 &dimInPix)																			__Th___;
		
		ND_ ArgSet_t  Arguments ()																							__NE___	{ return ArgSet_t{ this->_cmdbuf.get() }; }
		
		ND_ MBakedCommands		EndCommandBuffer ()																			__Th___;
		ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ()																		__Th___;

		MBARRIERMNGR_INHERIT_MBARRIERS

	protected:
		_MIndirectComputeCtx (const RenderTask &task)																		__Th___;
		_MIndirectComputeCtx (const RenderTask &task, MSoftwareCmdBufPtr cmdbuf)											__Th___;

		void  _Dispatch (const uint3 &groupCount)					{ DispatchThreadgroups( uint3{_states.localSize}, groupCount ); }
		void  _DispatchIndirect (MetalBuffer buffer, Bytes offset)	{ DispatchThreadgroupsIndirect( buffer, offset, uint3{_states.localSize} ); }

		void  _BindPipeline (MetalComputePipeline ppln, const uint3 &localSize);
	};



	//
	// Metal Compute Context implementation
	//
	
	template <typename CtxImpl>
	class _MComputeContextImpl final : public CtxImpl, public IComputeContext
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
		explicit _MComputeContextImpl (const RenderTask &task)														__Th___	: RawCtx{ task } {}

		template <typename RawCmdBufType>
		_MComputeContextImpl (const RenderTask &task, RawCmdBufType cmdbuf)											__Th___	: RawCtx{ task, RVRef(cmdbuf) } {}

		_MComputeContextImpl ()																						= delete;
		_MComputeContextImpl (const _MComputeContextImpl &)															= delete;

		void  BindPipeline (ComputePipelineID ppln)																	__Th_OV;
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)__Th_OV;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)						__Th_OV;
		
		using IComputeContext::Dispatch;
		void  Dispatch (const uint3 &groupCount)																	__Th_OV	{ RawCtx::_Dispatch( groupCount ); }

		void  DispatchIndirect (MetalBuffer buffer, Bytes offset)													__Th___	{ RawCtx::DispatchThreadgroupsWithIndirectBuffer( buffer, offset, this->_states.localSize ); }
		void  DispatchIndirect (BufferID buffer, Bytes offset)														__Th_OV;

		MBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectComputeContext		= Graphics::_hidden_::_MComputeContextImpl< Graphics::_hidden_::_MDirectComputeCtx >;
	using MIndirectComputeContext	= Graphics::_hidden_::_MComputeContextImpl< Graphics::_hidden_::_MIndirectComputeCtx >;

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
		auto&	cppln = _GetResourcesOrThrow( ppln );

		RawCtx::_BindPipeline( cppln.Handle(), cppln.LocalSize() );
	}
	
/*
=================================================
	BindDescriptorSet
=================================================
*/
	template <typename C>
	void  _MComputeContextImpl<C>::BindDescriptorSet (const DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets)
	{
		CHECK_THROW( dynamicOffsets.empty() );	// not supported yet
	
		auto&	desc_set = _GetResourcesOrThrow( ds );
		ASSERT( desc_set.ShaderStages() == EShaderStages::Compute );

		this->Arguments().SetBuffer( desc_set.Handle(), 0_b, MBufferIndex(index.mtlIndex.Compute()) );
	}

/*
=================================================
	PushConstant
=================================================
*/
	template <typename C>
	void  _MComputeContextImpl<C>::PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		Unused( offset, size, values, stages );
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
		auto&	buf = _GetResourcesOrThrow( bufferid );
		ASSERT( buf.Size() >= offset + sizeof(DispatchIndirectCommand) );

		RawCtx::_DispatchIndirect( buf.Handle(), offset );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
