// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Vulkan/Commands/VRenderTaskScheduler.h"
# include "graphics/Vulkan/Commands/VBarrierManager.h"
# include "graphics/Vulkan/Commands/VDrawBarrierManager.h"

namespace AE::Graphics::_hidden_
{
	
	//
	// Vulkan Direct Context base class
	//

	class _VBaseDirectContext : protected VulkanDeviceFn
	{
	// types
	public:
		static constexpr bool	IsIndirectContext = false;


	// variables
	protected:
		VCommandBuffer		_cmdbuf;


	// methods
	public:
		virtual ~_VBaseDirectContext ();

		ND_ bool			IsValid ()				const	{ return _cmdbuf.IsValid(); }
		ND_ VkCommandBuffer	EndCommandBuffer ();
		ND_ VCommandBuffer  ReleaseCommandBuffer ();

	protected:
		_VBaseDirectContext (VCommandBuffer cmdbuf, NtStringView dbgName);

		void  _DebugMarker (NtStringView text, RGBA8u color);
		void  _PushDebugGroup (NtStringView text, RGBA8u color);
		void  _PopDebugGroup ();

		void  _DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data);
	};
	

	
	//
	// Vulkan Direct Context base class with barrier manager
	//

	class VBaseDirectContext : public _VBaseDirectContext
	{
	// variables
	protected:
		VBarrierManager		_mngr;


	// methods
	public:
		explicit VBaseDirectContext (Ptr<VCommandBatch> batch);
		VBaseDirectContext (Ptr<VCommandBatch> batch, VCommandBuffer cmdbuf);
		~VBaseDirectContext () override;
		
		ND_ VkCommandBuffer	EndCommandBuffer ()			{ ASSERT( _NoPendingBarriers() );  return _VBaseDirectContext::EndCommandBuffer(); }
		ND_ VCommandBuffer  ReleaseCommandBuffer ()		{ ASSERT( _NoPendingBarriers() );  return _VBaseDirectContext::ReleaseCommandBuffer(); }

	protected:
		void  _CommitBarriers ();
		
		void  _DebugMarker (NtStringView text, RGBA8u color)		{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_DebugMarker( text, color ); }
		void  _PushDebugGroup (NtStringView text, RGBA8u color)		{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_PushDebugGroup( text, color ); }
		void  _PopDebugGroup ()										{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_PopDebugGroup(); }

		ND_ bool	_NoPendingBarriers ()	const	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetExtensions ()		const	{ return _mngr.GetDevice().GetExtensions(); }
	};


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
