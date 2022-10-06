// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	RayTracingCtx --> DirectRayTracingCtx   --> BarrierMngr --> Vulkan device 
				  \-> IndirectRayTracingCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics/Vulkan/Commands/VAccumBarriers.h"

namespace AE::Graphics::_hidden_
{

	//
	// Vulkan Direct Ray Tracing Context implementation
	//
	
	class _VDirectRayTracingCtx : public VBaseDirectContext
	{
	// variables
	protected:
		// cached states
		struct {
			VkPipelineLayout	pplnLayout		= Default;
			VkPipeline			pipeline		= Default;
		}					_states;


	// methods
	public:
		void  BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default);

		void  TraceRays (const uint3 &dim,
						 const VkStridedDeviceAddressRegionKHR &raygen,
						 const VkStridedDeviceAddressRegionKHR &miss,
						 const VkStridedDeviceAddressRegionKHR &hit,
						 const VkStridedDeviceAddressRegionKHR &callable);

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectRayTracingCtx (Ptr<VCommandBatch> batch) : VBaseDirectContext{ batch } {}
		_VDirectRayTracingCtx (Ptr<VCommandBatch> batch, VCommandBuffer cmdbuf) : VBaseDirectContext{ batch, RVRef(cmdbuf) } {}
		
		void  _BindPipeline (VkPipeline ppln, VkPipelineLayout layout);
		void  _PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages);
	};



	//
	// Vulkan Indirect Ray Tracing Context implementation
	//
	
	class _VIndirectRayTracingCtx : public VBaseIndirectContext
	{
	// variables
	protected:
		// cached states
		struct {
			VkPipelineLayout	pplnLayout		= Default;
			VkPipeline			pipeline		= Default;
		}					_states;


	// methods
	public:
		void  BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default);

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectRayTracingCtx (Ptr<VCommandBatch> batch) : VBaseIndirectContext{ batch } {}
		_VIndirectRayTracingCtx (Ptr<VCommandBatch> batch, VSoftwareCmdBufPtr cmdbuf) : VBaseIndirectContext{ batch, RVRef(cmdbuf) } {}
		
		void  _BindPipeline (VkPipeline ppln, VkPipelineLayout layout);
		void  _PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages);
	};


	
	//
	// Vulkan Ray Tracing Context implementation
	//

	template <typename CtxImpl>
	class _VRayTracingContextImpl : public CtxImpl, public IRayTracingContext
	{
	// types
	public:
		static constexpr bool	IsRayTracingContext			= true;
		static constexpr bool	IsVulkanRayTracingContext	= true;
	private:
		using RawCtx	= CtxImpl;
		using AccumBar	= VAccumBarriers< _VRayTracingContextImpl< CtxImpl >>;


	// methods
	public:
		explicit _VRayTracingContextImpl (Ptr<VCommandBatch> batch) : RawCtx{ batch } {}
		
		template <typename RawCmdBufType>
		_VRayTracingContextImpl (Ptr<VCommandBatch> batch, RawCmdBufType cmdbuf) : RawCtx{ batch, RVRef(cmdbuf) } {}

		_VRayTracingContextImpl () = delete;
		_VRayTracingContextImpl (const _VRayTracingContextImpl &) = delete;
		
		using RawCtx::BindDescriptorSet;

		void  BindPipeline (RayTracingPipelineID ppln) override;
		void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) override;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages) override;
		//void  StackSize () override;
		
		//void  TraceRays (const uint2 dim, const ShaderBindingTable &sbt) override;
		//void  TraceRays (const uint3 dim, const ShaderBindingTable &sbt) override;
		void  TraceRaysIndirect () override;

		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }
		
		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }

		ND_ AccumBar  AccumBarriers ()							{ return AccumBar{ *this }; }

		VBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectRayTracingContext		= _hidden_::_VRayTracingContextImpl< _hidden_::_VDirectRayTracingCtx >;
	using VIndirectRayTracingContext	= _hidden_::_VRayTracingContextImpl< _hidden_::_VIndirectRayTracingCtx >;

} // AE::Graphics

#endif	// AE_ENABLE_VULKAN
