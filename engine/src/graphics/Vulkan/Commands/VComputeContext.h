// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	ComputeCtx  --> DirectComputeCtx   --> BarrierMngr --> Vulkan device 
				\-> IndirectComputeCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VBakedCommands.h"
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics/Vulkan/Commands/VAccumBarriers.h"

namespace AE::Graphics::_hidden_
{
	
	//
	// Vulkan Direct Compute Context implementation
	//

	class _VDirectComputeCtx : public VBaseDirectContext
	{
	// variables
	protected:
		struct {
			static constexpr VkPipelineBindPoint	bindPoint		= VK_PIPELINE_BIND_POINT_COMPUTE;

							 VkPipelineLayout		pplnLayout		= Default;
		}										_states;


	// methods
	public:
		void  BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default);
		void  DispatchIndirect (VkBuffer buffer, Bytes offset);

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectComputeCtx (Ptr<VCommandBatch> batch) : VBaseDirectContext{ batch } {}
		_VDirectComputeCtx (Ptr<VCommandBatch> batch, VCommandBuffer cmdbuf) : VBaseDirectContext{ batch, RVRef(cmdbuf) } {}

		void  _Dispatch (const uint3 &groupCount);
		void  _DispatchBase (const uint3 &baseGroup, const uint3 &groupCount);
		void  _BindComputePipeline (VkPipeline ppln, VkPipelineLayout layout);
		void  _PushComputeConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages);
	};



	//
	// Vulkan Indirect Compute Context implementation
	//
	
	class _VIndirectComputeCtx : public VBaseIndirectContext
	{
	// variables
	private:
		struct {
			static constexpr VkPipelineBindPoint	bindPoint		= VK_PIPELINE_BIND_POINT_COMPUTE;

							 VkPipelineLayout		pplnLayout		= Default;
		}										_states;


	// methods
	public:
		void  BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default);
		void  DispatchIndirect (VkBuffer buffer, Bytes offset);
		
		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectComputeCtx (Ptr<VCommandBatch> batch) : VBaseIndirectContext{ batch } {}
		_VIndirectComputeCtx (Ptr<VCommandBatch> batch, VSoftwareCmdBufPtr cmdbuf) : VBaseIndirectContext{ batch, RVRef(cmdbuf) } {}

		void  _Dispatch (const uint3 &groupCount);
		void  _DispatchBase (const uint3 &baseGroup, const uint3 &groupCount);
		void  _BindComputePipeline (VkPipeline ppln, VkPipelineLayout layout);
		void  _PushComputeConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages);
	};



	//
	// Vulkan Compute Context implementation
	//
	
	template <typename CtxImpl>
	class _VComputeContextImpl : public CtxImpl, public IComputeContext
	{
	// types
	public:
		static constexpr bool	IsComputeContext		= true;
		static constexpr bool	IsVulkanComputeContext	= true;
	protected:
		static constexpr uint	_LocalArraySize			= 16;
	private:
		using RawCtx	= CtxImpl;
		using AccumBar	= VAccumBarriers< _VComputeContextImpl< CtxImpl >>;


	// methods
	public:
		explicit _VComputeContextImpl (Ptr<VCommandBatch> batch) : RawCtx{ batch } {}

		template <typename RawCmdBufType>
		_VComputeContextImpl (Ptr<VCommandBatch> batch, RawCmdBufType cmdbuf) : RawCtx{ batch, RVRef(cmdbuf) } {}

		_VComputeContextImpl () = delete;
		_VComputeContextImpl (const _VComputeContextImpl &) = delete;
		~_VComputeContextImpl () override {}

		using RawCtx::BindDescriptorSet;

		void  BindPipeline (ComputePipelineID ppln) override final;
		void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) override final;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages) override final;
		
		void  Dispatch (const uint3 &groupCount)								override final	{ RawCtx::_Dispatch( groupCount ); }
		void  DispatchBase (const uint3 &baseGroup, const uint3 &groupCount)	override final	{ RawCtx::_DispatchBase( baseGroup, groupCount ); }

		using RawCtx::DispatchIndirect;
		using IComputeContext::Dispatch;
		using IComputeContext::DispatchBase;

		void  DispatchIndirect (BufferID buffer, Bytes offset)	override final;
		
		void  CommitBarriers ()									override final	{ RawCtx::_CommitBarriers(); }
		
		void  DebugMarker (NtStringView text, RGBA8u color)		override final	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)	override final	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()									override final	{ RawCtx::_PopDebugGroup(); }
		
		ND_ AccumBar  AccumBarriers ()											{ return AccumBar{ *this }; }

		VBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectComputeContext		= _hidden_::_VComputeContextImpl< _hidden_::_VDirectComputeCtx >;
	using VIndirectComputeContext	= _hidden_::_VComputeContextImpl< _hidden_::_VIndirectComputeCtx >;

} // AE::Graphics

#endif // AE_ENABLE_VULKAN
