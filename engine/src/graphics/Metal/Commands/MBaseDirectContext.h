// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Metal/Commands/MBarrierManager.h"
# include "graphics/Metal/Commands/MDrawBarrierManager.h"
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
		~MBaseDirectContext ()												__NE_OV;

	protected:
		void  _CommitBarriers ();

		ND_ bool	_NoPendingBarriers ()									C_NE___	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetFeatures ()											C_NE___	{ return _mngr.GetDevice().GetFeatures(); }
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
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline MBaseDirectContext::MBaseDirectContext (const RenderTask &task, MCommandBuffer cmdbuf) __NE___ :
		_MBaseDirectContext{ RVRef(cmdbuf), DebugLabel{ task.DbgFullName() }},
		_mngr{ task }
	{
		ASSERT( _mngr.GetBatch().GetQueueType() == _cmdbuf.GetQueueType() );
	}

	inline MBaseDirectContext::MBaseDirectContext (const RenderTask &task) __NE___ :
		MBaseDirectContext{
			task,
			MCommandBuffer::CreateCommandBuffer( task.GetBatchPtr()->GetQueueType() )}
	{}
	
/*
=================================================
	destructor
=================================================
*/
	inline MBaseDirectContext::~MBaseDirectContext () __NE___
	{
		ASSERT( _NoPendingBarriers() );
	}

	
} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
