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
		virtual ~_MBaseDirectContext ();

		ND_ bool	IsValid ()	const	{ return _cmdbuf.IsValid(); }

	protected:
		_MBaseDirectContext (MCommandBuffer cmdbuf, NtStringView dbgName);

		void  _DebugMarker (MetalCommandEncoder encoder, NtStringView text) const;
		void  _PushDebugGroup (MetalCommandEncoder encoder, NtStringView text) const;
		void  _PopDebugGroup (MetalCommandEncoder encoder) const;

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
		explicit MBaseDirectContext (const RenderTask &task);
		MBaseDirectContext (const RenderTask &task, MCommandBuffer cmdbuf);
		~MBaseDirectContext () override;

	protected:
		void  _CommitBarriers ();

		ND_ bool	_NoPendingBarriers ()	const	{ return _mngr.NoPendingBarriers(); }
		ND_ auto&	_GetFeatures ()			const	{ return _mngr.GetDevice().GetFeatures(); }
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	constructor
=================================================
*/
	inline _MBaseDirectContext::_MBaseDirectContext (MCommandBuffer cmdbuf, NtStringView dbgName) :
		_cmdbuf{ RVRef( cmdbuf )}
	{
		DEBUG_ONLY( _cmdbuf.PushDebugGroup( dbgName ));
		Unused( dbgName );
	}

/*
=================================================
	destructor
=================================================
*/
	inline _MBaseDirectContext::~_MBaseDirectContext ()
	{
		DBG_CHECK_MSG( not IsValid(), "you forget to call 'EndCommandBuffer()' or 'ReleaseCommandBuffer()'" );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline MBaseDirectContext::MBaseDirectContext (const RenderTask &task, MCommandBuffer cmdbuf) :
		_MBaseDirectContext{ RVRef(cmdbuf), task.DbgFullName() },
		_mngr{ task }
	{
		ASSERT( _mngr.GetBatch().GetQueueType() == _cmdbuf.GetQueueType() );
	}

	inline MBaseDirectContext::MBaseDirectContext (const RenderTask &task) :
		MBaseDirectContext{
			task,
			MCommandBuffer::CreateCommandBuffer( task.GetBatchPtr()->GetQueueType() )}
	{}
	
/*
=================================================
	destructor
=================================================
*/
	inline MBaseDirectContext::~MBaseDirectContext ()
	{
		ASSERT( _NoPendingBarriers() );
	}

	
} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
