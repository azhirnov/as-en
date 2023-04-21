// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/RenderGraph/ResStateTracker.h"
#include "graphics/RenderGraph/RGCommandBatch.h"

namespace AE::RG::_hidden_
{

	//
	// Render Graph
	//

	class RenderGraph final : public ResStateTracker
	{
	// types
	private:
		using RGBatchData		= RGCommandBatchPtr::RGBatchData;
		using RGBatchDataPool_t	= FixedArray< RGBatchData, GraphicsConfig::MaxPendingCmdBatches*2 >;
		
		using SemToBatch_t		= FlatHashMap< ulong, CommandBatchPtr >;

		struct PerQueue
		{
			uint	submitIdx	= 0;
		};
		using Queues_t = StaticArray< PerQueue, uint(EQueueType::_Count) >;
		

		struct PerFrame
		{
			Queues_t	queues;
		};
		using Frames_t	= StaticArray< PerFrame, GraphicsConfig::MaxFrames >;


		struct OutSurfaceInfo
		{
			AsyncTask			acquireImageTask;
			RGCommandBatchPtr	forBatch;
		};
		static constexpr uint	MaxOutSurfaces = 4;
		using OutputSurfaces_t	= FixedMap< Ptr<App::IOutputSurface>, OutSurfaceInfo, MaxOutSurfaces >;


		struct CmdBatchBuilder
		{
		// variables
		private:
			RenderGraph &			_rg;
			RGCommandBatchPtr		_batch;

		// methods
		private:
			friend class RenderGraph;
			CmdBatchBuilder (RenderGraph &rg, RGCommandBatchPtr batch)											__NE___	: _rg{rg}, _batch{RVRef(batch)} {}

