// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	ComputeCtx  --> DirectComputeCtx   --> BarrierMngr --> Metal device 
				\-> IndirectComputeCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MRenderTaskScheduler.h"
# include "graphics/Metal/Commands/MBakedCommands.h"
# include "graphics/Metal/Commands/MBaseIndirectContext.h"
# include "graphics/Metal/Commands/MBaseDirectContext.h"
# include "graphics/Metal/Commands/MAccumBarriers.h"
# include "graphics/Metal/Commands/MArgumentSetter.h"

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
			uint3							localSize;
		}								_states;

		
	// methods
	public:
		void  DispatchThreadgroups (const uint3 &threadgroupsPerGrid, const uint3 &threadsPerThreadgroup);
		void  DispatchThreads (const uint3 &threadsPerGrid, const uint3 &threadsPerThreadgroup);
		void  DispatchThreadgroupsWithIndirectBuffer (MetalBuffer indirectBuffer, Bytes offset, const uint3 &threadsPerThreadgroup);

		void  SetImageblockWidth (const uint2 &dimInPix);
		
		ND_ ArgSet_t  Arguments ()									{ return ArgSet_t{ _encoder }; }

	protected:
		_MDirectComputeCtx (Ptr<MCommandBatch> batch);
		_MDirectComputeCtx (Ptr<MCommandBatch> batch, MCommandBuffer cmdbuf);

		ND_ MetalCommandEncoder  _BaseEncoder ();

		void  _Dispatch (const uint3 &groupCount);
		void  _DispatchBase (const uint3 &baseGroup, const uint3 &groupCount);
		
		void  _BindPipeline (MetalComputePipeline ppln, const uint3 &localSize);
		
		void  _DebugMarker (NtStringView text, RGBA8u);
		void  _PushDebugGroup (NtStringView text, RGBA8u);
		void  _PopDebugGroup ();
		
		void  _CommitBarriers ();
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
			uint3		localSize;
		}			_states;


	// methods
	public:
		void  DispatchThreadgroups (const uint3 &threadgroupsPerGrid, const uint3 &threadsPerThreadgroup);
		void  DispatchThreads (const uint3 &threadsPerGrid, const uint3 &threadsPerThreadgroup);
		void  DispatchThreadgroupsWithIndirectBuffer (MetalBuffer indirectBuffer, Bytes offset, const uint3 &threadsPerThreadgroup);

		void  SetImageblockWidth (const uint2 &dimInPix);
		
		ND_ ArgSet_t  Arguments ();

	protected:
		_MIndirectComputeCtx (Ptr<MCommandBatch> batch);
		_MIndirectComputeCtx (Ptr<MCommandBatch> batch, MSoftwareCmdBufPtr cmdbuf);

		void  _Dispatch (const uint3 &groupCount);
		void  _DispatchBase (const uint3 &baseGroup, const uint3 &groupCount);

		void  _BindPipeline (MetalComputePipeline ppln, const uint3 &localSize);
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
		explicit _MComputeContextImpl (Ptr<MCommandBatch> batch) : RawCtx{ batch } {}

		template <typename RawCmdBufType>
		_MComputeContextImpl (Ptr<MCommandBatch> batch, RawCmdBufType cmdbuf) : RawCtx{ batch, RVRef(cmdbuf) } {}

		_MComputeContextImpl () = delete;
		_MComputeContextImpl (const _MComputeContextImpl &) = delete;
		~_MComputeContextImpl () override {}

		void  BindPipeline (ComputePipelineID ppln) override final;
		void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) override final;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages) override final;
		
		void  Dispatch (const uint3 &groupCount)								override final	{ RawCtx::_Dispatch( groupCount ); }
		void  DispatchBase (const uint3 &baseGroup, const uint3 &groupCount)	override final	{ RawCtx::_DispatchBase( baseGroup, groupCount ); }

		using IComputeContext::Dispatch;
		using IComputeContext::DispatchBase;

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

#endif // AE_ENABLE_METAL
