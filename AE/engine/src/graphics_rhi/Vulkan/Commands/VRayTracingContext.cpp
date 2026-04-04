// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/Commands/VRayTracingContext.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"

namespace AE::Graphics::_hidden_
{
/*
=================================================
	EndCommandBuffer
=================================================
*/
	VkCommandBuffer  _VDirectRayTracingCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::RayTracing ));

		return VBaseDirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VCommandBuffer  _VDirectRayTracingCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::RayTracing ));

		return VBaseDirectContext::_ReleaseCommandBuffer();
	}

/*
=================================================
	TraceRays
=================================================
*/
	void  _VDirectRayTracingCtx::TraceRays (const uint3 &dim,
											const VkStridedDeviceAddressRegionKHR &raygen,
											const VkStridedDeviceAddressRegionKHR &miss,
											const VkStridedDeviceAddressRegionKHR &hit,
											const VkStridedDeviceAddressRegionKHR &callable) __Th___
	{
		VALIDATE_GCTX( TraceRays( _states.pplnLayout, dim ));

		vkCmdTraceRaysKHR( _cmdbuf.Get(), &raygen, &miss, &hit, &callable, dim.x, dim.y, dim.z );
	}

/*
=================================================
	_TraceRaysIndirect
=================================================
*/
	void  _VDirectRayTracingCtx::_TraceRaysIndirect (const VkStridedDeviceAddressRegionKHR&	raygen,
													 const VkStridedDeviceAddressRegionKHR&	miss,
													 const VkStridedDeviceAddressRegionKHR&	hit,
													 const VkStridedDeviceAddressRegionKHR&	callable,
													 VkDeviceAddress						indirectDeviceAddress) __Th___
	{
		VALIDATE_GCTX( TraceRaysIndirect( _states.pplnLayout, indirectDeviceAddress ));

		vkCmdTraceRaysIndirectKHR( _cmdbuf.Get(), &raygen, &miss, &hit, &callable, VkDeviceAddress(indirectDeviceAddress) );
	}

/*
=================================================
	_TraceRaysIndirect2
=================================================
*/
	void  _VDirectRayTracingCtx::_TraceRaysIndirect2 (VkDeviceAddress indirectDeviceAddress) __Th___
	{
		VALIDATE_GCTX( TraceRaysIndirect2( _states.pplnLayout, indirectDeviceAddress ));

		vkCmdTraceRaysIndirect2KHR( _cmdbuf.Get(), indirectDeviceAddress );
	}

/*
=================================================
	_BindPipeline
=================================================
*/
	void  _VDirectRayTracingCtx::_BindPipeline (VkPipeline ppln, VkPipelineLayout layout) __Th___
	{
		_states.pplnLayout = layout;
		vkCmdBindPipeline( _cmdbuf.Get(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, ppln );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	void  _VDirectRayTracingCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		VALIDATE_GCTX( BindDescriptorSet( _states.pplnLayout, index, ds ));

		vkCmdBindDescriptorSets( _cmdbuf.Get(), VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, _states.pplnLayout, index.vkIndex, 1, &ds, uint(dynamicOffsets.size()), dynamicOffsets.data() );
	}

/*
=================================================
	_PushConstant
=================================================
*/
	void  _VDirectRayTracingCtx::_PushConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages) __Th___
	{
		VALIDATE_GCTX( PushConstant( _states.pplnLayout, offset, size, values, stages ));

		vkCmdPushConstants( _cmdbuf.Get(), _states.pplnLayout, VEnumCast(stages), uint(offset), uint(size), values );
	}

/*
=================================================
	_SetStackSize
=================================================
*/
	void  _VDirectRayTracingCtx::_SetStackSize (Bytes size) __Th___
	{
		GCTX_CHECK( _states.pplnLayout != Default );

		vkCmdSetRayTracingPipelineStackSizeKHR( _cmdbuf.Get(), uint(size) );
	}

/*
=================================================
	PreprocessGeneratedCommands / ExecuteGeneratedCommands
=================================================
*/
	void  _VDirectRayTracingCtx::PreprocessGeneratedCommands (const VkGeneratedCommandsInfoEXT &info, VkCommandBuffer stateCmdbuf) __Th___
	{
		vkCmdPreprocessGeneratedCommandsEXT( _cmdbuf.Get(), &info, stateCmdbuf );
	}

	void  _VDirectRayTracingCtx::ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &info, bool isPreprocessed) __Th___
	{
		vkCmdExecuteGeneratedCommandsEXT( _cmdbuf.Get(), isPreprocessed, &info );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	EndCommandBuffer
=================================================
*/
	VBakedCommands  _VIndirectRayTracingCtx::EndCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::RayTracing ));

		return VBaseIndirectContext::_EndCommandBuffer();  // throw
	}

/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	VSoftwareCmdBufPtr  _VIndirectRayTracingCtx::ReleaseCommandBuffer () __Th___
	{
		ASSERT( _NoPendingBarriers() );
		GFX_DBG_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::RayTracing ));

		return VBaseIndirectContext::_ReleaseCommandBuffer();
	}

