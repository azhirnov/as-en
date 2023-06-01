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
# include "graphics/Metal/Commands/MBoundDescriptorSets.h"

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
	protected:
		struct {
			ushort3		localSize;
		}			_states;

		
	// methods
	public:
		ND_ ArgSet_t				Arguments ()												__NE___	{ return ArgSet_t{ _Encoder2() }; }
		
		ND_ MetalCommandBufferRC	EndCommandBuffer ()											__Th___;
		ND_ MCommandBuffer		 	ReleaseCommandBuffer ()										__Th___;

		MBARRIERMNGR_INHERIT_MBARRIERS

	protected:
		_MDirectRayTracingCtx (const RenderTask &task, MCommandBuffer cmdbuf, DebugLabel dbg)	__Th___;
		
		ND_ auto  						_Encoder ()												__NE___;
		ND_ MetalComputeCommandEncoder	_Encoder2 ()											__NE___	{ return MetalComputeCommandEncoder{ _cmdbuf.GetEncoder().Ptr() }; }

		void  _BindPipeline (MetalComputePipeline ppln, const uint3 &localSize);

		void  _DebugMarker (DebugLabel dbg)																{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_DebugMarker( dbg ); }
		void  _PushDebugGroup (DebugLabel dbg)															{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PushDebugGroup( dbg ); }
		void  _PopDebugGroup ()																			{ ASSERT( _NoPendingBarriers() );  _MBaseDirectContext::_PopDebugGroup(); }
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
		ND_ ArgSet_t			Arguments ()														__NE___	{ return ArgSet_t{ this->_cmdbuf.get() }; }
		
		ND_ MBakedCommands		EndCommandBuffer ()													__Th___;
		ND_ MSoftwareCmdBufPtr  ReleaseCommandBuffer ()												__Th___;

		MBARRIERMNGR_INHERIT_MBARRIERS

	protected:
		_MIndirectRayTracingCtx (const RenderTask &task, MSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)	__Th___;

		void  _BindPipeline (MetalComputePipeline ppln, const uint3 &localSize);
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

		using CmdBuf_t		= typename CtxImpl::CmdBuf_t;
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= MAccumBarriers< _MRayTracingContextImpl< CtxImpl >>;
		using DeferredBar	= MAccumDeferredBarriersForCtx< _MRayTracingContextImpl< CtxImpl >>;
		using Validator_t	= RayTracingContextValidation;

		
	// variables
	private:
		MBoundDescriptorSets	_boundDS;


	// methods
	public:
		explicit _MRayTracingContextImpl (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)		__Th___;

		_MRayTracingContextImpl ()																							= delete;
		_MRayTracingContextImpl (const _MRayTracingContextImpl &)															= delete;

		void  BindPipeline (RayTracingPipelineID ppln)																		__Th_OV;
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th_OV	{ _boundDS.Bind( *this, index, ds, dynamicOffsets ); }

		void  PushConstant (const PushConstantIndex &idx, Bytes size, const void *values, const ShaderStructName &typeName)	__Th_OV;
		using IRayTracingContext::PushConstant;

		void  SetStackSize (Bytes size)																						__Th_OV;

		void  TraceRays (const uint2 dim, RTShaderBindingID sbt)															__Th_OV;
		void  TraceRays (const uint3 dim, RTShaderBindingID sbt)															__Th_OV;
		
		void  TraceRays (const uint2 dim, const RTShaderBindingTable &sbt)													__Th_OV;
		void  TraceRays (const uint3 dim, const RTShaderBindingTable &sbt)													__Th_OV;
		
		void  TraceRaysIndirect (RTShaderBindingID sbt, BufferID indirectBuffer, Bytes indirectBufferOffset)				__Th_OV;
		void  TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectBufferOffset)		__Th_OV;	// _DispatchIndirect
		
		void  TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)										__Th_OV;	// _DispatchIndirect

		MBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using MDirectRayTracingContext		= Graphics::_hidden_::_MRayTracingContextImpl< Graphics::_hidden_::_MDirectRayTracingCtx >;
	using MIndirectRayTracingContext	= Graphics::_hidden_::_MRayTracingContextImpl< Graphics::_hidden_::_MIndirectRayTracingCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_MRayTracingContextImpl<C>::_MRayTracingContextImpl (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) :
		RawCtx{ task, RVRef(cmdbuf), dbg }
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute, task.GetQueueMask() ));
	}

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
	PushConstant
=================================================
*/
	template <typename C>
	void  _MRayTracingContextImpl<C>::PushConstant (const PushConstantIndex &idx, Bytes size, const void *values, const ShaderStructName &typeName)
	{
		Validator_t::PushConstant( idx, size, typeName );
		
		RawCtx::Arguments().SetBytes( values, size, MBufferIndex(idx.bufferId) );
	}
	
/*
=================================================
	SetStackSize
=================================================
*/
	template <typename C>
	void  _MRayTracingContextImpl<C>::SetStackSize (Bytes size)
	{
		Unused( size );
	}
	
/*
=================================================
	TraceRays
=================================================
*/
	template <typename C>
	void  _MRayTracingContextImpl<C>::TraceRays (const uint2 dim, RTShaderBindingID sbt)
	{
		Unused( dim, sbt );
	}
	
	template <typename C>
	void  _MRayTracingContextImpl<C>::TraceRays (const uint3 dim, RTShaderBindingID sbt)
	{
		Unused( dim, sbt );
	}
		
	template <typename C>
	void  _MRayTracingContextImpl<C>::TraceRays (const uint2 dim, const RTShaderBindingTable &sbt)
	{
		Unused( dim, sbt );
	}
	
	template <typename C>
	void  _MRayTracingContextImpl<C>::TraceRays (const uint3 dim, const RTShaderBindingTable &sbt)
	{
		Unused( dim, sbt );
	}
		
/*
=================================================
	TraceRaysIndirect
=================================================
*/
	template <typename C>
	void  _MRayTracingContextImpl<C>::TraceRaysIndirect (RTShaderBindingID sbt, BufferID indirectBuffer, Bytes indirectBufferOffset)
	{
		Unused( sbt, indirectBuffer, indirectBufferOffset );
	}
	
	template <typename C>
	void  _MRayTracingContextImpl<C>::TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectBufferOffset)
	{
		Unused( sbt, indirectBuffer, indirectBufferOffset );
	}
	
/*
=================================================
	TraceRaysIndirect2
=================================================
*/
	template <typename C>	
	void  _MRayTracingContextImpl<C>::TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)
	{
		Unused( indirectBuffer, indirectBufferOffset );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
