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
		void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;

		void  TraceRays (const uint3 &dim,
						 const VkStridedDeviceAddressRegionKHR &raygen,
						 const VkStridedDeviceAddressRegionKHR &miss,
						 const VkStridedDeviceAddressRegionKHR &hit,
						 const VkStridedDeviceAddressRegionKHR &callable)											__Th___;

		ND_ VkCommandBuffer	EndCommandBuffer ()																		__Th___;
		ND_ VCommandBuffer  ReleaseCommandBuffer ()																	__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VDirectRayTracingCtx (const RenderTask &task)														__Th___ : VBaseDirectContext{ task, ECtxType::RayTracing } {}
		_VDirectRayTracingCtx (const RenderTask &task, VCommandBuffer cmdbuf)										__Th___ : VBaseDirectContext{ task, RVRef(cmdbuf), ECtxType::RayTracing } {}
		
		void  _BindPipeline (VkPipeline ppln, VkPipelineLayout layout);
		void  _PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages);
		void  _SetStackSize (Bytes size);
		
		void  _TraceRaysIndirect2 (VkDeviceAddress indirectDeviceAddress);
		void  _TraceRaysIndirect (const VkStridedDeviceAddressRegionKHR &raygen,
								  const VkStridedDeviceAddressRegionKHR &miss,
								  const VkStridedDeviceAddressRegionKHR &hit,
								  const VkStridedDeviceAddressRegionKHR &callable,
								  VkDeviceAddress						indirectDeviceAddress);
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
		void  BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets = Default)__Th___;
		
		void  TraceRays (const uint3 &dim,
						 const VkStridedDeviceAddressRegionKHR &raygen,
						 const VkStridedDeviceAddressRegionKHR &miss,
						 const VkStridedDeviceAddressRegionKHR &hit,
						 const VkStridedDeviceAddressRegionKHR &callable)											__Th___;

		ND_ VBakedCommands		EndCommandBuffer ()																	__Th___;
		ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()																__Th___;

		VBARRIERMNGR_INHERIT_VKBARRIERS

	protected:
		explicit _VIndirectRayTracingCtx (const RenderTask &task)													__Th___ : VBaseIndirectContext{ task, ECtxType::RayTracing } {}
		_VIndirectRayTracingCtx (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf)									__Th___ : VBaseIndirectContext{ task, RVRef(cmdbuf), ECtxType::RayTracing } {}
		
		void  _BindPipeline (VkPipeline ppln, VkPipelineLayout layout);
		void  _PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages);
		void  _SetStackSize (Bytes size);
		
		void  _TraceRaysIndirect2 (VkDeviceAddress indirectDeviceAddress);
		void  _TraceRaysIndirect (const VkStridedDeviceAddressRegionKHR &raygen,
								  const VkStridedDeviceAddressRegionKHR &miss,
								  const VkStridedDeviceAddressRegionKHR &hit,
								  const VkStridedDeviceAddressRegionKHR &callable,
								  VkDeviceAddress						indirectDeviceAddress);
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

		using CmdBuf_t		= typename CtxImpl::CmdBuf_t;
	private:
		using RawCtx		= CtxImpl;
		using AccumBar		= VAccumBarriers< _VRayTracingContextImpl< CtxImpl >>;
		using DeferredBar	= VAccumDeferredBarriersForCtx< _VRayTracingContextImpl< CtxImpl >>;
		using Validator_t	= RayTracingContextValidation;


	// methods
	public:
		explicit _VRayTracingContextImpl (const RenderTask &task)													__Th___;
		_VRayTracingContextImpl (const RenderTask &task, CmdBuf_t cmdbuf)											__Th___;

		_VRayTracingContextImpl ()																					= delete;
		_VRayTracingContextImpl (const _VRayTracingContextImpl &)													= delete;
		
		using RawCtx::BindDescriptorSet;

		void  BindPipeline (RayTracingPipelineID ppln)																__Th_OV;
		void  BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets = Default)__Th_OV;
		void  PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)						__Th_OV;
		void  SetStackSize (Bytes size)																				__Th_OV	{ RawCtx::_SetStackSize( size ); }
		
		void  TraceRays (const uint2 dim, const RTShaderBindingTable &sbt)											__Th_OV;
		void  TraceRays (const uint3 dim, const RTShaderBindingTable &sbt)											__Th_OV;
		
		void  TraceRays (const uint2 dim, RTShaderBindingID sbt)													__Th_OV;
		void  TraceRays (const uint3 dim, RTShaderBindingID sbt)													__Th_OV;

		void  TraceRaysIndirect (const RTShaderBindingTable &sbt, VDeviceAddress address)							__Th___;
		void  TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes)					__Th_OV;
		void  TraceRaysIndirect (RTShaderBindingID sbt, BufferID indirectBuffer, Bytes)								__Th_OV;

		void  TraceRaysIndirect2 (VDeviceAddress address)															__Th___;
		void  TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)								__Th_OV;

		VBARRIERMNGR_INHERIT_BARRIERS
	};

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
	using VDirectRayTracingContext		= Graphics::_hidden_::_VRayTracingContextImpl< Graphics::_hidden_::_VDirectRayTracingCtx >;
	using VIndirectRayTracingContext	= Graphics::_hidden_::_VRayTracingContextImpl< Graphics::_hidden_::_VIndirectRayTracingCtx >;

} // AE::Graphics
	

