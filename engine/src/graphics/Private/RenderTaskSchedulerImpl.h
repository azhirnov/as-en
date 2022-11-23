
	
	
	//
	// Batch Complete Dependency Manager
	//

	class RTSCHEDULER::BatchCompleteDepsManager final : public Threading::ITaskDependencyManager
	{
	// methods
	public:
		~BatchCompleteDepsManager () __NE_OV {}

		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) __NE_OV;

		AE_GLOBALLY_ALLOC
	};

	
	
	//
	// Batch Submit Dependency Manager
	//

	class RTSCHEDULER::BatchSubmitDepsManager final : public Threading::ITaskDependencyManager
	{
	// methods
	public:
		~BatchSubmitDepsManager () __NE_OV {}

		bool  Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) __NE_OV;

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
			IAsyncTask{EThread::Renderer}, _frameId{frameId}, _config{cfg}
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
			IAsyncTask{EThread::Renderer}, _frameId{frameId}
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

		AsyncTask	task = MakeRC<RTSCHEDULER::BeginFrameTask>( frame_id, cfg );	// throw	// TODO: catch
		
		PROFILE_ONLY(
			if ( auto prof = GetProfiler() )
				prof->RequestNextFrame( frame_id );
		)

		EXLOCK( _beginDepsGuard );

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
		
		AsyncTask	task = MakeRC<RTSCHEDULER::EndFrameTask>( _frameId.load() );	// throw

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
	AddNextFrameDeps
=================================================
*/
	inline void  RTSCHEDULER::AddNextFrameDeps (ArrayView<AsyncTask> deps) __NE___
	{
		EXLOCK( _beginDepsGuard );

		for (auto& dep : deps)
			_beginDeps.push_back( dep );	// throw

		ASSERT( _beginDeps.size() <= _MaxBeginDeps );
	}
	
	inline void  RTSCHEDULER::AddNextFrameDeps (AsyncTask dep) __NE___
	{
		EXLOCK( _beginDepsGuard );
		_beginDeps.push_back( RVRef(dep) );	// throw
		ASSERT( _beginDeps.size() <= _MaxBeginDeps );
	}

#	undef CMDBATCH
#	undef DRAWCMDBATCH
#	undef RESMNGR
#	undef DEVICE
#	undef CMDPOOLMNGR
#	undef RTSCHEDULER
#	undef ENABLE_VK_TIMELINE_SEMAPHORE