/*
=================================================
	_SetStackSize
=================================================
*/
	void  _VIndirectRayTracingCtx::_SetStackSize (Bytes size) __Th___
	{
		auto&	cmd = _cmdbuf->CreateCmd< RayTracingSetStackSizeCmd >();	// throw

		cmd.size = uint(size);
	}

/*
=================================================
	TraceRays
=================================================
*/
	void  _VIndirectRayTracingCtx::TraceRays (const uint3 &dim,
											  const VkStridedDeviceAddressRegionKHR &raygen,
											  const VkStridedDeviceAddressRegionKHR &miss,
											  const VkStridedDeviceAddressRegionKHR &hit,
											  const VkStridedDeviceAddressRegionKHR &callable) __Th___
	{
		VALIDATE_GCTX( TraceRays( _states.pplnLayout, dim ));

		auto&	cmd = _cmdbuf->CreateCmd< TraceRaysCmd >();	// throw
		cmd.raygen		= raygen;
		cmd.miss		= miss;
		cmd.hit			= hit;
		cmd.callable	= callable;
		cmd.dim			= dim;
	}

/*
=================================================
	_TraceRaysIndirect2
=================================================
*/
	void  _VIndirectRayTracingCtx::_TraceRaysIndirect2 (VkDeviceAddress indirectDeviceAddress) __Th___
	{
		VALIDATE_GCTX( TraceRaysIndirect2( _states.pplnLayout, indirectDeviceAddress ));

		auto&	cmd = _cmdbuf->CreateCmd< TraceRaysIndirect2Cmd >();	// throw
		cmd.indirectDeviceAddress = indirectDeviceAddress;		// address to 'VkTraceRaysIndirectCommand2KHR'
	}

/*
=================================================
	_TraceRaysIndirect
=================================================
*/
	void  _VIndirectRayTracingCtx::_TraceRaysIndirect (const VkStridedDeviceAddressRegionKHR&	raygen,
													   const VkStridedDeviceAddressRegionKHR&	miss,
													   const VkStridedDeviceAddressRegionKHR&	hit,
													   const VkStridedDeviceAddressRegionKHR&	callable,
													   VkDeviceAddress							indirectDeviceAddress) __Th___
	{
		VALIDATE_GCTX( TraceRaysIndirect( _states.pplnLayout, indirectDeviceAddress ));

		auto&	cmd = _cmdbuf->CreateCmd< TraceRaysIndirectCmd >();	// throw
		cmd.raygen					= raygen;
		cmd.miss					= miss;
		cmd.hit						= hit;
		cmd.callable				= callable;
		cmd.indirectDeviceAddress	= indirectDeviceAddress;	// address to 'VkTraceRaysIndirectCommandKHR'
	}

/*
=================================================
	_BindPipeline
=================================================
*/
	void  _VIndirectRayTracingCtx::_BindPipeline (VkPipeline ppln, VkPipelineLayout layout) __Th___
	{
		_states.pplnLayout = layout;
		_cmdbuf->BindPipeline( VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, ppln, layout );
	}

/*
=================================================
	BindDescriptorSet
=================================================
*/
	void  _VIndirectRayTracingCtx::BindDescriptorSet (DescSetBinding index, VkDescriptorSet ds, ArrayView<uint> dynamicOffsets) __Th___
	{
		VALIDATE_GCTX( BindDescriptorSet( _states.pplnLayout, index, ds ));

		_cmdbuf->BindDescriptorSet( VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, _states.pplnLayout, index.vkIndex, ds, dynamicOffsets );
	}

/*
=================================================
	_PushConstant
=================================================
*/
	void  _VIndirectRayTracingCtx::_PushConstant (Bytes offset, Bytes size, const void* values, EShaderStages stages) __Th___
	{
		VALIDATE_GCTX( PushConstant( _states.pplnLayout, offset, size, values, stages ));

		_cmdbuf->PushConstant( _states.pplnLayout, offset, size, values, stages );
	}

