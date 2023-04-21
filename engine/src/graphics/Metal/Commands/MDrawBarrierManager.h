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
	// variables
	private:
		const MPrimaryCmdBufState	_primaryState;
		Ptr<MDrawCommandBatch>		_batch;		// can be null
		
		MDependencyInfo				_barrier;

		MResourceManager &			_resMngr;


	// methods
	public:
		explicit MDrawBarrierManager (Ptr<MDrawCommandBatch> batch)				__NE___;
		explicit MDrawBarrierManager (const MPrimaryCmdBufState &primaryState)	__NE___;
		
		template <typename ...IDs>
		ND_ decltype(auto)			Get (IDs ...ids)							__Th___	{ return _resMngr.GetResourcesOrThrow( ids... ); }
		
		ND_ MDevice const&			GetDevice ()								C_NE___	{ return _resMngr.GetDevice(); }
		ND_ MResourceManager&		GetResourceManager ()						C_NE___	{ return _resMngr; }
		ND_ FrameUID				GetFrameId ()								C_NE___	{ return _primaryState.frameId; }
		ND_ EQueueType				GetQueueType ()								C_NE___	{ return EQueueType::Graphics; }
		ND_ auto const&				GetPrimaryCtxState ()						C_NE___	{ return _primaryState; }

		ND_ bool					IsSecondary ()								C_NE___	{ return _batch != null; }
		ND_ Ptr<MDrawCommandBatch>	GetBatchPtr ()								C_NE___	{ return _batch.get(); }
		
		ND_ const MDependencyInfo*	GetBarriers ()								__NE___;
		ND_ bool					NoPendingBarriers ()						C_NE___;
		ND_ bool					HasPendingBarriers ()						C_NE___	{ return not NoPendingBarriers(); }
		
		ND_ uint					GetAttachmentIndex (AttachmentName name)	C_NE___;

		void  ClearBarriers ()													__NE___;
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState) __NE___;

	private:
		void  _Init ()															__NE___;
	};
	


/*
=================================================
	NoPendingBarriers
=================================================
*/
	forceinline bool  MDrawBarrierManager::NoPendingBarriers () C_NE___
	{
		return _barrier.scope == Default;
	}

/*
=================================================
	GetBarriers
=================================================
*/
	forceinline const MDependencyInfo*  MDrawBarrierManager::GetBarriers () __NE___
	{
		return HasPendingBarriers() ? &_barrier : null;
	}
	
/*
=================================================
	ClearBarriers
=================================================
*/
	forceinline void  MDrawBarrierManager::ClearBarriers () __NE___
	{
		_barrier.scope			= Default;
		_barrier.beforeStages	= Default;
		_barrier.afterStages	= Default;
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
