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
			ND_ CmdBatchBuilder &&	UseResource (ImageID      id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (BufferID     id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTGeometryID id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTSceneID    id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
		//	ND_ CmdBatchBuilder &&	UseResource (ImageViewID  id, EResourceState initial, EResourceState final)	rvNE___;
		//	ND_ CmdBatchBuilder &&	UseResource (BufferViewID id, EResourceState initial, EResourceState final)	rvNE___;
		
			ND_ CmdBatchBuilder &&	UseResource (ImageID      id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (BufferID     id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTGeometryID id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTSceneID    id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
			
			ND_ CmdBatchBuilder &&	UseResource (ImageID      id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (BufferID     id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTGeometryID id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
			ND_ CmdBatchBuilder &&	UseResource (RTSceneID    id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
		//	ND_ CmdBatchBuilder &&	UseResource (ImageViewID  id)												rvNE___;
		//	ND_ CmdBatchBuilder &&	UseResource (BufferViewID id)												rvNE___;
		
			ND_ CmdBatchBuilder &&	UploadMemory ()																rvNE___;
			ND_ CmdBatchBuilder &&	ReadbackMemory ()															rvNE___;

			ND_ RGCommandBatchPtr	Begin ()																	rvNE___	{ return RVRef(_batch); }
		};


	// variables
	private:
		GRenderTaskScheduler&		_rts;
		FrameUID					_prevFrameId;

		Frames_t					_frames;
		SemToBatch_t				_semToBatch;
		RGBatchDataPool_t			_rgDataPool;
		
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


	// input surface //
	
	// output surface //


	// graph //
	
		ND_ CmdBatchBuilder  CmdBatch (EQueueType queue, DebugLabel dbg)		__NE___	{ return CmdBatchBuilder{ *this, _CmdBatch( queue, dbg )}; }


	private:
		ND_ PerFrame &			_CurrentFrame ()								__NE___	{ return _frames[ _prevFrameId.Index() ]; }
		ND_ RGCommandBatchPtr	_CmdBatch (EQueueType queue, DebugLabel dbg)	__NE___;
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

		auto&	f = _CurrentFrame();

		for (auto& q : f.queues) {
			q.submitIdx = 0;
		}
		_rgDataPool.clear();
		_semToBatch.clear();

		return _rts.BeginFrame( cfg, deps );	// throw
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

		// TODO present

		AsyncTask	end_frame	= _rts.EndFrame( deps );	// throw
		auto&		f			= _CurrentFrame();
		
		for (usize i = 0; i < f.queues.size(); ++i)
		{
			if ( f.queues[i].submitIdx > 0 )
				_rts.SkipCmdBatches( EQueueType(i), UMax );
		}

		return end_frame;
	}


} // AE::RG::_hidden_
