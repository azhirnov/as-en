// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Vulkan/Commands/VBarrierManager.h"
# include "graphics/Vulkan/Commands/VDrawBarrierManager.h"
# include "graphics/Vulkan/Commands/VAccumDeferredBarriers.h"
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
		virtual ~_VBaseDirectContext ()													__NE___	{ DBG_CHECK_MSG( not _IsValid(), "you forget to call 'EndCommandBuffer()' or 'ReleaseCommandBuffer()'" ); }
		
		void  PipelineBarrier (const VkDependencyInfo &info);

	protected:
		_VBaseDirectContext (VCommandBuffer cmdbuf, DebugLabel dbg)						__Th___;

		ND_ bool	_IsValid ()															C_NE___	{ return _cmdbuf.IsValid() and _cmdbuf.IsRecording(); }

		void  _DebugMarker (DebugLabel dbg);
		void  _PushDebugGroup (DebugLabel dbg);
		void  _PopDebugGroup ();

		void  _DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data);

		ND_ VkCommandBuffer	_EndCommandBuffer ();
		ND_ VCommandBuffer  _ReleaseCommandBuffer ();

		ND_ static VCommandBuffer  _ReuseOrCreateCommandBuffer (const VCommandBatch &batch, VCommandBuffer cmdbuf)		__NE___;
		ND_ static VCommandBuffer  _ReuseOrCreateCommandBuffer (const VDrawCommandBatch &batch, VCommandBuffer cmdbuf)	__NE___;
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
		~VBaseDirectContext ()												__NE_OV	{ ASSERT( _NoPendingBarriers() ); }

	protected:
		void  _CommitBarriers ();
		
		void  _DebugMarker (DebugLabel dbg)											{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_DebugMarker( dbg ); }
		void  _PushDebugGroup (DebugLabel dbg)										{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_PushDebugGroup( dbg ); }
		void  _PopDebugGroup ()														{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_PopDebugGroup(); }
		
		ND_ VkCommandBuffer	_EndCommandBuffer ();

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
		//DEBUG_ONLY( _PopDebugGroup() );

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
	
/*
=================================================
	_ReuseOrCreateCommandBuffer
=================================================
*/
	inline VCommandBuffer  _VBaseDirectContext::_ReuseOrCreateCommandBuffer (const VCommandBatch &batch, VCommandBuffer cmdbuf) __NE___
	{
		if ( cmdbuf.IsValid() )
			return RVRef(cmdbuf);
		else
			return RenderTaskScheduler().GetCommandPoolManager().GetCommandBuffer(
						batch.GetQueueType(),
						batch.GetCmdBufType(),
						null );
	}
	
	inline VCommandBuffer  _VBaseDirectContext::_ReuseOrCreateCommandBuffer (const VDrawCommandBatch &batch, VCommandBuffer cmdbuf) __NE___
	{
		if ( cmdbuf.IsValid() )
			return RVRef(cmdbuf);
		else
			return RenderTaskScheduler().GetCommandPoolManager().GetCommandBuffer(
						batch.GetQueueType(),
						batch.GetCmdBufType(),
						&batch.GetPrimaryCtxState() );
	}
	
/*
=================================================
	PipelineBarrier
=================================================
*/
	inline void  _VBaseDirectContext::PipelineBarrier (const VkDependencyInfo &info)
	{
		vkCmdPipelineBarrier2KHR( _cmdbuf.Get(), &info );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline VBaseDirectContext::VBaseDirectContext (const RenderTask &task, VCommandBuffer cmdbuf) __Th___ :
		_VBaseDirectContext{	// throw
			_ReuseOrCreateCommandBuffer( *task.GetBatchPtr(), RVRef(cmdbuf) ),
			DebugLabel{ task.DbgFullName(), task.DbgColor() }
		},
		_mngr{ task }
	{
		ASSERT( _mngr.GetBatch().GetQueueType() == _cmdbuf.GetQueueType() );

		if ( auto* bar = _mngr.GetBatch().ExtractInitialBarriers( task.GetExecutionIndex() ))
			PipelineBarrier( *bar );
	}
		
	inline VBaseDirectContext::VBaseDirectContext (const RenderTask &task) __Th___ :
		VBaseDirectContext{ task, Default }
	{}

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
			PipelineBarrier( *bar );
			_mngr.ClearBarriers();
		}
	}
	
/*
=================================================
	_EndCommandBuffer
=================================================
*/
	inline VkCommandBuffer  VBaseDirectContext::_EndCommandBuffer ()
	{
		if ( auto* bar = _mngr.GetBatch().ExtractFinalBarriers( _mngr.GetRenderTask().GetExecutionIndex() ))
			PipelineBarrier( *bar );

		return _VBaseDirectContext::_EndCommandBuffer();
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
