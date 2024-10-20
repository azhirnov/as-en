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

		void  PipelineBarrier (const VkDependencyInfo &info)							__NE___;

	protected:
		explicit _VBaseDirectContext (VCommandBuffer cmdbuf)							__Th___;

		ND_ bool	_IsValid ()															C_NE___	{ return _cmdbuf.IsValid() and _cmdbuf.IsRecording(); }

		void  _DebugMarker (DebugLabel dbg)												__Th___	{ _cmdbuf.DebugMarker( *this, dbg.label, dbg.color ); }
		void  _PushDebugGroup (DebugLabel dbg)											__Th___	{ _cmdbuf.PushDebugGroup( *this, dbg.label, dbg.color ); }
		void  _PopDebugGroup ()															__Th___	{ _cmdbuf.PopDebugGroup( *this ); }

		void  _WriteTimestamp (const VQueryManager::Query &, uint index,
								EPipelineScope, VkPipelineStageFlagBits2 mask)			__Th___;

		void  _DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)		__Th___;

		ND_ VkCommandBuffer	_EndCommandBuffer ()										__Th___;
		ND_ VCommandBuffer  _ReleaseCommandBuffer ()									__Th___;

		ND_ static VCommandBuffer  _ReuseOrCreateCommandBuffer (const VDrawCommandBatch &batch, VCommandBuffer cmdbuf, DebugLabel dbg)						__NE___;
		ND_ static VCommandBuffer  _ReuseOrCreateCommandBuffer (const VCommandBatch &batch, VCommandBuffer cmdbuf, const RenderTask &task, DebugLabel dbg)	__NE___;
	private:
		ND_ static VCommandBuffer  _ReuseOrCreateCommandBuffer (const VCommandBatch &batch, VCommandBuffer cmdbuf, DebugLabel dbg, bool firstInQueue)		__NE___;
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
		VBaseDirectContext (const RenderTask &, VCommandBuffer, DebugLabel, ECtxType)		__Th___;
		~VBaseDirectContext ()																__NE_OV	{ ASSERT( _NoPendingBarriers() ); }

	protected:
			void  _CommitBarriers ()														__NE___;

		ND_ bool  _NoPendingBarriers ()														C_NE___	{ return _mngr.NoPendingBarriers(); }

		ND_ VkCommandBuffer	_EndCommandBuffer ()											__Th___;
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

		VulkanDeviceFn_Init( GraphicsScheduler().GetDevice() );
	}

/*
=================================================
	_DbgFillBuffer
=================================================
*/
	inline void  _VBaseDirectContext::_DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data) __Th___
	{
		GCTX_CHECK( buffer != Default );

		vkCmdFillBuffer( _cmdbuf.Get(), buffer, VkDeviceSize(offset), VkDeviceSize(size), data );
	}

/*
=================================================
	PipelineBarrier
=================================================
*/
	inline void  _VBaseDirectContext::PipelineBarrier (const VkDependencyInfo &info) __NE___
	{
		vkCmdPipelineBarrier2KHR( _cmdbuf.Get(), &info );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline VBaseDirectContext::VBaseDirectContext (const RenderTask &task, VCommandBuffer cmdbuf, DebugLabel dbg, ECtxType ctxType) __Th___ :
		_VBaseDirectContext{ _ReuseOrCreateCommandBuffer( *task.GetBatchPtr(), RVRef(cmdbuf), task, dbg )},  // throw
		_mngr{ task }
	{
		GCTX_CHECK( _mngr.GetBatch().GetQueueType() == _cmdbuf.GetQueueType() );

		GFX_DBG_ONLY(
			_mngr.ProfilerBeginContext( _cmdbuf.Get(), (dbg ? dbg : DebugLabel( task.DbgFullName(), task.DbgColor() )), ctxType );

			GraphicsScheduler().DbgCheckFrameId( _mngr.GetFrameId(), task.DbgFullName() );
		)
		Unused( ctxType );

		if ( auto bar = _mngr.GetBatch().ExtractInitialBarriers( task.GetExecutionIndex() ))
		{
			PipelineBarrier( *bar );
			GRAPHICS_DBG_SYNC( _DebugMarker({"Task.InitialBarriers"});)
		}
	}

/*
=================================================
	_CommitBarriers
=================================================
*/
	inline void  VBaseDirectContext::_CommitBarriers () __NE___
	{
		auto	bar = _mngr.GetBarriers();
		if_unlikely( bar )
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
	inline VkCommandBuffer  VBaseDirectContext::_EndCommandBuffer () __Th___
	{
		if ( auto bar = _mngr.GetBatch().ExtractFinalBarriers( _mngr.GetRenderTask().GetExecutionIndex() ))
		{
			GRAPHICS_DBG_SYNC( _DebugMarker({"Task.FinalBarriers"});)
			PipelineBarrier( *bar );
		}

		return _VBaseDirectContext::_EndCommandBuffer();  // throw
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_VULKAN
