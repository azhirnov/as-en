// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	RayTracingCtx	--> DirectRayTracingCtx   --> BarrierMngr --> Metal device 
					\-> IndirectRayTracingCtx --> BarrierMngr --> Backed commands
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
	// Metal Direct Ray Tracing Context implementation
	//

	class _MDirectRayTracingCtx : public MBaseDirectContext
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
		ND_ ArgSet_t  Arguments ()									{ return ArgSet_t{ _encoder }; }
		
		ND_ MetalCommandBufferRC	EndCommandBuffer ();
		ND_ MCommandBuffer		 	ReleaseCommandBuffer ();

	protected:
		_MDirectRayTracingCtx (const RenderTask &task);
		_MDirectRayTracingCtx (const RenderTask &task, MCommandBuffer cmdbuf);

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
	// Metal Indirect Ray Tracing Context implementation
	//
	
	class _MIndirectRayTracingCtx : public MBaseIndirectContext
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
		ND_ ArgSet_t  Arguments ()									{ return ArgSet_t{ this->_cmdbuf.get() }; }
		
		ND_ MBakedCommands		EndCommandBuffer ();
		ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ();

	protected:
		_MIndirectRayTracingCtx (const RenderTask &task);
		_MIndirectRayTracingCtx (const RenderTask &task, MSoftwareCmdBufPtr cmdbuf);

		void  _Dispatch (const uint3 &groupCount)					{ DispatchThreadgroups( uint3{_states.localSize}, groupCount ); }
		void  _DispatchIndirect (MetalBuffer buffer, Bytes offset)	{ DispatchThreadgroupsIndirect( buffer, offset, uint3{_states.localSize} ); }

		void  _BindPipeline (MetalComputePipeline ppln, const uint3 &localSize);
		void  _BindDescriptorSet (uint index, const MDescriptorSet &ds, ArrayView<uint> dynamicOffsets);
	};



	//
	// Metal Ray Tracing Context implementation
	//
	
	template <typename CtxImpl>
	class _MRayTracingContextImpl : public CtxImpl, public IRayTracingContext
	{
	// types
	public:
		static constexpr bool	IsRayTracingContext			= true;
		static constexpr bool	IsMetalRayTracingContext	= true;
	private:
		using RawCtx	= CtxImpl;
		using AccumBar	= MAccumBarriers< _MRayTracingContextImpl< CtxImpl >>;


	// methods
	public:
		explicit _MRayTracingContextImpl (const RenderTask &task) : RawCtx{ task } {}

		template <typename RawCmdBufType>
		_MRayTracingContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) : RawCtx{ task, RVRef(cmdbuf) } {}

		_MRayTracingContextImpl () = delete;
		_MRayTracingContextImpl (const _MRayTracingContextImpl &) = delete;

		void  BindPipeline (RayTracingPipelineID ppln) override final;
		void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) override final;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages) override final;

		void  SetStackSize (Bytes size);

		void  TraceRays (const uint2 dim, const ShaderBindingTable &sbt);
		void  TraceRays (const uint3 dim, const ShaderBindingTable &sbt);
		void  TraceRaysIndirect (const ShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectBufferOffset);	// _DispatchIndirect
		void  TraceRaysIndirect (BufferID indirectBuffer, Bytes indirectBufferOffset);									// _DispatchIndirect

		MBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectRayTracingContext		= _hidden_::_MRayTracingContextImpl< _hidden_::_MDirectRayTracingCtx >;
	using MIndirectRayTracingContext	= _hidden_::_MRayTracingContextImpl< _hidden_::_MIndirectRayTracingCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
/*
=================================================
	BindPipeline
=================================================
*/
	template <typename C>
	void  _MRayTracingContextImpl<C>::BindPipeline (RayTracingPipelineID ppln)
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
	void  _MRayTracingContextImpl<C>::BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets)
	{
		auto&	desc_set = _GetResourcesOrThrow( ds );

		RawCtx::_BindDescriptorSet( index, desc_set, dynamicOffsets );
	}

/*
=================================================
	PushConstant
=================================================
*/
	template <typename C>
	void  _MRayTracingContextImpl<C>::PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		Unused( offset, size, values, stages );
		// TODO
		//RawCtx::_PushComputeConstant( offset, size, values, stages );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
