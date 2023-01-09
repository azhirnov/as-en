// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/RenderGraph/ResStateTracker.h"

namespace AE::RG::_hidden_
{

	//
	// Render Grapg Command Batch pointer
	//
	class RGCommandBatchPtr
	{
	// types
	private:
		struct RenderTaskBuilder;
	public:
		class RGBatchData;


	// variables
	private:
		CommandBatchPtr		_cmdBatch;


	// methods
	public:
		RGCommandBatchPtr ()											__NE___	{}
		RGCommandBatchPtr (RC<CommandBatch> batch)						__NE___	: _cmdBatch{ RVRef(batch) } {}

		RGCommandBatchPtr (const RGCommandBatchPtr &)					__NE___ = default;
		RGCommandBatchPtr (RGCommandBatchPtr &&)						__NE___	= default;

		RGCommandBatchPtr&  operator = (const RGCommandBatchPtr &rhs)	__NE___	{ _cmdBatch = rhs._cmdBatch;		return *this; }
		RGCommandBatchPtr&  operator = (RGCommandBatchPtr && rhs)		__NE___	{ _cmdBatch = RVRef(rhs._cmdBatch);	return *this; }

		ND_ explicit operator bool ()									C_NE___	{ return bool{_cmdBatch}; }

		ND_ CommandBatchPtr		AsBatchRC ()							C_NE___	{ return _cmdBatch; }
		ND_ Ptr<CommandBatch>	AsBatch ()								C_NE___	{ return _cmdBatch.get(); }
		ND_ Ptr<RGBatchData>	AsRG ()									C_NE___;

		
	// render task api //

		template <typename TaskType, typename ...Ctor>
		ND_ RenderTaskBuilder	Task (Tuple<Ctor...>&&	ctor	= Default,
									  DebugLabel		dbg		= Default)	__NE___;
		
	  #ifdef AE_HAS_COROUTINE
		template <typename PromiseT>
		ND_ RenderTaskBuilder	Task (AE::Threading::CoroutineHandle<PromiseT>	handle,
									  DebugLabel								dbg		= Default)	__NE___;
	  #endif


	private:
		ND_ bool	_HasRenderTasks ()	__NE___	{ return _cmdBatch->CurrentCmdBufIndex() > 0; }
	};


	
	//
	// Render Graph Batch Data
	//
	class RGCommandBatchPtr::RGBatchData
	{
		friend class  RenderGraph;
		friend struct RGCommandBatchPtr::RenderTaskBuilder;

	// types
	private:
		struct InBatchState
		{
			EResourceState	initial;
			EResourceState	final;
			EResourceState	current;

			explicit InBatchState (EResourceState current)				__NE___ :
				initial{Default}, final{current}, current{current} {}

			InBatchState (EResourceState initial, EResourceState final) __NE___ :
				initial{initial}, final{final}, current{initial} {}
		};
		using ResStateInBatch_t	= FlatHashMap< ResourceKey, InBatchState, ResourceKeyHash >;
		using ResStateMap_t		= FlatHashMap< ResourceKey, EResourceState, ResourceKeyHash >;
		using PerTaskStates_t	= StaticArray< ResStateMap_t, GraphicsConfig::MaxCmdBufPerBatch >;
		using AccumBarriers_t	= CommandBatch::AccumBarriers_t;

		#if defined(AE_ENABLE_VULKAN)
			using PrimaryCmdBufState_t	= VPrimaryCmdBufState;

		#elif defined(AE_ENABLE_METAL)
			using PrimaryCmdBufState_t	= MPrimaryCmdBufState;

		#else
		#	error not implemented
		#endif


	// variables
	private:
		ResStateInBatch_t			_batchStates;
		PerTaskStates_t				_perTask;
		ResStateTracker &			_globalStates;

		Optional<AccumBarriers_t>	_initialBarriers;
		Optional<AccumBarriers_t>	_finalBarriers;
		
		DRC_ONLY( RWDataRaceCheck	_drCheck;)		// protects all except '_perTask'


