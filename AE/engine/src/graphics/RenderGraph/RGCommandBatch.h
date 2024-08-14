// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	RGCommandBatchPtr
		thread-safe:  no (only single thread)

	Warning: CmdBufExeIndex is not compatible with RenderGraph !
*/

#pragma once

#include "graphics/Private/EnumUtils.h"
#include "graphics/RenderGraph/ResStateTracker.h"

namespace AE::RG::_hidden_
{

	//
	// Render Graph Command Batch pointer
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

		RGCommandBatchPtr&  operator = (std::nullptr_t)					__NE___	{ _cmdBatch = null;					return *this; }
		RGCommandBatchPtr&  operator = (const RGCommandBatchPtr &rhs)	__NE___	{ _cmdBatch = rhs._cmdBatch;		return *this; }
		RGCommandBatchPtr&  operator = (RGCommandBatchPtr &&rhs)		__NE___	{ _cmdBatch = RVRef(rhs._cmdBatch);	return *this; }

		ND_ bool  operator == (const RGCommandBatchPtr &rhs)			C_NE___	{ return _cmdBatch == rhs._cmdBatch; }

		ND_ explicit operator bool ()									C_NE___	{ return bool{_cmdBatch}; }

		ND_ CommandBatchPtr		AsBatchRC ()							C_NE___	{ return _cmdBatch; }
		ND_ Ptr<CommandBatch>	AsBatch ()								C_NE___	{ return _cmdBatch.get(); }
		ND_ Ptr<RGBatchData>	AsRG ()									C_NE___;

		ND_ bool				IsRecording ()							C_NE___	{ return _cmdBatch and _cmdBatch->IsRecording(); }
		ND_ bool				IsSubmitted ()							C_NE___	{ return _cmdBatch and _cmdBatch->IsSubmitted(); }

		ND_ uint				CmdPool_IsEmpty ()						C_NE___	{ return not _cmdBatch or _cmdBatch->CmdPool_IsEmpty(); }
		ND_ bool				CmdPool_IsFirst (uint exeIndex)			C_NE___	{ return _cmdBatch and _cmdBatch->CmdPool_IsFirst( exeIndex ); }
		ND_ bool				CmdPool_IsLast (uint exeIndex)			C_NE___	{ return _cmdBatch and _cmdBatch->CmdPool_IsLast( exeIndex ); }


	// render task api //

		template <typename TaskType, typename ...Ctor>
		ND_ RenderTaskBuilder	Task (Tuple<Ctor...>&&	ctor,
									  DebugLabel		dbg = Default)	C_NE___;

	  #ifdef AE_HAS_COROUTINE
		ND_ RenderTaskBuilder	Task (RenderTaskCoro	coro,
									  DebugLabel		dbg	= Default)	C_NE___;
	  #endif

