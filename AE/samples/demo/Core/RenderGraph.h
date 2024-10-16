// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/GraphicsImpl.h"

namespace AE::Samples::Demo
{

	//
	// Render Graph
	//

	class RenderGraph
	{
	// variables
	private:
		Atomic<uint>				_submitIdx	{};

		RenderTaskScheduler&		_rts;
		const FrameUID				_prevFrameId;

		Ptr<IOutputSurface>			_surface;
		AsyncTask					_acquireSurfaceTask;
		CommandBatchPtr				_uiBatch;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)

		static constexpr uint		_UploadBatch		= 0;
		static constexpr uint		_UIBatch			= GraphicsConfig::MaxPendingCmdBatches-2;
		static constexpr uint		_ReadbackBatch		= GraphicsConfig::MaxPendingCmdBatches-1;
		static constexpr uint		_RenderBatch_First	= 1;
		static constexpr uint		_RenderBatch_Last	= _UIBatch-1;

		StaticAssert( _RenderBatch_First < _RenderBatch_Last );


	// methods
	public:
		RenderGraph ();


		// single threaded
		ND_ bool		BeginFrame (Ptr<IOutputSurface>		surface,
									const BeginFrameConfig	&cfg	= Default);

		template <typename ...Deps>
		ND_ AsyncTask	EndFrame (ArrayView<AsyncTask>	presentDeps,
								  const Tuple<Deps...>	&endDeps);

		ND_ AsyncTask	BeginOnSurface (const CommandBatchPtr	&batch,
										ArrayView<AsyncTask>	deps	= Default);


		ND_ FrameUID	GetPrevFrameId ()	const	{ return _prevFrameId; }
		ND_ FrameUID	GetNextFrameId ()	const	{ return _prevFrameId.Next(); }
		ND_ auto		GetSurface ()		const	{ DRC_SHAREDLOCK( _drCheck );  return _surface; }
		ND_ auto		GetSurfaceArg ()	const	{ DRC_SHAREDLOCK( _drCheck );  return ArgRef( *_surface ); }


		// thread safe
		ND_ CommandBatchPtr		Upload ();
		ND_ CommandBatchPtr		Readback ();
		ND_ CommandBatchPtr		Render (StringView name);
		ND_ CommandBatchPtr		UI ();
	};


/*
=================================================
	constructor
=================================================
*/
	inline RenderGraph::RenderGraph () :
		_rts{ GraphicsScheduler() },
		_prevFrameId{ _rts.GetFrameId() }
	{}

/*
=================================================
	BeginFrame
=================================================
*/
	inline bool  RenderGraph::BeginFrame (Ptr<IOutputSurface> surface, const BeginFrameConfig &cfg)
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _prevFrameId == _rts.GetFrameId() );

		_surface		= surface;
		_submitIdx.store( _RenderBatch_First );

		return _rts.BeginFrame( cfg );
	}

/*
=================================================
	EndFrame
=================================================
*/
	template <typename ...Deps>
	AsyncTask  RenderGraph::EndFrame (ArrayView<AsyncTask> presentDeps, const Tuple<Deps...> &endDeps)
	{
		DRC_EXLOCK( _drCheck );

		AsyncTask	present;
		if ( _surface and _acquireSurfaceTask )
		{
			present = _surface->End( presentDeps );
			CHECK_ERR( present );

			_surface = null;
		}

		AsyncTask	end_frame = _rts.EndFrame( TupleConcat( endDeps, Tuple{present} ));

		// start next frame only after present
		_rts.AddNextFrameDeps( present );

		_rts.SkipCmdBatches( EQueueType::Graphics, UMax );

		return end_frame;
	}

/*
=================================================
	BeginOnSurface
=================================================
*/
	inline AsyncTask  RenderGraph::BeginOnSurface (const CommandBatchPtr &batch, ArrayView<AsyncTask> deps)
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _surface );

		_acquireSurfaceTask = _surface->Begin( batch, (_uiBatch ? _uiBatch : batch), deps );
		return _acquireSurfaceTask;
	}

/*
=================================================
	Upload
=================================================
*/
	inline CommandBatchPtr  RenderGraph::Upload ()
	{
		DRC_SHAREDLOCK( _drCheck );
		return _rts.BeginCmdBatch( EQueueType::Graphics, _UploadBatch, {"Upload"} );
	}

/*
=================================================
	Readback
=================================================
*/
	inline CommandBatchPtr  RenderGraph::Readback ()
	{
		DRC_SHAREDLOCK( _drCheck );
		return _rts.BeginCmdBatch( EQueueType::Graphics, _ReadbackBatch, {"Readback"} );
	}

/*
=================================================
	Render
=================================================
*/
	inline CommandBatchPtr  RenderGraph::Render (StringView name)
	{
		DRC_SHAREDLOCK( _drCheck );

		uint	idx = _RenderBatch_First;
		for (;;)
		{
			if_likely( _submitIdx.CAS( INOUT idx, idx+1 ))
				break;

			ASSERT( idx >= _RenderBatch_First );
			ASSERT( idx <  _RenderBatch_Last );

			if ( idx < _RenderBatch_First or idx > _RenderBatch_Last )
				return null;	// overflow

			ThreadUtils::Pause();
		}

		CmdBatchDesc	desc;
		desc.queue		= EQueueType::Graphics;
		desc.submitIdx	= idx;
		desc.dbg		= {name};

		// first batch on graphics queue must reset queries
		if ( idx == _RenderBatch_First )
			desc.ResetQuery();

		return  _rts.BeginCmdBatch( desc );
	}

/*
=================================================
	UI
=================================================
*/
	inline CommandBatchPtr  RenderGraph::UI ()
	{
		DRC_SHAREDLOCK( _drCheck );

		_uiBatch = _rts.BeginCmdBatch( EQueueType::Graphics, _UIBatch, {"UI"} );
		return _uiBatch;
	}


} // AE::Samples::Demo