			void  _UseResource (ResourceKey key, EResourceState initial, EResourceState final)					__NE___;
		public:
			// 'initial' - resource state in first render task (command buffer)
			// 'final'   - resource state in last render task
			ND_ CmdBatchBuilder &&	UseResource (ImageID      id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (BufferID     id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTGeometryID id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTSceneID    id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (ImageViewID  id, EResourceState initial, EResourceState final)	rvNE___;
			ND_ CmdBatchBuilder &&	UseResource (BufferViewID id, EResourceState initial, EResourceState final)	rvNE___;
		
			ND_ CmdBatchBuilder &&	UseResource (ImageID      id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (BufferID     id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTGeometryID id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTSceneID    id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (ImageViewID  id, EResourceState initialOrFinal)				rvNE___	{ return RVRef(*this).UseResource( id, initialOrFinal, initialOrFinal ); }
			ND_ CmdBatchBuilder &&	UseResource (BufferViewID id, EResourceState initialOrFinal)				rvNE___	{ return RVRef(*this).UseResource( id, initialOrFinal, initialOrFinal ); }
			
			// resource will be in default state
			ND_ CmdBatchBuilder &&	UseResource (ImageID      id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (BufferID     id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTGeometryID id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTSceneID    id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (ImageViewID  id)												rvNE___	{ return RVRef(*this).UseResource( id, Default, Default ); }
			ND_ CmdBatchBuilder &&	UseResource (BufferViewID id)												rvNE___	{ return RVRef(*this).UseResource( id, Default, Default ); }
		
			template <typename ID>
			ND_ CmdBatchBuilder &&	UseResources (ArrayView<ID>, EResourceState initial, EResourceState final)	rvNE___;

			template <typename ID>
			ND_ CmdBatchBuilder &&	UseResources (ArrayView<ID> ids, EResourceState initialOrFinal)				rvNE___;

			template <typename ID>
			ND_ CmdBatchBuilder &&	UseResources (ArrayView<ID> ids)											rvNE___;

			ND_ CmdBatchBuilder &&	UploadMemory ()																rvNE___;
			ND_ CmdBatchBuilder &&	ReadbackMemory ()															rvNE___;

			ND_ RGCommandBatchPtr	Begin ()																	rvNE___	{ return RVRef(_batch); }
		};

		static constexpr auto	DefaultWaitTime	= GRenderTaskScheduler::DefaultWaitTime;


	// variables
	private:
		GRenderTaskScheduler&		_rts;
		FrameUID					_prevFrameId;
		Frames_t					_frames;

		// data for cyrrent frame
		AsyncTask					_beginFrame;
		SemToBatch_t				_semToBatch;
		RGBatchDataPool_t			_rgDataPool;
		OutputSurfaces_t			_outSurfaces;
		
		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		RenderGraph ()															__NE___;
		~RenderGraph ()															__NE___;

		
	// frame scope //

		template <typename ...Deps>
		ND_ AsyncTask	BeginFrame (const BeginFrameConfig	&cfg	= Default,
									const Tuple<Deps...>	&deps	= Default)	__Th___;

		template <typename ...Deps>
		ND_ AsyncTask	EndFrame (const Tuple<Deps...>	&deps)					__Th___;
		
		ND_ FrameUID	GetPrevFrameId ()										C_NE___	{ return _prevFrameId; }
		ND_ FrameUID	GetNextFrameId ()										C_NE___	{ return _prevFrameId.Next(); }

		ND_ bool		WaitAll (milliseconds timeout = DefaultWaitTime)		__NE___;


	// surface //
		ND_ AsyncTask	BeginOnSurface (Ptr<App::IOutputSurface> surface, RGCommandBatchPtr batch) __NE___;


	// graph //
	
		ND_ CmdBatchBuilder  CmdBatch (EQueueType queue, DebugLabel dbg)		__NE___	{ return CmdBatchBuilder{ *this, _CmdBatch( queue, dbg )}; }


	private:
		ND_ PerFrame &			_CurrentFrame ()								__NE___	{ return _frames[ _prevFrameId.Index() ]; }
		ND_ RGCommandBatchPtr	_CmdBatch (EQueueType queue, DebugLabel dbg)	__NE___;

			void				_ClearCurrentFrame ()							__NE___;
	};
//-----------------------------------------------------------------------------

	
	
/*
=================================================
	BeginFrame
=================================================
*/
	template <typename ...Deps>
	AsyncTask  RenderGraph::BeginFrame (const BeginFrameConfig &cfg, const Tuple<Deps...> &deps) __Th___
	{
		DRC_EXLOCK( _drCheck );

		_prevFrameId = _rts.GetFrameId();

		for (auto& q : _CurrentFrame().queues) {
			q.submitIdx = 0;
		}
		_ClearCurrentFrame();

		_beginFrame = _rts.BeginFrame( cfg, deps );	// throw
		return _beginFrame;
	}
	
/*
=================================================
	EndFrame
=================================================
*/
	template <typename ...Deps>
	AsyncTask  RenderGraph::EndFrame (const Tuple<Deps...> &deps) __Th___
	{
		DRC_EXLOCK( _drCheck );

		// present output surfaces
		FixedArray< AsyncTask, MaxOutSurfaces >		present_tasks;

		for (auto [surface, info] : _outSurfaces)
		{
			ASSERT( info.acquireImageTask );

			if_likely( auto task = surface->End( Default ))	// will present after batch submission
				present_tasks.push_back( task );
		}
		
		AsyncTask	end_frame	= _rts.EndFrame( deps );	// throw
		auto&		f			= _CurrentFrame();
		
		for (usize i = 0; i < f.queues.size(); ++i)
		{
			if ( f.queues[i].submitIdx > 0 )
				_rts.SkipCmdBatches( EQueueType(i), UMax );
		}
		
		// start next frame only after present
		_rts.AddNextFrameDeps( present_tasks );

		_beginFrame = null;

		return end_frame;
	}
//-----------------------------------------------------------------------------
	


/*
=================================================
	UseResources
=================================================
*/
	template <typename ID>
	RenderGraph::CmdBatchBuilder&&  RenderGraph::CmdBatchBuilder::UseResources (ArrayView<ID> ids, EResourceState initial, EResourceState final) rvNE___
	{
		for (auto id : ids) {
			_UseResource( ResourceKey{id}, initial, final );
		}
		return RVRef(*this);
	}

	template <typename ID>
	RenderGraph::CmdBatchBuilder&&  RenderGraph::CmdBatchBuilder::UseResources (ArrayView<ID> ids, EResourceState initialOrFinal) rvNE___
	{
		for (auto id : ids) {
			_UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );
		}
		return RVRef(*this);
	}

	template <typename ID>
	RenderGraph::CmdBatchBuilder&&  RenderGraph::CmdBatchBuilder::UseResources (ArrayView<ID> ids) rvNE___
	{
		for (auto id : ids) {
			_UseResource( ResourceKey{id}, Default, Default );
		}
		return RVRef(*this);
	}


} // AE::RG::_hidden_
