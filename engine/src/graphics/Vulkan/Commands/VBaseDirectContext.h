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

		ND_ bool	IsValid ()	const	{ return _cmdbuf.IsValid(); }

	protected:
		_VBaseDirectContext (VCommandBuffer cmdbuf, NtStringView dbgName, RGBA8u dbgColor);

		void  _DebugMarker (NtStringView text, RGBA8u color);
		void  _PushDebugGroup (NtStringView text, RGBA8u color);
		void  _PopDebugGroup ();

		void  _DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data);

		ND_ VkCommandBuffer	_EndCommandBuffer ();
		ND_ VCommandBuffer  _ReleaseCommandBuffer ();
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
		explicit VBaseDirectContext (const VRenderTask &task);
		VBaseDirectContext (const VRenderTask &task, VCommandBuffer cmdbuf);
		~VBaseDirectContext () override;

	protected:
		void  _CommitBarriers ();
		
		void  _DebugMarker (NtStringView text, RGBA8u color)		{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_DebugMarker( text, color ); }
		void  _PushDebugGroup (NtStringView text, RGBA8u color)		{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_PushDebugGroup( text, color ); }
		void  _PopDebugGroup ()										{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_PopDebugGroup(); }

		ND_ bool	_NoPendingBarriers ()	const	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetExtensions ()		const	{ return _mngr.GetDevice().GetExtensions(); }
		ND_ auto&	_GetFeatures ()			const	{ return _mngr.GetDevice().GetProperties().features; }
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	constructor
=================================================
*/
	inline _VBaseDirectContext::_VBaseDirectContext (VCommandBuffer cmdbuf, NtStringView dbgName, RGBA8u dbgColor) :
		_cmdbuf{ RVRef( cmdbuf )}
	{
		VulkanDeviceFn_Init( RenderTaskScheduler().GetDevice() );
		DEBUG_ONLY(
			_PushDebugGroup( dbgName, dbgColor );
		)
		Unused( dbgName, dbgColor );
	}

/*
=================================================
	destructor
=================================================
*/
	inline _VBaseDirectContext::~_VBaseDirectContext ()
	{
		DBG_CHECK_MSG( not IsValid(), "you forget to call 'EndCommandBuffer()' or 'ReleaseCommandBuffer()'" );
	}
	
/*
=================================================
	_EndCommandBuffer
=================================================
*/
	inline VkCommandBuffer  _VBaseDirectContext::_EndCommandBuffer ()
	{
		ASSERT( _cmdbuf.IsValid() );

		VkCommandBuffer	cmd = _cmdbuf.Get();

		DEBUG_ONLY( _PopDebugGroup() );
		VK_CALL( vkEndCommandBuffer( cmd ));

		_cmdbuf.Release();
		return cmd;
	}
	
/*
=================================================
	_ReleaseCommandBuffer
=================================================
*/
	inline VCommandBuffer  _VBaseDirectContext::_ReleaseCommandBuffer ()
	{
		ASSERT( _cmdbuf.IsValid() );

		// don't call vkEndCommandBuffer
		DEBUG_ONLY( _PopDebugGroup() );

		VCommandBuffer	res = RVRef(_cmdbuf);
		ASSERT( not _cmdbuf.IsValid() );
		return res;
	}

/*
=================================================
	_DebugMarker
=================================================
*/
	inline void  _VBaseDirectContext::_DebugMarker (NtStringView text, RGBA8u color)
	{
		ASSERT( not text.empty() );

		VkDebugUtilsLabelEXT	info = {};
		info.sType		= VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		info.pLabelName	= text.size() ? text.c_str() : "";
		MemCopy( info.color, RGBA32f{color} );
			
		vkCmdInsertDebugUtilsLabelEXT( _cmdbuf.Get(), &info );
	}
	
/*
=================================================
	_PushDebugGroup
=================================================
*/
	inline void  _VBaseDirectContext::_PushDebugGroup (NtStringView text, RGBA8u color)
	{
		ASSERT( not text.empty() );

		VkDebugUtilsLabelEXT	info = {};
		info.sType		= VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		info.pLabelName	= text.size() ? text.c_str() : "";
		MemCopy( info.color, RGBA32f{color} );

		vkCmdBeginDebugUtilsLabelEXT( _cmdbuf.Get(), &info );
	}
	
/*
=================================================
	_PopDebugGroup
=================================================
*/
	inline void  _VBaseDirectContext::_PopDebugGroup ()
	{
		vkCmdEndDebugUtilsLabelEXT( _cmdbuf.Get() );
	}
	
/*
=================================================
	_DbgFillBuffer
=================================================
*/
	inline void  _VBaseDirectContext::_DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)
	{
		ASSERT( buffer != Default );

		vkCmdFillBuffer( _cmdbuf.Get(), buffer, VkDeviceSize(offset), VkDeviceSize(size), data );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline VBaseDirectContext::VBaseDirectContext (const VRenderTask &task) :
		VBaseDirectContext{
			task,
			RenderTaskScheduler().GetCommandPoolManager().GetCommandBuffer(
						task.GetBatchPtr()->GetQueueType(),
						task.GetBatchPtr()->GetCmdBufType(),
						null )}
	{}
	
	inline VBaseDirectContext::VBaseDirectContext (const VRenderTask &task, VCommandBuffer cmdbuf) :
		_VBaseDirectContext{ RVRef(cmdbuf), task.DbgFullName(), task.DbgColor() },
		_mngr{ task }
	{
		ASSERT( _mngr.GetBatch().GetQueueType() == _cmdbuf.GetQueueType() );
	}
	
/*
=================================================
	destructor
=================================================
*/
	inline VBaseDirectContext::~VBaseDirectContext ()
	{
		ASSERT( _NoPendingBarriers() );
	}

/*
=================================================
	_CommitBarriers
=================================================
*/
	inline void  VBaseDirectContext::_CommitBarriers ()
	{
		auto* bar = _mngr.GetBarriers();
		if_unlikely( bar != null )
		{
			vkCmdPipelineBarrier2KHR( _cmdbuf.Get(), bar );
			_mngr.ClearBarriers();
		}
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
