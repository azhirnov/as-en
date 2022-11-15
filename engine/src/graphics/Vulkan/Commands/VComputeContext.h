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
		void  BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default);
		void  DispatchIndirect (VkBuffer buffer, Bytes offset);
		
		ND_ VkCommandBuffer	EndCommandBuffer ();
		ND_ VCommandBuffer  ReleaseCommandBuffer ();

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectComputeCtx (const RenderTask &task);
		_VDirectComputeCtx (const RenderTask &task, VCommandBuffer cmdbuf);

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
		void  BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default);
		void  DispatchIndirect (VkBuffer buffer, Bytes offset);
		
		ND_ VBakedCommands		EndCommandBuffer ();
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ();

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectComputeCtx (const RenderTask &task);
		_VIndirectComputeCtx (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf);

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
		explicit _VComputeContextImpl (const RenderTask &task) : RawCtx{ task } {}

		template <typename RawCmdBufType>
		_VComputeContextImpl (const RenderTask &task, RawCmdBufType cmdbuf) : RawCtx{ task, RVRef(cmdbuf) } {}

		_VComputeContextImpl () = delete;
		_VComputeContextImpl (const _VComputeContextImpl &) = delete;
		~_VComputeContextImpl () __NE_OV {}

		using RawCtx::BindDescriptorSet;

		void  BindPipeline (ComputePipelineID ppln)															override;
		void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)	override;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)				override;
		
		using IComputeContext::Dispatch;
		void  Dispatch (const uint3 &groupCount)															override	{ RawCtx::_Dispatch( groupCount ); }
		
		void  DispatchBase (const uint3 &baseGroup, const uint3 &groupCount)											{ RawCtx::_DispatchBase( baseGroup, groupCount ); }
		void  DispatchBase (const uint2 &baseGroup, const uint2 &groupCount)											{ return DispatchBase( uint3{ baseGroup, 0u }, uint3{ groupCount, 1u }); }
		
		using RawCtx::DispatchIndirect;

		void  DispatchIndirect (BufferID buffer, Bytes offset)												override;
		
		void  DebugMarker (NtStringView text, RGBA8u color)													override	{ RawCtx::_DebugMarker( text, color ); }
		void  PushDebugGroup (NtStringView text, RGBA8u color)												override	{ RawCtx::_PushDebugGroup( text, color ); }
		void  PopDebugGroup ()																				override	{ RawCtx::_PopDebugGroup(); }

		VBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectComputeContext		= _hidden_::_VComputeContextImpl< _hidden_::_VDirectComputeCtx >;
	using VIndirectComputeContext	= _hidden_::_VComputeContextImpl< _hidden_::_VIndirectComputeCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{

/*
=================================================
	BindPipeline
=================================================
*/
	template <typename C>
	void  _VComputeContextImpl<C>::BindPipeline (ComputePipelineID ppln)
	{
		auto*	cppln = this->_mngr.Get( ppln );
		CHECK_ERRV( cppln );

		RawCtx::_BindComputePipeline( cppln->Handle(), cppln->Layout() );
	}
	
/*
=================================================
	BindDescriptorSet
=================================================
*/
	template <typename C>
	void  _VComputeContextImpl<C>::BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets)
	{
		auto*	desc_set = this->_mngr.Get( ds );
		CHECK_ERRV( desc_set );

		RawCtx::BindDescriptorSet( index, desc_set->Handle(), dynamicOffsets );
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
		auto*	buf = this->_mngr.Get( bufferid );
		CHECK_ERRV( buf );
		ASSERT( buf->Size() >= offset + sizeof(DispatchIndirectCommand) );

		RawCtx::DispatchIndirect( buf->Handle(), offset );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	BindDescriptorSet
=================================================
*/
	inline void  _VDirectComputeCtx::BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets)
	{
		ASSERT( _states.pplnLayout != Default );
		ASSERT( ds != Default );

		this->vkCmdBindDescriptorSets( this->_cmdbuf.Get(), _bindPoint, _states.pplnLayout, index, 1, &ds, uint(dynamicOffsets.size()), dynamicOffsets.data() );
	}
	
/*
=================================================
	_BindComputePipeline
=================================================
*/
	inline void  _VDirectComputeCtx::_BindComputePipeline (VkPipeline ppln, VkPipelineLayout layout)
	{
		_states.pplnLayout = layout;
		this->vkCmdBindPipeline( this->_cmdbuf.Get(), _bindPoint, ppln );
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

		this->vkCmdDispatch( this->_cmdbuf.Get(), groupCount.x, groupCount.y, groupCount.z );
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

		this->vkCmdDispatchIndirect( this->_cmdbuf.Get(), buffer, VkDeviceSize(offset) );
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

		this->vkCmdDispatchBaseKHR( this->_cmdbuf.Get(), baseGroup.x, baseGroup.y, baseGroup.z, groupCount.x, groupCount.y, groupCount.z );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	BindDescriptorSet
=================================================
*/
	inline void  _VIndirectComputeCtx::BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets)
	{
		ASSERT( _states.pplnLayout != Default );

		_cmdbuf->BindDescriptorSet( _bindPoint, _states.pplnLayout, index, ds, dynamicOffsets );
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