	// methods
	public:
		explicit RGBatchData (ResStateTracker &gs)									__NE___ : _globalStates{gs} {}
		~RGBatchData ()																__NE___	{}


	// task api //
		template <typename ID, typename Ctx>
		void  ResourceState (uint taskIdx, Ctx &ctx, ID id, EResourceState state)	__Th___;

		template <typename Ctx>
		void  FinalBarriers (uint taskIdx, Ctx &ctx)								C_Th___;
		
		//void  SetRenderPassInitialStates (uint taskIdx, INOUT RenderPassDesc &)		__Th___;
		void  SetRenderPassFinalStates (uint taskIdx, const PrimaryCmdBufState_t &)	__Th___;
	};



	//
	// Render Task Builder
	//
	struct RGCommandBatchPtr::RenderTaskBuilder
	{
		friend class RGCommandBatchPtr;
		
	// types
	private:
		using AccumBarriers_t	= CommandBatch::AccumBarriers_t;
		using ResStateMap_t		= RGBatchData::ResStateMap_t;


	// variables
	private:
		CommandBatch &		_cmdBatch;
		RC<RenderTask>		_rtask;
		bool				_last			= false;

		AccumBarriers_t		_initialBarriers;
		AccumBarriers_t		_finalBarriers;


	// methods
	private:
		RenderTaskBuilder (CommandBatch &batch, RC<RenderTask> task)					__NE___;

			void  _UseResource (ResourceKey key, EResourceState state)					__NE___;
		
		ND_ RGBatchData &			_RGBatch ()											__NE___	{ return *Cast<RGBatchData>( _cmdBatch.GetUserData() ); }
		ND_ uint					_ExeIdx ()											C_NE___	{ return _rtask->GetExecutionIndex(); }
		ND_ ResStateMap_t &			_StateMap ()										__NE___	{ return _RGBatch()._perTask[ _ExeIdx() ]; }

