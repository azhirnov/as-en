// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Metal/Commands/MRenderTaskScheduler.h"
# include "graphics/Metal/Commands/MBarrierManager.h"
# include "graphics/Metal/Commands/MDrawBarrierManager.h"

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

		ND_ bool					IsValid ()				const	{ return _cmdbuf.IsValid(); }
		ND_ MetalCommandBufferRC	EndCommandBuffer ();
		ND_ MCommandBuffer		 	ReleaseCommandBuffer ();

	protected:
		_MBaseDirectContext (MCommandBuffer cmdbuf, NtStringView dbgName);

		void  _DebugMarker (MetalCommandEncoder encoder, NtStringView text) const;
		void  _PushDebugGroup (MetalCommandEncoder encoder, NtStringView text) const;
		void  _PopDebugGroup (MetalCommandEncoder encoder) const;

		//void  _DbgFillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data);
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
		explicit MBaseDirectContext (Ptr<MCommandBatch> batch);
		MBaseDirectContext (Ptr<MCommandBatch> batch, MCommandBuffer cmdbuf);
		~MBaseDirectContext () override;
		
		ND_ MetalCommandBufferRC	EndCommandBuffer ()		{ ASSERT( _NoPendingBarriers() );  return _MBaseDirectContext::EndCommandBuffer(); }
		ND_ MCommandBuffer		 	ReleaseCommandBuffer ()	{ ASSERT( _NoPendingBarriers() );  return _MBaseDirectContext::ReleaseCommandBuffer(); }

	protected:
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
	
/*
=================================================
	EndCommandBuffer
=================================================
*/
	inline MetalCommandBufferRC  _MBaseDirectContext::EndCommandBuffer ()
	{
		ASSERT( _cmdbuf.IsValid() );
		
		DEBUG_ONLY( _cmdbuf.PopDebugGroup() );

		return _cmdbuf.Release();
	}
	
/*
=================================================
	ReleaseCommandBuffer
=================================================
*/
	inline MCommandBuffer  _MBaseDirectContext::ReleaseCommandBuffer ()
	{
		ASSERT( _cmdbuf.IsValid() );

		DEBUG_ONLY( _cmdbuf.PopDebugGroup() );
		
		MCommandBuffer	tmp = RVRef(_cmdbuf);
		ASSERT( not _cmdbuf.IsValid() );
		return tmp;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline MBaseDirectContext::MBaseDirectContext (Ptr<MCommandBatch> batch, MCommandBuffer cmdbuf) :
		_MBaseDirectContext{ RVRef(cmdbuf), batch->DbgName() },
		_mngr{ batch }
	{
		ASSERT( batch->GetQueueType() == _cmdbuf.GetQueueType() );
	}

	inline MBaseDirectContext::MBaseDirectContext (Ptr<MCommandBatch> batch) :
		MBaseDirectContext{
			batch,
			MCommandBuffer::CreateCommandBuffer( batch->GetQueueType() )}
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
