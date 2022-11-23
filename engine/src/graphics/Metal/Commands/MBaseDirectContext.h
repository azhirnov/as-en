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
		MCommandBuffer	_cmdbuf;


	// methods
	public:
		virtual ~_MBaseDirectContext ()										__NE___;

	protected:
		_MBaseDirectContext (MCommandBuffer cmdbuf, NtStringView dbgName)	__Th___;

		ND_ bool	_IsValid ()												C_NE___	{ return _cmdbuf.IsValid(); }

		void  _DebugMarker (MetalCommandEncoder encoder, NtStringView text);
		void  _PushDebugGroup (MetalCommandEncoder encoder, NtStringView text);
		void  _PopDebugGroup (MetalCommandEncoder encoder);

		//void  _DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data);

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
		explicit MBaseDirectContext (const RenderTask &task)				__Th___;
		MBaseDirectContext (const RenderTask &task, MCommandBuffer cmdbuf)	__Th___;
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
	inline _MBaseDirectContext::_MBaseDirectContext (MCommandBuffer cmdbuf, NtStringView dbgName) __Th___ :
		_cmdbuf{ RVRef( cmdbuf )}
	{
		CHECK_THROW( _IsValid() );

		DEBUG_ONLY( _cmdbuf.PushDebugGroup( dbgName ));
		Unused( dbgName );
	}

/*
=================================================
	destructor
=================================================
*/
	inline _MBaseDirectContext::~_MBaseDirectContext () __NE___
	{
		DBG_CHECK_MSG( not _IsValid(), "you forget to call 'EndCommandBuffer()' or 'ReleaseCommandBuffer()'" );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline MBaseDirectContext::MBaseDirectContext (const RenderTask &task, MCommandBuffer cmdbuf) __Th___ :
		_MBaseDirectContext{ RVRef(cmdbuf), task.DbgFullName() },	// throw
		_mngr{ task }
	{
		ASSERT( _mngr.GetBatch().GetQueueType() == _cmdbuf.GetQueueType() );
	}

	inline MBaseDirectContext::MBaseDirectContext (const RenderTask &task) __Th___ :
		MBaseDirectContext{	// throw
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