namespace AE::Graphics::_hidden_
{
/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_VRayTracingContextImpl<C>::_VRayTracingContextImpl (const RenderTask &task) : RawCtx{ task }
	{
		CHECK_THROW( AnyBits( EQueueMask::Graphics | EQueueMask::AsyncCompute, task.GetQueueMask() ));
	}
		
	template <typename C>
	_VRayTracingContextImpl<C>::_VRayTracingContextImpl (const RenderTask &task, CmdBuf_t cmdbuf) :
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
	void  _VRayTracingContextImpl<C>::BindPipeline (RayTracingPipelineID ppln)
	{
		auto&	rt_ppln = _GetResourcesOrThrow( ppln );

		RawCtx::_BindPipeline( rt_ppln.Handle(), rt_ppln.Layout() );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	template <typename C>
	void  _VRayTracingContextImpl<C>::BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets)
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
	void  _VRayTracingContextImpl<C>::PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		Validator_t::PushConstant( size );

		RawCtx::_PushConstant( offset, size, values, stages );
	}

/*
=================================================
	TraceRays
=================================================
*/
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRays (const uint2 dim, const RTShaderBindingTable &sbt)
	{
		return TraceRays( uint3{ dim.x, dim.y, 1u }, sbt );
	}

	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRays (const uint3 dim, const RTShaderBindingTable &sbt)
	{
		RawCtx::TraceRays( dim, sbt.raygen, sbt.miss, sbt.hit, sbt.callable );
	}
	
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRays (const uint2 dim, RTShaderBindingID sbtId)
	{
		auto&	sbt = _GetResourcesOrThrow( sbtId ).GetSBT();
		RawCtx::TraceRays( uint3{ dim.x, dim.y, 1u }, sbt.raygen, sbt.miss, sbt.hit, sbt.callable );
	}
	
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRays (const uint3 dim, RTShaderBindingID sbtId)
	{
		auto&	sbt = _GetResourcesOrThrow( sbtId ).GetSBT();
		RawCtx::TraceRays( dim, sbt.raygen, sbt.miss, sbt.hit, sbt.callable );
	}

