// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/REnumCast.h"
# include "graphics/Remote/Commands/RBaseContext.h"
# include "graphics/Remote/Commands/RAccumBarriers.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Ray Tracing Context implementation
	//

	class RRayTracingContext final : public Graphics::_hidden_::RBaseContext, public IRayTracingContext
	{
	// types
	private:
		using AccumBar		= Graphics::_hidden_::RAccumBarriers< RRayTracingContext >;
		using DeferredBar	= Graphics::_hidden_::RAccumDeferredBarriersForCtx< RRayTracingContext >;
		using Validator_t	= Graphics::_hidden_::RayTracingContextValidation;


	// methods
	public:
		explicit RRayTracingContext (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)			__Th___;

		RRayTracingContext ()																								= delete;
		RRayTracingContext (const RRayTracingContext &)																		= delete;

		void  BindPipeline (RayTracingPipelineID ppln)																		__Th_OV;
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th_OV;

		void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th_OV;
		using IRayTracingContext::PushConstant;

		void  SetStackSize (Bytes size)																						__Th_OV;

		void  TraceRays (const uint2 dim, const RTShaderBindingTable &sbt)													__Th_OV	{ TraceRays( uint3{dim,1u}, sbt.id ); }
		void  TraceRays (const uint3 dim, const RTShaderBindingTable &sbt)													__Th_OV	{ TraceRays( dim, sbt.id ); }
		void  TraceRays (const uint2 dim, RTShaderBindingID sbt)															__Th_OV	{ TraceRays( uint3{dim,1u}, sbt ); }
		void  TraceRays (const uint3 dim, RTShaderBindingID sbt)															__Th_OV;

		void  TraceRays (const uint2 dim, RmRTShaderBindingID sbt)															__Th___	{ TraceRays( uint3{dim,1u}, sbt ); }
		void  TraceRays (const uint3 dim, RmRTShaderBindingID sbt)															__Th___;

		void  TraceRaysIndirect (const RTShaderBindingTable &sbt, DeviceAddress address)									__Th_OV;	// Vulkan host
		void  TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectOffset)			__Th_OV;
		void  TraceRaysIndirect (RTShaderBindingID sbt, BufferID indirectBuffer, Bytes)										__Th_OV;

		void  TraceRaysIndirect (const RTShaderBindingTable &sbt, RmBufferID indirectBuffer, Bytes)							__Th___;
		void  TraceRaysIndirect (RmRTShaderBindingID sbt, RmBufferID indirectBuffer, Bytes)									__Th___;

		void  TraceRaysIndirect2 (DeviceAddress address)																	__Th_OV;	// Vulkan host
		void  TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)										__Th_OV;
		void  TraceRaysIndirect2 (RmBufferID indirectBuffer, Bytes indirectBufferOffset)									__Th___;

		ND_ RmCommandBufferID	EndCommandBuffer ()																			__Th___	{ return _EndCommandBuffer( ECtxType::RayTracing ); }
		ND_ CmdBuf_t			ReleaseCommandBuffer ()																		__Th___	{ return _ReleaseCommandBuffer( ECtxType::RayTracing ); }

		RBARRIERMNGR_INHERIT_BARRIERS
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
