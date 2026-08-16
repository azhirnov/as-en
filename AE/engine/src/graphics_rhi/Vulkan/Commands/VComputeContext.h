// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	ComputeCtx  --> DirectComputeCtx   --> BarrierMngr --> Vulkan device
				\-> IndirectComputeCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VBakedCommands.h"
# include "graphics_rhi/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics_rhi/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics_rhi/Vulkan/Commands/VAccumBarriers.h"

namespace AE::Graphics::_hidden_
{

	//
	// Vulkan Direct Compute Context implementation
	//

	class _VDirectComputeCtx : public VBaseDirectContext
	{
	// types
	private:
		using Validator_t	= ComputeContextValidation;


	// variables
	protected:
		struct {
			VkPipelineLayout	pplnLayout	= Default;
		}					_states;

		static constexpr VkPipelineBindPoint	_bindPoint	= VK_PIPELINE_BIND_POINT_COMPUTE;


	// methods
	public:
		void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;
		void  DispatchIndirect (VkBuffer buffer, Bytes offset)														__Th___;

		void  PreprocessGeneratedCommands (const VkGeneratedCommandsInfoEXT &, VkCommandBuffer)						__Th___;
		void  ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &, bool isPreprocessed)					__Th___;

		ND_ VkCommandBuffer	EndCommandBuffer ()																		__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()																	__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		_VDirectComputeCtx (RenderCoroRef task, VCommandBuffer cmdbuf, DebugLabel dbg)								__Th___ : VBaseDirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::Compute } {}

		void  _Dispatch (const uint3 &groupCount)																	__Th___;
		void  _DispatchBase (const uint3 &baseGroup, const uint3 &groupCount)										__Th___;
		void  _BindComputePipeline (VkPipeline ppln, VkPipelineLayout layout)										__NE___;
		void  _PushComputeConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages)				__Th___;
	};



	//
	// Vulkan Indirect Compute Context implementation
	//

	class _VIndirectComputeCtx : public VBaseIndirectContext
	{
	// types
	private:
		using Validator_t	= ComputeContextValidation;


	// variables
	private:
		struct {
			VkPipelineLayout	pplnLayout	= Default;
		}					_states;

		static constexpr VkPipelineBindPoint	_bindPoint	= VK_PIPELINE_BIND_POINT_COMPUTE;


	// methods
	public:
		void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;
		void  DispatchIndirect (VkBuffer buffer, Bytes offset)														__Th___;

		void  PreprocessGeneratedCommands (const VkGeneratedCommandsInfoEXT &, VkCommandBuffer)						__Th___;
		void  ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &, bool isPreprocessed)					__Th___;

		ND_ VBakedCommands		EndCommandBuffer ()																	__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()																__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		_VIndirectComputeCtx (RenderCoroRef task, VSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)						__Th___ : VBaseIndirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::Compute } {}

		void  _Dispatch (const uint3 &groupCount)																	__Th___;
		void  _DispatchBase (const uint3 &baseGroup, const uint3 &groupCount)										__Th___;
		void  _BindComputePipeline (VkPipeline ppln, VkPipelineLayout layout)										__Th___;
		void  _PushComputeConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages)				__Th___;
	};



	//
	// Vulkan Compute Context implementation
	//

	template <typename CtxImpl>
	class _VComputeContextImpl final : public CtxImpl, public IComputeContextVk
	{
	// types
	public:
		using CmdBuf_t		= typename CtxImpl::CmdBuf_t;
		using RenderCoroRef	= typename CtxImpl::RenderCoroRef;
	private:
		static constexpr uint	_LocalArraySize		= 16;

		using RawCtx		= CtxImpl;
		using AccumBar		= AccumBarriers< _VComputeContextImpl< CtxImpl >>;
		using DeferredBar	= AccumDeferredBarriersForCtx< _VComputeContextImpl< CtxImpl >>;
		using Validator_t	= ComputeContextValidation;


	// methods
	public:
		explicit _VComputeContextImpl (RenderCoroRef task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)				__Th___;

		_VComputeContextImpl ()																								= delete;
		_VComputeContextImpl (const _VComputeContextImpl &)																	= delete;

		using RawCtx::BindDescriptorSet;

		void  BindPipeline (ComputePipelineID ppln)																			__Th_OV;
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)		__Th_OV;

		void  PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName)	__Th_OV;
		using IComputeContext::PushConstant;

		using IComputeContext::Dispatch;
		void  Dispatch (const uint3 &groupCount)																			__Th_OV	{ RawCtx::_Dispatch( groupCount ); }

		void  DispatchBase (const uint3 &baseGroup, const uint3 &groupCount)												__Th___	{ RawCtx::_DispatchBase( baseGroup, groupCount ); }
		void  DispatchBase (const uint2 &baseGroup, const uint2 &groupCount)												__Th___	{ return DispatchBase( uint3{ baseGroup, 0u }, uint3{ groupCount, 1u }); }

		using RawCtx::DispatchIndirect;

		void  DispatchIndirect (BufferID buffer, Bytes offset)																__Th_OV;

		// indirect commands //
		void  PreprocessGeneratedCommands (const PreprocessGeneratedCommandsCmd &)											__Th_OV;
		void  PreprocessGeneratedCommands (const PreprocessGeneratedCommands2Cmd &)											__Th_OV;

		void  BindInitialPipeline (IndirectExecutionSetID)																	__Th_OV;

		void  ExecuteGeneratedCommands (const ExecuteGeneratedCommandsCmd &)												__Th_OV;
		void  ExecuteGeneratedCommands (const ExecuteGeneratedCommands2Cmd &)												__Th_OV;

		VBARRIERMNGR_INHERIT_BARRIERS
	};

	extern template class _VComputeContextImpl< _VDirectComputeCtx >;
	extern template class _VComputeContextImpl< _VIndirectComputeCtx >;

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectComputeContext		= Graphics::_hidden_::_VComputeContextImpl< Graphics::_hidden_::_VDirectComputeCtx >;
	using VIndirectComputeContext	= Graphics::_hidden_::_VComputeContextImpl< Graphics::_hidden_::_VIndirectComputeCtx >;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
