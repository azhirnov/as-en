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
			VkPipelineLayout	pplnLayout	= Default;
		}					_states;
		
		static constexpr VkPipelineBindPoint	_bindPoint	= VK_PIPELINE_BIND_POINT_COMPUTE;


	// methods
	public:
		void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;
		void  DispatchIndirect (VkBuffer buffer, Bytes offset)														__Th___;
		
		ND_ VkCommandBuffer	EndCommandBuffer ()																		__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()																	__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectComputeCtx (const RenderTask &task)														__Th___;
		_VDirectComputeCtx (const RenderTask &task, VCommandBuffer cmdbuf)											__Th___;

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
			VkPipelineLayout	pplnLayout	= Default;
		}					_states;
		
		static constexpr VkPipelineBindPoint	_bindPoint	= VK_PIPELINE_BIND_POINT_COMPUTE;


	// methods
	public:
		void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;
		void  DispatchIndirect (VkBuffer buffer, Bytes offset)														__Th___;
		
		ND_ VBakedCommands		EndCommandBuffer ()																	__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()																__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectComputeCtx (const RenderTask &task)														__Th___;
		_VIndirectComputeCtx (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf)									__Th___;

		void  _Dispatch (const uint3 &groupCount);
		void  _DispatchBase (const uint3 &baseGroup, const uint3 &groupCount);
		void  _BindComputePipeline (VkPipeline ppln, VkPipelineLayout layout);
		void  _PushComputeConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages);
	};



	//
	// Vulkan Compute Context implementation
	//
	
	template <typename CtxImpl>
	class _VComputeContextImpl final : public CtxImpl, public IComputeContext
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
		explicit _VComputeContextImpl (const RenderTask &task)														__Th___;

		template <typename RawCmdBufType>
		_VComputeContextImpl (const RenderTask &task, RawCmdBufType cmdbuf)											__Th___;

		_VComputeContextImpl ()																						= delete;
		_VComputeContextImpl (const _VComputeContextImpl &)															= delete;
		~_VComputeContextImpl ()																					__NE_OV {}

		using RawCtx::BindDescriptorSet;

		void  BindPipeline (ComputePipelineID ppln)																	__Th_OV;
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)__Th_OV;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)						__Th_OV;
		
		using IComputeContext::Dispatch;
		void  Dispatch (const uint3 &groupCount)																	__Th_OV	{ RawCtx::_Dispatch( groupCount ); }
		
		void  DispatchBase (const uint3 &baseGroup, const uint3 &groupCount)										__Th___	{ RawCtx::_DispatchBase( baseGroup, groupCount ); }
		void  DispatchBase (const uint2 &baseGroup, const uint2 &groupCount)										__Th___	{ return DispatchBase( uint3{ baseGroup, 0u }, uint3{ groupCount, 1u }); }
		
		using RawCtx::DispatchIndirect;

		void  DispatchIndirect (BufferID buffer, Bytes offset)														__Th_OV;

		VBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectComputeContext		= Graphics::_hidden_::_VComputeContextImpl< Graphics::_hidden_::_VDirectComputeCtx >;
	using VIndirectComputeContext	= Graphics::_hidden_::_VComputeContextImpl< Graphics::_hidden_::_VIndirectComputeCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_VComputeContextImpl<C>::_VComputeContextImpl (const RenderTask &task) : RawCtx{ task }
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute, task.GetQueueMask() ));
	}
		
	template <typename C>
	template <typename RawCmdBufType>
	_VComputeContextImpl<C>::_VComputeContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) :
		RawCtx{ task, RVRef(cmdbuf) }
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute, task.GetQueueMask() ));
	}

/*
=================================================
	BindPipeline
=================================================
*/
	template <typename C>
	void  _VComputeContextImpl<C>::BindPipeline (ComputePipelineID ppln)
	{
		auto&	cppln = _GetResourcesOrThrow( ppln );

		RawCtx::_BindComputePipeline( cppln.Handle(), cppln.Layout() );
	}
	
/*
=================================================
	BindDescriptorSet
=================================================
*/
	template <typename C>
	void  _VComputeContextImpl<C>::BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets)
	{
		auto&	desc_set = _GetResourcesOrThrow( ds );

		RawCtx::BindDescriptorSet( index, desc_set.Handle(), dynamicOffsets );
	}

/*
=================================================
	PushConstant
=================================================
*/
	template <typename C>
	void  _VComputeContextImpl<C>::PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		RawCtx::_PushComputeConstant( offset, size, values, stages );
	}
	
