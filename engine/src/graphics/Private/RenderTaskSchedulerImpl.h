// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

	// Macro defined in 'RenderTaskSchedulerDecl.h'
	
namespace AE::Graphics
{

	//
	// Batch Complete Dependency Manager
	//

	class RTSCHEDULER::BatchCompleteDepsManager final : public Threading::ITaskDependencyManager
	{
	// methods
	public:
		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex)	__NE_OV;
		
		AE_SCHEDULER_PROFILING(
			void  DbgDetectDeadlock (const CheckDepFn_t &fn)					__NE_OV;)

		AE_GLOBALLY_ALLOC
	};

	
	
	//
	// Batch Submit Dependency Manager
	//

	class RTSCHEDULER::BatchSubmitDepsManager final : public Threading::ITaskDependencyManager
	{
	// methods
	public:
		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex)	__NE_OV;
		
		AE_SCHEDULER_PROFILING(
			void  DbgDetectDeadlock (const CheckDepFn_t &fn)					__NE_OV;)

		AE_GLOBALLY_ALLOC
	};



	//
	// Begin Frame Task
	//

	class RTSCHEDULER::BeginFrameTask final : public Threading::IAsyncTask
	{
	private:
		const FrameUID			_frameId;
		const BeginFrameConfig	_config;

	public:
		BeginFrameTask (FrameUID frameId, const BeginFrameConfig &cfg) :
			IAsyncTask{ETaskQueue::Renderer}, _frameId{frameId}, _config{cfg}
		{}
			
		void  Run ()			override;
		void  OnCancel ()		__NE_OV;

		StringView  DbgName ()	C_NE_OF { return "BeginFrame"; }
	};



	//
	// End Frame Task
	//

	class RTSCHEDULER::EndFrameTask final : public Threading::IAsyncTask
	{
	private:
		const FrameUID	_frameId;

	public:
		explicit EndFrameTask (FrameUID frameId) :
			IAsyncTask{ETaskQueue::Renderer}, _frameId{frameId}
		{}
			
		void  Run ()			override;
		void  OnCancel ()		__NE_OV;

		StringView  DbgName ()	C_NE_OF { return "EndFrame"; }
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	BeginFrame
=================================================
*/
	template <typename ...Deps>
	inline AsyncTask  RTSCHEDULER::BeginFrame (const BeginFrameConfig &cfg, const Tuple<Deps...> &deps) __Th___
	{
		CHECK_ERR( _SetState( EState::Idle, EState::BeginFrame ),
				   Scheduler().GetCanceledTask() );

		FrameUID	frame_id = _frameId.Inc();
		_perFrameUID[ frame_id.Index() ].store( frame_id );

		AsyncTask	task = MakeRC< RTSCHEDULER::BeginFrameTask >( frame_id, cfg );	// throw	// TODO: catch
		
		PROFILE_ONLY(
			if ( auto prof = GetProfiler() )
				prof->RequestNextFrame( frame_id );
		)

		EXLOCK( _beginDepsGuard );
		// TODO: copy to temp array?

		if_likely( Scheduler().Run( task, TupleConcat( Tuple{ ArrayView<AsyncTask>{ _beginDeps }}, deps )))
		{
			_beginDeps.clear();
			return task;
		}
		else
		{
			CHECK( _SetState( EState::BeginFrame, EState::Idle ));
			return Scheduler().GetCanceledTask();
		}
	}
	
/*
=================================================
	EndFrame
=================================================
*/
	template <typename ...Deps>
	inline AsyncTask  RTSCHEDULER::EndFrame (const Tuple<Deps...> &deps) __Th___
	{
		CHECK_ERR( AnyEqual( _GetState(), EState::BeginFrame, EState::RecordFrame ),
				   Scheduler().GetCanceledTask() );
		
		AsyncTask	task = MakeRC< RTSCHEDULER::EndFrameTask >( _frameId.load() );	// throw	// TODO: catch

		if_likely( Scheduler().Run( task, deps ))
		{
			AddNextFrameDeps( task );
			return task;
		}
		else
			return Scheduler().GetCanceledTask();
	}

/*
=================================================
	GAutorelease::dtor
=================================================
*/
	template <usize IndexSize, usize GenerationSize, uint UID>
	GAutorelease< HandleTmpl< IndexSize, GenerationSize, UID >>::~GAutorelease () __NE___
	{
		if ( _id )
			RenderTaskScheduler().GetResourceManager().ReleaseResource( _id );
	}

} // AE::Graphics
	

namespace AE
{
/*
=================================================
	RenderTaskScheduler
=================================================
*/
	ND_ forceinline Graphics::RTSCHEDULER&  RenderTaskScheduler () __NE___
	{
		return *Graphics::RTSCHEDULER::_Instance();
	}
	
/*
=================================================
	RenderGraph
=================================================
*/
	ND_ forceinline Graphics::RTSCHEDULER::RenderGraphImpl&  RenderGraph () __NE___
	{
		return RenderTaskScheduler().Graph();
	}

} // AE
//-----------------------------------------------------------------------------

#undef RTSCHEDULER