		template <typename ...Deps>
		AsyncTask  SubmitAsTask (const Tuple<Deps...>&	deps = Default)	__NE___;
	};



	//
	// Render Graph Batch Data
	//
	class RGCommandBatchPtr::RGBatchData
	{
		friend class  RenderGraph;
		friend class  RGCommandBatchPtr;
		friend struct RGCommandBatchPtr::RenderTaskBuilder;

	// types
	private:
		struct InBatchState
		{
			EResourceState		initial;
			EResourceState		final;
			EResourceState		current;

			InBatchState (EResourceState initial, EResourceState final) __NE___ : initial{initial}, final{final}, current{initial} {}
		};

		struct InTaskState
		{
			EResourceState		current;
			EResourceState		final;

			explicit InTaskState (EResourceState current)				__NE___ : current{current}, final{current} {}
			InTaskState (EResourceState current, EResourceState final)	__NE___ : current{current}, final{final} {}
		};

		using ResStateInBatch_t	= FlatHashMap< ResourceKey, InBatchState, ResourceKeyHash >;
		using ResStateMap_t		= FlatHashMap< ResourceKey, InTaskState, ResourceKeyHash >;

		struct ResStateMap
		{
			ResStateMap_t				map;
			DRC_ONLY( RWDataRaceCheck	drCheck;)

			ResStateMap ()						__NE___	{}
			ResStateMap (ResStateMap &&other)	__NE___	: map{RVRef(other.map)} {}
		};

		using PerTaskStates_t	= StaticArray< ResStateMap, GraphicsConfig::MaxCmdBufPerBatch >;
		using AccumBarriers_t	= CommandBatch::AccumBarriers_t;
		using TaskArr_t			= FixedArray< AsyncTask, GraphicsConfig::MaxCmdBufPerBatch >;
		using AtomicBits_t		= Atomic< uint >;

		StaticAssert( CT_SizeOfInBits<AtomicBits_t::value_type> >= GraphicsConfig::MaxCmdBufPerBatch );


	// variables
	private:
		ResStateInBatch_t			_batchStates;
		AtomicBits_t				_readbackMemory		{0};

		PerTaskStates_t				_perTask;
		ResStateTracker &			_globalStates;

		Optional<AccumBarriers_t>	_initialBarriers;
		Optional<AccumBarriers_t>	_finalBarriers;
		TaskArr_t					_renderTasks;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)		// protects all except '_perTask'


	// methods
	public:
		explicit RGBatchData (ResStateTracker &gs)												__NE___ : _globalStates{gs} {}
		RGBatchData (RGBatchData &&)															__NE___;
		~RGBatchData ()																			__NE___	{}


	// task api //
		ND_ bool  HasReadbackMemoryBarrier (uint taskIdx, EResourceState srcState)				__NE___;

			template <typename Ctx>
			void  ReadbackMemoryBarrier (uint taskIdx, Ctx &ctx, EResourceState srcState)		__NE___;

			void  AddSurfaceTargets (uint taskIdx, ArrayView<App::IOutputSurface_RenderTarget>)	__NE___;

			template <typename ID>
		ND_ bool  CheckResourceState (uint taskIdx, ID id, EResourceState state)				C_Th___;

			template <typename ID, typename Ctx>
			void  ResourceState (uint taskIdx, Ctx &ctx, ID id, EResourceState state)			__Th___;

			template <typename ID>
		ND_ auto  ResetResourceState (uint taskIdx, ID id, EResourceState state)				__NE___ -> EResourceState;

			template <typename Ctx>
			void  FinalBarriers (uint taskIdx, Ctx &ctx)										C_Th___;

			void  SetRenderPassInitialStates (uint taskIdx, INOUT RenderPassDesc &)				__Th___;
			void  SetRenderPassFinalStates (uint taskIdx, const PrimaryCmdBufState_t &)			__Th___;


	private:
		ND_ bool  _CheckResourceState (uint taskIdx, ResourceKey key, EResourceState state)								C_NE___;
		ND_ bool  _ResourceState (uint taskIdx, ResourceKey key, EResourceState newState, OUT EResourceState &oldState)	__NE___;
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
		using ResStateMap		= RGBatchData::ResStateMap;
		using InTaskState		= RGBatchData::InTaskState;


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

			void  _UseResource (ResourceKey key, EResourceState, EResourceState)		__NE___;
			void  _BeforeRun ()															__NE___;

		ND_ RGBatchData &			_RGBatch ()											__NE___	{ return *Cast<RGBatchData>( _cmdBatch.GetUserData() ); }
		ND_ uint					_ExeIdx ()											C_NE___	{ return _rtask->GetExecutionIndex(); }
		ND_ ResStateMap &			_StateMap ()										__NE___	{ return _RGBatch()._perTask[ _ExeIdx() ]; }

	public:
		// resource state inside render task.
		ND_ RenderTaskBuilder &&	UseResource (ImageID      id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (BufferID     id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (RTGeometryID id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (RTSceneID    id, EResourceState initial, EResourceState final)	rvNE___	{ _UseResource( ResourceKey{id}, initial, final );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (ImageViewID  id, EResourceState initial, EResourceState final)	rvNE___;
		ND_ RenderTaskBuilder &&	UseResource (BufferViewID id, EResourceState initial, EResourceState final)	rvNE___;

		ND_ RenderTaskBuilder &&	UseResource (ImageID      id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (BufferID     id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (RTGeometryID id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (RTSceneID    id, EResourceState initialOrFinal)				rvNE___	{ _UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (ImageViewID  id, EResourceState initialOrFinal)				rvNE___	{ return RVRef(*this).UseResource( id, initialOrFinal, initialOrFinal ); }
		ND_ RenderTaskBuilder &&	UseResource (BufferViewID id, EResourceState initialOrFinal)				rvNE___	{ return RVRef(*this).UseResource( id, initialOrFinal, initialOrFinal ); }

		// resource will be in default state
		ND_ RenderTaskBuilder &&	UseResource (ImageID      id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (BufferID     id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (RTGeometryID id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (RTSceneID    id)												rvNE___	{ _UseResource( ResourceKey{id}, Default, Default );  return RVRef(*this); }
		ND_ RenderTaskBuilder &&	UseResource (ImageViewID  id)												rvNE___	{ return RVRef(*this).UseResource( id, Default, Default ); }
		ND_ RenderTaskBuilder &&	UseResource (BufferViewID id)												rvNE___	{ return RVRef(*this).UseResource( id, Default, Default ); }

		template <typename ArrayType>
		ND_ RenderTaskBuilder &&	UseResources (const ArrayType&, EResourceState initial, EResourceState final) rvNE___;

		template <typename ArrayType>
		ND_ RenderTaskBuilder &&	UseResources (const ArrayType &, EResourceState initialOrFinal)				rvNE___;

		template <typename ArrayType>
		ND_ RenderTaskBuilder &&	UseResources (const ArrayType &ids)											rvNE___;

		// last render task may submit command batch to the GPU
		ND_ RenderTaskBuilder &&	SubmitBatch ()																rvNE___	{ _last = true;  return RVRef(*this); }

		template <typename ...Deps>
		ND_ AsyncTask				Run (const Tuple<Deps...>& deps = Default)									rvNE___;
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
		EResourceState	old_state;
		if ( not _ResourceState( taskIdx, ResourceKey{id}, newState, OUT old_state ))
			return;  // resource state is not tracked

		if ( AllBits( newState, EResourceState::Invalidate ))
			return;  // skip barrier

		if ( EResourceState_IsUnnecessaryBarrier( old_state, newState ))
			return;  // skip barrier

	  #if AE_GRAPHICS_DBG_SYNC
		{
			auto	msg = _globalStates.BarrierToString( ResourceKey{id}, old_state, newState );
			ctx.DebugMarker({ StringView{msg} });
		}
		if ( old_state == Default )
			AE_LOGW( _globalStates.KeyToString(ResourceKey{id}) << " previous state is General or not known" );
	  #endif

		if constexpr( IsSameTypes< ID, ImageID >){
			if ( EResourceState_RequireImageBarrier( old_state, newState, False{"strict"} ))
				ctx.ImageBarrier( id, old_state, newState );	// throw
		}else{
			if ( EResourceState_RequireMemoryBarrier( old_state, newState, False{"strict"} ))
				ctx.MemoryBarrier( old_state, newState );		// throw
		}
	}

/*
=================================================
	ResetResourceState
=================================================
*/
	template <typename ID>
	auto  RGCommandBatchPtr::RGBatchData::ResetResourceState (uint taskIdx, ID id, EResourceState newState) __NE___ -> EResourceState
	{
		EResourceState	old_state = Default;
		Unused( _ResourceState( taskIdx, ResourceKey{id}, newState, OUT old_state ));
		return old_state;
	}

/*
=================================================
	CheckResourceState
=================================================
*/
	template <typename ID>
	bool  RGCommandBatchPtr::RGBatchData::CheckResourceState (uint taskIdx, ID id, EResourceState state) C_Th___
	{
		return _CheckResourceState( taskIdx, ResourceKey{id}, state );
	}

/*
=================================================
	FinalBarriers
=================================================
*/
	template <typename Ctx>
	void  RGCommandBatchPtr::RGBatchData::FinalBarriers (uint taskIdx, Ctx &ctx) C_Th___
	{
		auto&	rs = _perTask[ taskIdx ];
		DRC_EXLOCK( rs.drCheck );

		GRAPHICS_DBG_SYNC( ctx.DebugMarker({"RG.FinalBarriers"});)

		for (auto& [id, state] : rs.map)
		{
			// skip 'write -> write' barrier here,
			// it will be added in 'batch to batch' transition or
			// in first 'ResourceState()' call in next render task.
			if_unlikely( not EResourceState_IsSameStates( state.current, state.final ))
			{
				if ( id.IsImage() )
					ctx.ImageBarrier( id.AsImage(), state.current, state.final );	// throw
				else
					ctx.MemoryBarrier( state.current, state.final );				// throw
			}
		}

		ctx.CommitBarriers();	// throw
	}

/*
=================================================
	ReadbackMemoryBarrier
=================================================
*/
	inline bool  RGCommandBatchPtr::RGBatchData::HasReadbackMemoryBarrier (uint taskIdx, EResourceState srcState) __NE___
	{
		ASSERT( taskIdx < GraphicsConfig::MaxCmdBufPerBatch );
		ASSERT( AnyEqual( srcState, EResourceState::CopyDst ));
		Unused( srcState );

		// memory barrier 'Copy -> Host' is already issued if added in subsequent tasks too

		constexpr	uint	mask	= ToBitMask<uint>( GraphicsConfig::MaxCmdBufPerBatch );
		const		uint	bits	= (_readbackMemory.load() & mask) & ~ToBitMask<uint>( taskIdx+1 );
		return bits != 0;
	}

	template <typename Ctx>
	void  RGCommandBatchPtr::RGBatchData::ReadbackMemoryBarrier (uint taskIdx, Ctx &ctx, EResourceState srcState) __NE___
	{
		if_unlikely( not HasReadbackMemoryBarrier( taskIdx, srcState ))
		{
			_readbackMemory.Or( 1u << taskIdx );
			ctx.MemoryBarrier( EResourceState::CopyDst, EResourceState::Host_Read );
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	AsRG
=================================================
*/
	inline Ptr<RGCommandBatchPtr::RGBatchData>  RGCommandBatchPtr::AsRG () C_NE___
	{
		CHECK_ERR( _cmdBatch );
		return Cast<RGBatchData>( _cmdBatch->GetUserData() );
	}

/*
=================================================
	Task
=================================================
*/
	template <typename TaskType, typename ...Ctor>
	RGCommandBatchPtr::RenderTaskBuilder  RGCommandBatchPtr::Task (Tuple<Ctor...>&& ctorArgs, DebugLabel dbg) C_NE___
	{
		GFX_DBG_ONLY(
			if ( dbg.color == DebugLabel::ColorTable::Undefined )
				dbg.color = AsBatch()->DbgColor();
		)

		auto	task = ctorArgs.Apply([this, dbg] (auto&& ...args) __NE___
									  { return MakeRC<TaskType>( FwdArg<decltype(args)>(args)..., AsBatchRC(), dbg ); });

		return RenderTaskBuilder{ *AsBatch(), RVRef(task) };
	}

/*
=================================================
	Task
=================================================
*/
#ifdef AE_HAS_COROUTINE
	inline RGCommandBatchPtr::RenderTaskBuilder  RGCommandBatchPtr::Task (RenderTaskCoro coro, DebugLabel dbg) C_NE___
	{
		GFX_DBG_ONLY(
			if ( dbg.color == DebugLabel::ColorTable::Undefined )
				dbg.color = AsBatch()->DbgColor();
		)

		RC<RenderTask>	task;
		if_likely( coro and coro.Promise()._Init( AsBatchRC(), Default, dbg ))
			task = RC<RenderTask>{coro};

		return RenderTaskBuilder{ *AsBatch(), RVRef(task) };
	}
#endif

/*
=================================================
	SubmitAsTask
=================================================
*/
	template <typename ...Deps>
	AsyncTask  RGCommandBatchPtr::SubmitAsTask (const Tuple<Deps...>& deps) __NE___
	{
		CHECK_ERR( _cmdBatch );

		auto&	rg_batch = *AsRG();
		DRC_EXLOCK( rg_batch._drCheck );

		auto	barriers = rg_batch._finalBarriers->Get();
		CHECK_ERR( barriers == null );

		return _cmdBatch->SubmitAsTask( TupleConcat( deps, Tuple{ArrayView<AsyncTask>{rg_batch._renderTasks}} ));
	}
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
		_BeforeRun();
		return _cmdBatch.RunTask( RVRef(_rtask), deps, _initialBarriers.Get(), _finalBarriers.Get(), Bool{_last} );
	}

/*
=================================================
	UseResources
=================================================
*/
	template <typename ArrayType>
	RGCommandBatchPtr::RenderTaskBuilder&&  RGCommandBatchPtr::RenderTaskBuilder::UseResources (const ArrayType &ids, EResourceState initial, EResourceState final) rvNE___
	{
		for (auto& id : ArrayView{ids})
		{
			StaticAssert( IsHandleTmpl< decltype(id) >);
			_UseResource( ResourceKey{id}, initial, final );
		}
		return RVRef(*this);
	}

	template <typename ArrayType>
	RGCommandBatchPtr::RenderTaskBuilder&&  RGCommandBatchPtr::RenderTaskBuilder::UseResources (const ArrayType &ids, EResourceState initialOrFinal) rvNE___
	{
		for (auto& id : ArrayView{ids})
		{
			StaticAssert( IsHandleTmpl< decltype(id) >);
			_UseResource( ResourceKey{id}, initialOrFinal, initialOrFinal );
		}
		return RVRef(*this);
	}

	template <typename ArrayType>
	RGCommandBatchPtr::RenderTaskBuilder&&  RGCommandBatchPtr::RenderTaskBuilder::UseResources (const ArrayType &ids) rvNE___
	{
		for (auto& id : ArrayView{ids})
		{
			StaticAssert( IsHandleTmpl< decltype(id) >);
			_UseResource( ResourceKey{id}, Default, Default );
		}
		return RVRef(*this);
	}


} // AE::RG::_hidden_