/*
=================================================
	DispatchIndirect
=================================================
*/
	template <typename C>
	void  _VComputeContextImpl<C>::DispatchIndirect (BufferID bufferid, Bytes offset)
	{
		auto&	buf = _GetResourcesOrThrow( bufferid );
		ASSERT( buf.Size() >= offset + sizeof(DispatchIndirectCommand) );

		RawCtx::DispatchIndirect( buf.Handle(), offset );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	BindDescriptorSet
=================================================
*/
	inline void  _VDirectComputeCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets)
	{
		ASSERT( _states.pplnLayout != Default );
		ASSERT( ds != Default );

		vkCmdBindDescriptorSets( _cmdbuf.Get(), _bindPoint, _states.pplnLayout, index.vkIndex, 1, &ds, uint(dynamicOffsets.size()), dynamicOffsets.data() );
	}
	
/*
=================================================
	_BindComputePipeline
=================================================
*/
	inline void  _VDirectComputeCtx::_BindComputePipeline (VkPipeline ppln, VkPipelineLayout layout)
	{
		_states.pplnLayout = layout;
		vkCmdBindPipeline( _cmdbuf.Get(), _bindPoint, ppln );
	}
	
/*
=================================================
	_Dispatch
=================================================
*/
	inline void  _VDirectComputeCtx::_Dispatch (const uint3 &groupCount)
	{
		ASSERT( All( groupCount >= 1u ));
		ASSERT( _NoPendingBarriers() );

		vkCmdDispatch( _cmdbuf.Get(), groupCount.x, groupCount.y, groupCount.z );
	}
	
/*
=================================================
	DispatchIndirect
=================================================
*/
	inline void  _VDirectComputeCtx::DispatchIndirect (VkBuffer buffer, Bytes offset)
	{
		ASSERT( buffer != Default );
		ASSERT( _NoPendingBarriers() );

		vkCmdDispatchIndirect( _cmdbuf.Get(), buffer, VkDeviceSize(offset) );
	}
	
/*
=================================================
	_DispatchBase
=================================================
*/
	inline void  _VDirectComputeCtx::_DispatchBase (const uint3 &baseGroup, const uint3 &groupCount)
	{
		ASSERT( _GetExtensions().deviceGroup );
		ASSERT( All( groupCount >= 1u ));
		ASSERT( _NoPendingBarriers() );

		vkCmdDispatchBaseKHR( _cmdbuf.Get(), baseGroup.x, baseGroup.y, baseGroup.z, groupCount.x, groupCount.y, groupCount.z );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	BindDescriptorSet
=================================================
*/
	inline void  _VIndirectComputeCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets)
	{
		ASSERT( _states.pplnLayout != Default );

		_cmdbuf->BindDescriptorSet( _bindPoint, _states.pplnLayout, index.vkIndex, ds, dynamicOffsets );
	}
	
/*
=================================================
	_BindComputePipeline
=================================================
*/
	inline void  _VIndirectComputeCtx::_BindComputePipeline (VkPipeline ppln, VkPipelineLayout layout)
	{
		_states.pplnLayout = layout;
		
		_cmdbuf->BindPipeline( _bindPoint, ppln, layout );
	}
	
/*
=================================================
	_PushComputeConstant
=================================================
*/
	inline void  _VIndirectComputeCtx::_PushComputeConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		ASSERT( _states.pplnLayout != Default );

		_cmdbuf->PushConstant( _states.pplnLayout, offset, size, values, stages );
	}
	
/*
=================================================
	_Dispatch
=================================================
*/
	inline void  _VIndirectComputeCtx::_Dispatch (const uint3 &groupCount)
	{
		ASSERT( All( groupCount >= 1u ));
		
		auto&	cmd = _cmdbuf->CreateCmd< DispatchCmd >();	// throw
		MemCopy( OUT cmd.groupCount, &groupCount, Sizeof( cmd.groupCount ));
	}
	
/*
=================================================
	DispatchIndirect
=================================================
*/
	inline void  _VIndirectComputeCtx::DispatchIndirect (VkBuffer buffer, Bytes offset)
	{
		ASSERT( buffer != Default );
		
		auto&	cmd = _cmdbuf->CreateCmd< DispatchIndirectCmd >();	// throw
		cmd.buffer	= buffer;
		cmd.offset	= offset;
	}
	
/*
=================================================
	_DispatchBase
=================================================
*/
	inline void  _VIndirectComputeCtx::_DispatchBase (const uint3 &baseGroup, const uint3 &groupCount)
	{
		ASSERT( _GetExtensions().deviceGroup );
		ASSERT( All( groupCount >= 1u ));
		
		auto&	cmd = _cmdbuf->CreateCmd< DispatchBaseCmd >();	// throw
		MemCopy( OUT cmd.baseGroup,  &baseGroup,  Sizeof( cmd.baseGroup ));
		MemCopy( OUT cmd.groupCount, &groupCount, Sizeof( cmd.groupCount ));
	}

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