	public:
	//	ND_ RenderTaskBuilder &&	UseResource (ImageViewID  id, EResourceState state)	rvNE___;
	//	ND_ RenderTaskBuilder &&	UseResource (BufferViewID id, EResourceState state)	rvNE___;
		ND_ RenderTaskBuilder &&	UseResource (ImageID      id, EResourceState state)	rvNE___	{ _UseResource( ResourceKey{id}, state );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (BufferID     id, EResourceState state)	rvNE___	{ _UseResource( ResourceKey{id}, state );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (RTGeometryID id, EResourceState state)	rvNE___	{ _UseResource( ResourceKey{id}, state );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (RTSceneID    id, EResourceState state)	rvNE___	{ _UseResource( ResourceKey{id}, state );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	Last ()												rvNE___	{ _last = true;  return RVRef(*this); }

		template <typename ...Deps>
		ND_ AsyncTask				Run (const Tuple<Deps...>& deps = Default)			rvNE___;
	};
//-----------------------------------------------------------------------------


		
/*
=================================================
	ResourceState
=================================================
*/
	template <typename ID, typename Ctx>
	void  RGCommandBatchPtr::RGBatchData::ResourceState (uint taskIdx, Ctx &ctx, ID id, EResourceState newState) __Th___
	{
		constexpr auto	default_state	= EResourceState::Invalidate | EResourceState::General;
		auto			[it, inserted]	= _perTask[ taskIdx ].emplace( ResourceKey{id}, default_state );	// throw
		EResourceState	old_state		= it->second;

		if_unlikely( AllBits( newState, EResourceState::Invalidate ))
		{
			it->second = old_state | EResourceState::Invalidate;
			return;
		}

		if constexpr( IsSameTypes< ID, ImageID >)
			ctx.ImageBarrier( id, old_state, newState );	// throw
		else
			ctx.MemoryBarrier( old_state, newState );		// throw
			
		it->second = newState;
	}
	
/*
=================================================
	FinalBarriers
=================================================
*/
	template <typename Ctx>
	void  RGCommandBatchPtr::RGBatchData::FinalBarriers (uint taskIdx, Ctx &ctx) C_Th___
	{
		DRC_SHAREDLOCK( _drCheck );
		
		auto&	map	= _perTask[ taskIdx ];
		for (const auto& [id, state] : _batchStates)
		{
			auto	it = map.find( id );
			if_likely( it != map.end() )
			{
				if_unlikely( it->second != state.final )
				{
					if ( id.IsImage() )
						ctx.ImageBarrier( id.AsImage(), it->second, state.final );	// throw
					else
						ctx.MemoryBarrier( it->second, state.final );				// throw
				}
			}
		}
		ctx.CommitBarriers();	// throw
	}
//-----------------------------------------------------------------------------

	
	
/*
=================================================
	AsRG
=================================================
*/
	inline Ptr<RGCommandBatchPtr::RGBatchData>  RGCommandBatchPtr::AsRG () C_NE___
	{
		return Cast<RGBatchData>( _cmdBatch->GetUserData() );
	}

/*
=================================================
	Task
=================================================
*/
	template <typename TaskType, typename ...Ctor>
	RGCommandBatchPtr::RenderTaskBuilder  RGCommandBatchPtr::Task (Tuple<Ctor...>&& ctorArgs, DebugLabel dbg) __NE___
	{
		RC<RenderTask>	task;
		
		PROFILE_ONLY(
			if ( dbg.color == DebugLabel::ColorTable.Undefined )
				dbg.color = AsBatch()->DbgColor();
		)
		try {
			task = ctorArgs.Apply([this, dbg] (auto&& ...args)
								  { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., AsBatchRC(), dbg ); });	// throw
		}
		catch(...) {}

		return RenderTaskBuilder{ *AsBatch(), RVRef(task) };
	}
		
#ifdef AE_HAS_COROUTINE
	template <typename PromiseT>
	RGCommandBatchPtr::RenderTaskBuilder  RGCommandBatchPtr::Task (AE::Threading::CoroutineHandle<PromiseT> handle, DebugLabel dbg) __NE___
	{
		RC<RenderTask>	task;
		
		PROFILE_ONLY(
			if ( dbg.color == DebugLabel::ColorTable.Undefined )
				dbg.color = AsBatch()->DbgColor();
		)
		try {
			task = MakeRC<AE::Threading::_hidden_::RenderTaskCoroutineRunner>( RVRef(handle), AsBatchRC(), dbg );	// throw
		}
		catch(...) {}

		return RenderTaskBuilder{ *AsBatch(), RVRef(task) };
	}
#endif
//-----------------------------------------------------------------------------


	
/*
=================================================
	constructor
=================================================
*/
	inline RGCommandBatchPtr::RenderTaskBuilder::RenderTaskBuilder (CommandBatch &batch, RC<RenderTask> task) __NE___ :
		_cmdBatch{ batch },
		_rtask{ RVRef(task) },
		_initialBarriers{ batch.DeferredBarriers() },
		_finalBarriers{ batch.DeferredBarriers() }
	{}
	
/*
=================================================
	Run
=================================================
*/
	template <typename ...Deps>
	AsyncTask  RGCommandBatchPtr::RenderTaskBuilder::Run (const Tuple<Deps...>& deps) rvNE___
	{
		const bool	is_first	= _rtask->IsFirstInBatch();
		auto&		rg_batch	= *Cast<RGBatchData>( _cmdBatch.GetUserData() );
		DRC_EXLOCK( rg_batch._drCheck );

		if_unlikely( is_first )
			_initialBarriers.Merge( INOUT *rg_batch._initialBarriers );

		if_unlikely( _last )
		{
			_finalBarriers.Merge( INOUT *rg_batch._finalBarriers );

			for (const auto& [id, state] : rg_batch._batchStates)
			{
				if ( state.current != state.final )
				{
					if ( id.IsImage() )
						_finalBarriers.ImageBarrier( id.AsImage(), state.current, state.final );
					else
						_finalBarriers.MemoryBarrier( state.current, state.final );
				}
			}

		}
		return _cmdBatch.RunTask( RVRef(_rtask), deps, _initialBarriers.Get(), _finalBarriers.Get(), Bool{_last} );
	}


} // AE::RG::_hidden_
