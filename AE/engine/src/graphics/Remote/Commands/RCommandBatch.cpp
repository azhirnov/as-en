// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RCommandBatch.h"
# include "graphics/Remote/RRenderTaskScheduler.h"

namespace AE::Graphics
{
#	include "graphics/Private/CommandBatch.cpp.h"

	using namespace AE::RemoteGraphics;

/*
=================================================
	GetCommands
----
	not thread safe !!!
=================================================
*/
	void  RCommandBatch::CmdBufPool::GetCommands (OUT RmCommandBufferID* cmdbufs, OUT uint &cmdbufCount, uint maxCount) __NE___
	{
		Unused( _GetBakedCommands( OUT cmdbufs, OUT cmdbufCount, maxCount,
			[](OUT RmCommandBufferID& dst, auto& src, uint)
			{
				dst = src.baked.Release();
				PlacementDelete( src.baked );
				return true;
			}));

		_cmdTypes.store( 0, EMemoryOrder::Release );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	RCommandBatch::RCommandBatch () __NE___
	{}

	RCommandBatch::~RCommandBatch () __NE___
	{
		CHECK( not _batchId );
		CHECK( not _tlSemaphore );
	}

/*
=================================================
	_Create
=================================================
*/
	bool  RCommandBatch::_Create (FrameUID frameId, const CmdBatchDesc &desc) __NE___
	{
		ASSERT( _batchId == Default );

		_submitMode	= ESubmitMode::Immediately;
		_queueType	= desc.queue;
		_frameId	= frameId;
		_submitIdx	= CheckCast<ubyte>(desc.submitIdx);
		_userData	= desc.userData;

		_perTaskBarriers.fill( null );

		GFX_DBG_ONLY(
			_profiler = GraphicsScheduler().GetProfiler();
			if ( _profiler )
				_profiler->BeginBatch( frameId, this, desc.dbg.label );

			_dbgName	= desc.dbg.label;
			_dbgColor	= desc.dbg.color;
		)

		Msg::RTS_CreateBatch				msg;
		RC<Msg::RTS_CreateBatch_Response>	res;

		msg.queue		= desc.queue;
		msg.flags		= desc.flags;
		msg.frameId		= frameId;
		msg.submitIdx	= desc.submitIdx;
		msg.dbgLabel	= DebugLabel{ _dbgName, _dbgColor };

		CHECK_ERR( GraphicsScheduler().GetDevice().SendAndWait( msg, OUT res ));

		_batchId		= res->batchId;
		_tlSemaphore	= res->semaphoreId;
		CHECK_ERR( _batchId );

		CHECK_ERR( _status.Set( EStatus::Destroyed, EStatus::Initial ));
		return true;
	}

/*
=================================================
	_ReleaseObject
=================================================
*/
	void  RCommandBatch::_ReleaseObject () __NE___
	{
		MemoryBarrier( EMemoryOrder::Acquire );

		const EStatus	status = _status.load();

		if_unlikely( status != EStatus::Completed )
		{
			_cmdPool.Reset();
		}
		ASSERT( status == EStatus::Completed );

		// delete anyway
		_status.store( EStatus::Destroyed );

		GFX_DBG_ONLY({
			EXLOCK( _onCompleteDepsGuard );
			CHECK( _onCompleteDeps.empty() );
		})
		GFX_DBG_ONLY({
			EXLOCK( _onSubmitDepsGuard );
			CHECK( _onSubmitDeps.empty() );
		})

		MemoryBarrier( EMemoryOrder::Release );
		RenderTaskScheduler::CommandBatchApi::Recycle( this );

		if ( _batchId )
		{
			Msg::RTS_DestroyBatch	msg;
			msg.batchId		= _batchId;
			msg.semaphoreId = _tlSemaphore;
			CHECK( GraphicsScheduler().GetDevice().Send( msg ));
		}
		_batchId		= Default;
		_tlSemaphore	= Default;
	}

/*
=================================================
	Wait
=================================================
*/
	bool  RCommandBatch::Wait (nanoseconds timeout) __NE___
	{
		CHECK_ERR( _batchId );

		Msg::RTS_WaitBatch			msg;
		RC<Msg::DefaultResponse>	res;

		msg.id		= Handle();
		msg.timeout	= timeout;

		CHECK_ERR( GraphicsScheduler().GetDevice().SendAndWait( msg, OUT res ));
		return res->ok;
	}

/*
=================================================
	GetSemaphore
=================================================
*/
	RemoteCmdBatchDependency  RCommandBatch::GetSemaphore () C_NE___
	{
		return { _tlSemaphore, _tlSemaphoreVal.load() };
	}

/*
=================================================
	_GetInputDependencies
=================================================
*/
	bool  RCommandBatch::_GetInputDependencies (OUT Pair<RmSemaphoreID, ulong>* deps, OUT uint &count, const usize maxCount) __NE___
	{
		count = 0;

		EXLOCK( _gpuInDepsGuard );
		if_likely( _gpuInDeps.empty() )
			return true;

		CHECK( _gpuInDeps.size() < maxCount );

		count = uint(Min( _gpuInDeps.size(), maxCount ));

		uint	i = 0;
		for (auto [sem, val] : _gpuInDeps) {
			deps[i++] = { sem, val };
		}

		_gpuInDeps.clear();
		return true;
	}

/*
=================================================
	_GetOutputDependencies
=================================================
*/
	bool  RCommandBatch::_GetOutputDependencies (OUT Pair<RmSemaphoreID, ulong>* deps, OUT uint &count, usize maxCount) __NE___
	{
		count = 0;

		EXLOCK( _gpuOutDepsGuard );
		if_likely( _gpuOutDeps.empty() )
			return true;

		CHECK( _gpuOutDeps.size() < maxCount );

		count = uint(Min( _gpuOutDeps.size(), maxCount ));

		uint	i = 0;
		for (auto [sem, val] : _gpuOutDeps) {
			deps[i++] = { sem, val };
		}

		_gpuOutDeps.clear();
		return true;
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
