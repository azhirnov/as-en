// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	thread-safe:  no
*/

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
			uint				submitIdx	= 0;
		};
		using Queues_t = StaticArray< PerQueue, uint(EQueueType::_Count) >;


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
			RenderGraph &		_rg;
			RGCommandBatchPtr	_batch;

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

			template <typename ArrayType>
			ND_ CmdBatchBuilder &&	UseResources (const ArrayType &, EResourceState initial, EResourceState final) rvNE___;

			template <typename ArrayType>
			ND_ CmdBatchBuilder &&	UseResources (const ArrayType &ids, EResourceState initialOrFinal)			rvNE___;

			template <typename ArrayType>
			ND_ CmdBatchBuilder &&	UseResources (const ArrayType &ids)											rvNE___;

			// GPU -> CPU sync
			// Batch is submitted as single command and can be synchronized with host (CPU) only before and after batch command execution.
			// So Readback sync is defined for command batch instead of specific command buffer.
			//
			// ReadbackMemory	- barrier: CopyDst -> HostRead
			//
			ND_ CmdBatchBuilder &&	ReadbackMemory ()															rvNE___;

			ND_ RGCommandBatchPtr	Begin ()																	rvNE___	{ return RVRef(_batch); }
		};


	// variables
	private:
		RenderTaskScheduler&		_rts;
		FrameUID					_prevFrameId;
		Queues_t					_queues;

		bool						_resetQuery		: 1;
		const bool					_hostQueryReset	: 1;

		// data for current frame
		SemToBatch_t				_semToBatch;
		OutputSurfaces_t			_outSurfaces;

		alignas(AE_CACHE_LINE)
		  RGBatchDataPool_t			_rgDataPool;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		RenderGraph ()															__NE___;
		~RenderGraph ()															__NE___;


	// frame scope //
		ND_ bool		WaitNextFrame (const EThreadArray &	threads,
									   nanoseconds			timeout)			__NE___	{ return _rts.WaitNextFrame( threads, timeout ); }

		ND_ bool		BeginFrame (const BeginFrameConfig &cfg = Default)		__NE___;

		template <typename ...Deps>
		ND_ AsyncTask	EndFrame (const Tuple<Deps...>	&deps)					__NE___;

		// alternative version
			void		OnBeginFrame (FrameUID prevFrameId)						__NE___;
			void		OnEndFrame ()											__NE___;

		ND_ FrameUID	GetFrameId ()											C_NE___	{ return _rts.GetFrameId(); }
		ND_ FrameUID	GetPrevFrameId ()										C_NE___	{ return _prevFrameId; }
		ND_ FrameUID	GetNextFrameId ()										C_NE___	{ return _prevFrameId.Next(); }

		ND_ bool		WaitAll (milliseconds timeout)							__NE___;


	// surface //
		ND_ AsyncTask	BeginOnSurface (Ptr<App::IOutputSurface>	surface,
										RGCommandBatchPtr			batch)		__NE___;


	// graph //
		ND_ CmdBatchBuilder  CmdBatch (EQueueType queue, DebugLabel dbg)		__NE___	{ return CmdBatchBuilder{ *this, _CmdBatch( queue, dbg )}; }


	private:
		ND_ RGCommandBatchPtr	_CmdBatch (EQueueType queue, DebugLabel dbg)	__NE___;

			void				_ClearCurrentFrame ()							__NE___;

		ND_ FixedArray<AsyncTask, MaxOutSurfaces>  _PresentSurfaces ()			C_NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	BeginFrame
=================================================
*/
	inline bool  RenderGraph::BeginFrame (const BeginFrameConfig &cfg) __NE___
	{
		auto	fid	= _rts.GetFrameId();
		bool	res = _rts.BeginFrame( cfg );

		OnBeginFrame( fid );
		return res;
	}

	inline void  RenderGraph::OnBeginFrame (const FrameUID prevFrameId) __NE___
	{
		DRC_EXLOCK( _drCheck );

		_prevFrameId = prevFrameId;
		ASSERT( prevFrameId.Next() == _rts.GetFrameId() );

		for (auto& q : _queues) {
			q.submitIdx = 0;
		}
		_resetQuery = not _hostQueryReset;

		_ClearCurrentFrame();
	}

/*
=================================================
	EndFrame
=================================================
*/
	template <typename ...Deps>
	AsyncTask  RenderGraph::EndFrame (const Tuple<Deps...> &deps) __NE___
	{
		DRC_EXLOCK( _drCheck );

		// present output surfaces
		const auto	present_tasks = _PresentSurfaces();

		AsyncTask	end_frame	= _rts.EndFrame( deps );

		for (usize i = 0; i < _queues.size(); ++i)
		{
			if ( _queues[i].submitIdx > 0 )
				_rts.SkipCmdBatches( EQueueType(i), UMax );
		}

		// start next frame only after present
		_rts.AddNextFrameDeps( present_tasks );

		return end_frame;
	}

	inline void  RenderGraph::OnEndFrame () __NE___
	{
		DRC_EXLOCK( _drCheck );

		// present output surfaces
		const auto	present_tasks = _PresentSurfaces();

		for (usize i = 0; i < _queues.size(); ++i)
		{
			if ( _queues[i].submitIdx > 0 )
				_rts.SkipCmdBatches( EQueueType(i), UMax );
		}

		// start next frame only after present
		_rts.AddNextFrameDeps( present_tasks );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	UseResources
=================================================
*/
	template <typename ArrayType>
	RenderGraph::CmdBatchBuilder&&  RenderGraph::CmdBatchBuilder::UseResources (const ArrayType &ids, EResourceState initial, EResourceState final) rvNE___
	{
		for (auto& id : ArrayView{ids})
		{
			StaticAssert( IsHandleTmpl< decltype(id) >);
			_UseResource( ResourceKey{id}, initial, final );
		}
		return RVRef(*this);
	}

	template <typename ArrayType>
	RenderGraph::CmdBatchBuilder&&  RenderGraph::CmdBatchBuilder::UseResources (const ArrayType &ids, EResourceState initialOrFinal) rvNE___
	{
		for (auto& id : ArrayView{ids})
		{
			StaticAssert( IsHandleTmpl< decltype(id) >);
			_UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );
		}
		return RVRef(*this);
	}

	template <typename ArrayType>
	RenderGraph::CmdBatchBuilder&&  RenderGraph::CmdBatchBuilder::UseResources (const ArrayType &ids) rvNE___
	{
		for (auto& id : ArrayView{ids})
		{
			StaticAssert( IsHandleTmpl< decltype(id) >);
			_UseResource( ResourceKey{id}, Default, Default );
		}
		return RVRef(*this);
	}


} // AE::RG::_hidden_
