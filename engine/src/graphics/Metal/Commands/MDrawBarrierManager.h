// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Metal/MResourceManager.h"
# include "graphics/Metal/Commands/MRenderTaskScheduler.h"
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


	// methods
	public:
		explicit MDrawBarrierManager (Ptr<MDrawCommandBatch> batch);
		explicit MDrawBarrierManager (const MPrimaryCmdBufState &primaryState);

		template <typename ID>
		ND_ auto*						Get (ID id)						{ return GetResourceManager().GetResource( id ); }

		ND_ MDevice const&				GetDevice ()			const	{ return RenderTaskScheduler().GetDevice(); }
		ND_ MResourceManager&			GetResourceManager ()	const	{ return RenderTaskScheduler().GetResourceManager(); }
		ND_ FrameUID					GetFrameId ()			const	{ return _primaryState.frameId; }
		ND_ EQueueType					GetQueueType ()			const	{ return EQueueType::Graphics; }
		ND_ MPrimaryCmdBufState const&	GetPrimaryCtxState ()	const	{ return _primaryState; }

		ND_ bool						IsSecondary ()			const	{ return _batch != null; }
		//ND_ MDrawCommandBatch &		GetBatch ()				const	{ return *_batch; }
		//ND_ RC<MDrawCommandBatch>		GetBatchRC ()			const	{ return _batch->GetRC<VDrawCommandBatch>(); }
		
		ND_ const BarrierInfo*			GetBarriers ();
		ND_ bool						NoPendingBarriers () const	{ return true; }
		ND_ bool						HasPendingBarriers () const	{ return false; }

		void  ClearBarriers () {}
		void  AttachmentBarrier (AttachmentName name, EResourceState srcState, EResourceState dstState);

	private:
		void  _Init ();
	};

} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
