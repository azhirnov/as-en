// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Metal/MResourceManager.h"
# include "graphics/Metal/MRenderTaskScheduler.h"
# include "graphics/Metal/Commands/MBarrierManager.h"

namespace AE::Graphics::_hidden_
{

	//
	// Metal Draw Command Context Barrier Manager
	//

	class MDrawBarrierManager
	{
	// types
	public:
		using BarrierInfo = MBarrierManager::BarrierInfo;


	// variables
	private:
		const MPrimaryCmdBufState	_primaryState;
		Ptr<MDrawCommandBatch>		_batch;		// can be null
		
		MResourceManager &			_resMngr;


	// methods
	public:
		explicit MDrawBarrierManager (Ptr<MDrawCommandBatch> batch)				__NE___;
		explicit MDrawBarrierManager (const MPrimaryCmdBufState &primaryState)	__NE___;
		
		template <typename ID>
		ND_ auto*						GetPtr (ID id)							__NE___	{ return _resMngr.GetResource( id ); }

		template <typename ID>
		ND_ auto&						Get (ID id)								__Th___	{ return _resMngr.GetResourcesOrThrow( id ).Get<0>(); }
		
		template <typename ID0, typename ID1, typename ...IDs>
		ND_ auto						Get (ID0 id0, ID1 id1, IDs ...ids)		__Th___ { return _resMngr.GetResourcesOrThrow( id0, id1, ids... ); }
		
		ND_ MDevice const&				GetDevice ()							C_NE___	{ return _resMngr.GetDevice(); }
		ND_ MResourceManager&			GetResourceManager ()					C_NE___	{ return _resMngr; }
		ND_ FrameUID					GetFrameId ()							C_NE___	{ return _primaryState.frameId; }
		ND_ EQueueType					GetQueueType ()							C_NE___	{ return EQueueType::Graphics; }
		ND_ MPrimaryCmdBufState const&	GetPrimaryCtxState ()					C_NE___	{ return _primaryState; }

		ND_ bool						IsSecondary ()							C_NE___	{ return _batch != null; }
		ND_ MDrawCommandBatch *			GetBatchPtr ()							C_NE___	{ return _batch.get(); }
		
		ND_ const BarrierInfo*			GetBarriers ()							__NE___	{ return null; }	// TODO
		ND_ bool						NoPendingBarriers ()					C_NE___	{ return true; }
		ND_ bool						HasPendingBarriers ()					C_NE___	{ return false; }

		void  ClearBarriers ()													__NE___	{}
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState) __NE___	{ Unused( name, srcState, dstState ); }

	private:
		void  _Init ();
	};

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
