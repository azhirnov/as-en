// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Metal/Commands/MBarrierManager.h"
# include "graphics/Metal/Commands/MDrawBarrierManager.h"
# include "graphics/Metal/Commands/MAccumDeferredBarriers.h"
# include "graphics/Metal/MRenderTaskScheduler.h"

namespace AE::Graphics::_hidden_
{
	
	//
	// Metal Direct Context base class
	//

	class _MBaseDirectContext
	{
	// types
	public:
		static constexpr bool	IsIndirectContext = false;


	// variables
	protected:
		MCommandBuffer		_cmdbuf;


	// methods
	public:
		virtual ~_MBaseDirectContext ()									__NE___;
		
		void  PipelineBarrier (const MDependencyInfo &info);

	protected:
		_MBaseDirectContext (MCommandBuffer cmdbuf, DebugLabel dbg)		__NE___;

		void  _DebugMarker (DebugLabel dbg);
		void  _PushDebugGroup (DebugLabel dbg);
		void  _PopDebugGroup ();

		//void  _DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data);
		
		ND_ bool					_IsValid ()							C_NE___	{ return _cmdbuf.IsRecording(); }

		ND_ bool					_EndEncoding ();
		ND_ MetalCommandBufferRC	_EndCommandBuffer ();
		ND_ MCommandBuffer		 	_ReleaseCommandBuffer ();

		ND_ static MCommandBuffer  _ReuseOrCreateCommandBuffer (const MCommandBatch &batch, MCommandBuffer cmdbuf)		__NE___;
		ND_ static MCommandBuffer  _ReuseOrCreateCommandBuffer (const MDrawCommandBatch &batch, MCommandBuffer cmdbuf)	__NE___;
	};
	

	
	//
	// Metal Direct Context base class with barrier manager
	//

	class MBaseDirectContext : public _MBaseDirectContext
	{
	// variables
	protected:
		MBarrierManager		_mngr;


	// methods
	public:
		explicit MBaseDirectContext (const RenderTask &task)				__NE___;
		MBaseDirectContext (const RenderTask &task, MCommandBuffer cmdbuf)	__NE___;
		~MBaseDirectContext ()												__NE_OV	{ ASSERT( _NoPendingBarriers() ); }

	protected:
			void	_CommitBarriers ();

		ND_ bool	_NoPendingBarriers ()									C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetFeatures ()											C_NE___	{ return _mngr.GetDevice().GetFeatures(); }

		ND_ MetalCommandBufferRC	_EndCommandBuffer ();
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	constructor
=================================================
*/
	inline _MBaseDirectContext::_MBaseDirectContext (MCommandBuffer cmdbuf, DebugLabel dbg) __NE___ :
		_cmdbuf{ RVRef( cmdbuf )}
	{
		DEBUG_ONLY( _PushDebugGroup( dbg );)
		Unused( dbg );
	}

/*
=================================================
	destructor
=================================================
*/
	inline _MBaseDirectContext::~_MBaseDirectContext () __NE___
	{
		DBG_CHECK_MSG( not (_cmdbuf.HasCmdBuf() or _cmdbuf.HasEncoder()), "you forget to call 'EndCommandBuffer()' or 'ReleaseCommandBuffer()'" );
	}

/*
=================================================
	_ReuseOrCreateCommandBuffer
=================================================
*/
	inline MCommandBuffer  _MBaseDirectContext::_ReuseOrCreateCommandBuffer (const MCommandBatch &batch, MCommandBuffer cmdbuf) __NE___
	{
		if ( cmdbuf.HasCmdBuf() or cmdbuf.HasEncoder() )
			return RVRef(cmdbuf);
		else
			return MCommandBuffer::CreateCommandBuffer( batch.GetQueueType() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline MBaseDirectContext::MBaseDirectContext (const RenderTask &task, MCommandBuffer cmdbuf) __NE___ :
		_MBaseDirectContext{
			_ReuseOrCreateCommandBuffer( *task.GetBatchPtr(), RVRef(cmdbuf) ),
			DebugLabel{ task.DbgFullName() }
		},
		_mngr{ task }
	{
		ASSERT( _mngr.GetBatch().GetQueueType() == _cmdbuf.GetQueueType() );

		if ( auto* bar = _mngr.GetBatch().ExtractInitialBarriers( task.GetExecutionIndex() ))
			PipelineBarrier( *bar );
	}
		
	inline MBaseDirectContext::MBaseDirectContext (const RenderTask &task) __NE___ :
		MBaseDirectContext{ task, MCommandBuffer{} }
	{}
	
/*
=================================================
	_EndCommandBuffer
=================================================
*/
	inline MetalCommandBufferRC  MBaseDirectContext::_EndCommandBuffer ()
	{
		if ( auto* bar = _mngr.GetBatch().ExtractFinalBarriers( _mngr.GetRenderTask().GetExecutionIndex() ))
			PipelineBarrier( *bar );

		return _MBaseDirectContext::_EndCommandBuffer();
	}

/*
=================================================
	_CommitBarriers
=================================================
*/
	inline void  MBaseDirectContext::_CommitBarriers ()
	{
		auto* bar = _mngr.GetBarriers();
		if_unlikely( bar != null )
		{
			PipelineBarrier( *bar );
			_mngr.ClearBarriers();
		}
	}

	
} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
