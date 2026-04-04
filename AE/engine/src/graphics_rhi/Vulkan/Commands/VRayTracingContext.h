// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	RayTracingCtx --> DirectRayTracingCtx   --> BarrierMngr --> Vulkan device
				  \-> IndirectRayTracingCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics_rhi/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics_rhi/Vulkan/Commands/VAccumBarriers.h"

namespace AE::Graphics::_hidden_
{

	//
	// Vulkan Direct Ray Tracing Context implementation
	//

	class _VDirectRayTracingCtx : public VBaseDirectContext
	{
	// types
	private:
		using Validator_t	= RayTracingContextValidation;


	// variables
	protected:
		// cached states
		struct {
			VkPipelineLayout	pplnLayout	= Default;
		}					_states;


	// methods
	public:
		void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;

		void  TraceRays (const uint3 &dim,
						 const VkStridedDeviceAddressRegionKHR &raygen,
						 const VkStridedDeviceAddressRegionKHR &miss,
						 const VkStridedDeviceAddressRegionKHR &hit,
						 const VkStridedDeviceAddressRegionKHR &callable)							__Th___;

		void  PreprocessGeneratedCommands (const VkGeneratedCommandsInfoEXT &, VkCommandBuffer)		__Th___;
		void  ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &, bool isPreprocessed)	__Th___;

		ND_ VkCommandBuffer	EndCommandBuffer ()														__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()													__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		_VDirectRayTracingCtx (RenderCoroRef task, VCommandBuffer cmdbuf, DebugLabel dbg)			__Th___ : VBaseDirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::RayTracing } {}

		void  _BindPipeline (VkPipeline ppln, VkPipelineLayout layout)								__Th___;
		void  _PushConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages)	__Th___;
		void  _SetStackSize (Bytes size)															__Th___;

		void  _TraceRaysIndirect2 (VkDeviceAddress indirectDeviceAddress)							__Th___;
		void  _TraceRaysIndirect (const VkStridedDeviceAddressRegionKHR &raygen,
								  const VkStridedDeviceAddressRegionKHR &miss,
								  const VkStridedDeviceAddressRegionKHR &hit,
								  const VkStridedDeviceAddressRegionKHR &callable,
								  VkDeviceAddress						indirectDeviceAddress)		__Th___;
	};



	//
	// Vulkan Indirect Ray Tracing Context implementation
	//

	class _VIndirectRayTracingCtx : public VBaseIndirectContext
	{
	// types
	private:
		using Validator_t	= RayTracingContextValidation;


	// variables
	protected:
		// cached states
		struct {
			VkPipelineLayout	pplnLayout	= Default;
		}					_states;


	// methods
	public:
		void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;

		void  TraceRays (const uint3 &dim,
						 const VkStridedDeviceAddressRegionKHR &raygen,
						 const VkStridedDeviceAddressRegionKHR &miss,
						 const VkStridedDeviceAddressRegionKHR &hit,
						 const VkStridedDeviceAddressRegionKHR &callable)							__Th___;

		void  PreprocessGeneratedCommands (const VkGeneratedCommandsInfoEXT &, VkCommandBuffer)		__Th___;
		void  ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &, bool isPreprocessed)	__Th___;

		ND_ VBakedCommands		EndCommandBuffer ()													__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()												__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		_VIndirectRayTracingCtx (RenderCoroRef task, VSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)		__Th___ : VBaseIndirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::RayTracing } {}

		void  _BindPipeline (VkPipeline ppln, VkPipelineLayout layout)								__Th___;
		void  _PushConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages)	__Th___;
		void  _SetStackSize (Bytes size)															__Th___;

		void  _TraceRaysIndirect2 (VkDeviceAddress indirectDeviceAddress)							__Th___;
		void  _TraceRaysIndirect (const VkStridedDeviceAddressRegionKHR &raygen,
								  const VkStridedDeviceAddressRegionKHR &miss,
								  const VkStridedDeviceAddressRegionKHR &hit,
								  const VkStridedDeviceAddressRegionKHR &callable,
								  VkDeviceAddress						indirectDeviceAddress)		__Th___;
	};



	//
	// Vulkan Ray Tracing Context implementation
	//

	template <typename CtxImpl>
	class _VRayTracingContextImpl final : public CtxImpl, public IRayTracingContextVk
	{
	// types
	public:
		using CmdBuf_t		= typename CtxImpl::CmdBuf_t;
		using RenderCoroRef	= typename CtxImpl::RenderCoroRef;
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= AccumBarriers< _VRayTracingContextImpl< CtxImpl >>;
		using DeferredBar	= AccumDeferredBarriersForCtx< _VRayTracingContextImpl< CtxImpl >>;
		using Validator_t	= RayTracingContextValidation;


	// methods
	public:
		explicit _VRayTracingContextImpl (RenderCoroRef task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)			__Th___;

		_VRayTracingContextImpl ()																							= delete;
		_VRayTracingContextImpl (const _VRayTracingContextImpl &)															= delete;

		using RawCtx::BindDescriptorSet;

		void  BindPipeline (RayTracingPipelineID ppln)																		__Th_OV;
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th_OV;

		void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th_OV;
		using IRayTracingContext::PushConstant;

		void  SetStackSize (Bytes size)																						__Th_OV	{ RawCtx::_SetStackSize( size ); }

		void  TraceRays (const uint2 dim, const RTShaderBindingTable &sbt)													__Th_OV;
		void  TraceRays (const uint3 dim, const RTShaderBindingTable &sbt)													__Th_OV;

		void  TraceRays (const uint2 dim, RTShaderBindingID sbt)															__Th_OV;
		void  TraceRays (const uint3 dim, RTShaderBindingID sbt)															__Th_OV;

		void  TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes)							__Th_OV;
		void  TraceRaysIndirect (RTShaderBindingID sbt, BufferID indirectBuffer, Bytes)										__Th_OV;

		void  TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)										__Th_OV;

		void  TraceRaysIndirectAddress (const RTShaderBindingTable &sbt, DeviceAddress address)								__Th_OV;
		void  TraceRaysIndirectAddress2 (DeviceAddress address)																__Th_OV;

		// indirect commands //
		void  PreprocessGeneratedCommands (const PreprocessGeneratedCommandsCmd &)											__Th_OV;
		void  PreprocessGeneratedCommands (const PreprocessGeneratedCommands2Cmd &)											__Th_OV;

		void  BindInitialPipeline (IndirectExecutionSetID)																	__Th_OV;

		void  ExecuteGeneratedCommands (const ExecuteGeneratedCommandsCmd &)												__Th_OV;
		void  ExecuteGeneratedCommands (const ExecuteGeneratedCommands2Cmd &)												__Th_OV;

		VBARRIERMNGR_INHERIT_BARRIERS
	};

    extern template class _VRayTracingContextImpl< _VDirectRayTracingCtx >;
    extern template class _VRayTracingContextImpl< _VIndirectRayTracingCtx >;

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectRayTracingContext		= Graphics::_hidden_::_VRayTracingContextImpl< Graphics::_hidden_::_VDirectRayTracingCtx >;
	using VIndirectRayTracingContext	= Graphics::_hidden_::_VRayTracingContextImpl< Graphics::_hidden_::_VIndirectRayTracingCtx >;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
