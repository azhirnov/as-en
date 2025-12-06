// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::_Coro_
{
/*
=================================================
	DbgFullName
=================================================
*/
#if AE_DBG_GRAPHICS
	String  DrawTaskImpl::DbgFullName () C_NE___
	{
		return String{_batch->DbgName()} << " |" << ToString(_drawIndex) << "| " << DbgName();
	}
#endif

} // AE::_Coro_
//-----------------------------------------------------------------------------



namespace AE::Graphics
{
/*
=================================================
	_Create
=================================================
*/
	bool  DrawCommandBatch::_Create (const PrimaryCmdBufState_t &primaryState,
									 ArrayView<Viewport_t> viewports, ArrayView<Scissor_t> scissors,
									 DebugLabel dbg) __NE___
	{
		CHECK_ERR( primaryState.IsValid() );
		ASSERT( primaryState.useSecondaryCmdbuf != 0 );

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

/*
=================================================
	_ReleaseObject
=================================================
*/
	void  DrawCommandBatch::_ReleaseObject () __NE___
	{
		MemoryBarrier( EMemoryOrder::Acquire );

		const EStatus	status = _status.load();

		if_unlikely( status != EStatus::Submitted )
		{
			_cmdPool.Reset();
		}
		ASSERT( status == EStatus::Submitted );

		// delete anyway
		_status.store( EStatus::Destroyed );

		_primaryState = Default;
		_viewports.clear();
		_scissors.clear();

	  #ifdef AE_ENABLE_METAL
		_encoder = null;
	  #endif

		GFX_DBG_ONLY(
			_profiler = null;
		)
		MemoryBarrier( EMemoryOrder::Release );
		RenderTaskScheduler::DrawCommandBatchApi::Recycle( this );
	}

} // AE::Graphics