/*
=================================================
	TraceRaysIndirect
=================================================
*/
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRaysIndirect (const RTShaderBindingTable &sbt, VDeviceAddress address)
	{
		RawCtx::_TraceRaysIndirect( sbt.raygen, sbt.miss, sbt.hit, sbt.callable, address );
	}
	
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectBufferOffset)
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		Validator_t::TraceRaysIndirect( buf, indirectBufferOffset );

		RawCtx::_TraceRaysIndirect( sbt.raygen, sbt.miss, sbt.hit, sbt.callable, buf.GetDeviceAddress() + indirectBufferOffset );
	}
	
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRaysIndirect (RTShaderBindingID sbtId, BufferID indirectBuffer, Bytes indirectBufferOffset)
	{
		auto	[buf, sbt_obj]	= _GetResourcesOrThrow( indirectBuffer, sbtId );
		auto&	sbt				= sbt_obj.GetSBT();
		Validator_t::TraceRaysIndirect( buf, indirectBufferOffset );

		RawCtx::_TraceRaysIndirect( sbt.raygen, sbt.miss, sbt.hit, sbt.callable, buf.GetDeviceAddress() + indirectBufferOffset );
	}
	
/*
=================================================
	TraceRaysIndirect2
=================================================
*/
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRaysIndirect2 (VDeviceAddress address)
	{
		RawCtx::_TraceRaysIndirect2( address );
	}
	
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset)
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		Validator_t::TraceRaysIndirect2( buf, indirectBufferOffset );
		
		RawCtx::_TraceRaysIndirect2( buf.GetDeviceAddress() + indirectBufferOffset );
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
		RayTracingContextValidation::TraceRays( dim );

		vkCmdTraceRaysKHR( _cmdbuf.Get(), &raygen, &miss, &hit, &callable, dim.x, dim.y, dim.z );
	}
	
/*
=================================================
	_TraceRaysIndirect
=================================================
*/
	inline void  _VDirectRayTracingCtx::_TraceRaysIndirect (const VkStridedDeviceAddressRegionKHR&	raygen,
															const VkStridedDeviceAddressRegionKHR&	miss,
															const VkStridedDeviceAddressRegionKHR&	hit,
															const VkStridedDeviceAddressRegionKHR&	callable,
															VkDeviceAddress							indirectDeviceAddress)
	{
		ASSERT( _states.pipeline != Default );
		ASSERT( indirectDeviceAddress != Default );

		vkCmdTraceRaysIndirectKHR( _cmdbuf.Get(), &raygen, &miss, &hit, &callable, VkDeviceAddress(indirectDeviceAddress) );
	}
	
/*
=================================================
	_TraceRaysIndirect2
=================================================
*/
	inline void  _VDirectRayTracingCtx::_TraceRaysIndirect2 (VkDeviceAddress indirectDeviceAddress)
	{
		ASSERT( _states.pipeline != Default );
		ASSERT( indirectDeviceAddress != Default );

		vkCmdTraceRaysIndirect2KHR( _cmdbuf.Get(), indirectDeviceAddress );
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
	inline void  _VDirectRayTracingCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets)
	{
		ASSERT( _states.pplnLayout != Default );
		ASSERT( ds != Default );

		vkCmdBindDescriptorSets( _cmdbuf.Get(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, _states.pplnLayout, index.vkIndex, 1, &ds, uint(dynamicOffsets.size()), dynamicOffsets.data() );
	}
	
/*
=================================================
	_PushConstant
=================================================
*/
	inline void  _VDirectRayTracingCtx::_PushConstant (Bytes offset, Bytes size, const void *values, EShaderStages stages)
	{
		ASSERT( _states.pplnLayout != Default );

		vkCmdPushConstants( _cmdbuf.Get(), _states.pplnLayout, VEnumCast(stages), uint(offset), uint(size), values );
	}
		
/*
=================================================
	_SetStackSize
=================================================
*/
	inline void  _VDirectRayTracingCtx::_SetStackSize (Bytes size)
	{
		ASSERT( _states.pipeline != Default );

		vkCmdSetRayTracingPipelineStackSizeKHR( _cmdbuf.Get(), uint(size) );
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
	inline void  _VIndirectRayTracingCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets)
	{
		_cmdbuf->BindDescriptorSet( VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, _states.pplnLayout, index.vkIndex, ds, dynamicOffsets );
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
