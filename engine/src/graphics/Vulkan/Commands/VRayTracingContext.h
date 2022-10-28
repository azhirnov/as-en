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

		ND_ VkCommandBuffer	EndCommandBuffer ();
		ND_ VCommandBuffer  ReleaseCommandBuffer ();

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectRayTracingCtx (const VRenderTask &task);
		_VDirectRayTracingCtx (const VRenderTask &task, VCommandBuffer cmdbuf);
		
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
		
		ND_ VBakedCommands		EndCommandBuffer ();
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ();

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectRayTracingCtx (const VRenderTask &task);
		_VIndirectRayTracingCtx (const VRenderTask &task, VSoftwareCmdBufPtr cmdbuf);
		
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
		explicit _VRayTracingContextImpl (const VRenderTask &task) : RawCtx{ task } {}
		
		template <typename RawCmdBufType>
		_VRayTracingContextImpl (const VRenderTask &task, RawCmdBufType cmdbuf) : RawCtx{ task, RVRef(cmdbuf) } {}

		_VRayTracingContextImpl () = delete;
		_VRayTracingContextImpl (const _VRayTracingContextImpl &) = delete;
		
		using RawCtx::BindDescriptorSet;

		void  BindPipeline (RayTracingPipelineID ppln) override final;
		void  BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default) override final;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages) override final;
		//void  SetStackSize () override final;
		
		//void  TraceRays (const uint2 dim, const ShaderBindingTable &sbt) override final;
		//void  TraceRays (const uint3 dim, const ShaderBindingTable &sbt) override final;
		void  TraceRaysIndirect () override final;

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
	

namespace AE::Graphics::_hidden_
{

/*
=================================================
	BindPipeline
=================================================
*/
	template <typename C>
	void  _VRayTracingContextImpl<C>::BindPipeline (RayTracingPipelineID ppln)
	{
		auto*	rt_ppln = this->_mngr.Get( ppln );
		CHECK_ERRV( rt_ppln );

		RawCtx::_BindPipeline( rt_ppln->Handle(), rt_ppln->Layout() );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	template <typename C>
	void  _VRayTracingContextImpl<C>::BindDescriptorSet (uint index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets)
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
	void  _VRayTracingContextImpl<C>::PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		ASSERT( IsAligned( size, sizeof(uint) ));

		RawCtx::_PushConstant( offset, size, values, stages );
	}
	
/*
=================================================
	TraceRays
=================================================
*
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRays (const uint2 dim, const ShaderBindingTable &sbt)
	{
		return TraceRays( uint3{ dim.x, dim.y, 1u }, sbt );
	}

	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRays (const uint3 dim, const ShaderBindingTable &sbt)
	{
		RawCtx::TraceRays( dim, sbt.raygen, sbt.miss, sbt.hit, sbt.callable );
	}

/*
=================================================
	TraceRaysIndirect
=================================================
*/
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRaysIndirect ()
	{
		// TODO
	}
//-----------------------------------------------------------------------------
	

	
/*
=================================================
	TraceRays
=================================================
*/
	inline void  _VDirectRayTracingCtx::TraceRays (const uint3 &dim,
													const VkStridedDeviceAddressRegionKHR &raygen,
													const VkStridedDeviceAddressRegionKHR &miss,
													const VkStridedDeviceAddressRegionKHR &hit,
													const VkStridedDeviceAddressRegionKHR &callable)
	{
		ASSERT( _states.pipeline != Default );

		vkCmdTraceRaysKHR( _cmdbuf.Get(), &raygen, &miss, &hit, &callable, dim.x, dim.y, dim.z );
	}

/*
=================================================
	_BindPipeline
=================================================
*/
	inline void  _VDirectRayTracingCtx::_BindPipeline (VkPipeline ppln, VkPipelineLayout layout)
	{
		if_likely( _states.pipeline == ppln )
			return;

		_states.pipeline	= ppln;
		_states.pplnLayout	= layout;
		vkCmdBindPipeline( _cmdbuf.Get(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, _states.pipeline );
	}
	
/*
=================================================
	BindDescriptorSet
=================================================
*/
	inline void  _VDirectRayTracingCtx::BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets)
	{
		ASSERT( _states.pplnLayout != Default );
		ASSERT( ds != Default );

		vkCmdBindDescriptorSets( _cmdbuf.Get(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, _states.pplnLayout, index, 1, &ds, uint(dynamicOffsets.size()), dynamicOffsets.data() );
	}
	
/*
=================================================
	_PushConstant
=================================================
*/
	inline void  _VDirectRayTracingCtx::_PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		ASSERT( IsAligned( size, sizeof(uint) ));
		ASSERT( _states.pplnLayout != Default );

		vkCmdPushConstants( _cmdbuf.Get(), _states.pplnLayout, VEnumCast(stages), uint(offset), uint(size), values );
	}
//-----------------------------------------------------------------------------
	

	
/*
=================================================
	_BindPipeline
=================================================
*/
	inline void  _VIndirectRayTracingCtx::_BindPipeline (VkPipeline ppln, VkPipelineLayout layout)
	{
		if_likely( _states.pipeline == ppln )
			return;

		_states.pipeline	= ppln;
		_states.pplnLayout	= layout;

		_cmdbuf->BindPipeline( VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, ppln, layout );
	}
	
/*
=================================================
	BindDescriptorSet
=================================================
*/
	inline void  _VIndirectRayTracingCtx::BindDescriptorSet (uint index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets)
	{
		CHECK_ERRV( _states.pplnLayout != Default );

		_cmdbuf->BindDescriptorSet( VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, _states.pplnLayout, index, ds, dynamicOffsets );
	}

/*
=================================================
	_PushConstant
=================================================
*/
	inline void  _VIndirectRayTracingCtx::_PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		CHECK_ERRV( _states.pplnLayout != Default );
		ASSERT( IsAligned( size, sizeof(uint) ));

		_cmdbuf->PushConstant( _states.pplnLayout, offset, size, values, stages );
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
