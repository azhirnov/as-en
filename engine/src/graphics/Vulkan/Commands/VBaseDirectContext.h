// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Private/ContextValidation.h"
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
		
		using CmdBuf_t = VCommandBuffer;


	// variables
	protected:
		VCommandBuffer		_cmdbuf;


	// methods
	public:
		virtual ~_VBaseDirectContext ()													__NE___	{ DBG_CHECK_MSG( not _IsValid(), "you forget to call 'EndCommandBuffer()' or 'ReleaseCommandBuffer()'" ); }
		
		void  PipelineBarrier (const VkDependencyInfo &info);

	protected:
		explicit _VBaseDirectContext (VCommandBuffer cmdbuf)							__Th___;

		ND_ bool	_IsValid ()															C_NE___	{ return _cmdbuf.IsValid() and _cmdbuf.IsRecording(); }

		void  _DebugMarker (DebugLabel dbg)														{ _cmdbuf.DebugMarker( *this, dbg.label, dbg.color ); }
		void  _PushDebugGroup (DebugLabel dbg)													{ _cmdbuf.PushDebugGroup( *this, dbg.label, dbg.color ); }
		void  _PopDebugGroup ()																	{ _cmdbuf.PopDebugGroup( *this ); }

		void  _DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data);

		ND_ VkCommandBuffer	_EndCommandBuffer ();
		ND_ VCommandBuffer  _ReleaseCommandBuffer ();

		ND_ static VCommandBuffer  _ReuseOrCreateCommandBuffer (const VCommandBatch &batch, VCommandBuffer cmdbuf, DebugLabel dbg, bool firstInQueue)	__NE___;
		ND_ static VCommandBuffer  _ReuseOrCreateCommandBuffer (const VCommandBatch &batch, VCommandBuffer cmdbuf, const RenderTask &task)				__NE___;
		ND_ static VCommandBuffer  _ReuseOrCreateCommandBuffer (const VDrawCommandBatch &batch, VCommandBuffer cmdbuf, DebugLabel dbg)					__NE___;
	};
	

	
	//
	// Vulkan Direct Context base class with barrier manager
	//

	class VBaseDirectContext : public _VBaseDirectContext
	{
	// types
	protected:
		using ECtxType = IGraphicsProfiler::EContextType;


	// variables
	protected:
		VBarrierManager		_mngr;


	// methods
	public:
		explicit VBaseDirectContext (const RenderTask &task, ECtxType ctxType)	__Th___	: VBaseDirectContext{ task, Default, ctxType } {}
		VBaseDirectContext (const RenderTask &, VCommandBuffer, ECtxType)		__Th___;
		~VBaseDirectContext ()													__NE_OV	{ ASSERT( _NoPendingBarriers() ); }

	protected:
		void  _CommitBarriers ();
		
		void  _DebugMarker (DebugLabel dbg)												{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_DebugMarker( dbg ); }
		void  _PushDebugGroup (DebugLabel dbg)											{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_PushDebugGroup( dbg ); }
		void  _PopDebugGroup ()															{ ASSERT( _NoPendingBarriers() );  _VBaseDirectContext::_PopDebugGroup(); }
		
		ND_ VkCommandBuffer	_EndCommandBuffer ();

		ND_ bool	_NoPendingBarriers ()										C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetExtensions ()											C_NE___	{ return _mngr.GetDevice().GetExtensions(); }
		ND_ auto&	_GetFeatures ()												C_NE___	{ return _mngr.GetDevice().GetProperties().features; }
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	constructor
=================================================
*/
	inline _VBaseDirectContext::_VBaseDirectContext (VCommandBuffer cmdbuf) __Th___ :
		_cmdbuf{ RVRef( cmdbuf )}
	{
		CHECK_THROW( _IsValid() );

		VulkanDeviceFn_Init( RenderTaskScheduler().GetDevice() );
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
	inline VBaseDirectContext::VBaseDirectContext (const RenderTask &task, VCommandBuffer cmdbuf, ECtxType ctxType) __Th___ :
		_VBaseDirectContext{_ReuseOrCreateCommandBuffer( *task.GetBatchPtr(), RVRef(cmdbuf), task )},  // throw
		_mngr{ task }
	{
		ASSERT( _mngr.GetBatch().GetQueueType() == _cmdbuf.GetQueueType() );
		
		DBG_GRAPHICS_ONLY( _mngr.ProfilerBeginContext( _cmdbuf.Get(), ctxType ); )

		if ( auto* bar = _mngr.GetBatch().ExtractInitialBarriers( task.GetExecutionIndex() ))
			PipelineBarrier( *bar );
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
