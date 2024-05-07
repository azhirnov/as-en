// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/RResourceManager.h"
# include "graphics/Remote/Commands/RDrawCommandBatch.h"

namespace AE::Graphics::_hidden_
{

	//
	// Remote Graphics Draw Command Context Barrier Manager
	//

	class RDrawBarrierManager
	{
	// variables
	private:
		const RPrimaryCmdBufState			_primaryState;
		const Ptr<RDrawCommandBatch>		_batch;			// can be null
		const uint							_drawIndex	= UMax;

		RResourceManager &					_resMngr;

		RC<ArrayWStream>					_memStream;
		Unique<Serializing::Serializer>		_ser;
		uint								_cmdCount	= 0;


	// methods
	public:
		RDrawBarrierManager (Ptr<RDrawCommandBatch> batch, uint drawIdx)		__NE___;
		explicit RDrawBarrierManager (const RPrimaryCmdBufState &primaryState)	__NE___;

		template <typename ...IDs>
		ND_ decltype(auto)			Get (IDs ...ids)							__Th___	{ return _resMngr.GetResourcesOrThrow( ids... ); }

		ND_ RDevice const&			GetDevice ()								C_NE___	{ return _resMngr.GetDevice(); }
		ND_ RResourceManager&		GetResourceManager ()						C_NE___	{ return _resMngr; }
		ND_ FrameUID				GetFrameId ()								C_NE___	{ return _primaryState.frameId; }
		ND_ EQueueType				GetQueueType ()								C_NE___	{ return EQueueType::Graphics; }
		ND_ auto const&				GetPrimaryCtxState ()						C_NE___	{ return _primaryState; }

		ND_ bool					IsSecondary ()								C_NE___	{ return _batch != null; }
		ND_ Ptr<RDrawCommandBatch>	GetBatchPtr ()								C_NE___	{ return _batch.get(); }
		ND_ uint					GetDrawOrderIndex ()						C_NE___	{ return _drawIndex; }

		ND_ RDependencyInfo			GetBarriers ()								__NE___;
		ND_ bool					NoPendingBarriers ()						C_NE___	{ return _cmdCount == 0; }
		ND_ bool					HasPendingBarriers ()						C_NE___	{ return _cmdCount != 0; }

		ND_ uint					GetAttachmentIndex (AttachmentName::Ref)	C_NE___;

			void					ClearBarriers ()							__NE___;
	};


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_REMOTE_GRAPHICS
