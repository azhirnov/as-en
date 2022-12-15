// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Vulkan/Commands/VBarrierManager.h"
# include "graphics/Vulkan/Commands/VDrawBarrierManager.h"
# include "graphics/Vulkan/VRenderTaskScheduler.h"

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
		virtual ~_VBaseDirectContext ()													__NE___;

	protected:
		_VBaseDirectContext (VCommandBuffer cmdbuf, DebugLabel dbg)						__Th___;

		ND_ bool	_IsValid ()															C_NE___	{ return _cmdbuf.IsValid() and _cmdbuf.IsRecording(); }

		void  _DebugMarker (DebugLabel dbg);
		void  _PushDebugGroup (DebugLabel dbg);
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
		explicit VBaseDirectContext (const RenderTask &task)				__Th___;
		VBaseDirectContext (const RenderTask &task, VCommandBuffer cmdbuf)	__Th___;
		~VBaseDirectContext ()												__NE_OV;

	protected:
		void  _CommitBarriers ();
		
		void  _DebugMarker (DebugLabel dbg)											{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_DebugMarker( dbg ); }
		void  _PushDebugGroup (DebugLabel dbg)										{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_PushDebugGroup( dbg ); }
		void  _PopDebugGroup ()														{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_PopDebugGroup(); }

		ND_ bool	_NoPendingBarriers ()									C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetExtensions ()										C_NE___	{ return _mngr.GetDevice().GetExtensions(); }
		ND_ auto&	_GetFeatures ()											C_NE___	{ return _mngr.GetDevice().GetProperties().features; }
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	constructor
=================================================
*/
	inline _VBaseDirectContext::_VBaseDirectContext (VCommandBuffer cmdbuf, DebugLabel dbg) __Th___ :
		_cmdbuf{ RVRef( cmdbuf )}
	{
		CHECK_THROW( _IsValid() );

		VulkanDeviceFn_Init( RenderTaskScheduler().GetDevice() );
		DEBUG_ONLY(
			_PushDebugGroup( dbg );
		)
		Unused( dbg );
	}

/*
=================================================
	destructor
=================================================
*/
	inline _VBaseDirectContext::~_VBaseDirectContext () __NE___
	{
		DBG_CHECK_MSG( not _IsValid(), "you forget to call 'EndCommandBuffer()' or 'ReleaseCommandBuffer()'" );
	}
	
/*
=================================================
	_EndCommandBuffer
=================================================
*/
	inline VkCommandBuffer  _VBaseDirectContext::_EndCommandBuffer ()
	{
		ASSERT( _IsValid() );

		VkCommandBuffer	cmd = _cmdbuf.Get();

		DEBUG_ONLY( _PopDebugGroup() );

		// end recording and release ownership
		CHECK_THROW( _cmdbuf.EndAndRelease() );	// throw

		return cmd;
	}
	
/*
=================================================
	_ReleaseCommandBuffer
=================================================
*/
	inline VCommandBuffer  _VBaseDirectContext::_ReleaseCommandBuffer ()
	{
		ASSERT( _IsValid() );

		// don't call vkEndCommandBuffer
		DEBUG_ONLY( _PopDebugGroup() );

		VCommandBuffer	res = RVRef(_cmdbuf);
		ASSERT( not _IsValid() );
		return res;
	}

/*
=================================================
	_DebugMarker
=================================================
*/
	inline void  _VBaseDirectContext::_DebugMarker (DebugLabel dbg)
	{
		ASSERT( not dbg.label.empty() );

		VkDebugUtilsLabelEXT	info = {};
		info.sType		= VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		info.pLabelName	= NtStringView{dbg.label}.c_str();
		MemCopy( info.color, RGBA32f{dbg.color} );
			
		vkCmdInsertDebugUtilsLabelEXT( _cmdbuf.Get(), &info );
	}
	
/*
=================================================
	_PushDebugGroup
=================================================
*/
	inline void  _VBaseDirectContext::_PushDebugGroup (DebugLabel dbg)
	{
		ASSERT( not dbg.label.empty() );

		VkDebugUtilsLabelEXT	info = {};
		info.sType		= VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		info.pLabelName	= NtStringView{dbg.label}.c_str();
		MemCopy( info.color, RGBA32f{dbg.color} );

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
	inline VBaseDirectContext::VBaseDirectContext (const RenderTask &task) __Th___ :
		VBaseDirectContext{	// throw
			task,
			RenderTaskScheduler().GetCommandPoolManager().GetCommandBuffer(
						task.GetBatchPtr()->GetQueueType(),
						task.GetBatchPtr()->GetCmdBufType(),
						null )}
	{}
	
	inline VBaseDirectContext::VBaseDirectContext (const RenderTask &task, VCommandBuffer cmdbuf) __Th___ :
		_VBaseDirectContext{ RVRef(cmdbuf), DebugLabel{ task.DbgFullName(), task.DbgColor() }},	// throw
		_mngr{ task }
	{
		ASSERT( _mngr.GetBatch().GetQueueType() == _cmdbuf.GetQueueType() );
	}
	
/*
=================================================
	destructor
=================================================
*/
	inline VBaseDirectContext::~VBaseDirectContext () __NE___
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