/*
=================================================
	PreprocessGeneratedCommands / ExecuteGeneratedCommands
=================================================
*/
	void  _VIndirectRayTracingCtx::PreprocessGeneratedCommands (const VkGeneratedCommandsInfoEXT &info, VkCommandBuffer stateCmdbuf) __Th___
	{
		auto&	cmd	= _cmdbuf->CreateCmd< PreprocessIndirectCommandsCmd >();	// throw
		cmd.info		= info;
		cmd.stateCmdbuf	= stateCmdbuf;
	}

	void  _VIndirectRayTracingCtx::ExecuteGeneratedCommands (const VkGeneratedCommandsInfoEXT &info, bool isPreprocessed) __Th___
	{
		auto&	cmd	= _cmdbuf->CreateCmd< ExecuteIndirectCommandsCmd >();	// throw
		cmd.info			= info;
		cmd.isPreprocessed	= isPreprocessed;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	template <typename C>
	_VRayTracingContextImpl<C>::_VRayTracingContextImpl (RenderCoroRef task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
		RawCtx{ task, RVRef(cmdbuf), dbg }
	{
		Validator_t::CtxInit( task.QueueMask() );
	}

/*
=================================================
	BindPipeline
=================================================
*/
	template <typename C>
	void  _VRayTracingContextImpl<C>::BindPipeline (RayTracingPipelineID ppln) __Th___
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
	void  _VRayTracingContextImpl<C>::BindDescriptorSet (DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets) __Th___
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
	void  _VRayTracingContextImpl<C>::PushConstant (const PushConstantIndex &idx, Bytes size, const void* values, ShaderStructName::Ref typeName) __Th___
	{
		VALIDATE_GCTX( PushConstant( idx, size, typeName ));
		Unused( typeName );
		RawCtx::_PushConstant( idx.vulkanOffset, size, values, EShaderStages(0) | idx.stage );
	}

/*
=================================================
	TraceRays
=================================================
*/
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRays (const uint2 dim, const RTShaderBindingTable &sbt) __Th___
	{
		return TraceRays( uint3{ dim.x, dim.y, 1u }, sbt );
	}

	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRays (const uint3 dim, const RTShaderBindingTable &sbt) __Th___
	{
		RawCtx::TraceRays( dim, sbt.raygen, sbt.miss, sbt.hit, sbt.callable );
	}

	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRays (const uint2 dim, RTShaderBindingID sbtId) __Th___
	{
		auto&	sbt = _GetResourcesOrThrow( sbtId ).GetSBT();
		RawCtx::TraceRays( uint3{ dim.x, dim.y, 1u }, sbt.raygen, sbt.miss, sbt.hit, sbt.callable );
	}

	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRays (const uint3 dim, RTShaderBindingID sbtId) __Th___
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
	void  _VRayTracingContextImpl<C>::TraceRaysIndirectAddress (const RTShaderBindingTable &sbt, DeviceAddress address) __Th___
	{
		RawCtx::_TraceRaysIndirect( sbt.raygen, sbt.miss, sbt.hit, sbt.callable, BitCast<VkDeviceAddress>(address) );
	}

	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRaysIndirect (const RTShaderBindingTable &sbt, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( TraceRaysIndirect( buf.Description(), indirectBufferOffset ));

		RawCtx::_TraceRaysIndirect( sbt.raygen, sbt.miss, sbt.hit, sbt.callable,
									BitCast<VkDeviceAddress>( buf.GetDeviceAddress() + indirectBufferOffset ));
	}

	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRaysIndirect (RTShaderBindingID sbtId, BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		auto	[buf, sbt_obj]	= _GetResourcesOrThrow( indirectBuffer, sbtId );
		auto&	sbt				= sbt_obj.GetSBT();
		VALIDATE_GCTX( TraceRaysIndirect( buf.Description(), indirectBufferOffset ));

		RawCtx::_TraceRaysIndirect( sbt.raygen, sbt.miss, sbt.hit, sbt.callable,
									BitCast<VkDeviceAddress>( buf.GetDeviceAddress() + indirectBufferOffset ));
	}

/*
=================================================
	TraceRaysIndirect2
=================================================
*/
	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRaysIndirectAddress2 (DeviceAddress address) __Th___
	{
		RawCtx::_TraceRaysIndirect2( BitCast<VkDeviceAddress>(address) );
	}

	template <typename C>
	void  _VRayTracingContextImpl<C>::TraceRaysIndirect2 (BufferID indirectBuffer, Bytes indirectBufferOffset) __Th___
	{
		auto&	buf = _GetResourcesOrThrow( indirectBuffer );
		VALIDATE_GCTX( TraceRaysIndirect2( buf.Description(), indirectBufferOffset ));

		RawCtx::_TraceRaysIndirect2( BitCast<VkDeviceAddress>( buf.GetDeviceAddress() + indirectBufferOffset ));
	}

/*
=================================================
	BindInitialPipeline
=================================================
*/
	template <typename C>
	void  _VRayTracingContextImpl<C>::BindInitialPipeline (IndirectExecutionSetID id) __Th___
	{
		auto&	exec_set = _GetResourcesOrThrow( id );
		auto	state	 = exec_set.GetInitialState();

		RawCtx::_BindPipeline( state.pipeline, state.layout );
	}

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------

# define VCTX_TYPE	_VRayTracingContextImpl
# include "graphics_rhi/Vulkan/Commands/VIndirectCommandExecutorImpl.cpp.h"
# undef VCTX_TYPE

//-----------------------------------------------------------------------------
namespace AE::Graphics::_hidden_
{
    template class _VRayTracingContextImpl< _VDirectRayTracingCtx >;
    template class _VRayTracingContextImpl< _VIndirectRayTracingCtx >;

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
