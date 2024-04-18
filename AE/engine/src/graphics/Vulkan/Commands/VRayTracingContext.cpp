// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Commands/VRayTracingContext.h"

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
		DBG_GRAPHICS_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::RayTracing ); )

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
		DBG_GRAPHICS_ONLY( _mngr.ProfilerEndContext( _cmdbuf.Get(), ECtxType::RayTracing ); )

		return VBaseDirectContext::_ReleaseCommandBuffer();
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
		DBG_GRAPHICS_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::RayTracing ); )

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
		DBG_GRAPHICS_ONLY( _mngr.ProfilerEndContext( *_cmdbuf, ECtxType::RayTracing ); )

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


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
