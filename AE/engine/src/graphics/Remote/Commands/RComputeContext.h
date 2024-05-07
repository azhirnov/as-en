// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RBaseContext.h"
# include "graphics/Remote/Commands/RAccumBarriers.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Compute Context implementation
	//

	class RComputeContext final : public Graphics::_hidden_::RBaseContext, public IComputeContext
	{
	// types
	private:
		using AccumBar		= Graphics::_hidden_::RAccumBarriers< RComputeContext >;
		using DeferredBar	= Graphics::_hidden_::RAccumDeferredBarriersForCtx< RComputeContext >;
		using Validator_t	= Graphics::_hidden_::ComputeContextValidation;


	// methods
	public:
		explicit RComputeContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)			__Th___;

		RComputeContext ()																								= delete;
		RComputeContext (const RComputeContext &)																		= delete;

		void  BindPipeline (ComputePipelineID ppln)																		__Th_OV;
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)	__Th_OV;

		void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref)		__Th_OV;
		using IComputeContext::PushConstant;

		using IComputeContext::Dispatch;
		void  Dispatch (const uint3 &groupCount)																		__Th_OV;

		void  DispatchIndirect (BufferID buffer, Bytes offset)															__Th_OV;
		void  DispatchIndirect (RmBufferID buffer, Bytes offset)														__Th___;

		ND_ RmCommandBufferID	EndCommandBuffer ()																		__Th___	{ return _EndCommandBuffer( ECtxType::Compute ); }
		ND_ CmdBuf_t			ReleaseCommandBuffer ()																	__Th___	{ return _ReleaseCommandBuffer( ECtxType::Compute ); }

		RBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
