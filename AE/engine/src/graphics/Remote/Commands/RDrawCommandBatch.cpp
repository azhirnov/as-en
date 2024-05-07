// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Commands/RDrawCommandBatch.h"
# include "graphics/Remote/RRenderTaskScheduler.h"

namespace AE::Graphics
{
#	include "graphics/Private/DrawCommandBatch.cpp.h"

	using namespace AE::RemoteGraphics;

/*
=================================================
	GetCommands
----
	not thread safe !!!
=================================================
*/
	void  RDrawCommandBatch::CmdBufPool::GetCommands (OUT RmDrawCommandBufferID* cmdbufs, OUT uint &cmdbufCount, uint maxCount) __NE___
	{
		Unused( _GetBakedCommands( OUT cmdbufs, OUT cmdbufCount, maxCount,
			[](OUT RmDrawCommandBufferID& dst, auto& src, uint)
			{
				dst = src.baked.Release();
				PlacementDelete( src.baked );
				return true;
			}));

		_cmdTypes.store( 0, EMemoryOrder::Release );
	}

/*
=================================================
	GetCmdBuffers
----
	same as 'EndAllSecondary()' for Metal
=================================================
*/
	bool  RDrawCommandBatch::GetCmdBuffers (OUT uint &count, INOUT StaticArray< RmDrawCommandBufferID, GraphicsConfig::MaxCmdBufPerBatch > &cmdbufs) __NE___
	{
		EStatus	old_status = _status.Set( EStatus::Submitted );
		CHECK_ERR( AnyEqual( old_status, EStatus::Recording, EStatus::Submitted, EStatus::Pending ));

		_cmdPool.Lock();
		CHECK_ERR( _cmdPool.IsReady() );

		_cmdPool.GetCommands( OUT cmdbufs.data(), OUT count, uint(cmdbufs.size()) );
		return true;
	}

/*
=================================================
	_Create
=================================================
*
	bool  RDrawCommandBatch::_Create (const PrimaryCmdBufState_t &primaryState,
									  ArrayView<Viewport_t> viewports, ArrayView<Scissor_t> scissors,
									  DebugLabel dbg) __NE___
	{
		CHECK_ERR( primaryState.IsValid() );
		ASSERT( primaryState.useSecondaryCmdbuf );

		_cmdPool.Reset();

		_primaryState	= primaryState;
		_viewports		= viewports;
		_scissors		= scissors;

		GFX_DBG_ONLY(
			_profiler = GraphicsScheduler().GetProfiler();
			if ( _profiler )
				_profiler->BeginDrawBatch( this, dbg.label );

			_dbgName	= dbg.label;
			_dbgColor	= dbg.color;
		)
		Unused( dbg );

		CHECK_ERR( _status.Set( EStatus::Destroyed, EStatus::Recording ));
		return true;
	}
*/

} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
