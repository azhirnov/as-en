// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define CMDPOOLMNGR		VCommandPoolManager

#elif defined(AE_ENABLE_METAL)

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)

#else
#	error not implemented
#endif
//-----------------------------------------------------------------------------


/*
=================================================
	Recycle
=================================================
*/
	void  RenderTaskScheduler::CommandBatchApi::Recycle (CommandBatch_t* ptr) __NE___
	{
		auto&	rts = GraphicsScheduler();
		CHECK( rts._batchPool.Unassign( ptr ));
	}

/*
=================================================
	Submit
=================================================
*/
	void  RenderTaskScheduler::CommandBatchApi::Submit (CommandBatch_t &batch, const ESubmitMode mode) __NE___
	{
		// TODO: return AsyncMutex instead of lock per-queue mutex

		auto&		rts			= GraphicsScheduler();
		const uint	submit_idx	= batch.GetSubmitIndex();

		// should never happen
		ASSERT( submit_idx < _MaxPendingBatches );

		// add to pending batches
		{
			QueueData::Bitfield	bf;
			bf.packed.pending = 1ull << submit_idx;

			auto&	q = rts._queueMap[ uint(batch.GetQueueType()) ];

			q.pending[ submit_idx ] = batch.GetRC();

			const auto	old = BitCast<QueueData::Bitfield>( q.bits.fetch_or( bf.value, EMemoryOrder::Release ));
			CHECK( not AnyBits( old.packed.pending, bf.packed.pending )); // already exists
		}

		switch_enum( mode )
		{
			case ESubmitMode::Auto :
			case ESubmitMode::Deferred :
				break;

			// try to submit
			case ESubmitMode::Immediately :
			case ESubmitMode::Force :
				rts._FlushQueue( batch.GetQueueType(), batch.GetFrameId(), (mode == ESubmitMode::Force) );
				break;
		}
		switch_end
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Recycle
=================================================
*/
	void  RenderTaskScheduler::DrawCommandBatchApi::Recycle (DrawCommandBatch_t* ptr) __NE___
	{
		auto&	rts = GraphicsScheduler();
		CHECK( rts._drawBatchPool.Unassign( ptr ));
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Resolve
=================================================
*/
	bool  RenderTaskScheduler::BatchSubmitDepsManager::Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) __NE___
	{
		if_likely( auto* batch_pp = dep.GetIf< CmdBatchOnSubmit >() )
		{
			if_unlikely( batch_pp->ptr == null )
				return true;

			CHECK_ERR( (*batch_pp->ptr)._AddOnSubmitDependency( RVRef(task), INOUT bitIndex ));
			return true;
		}

		return false;
	}

/*
=================================================
	DbgDetectDeadlock
=================================================
*/
#ifdef AE_DEBUG
	void  RenderTaskScheduler::BatchSubmitDepsManager::DbgDetectDeadlock (const CheckDepFn_t &fn) __NE___
	{
		GraphicsScheduler().DbgForEachBatch( fn, True{"pending only"} );
	}
#endif
//-----------------------------------------------------------------------------



/*
=================================================
	Resolve
=================================================
*/
	bool  RenderTaskScheduler::BatchCompleteDepsManager::Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) __NE___
	{
		if_likely( auto* batch_pp = dep.GetIf< RC<CommandBatch_t> >() )
		{
			if_unlikely( *batch_pp == null )
				return true;

			CHECK_ERR( (**batch_pp)._AddOnCompleteDependency( RVRef(task), INOUT bitIndex ));
			return true;
		}
		return false;
	}

/*
=================================================
	DbgDetectDeadlock
=================================================
*/
#ifdef AE_DEBUG
	void  RenderTaskScheduler::BatchCompleteDepsManager::DbgDetectDeadlock (const CheckDepFn_t &fn) __NE___
	{
		GraphicsScheduler().DbgForEachBatch( fn, False{"submitted only"} );
	}
#endif
//-----------------------------------------------------------------------------



/*
=================================================
	EndFrameTask::Run
=================================================
*/
	void  RenderTaskScheduler::EndFrameTask::Run () __Th___
	{
		if_unlikely( not _RunImpl() )
		{
			_ResetStates();
			CHECK_TE( false, "EndFrame - failed" );
		}

		MemoryManager().GetGraphicsFrameAllocator().EndFrame( _frameId );
	}

/*
=================================================
	EndFrameTask::_RunImpl
=================================================
*/
	bool  RenderTaskScheduler::EndFrameTask::_RunImpl () __Th___
	{
		auto&	rts = GraphicsScheduler();

		// check frame UID
		CHECK_ERR_MSG( rts._perFrameUID[ _frameId.Index() ].load() == _frameId,
			"Invalid frame UID, task will be canceled" );

		const auto	q_mask = rts.GetDevice().GetAvailableQueues();

		// Force flush all pending batches.
		// Timeline semaphore allow to submit commands without strict ordering.
		for (auto q = q_mask; q != Zero;)
		{
			rts._FlushQueue( ExtractBitIndex<EQueueType>( INOUT q ), _frameId, true );
		}

		for (auto& q : rts._queueMap)
		{
			auto	bf = BitCast<QueueData::Bitfield>( q.bits.exchange( 0 ));

			CHECK_ERR_MSG( bf.packed.pending == bf.packed.required and
						   bf.packed.pending == bf.packed.submitted,
				"Some batches are not submitted, EndFrame task must depends on all tasks which submit batches" );

			q.pending.fill( Default );
		}

		rts._resMngr->_OnEndFrame( _frameId );

		CHECK_ERR_MSG( rts._status.Set( RTS_EStatus::RecordFrame, RTS_EStatus::Idle ),
			"incorrect render task scheduler status, must be 'EStatus::RecordFrame'" );

		GFX_DBG_ONLY(
			CHECK( _frameId == rts.DbgFrameId() );
			rts._dbgFrameId.store( Default );
		)

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		CHECK_ERR( rts._EndFrame( _frameId ));
	  #endif

		return true;
	}

/*
=================================================
	EndFrameTask::OnCancel
=================================================
*/
	void  RenderTaskScheduler::EndFrameTask::OnCancel () __NE___
	{
		_ResetStates();
	}

/*
=================================================
	EndFrameTask::_ResetStates
=================================================
*/
	inline void  RenderTaskScheduler::EndFrameTask::_ResetStates () __NE___
	{
		auto&	rts = GraphicsScheduler();

		// check frame UID
		CHECK_MSG( rts._perFrameUID[ _frameId.Index() ].load() == _frameId, "Invalid frame UID" );

		for (auto& q : rts._queueMap)
		{
			q.bits.store( 0 );
			q.pending.fill( Default );
		}

		Unused( rts._status.Set( RTS_EStatus::Idle ));
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	RenderTaskScheduler::RenderTaskScheduler (const Device_t &dev) __NE___ :
		_device{ dev },
		_submitDepMngr{ MakeRC<BatchSubmitDepsManager>() },
		_completeDepMngr{ MakeRC<BatchCompleteDepsManager>() }
	{
		_lastUpdate.store( TimePoint_t::clock::now() );
	}

/*
=================================================
	destructor
=================================================
*/
	RenderTaskScheduler::~RenderTaskScheduler () __NE___
	{
		Deinitialize();
	}

/*
=================================================
	_Instance
=================================================
*/
	INTERNAL_LINKAGE( InPlace<RenderTaskScheduler>  s_RenderTaskScheduler );

	RenderTaskScheduler&  RenderTaskScheduler::_Instance () __NE___
	{
		return s_RenderTaskScheduler.AsRef();
	}

/*
=================================================
	InstanceCtor
=================================================
*/
	void  RenderTaskScheduler::InstanceCtor::Create (const Device_t &dev) __NE___
	{
		s_RenderTaskScheduler.Create( dev );

		MemoryBarrier( EMemoryOrder::Release );
	}

	void  RenderTaskScheduler::InstanceCtor::Destroy () __NE___
	{
		MemoryBarrier( EMemoryOrder::Acquire );

		s_RenderTaskScheduler.Destroy();

		MemoryBarrier( EMemoryOrder::Release );
	}

/*
=================================================
	Initialize
=================================================
*/
	bool  RenderTaskScheduler::Initialize (const GraphicsCreateInfo &info) __NE___
	{
		CHECK_ERR( _status.Set( EStatus::Initial, EStatus::Initialization ));

		CHECK_ERR( info.maxFrames >= GraphicsConfig::MinFrames );
		CHECK_ERR( info.maxFrames <= GraphicsConfig::MaxFrames );

		_frameId.store( FrameUID::Init( info.maxFrames ));

		CHECK_ERR( _device.IsInitialized() );

		#if defined(AE_ENABLE_VULKAN)
		# if AE_VK_TIMELINE_SEMAPHORE
			// GPU to GPU sync requires timeline semaphore
			CHECK_ERR( _device.GetVExtensions().timelineSemaphore );
		# else
			// if timeline semaphore is not supported - allow only one queue
			CHECK_ERR( _device.GetQueues().size() == 1 );
		# endif
		#elif defined(AE_ENABLE_METAL)
			// MTLSharedEvent must be supported
		#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
			CHECK_ERR( _Initialize( info ));
		#else
		#	error not implemented
		#endif

		for (uint i = 0; i < info.maxFrames; ++i) {
			_perFrame[i].submitted.reserve( _MaxSubmittedBatches );		// throw
		}

		_resMngr.reset( new ResourceManager{ _device });
		CHECK_ERR( _resMngr->Initialize( info ));

	  #ifdef AE_ENABLE_VULKAN
		_cmdPoolMngr.reset( new VCommandPoolManager{ _device });
	  #endif

		Scheduler().RegisterDependency< CmdBatchOnSubmit >( _submitDepMngr );
		Scheduler().RegisterDependency< RC<CommandBatch_t> >( _completeDepMngr );

		if ( info.useRenderGraph )
			_rg.reset( new RenderGraph_t{} );

		CHECK_ERR( _status.Set( EStatus::Initialization, EStatus::Idle ));
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  RenderTaskScheduler::Deinitialize () __NE___
	{
		const auto	state = _status.Set( EStatus::Destroyed );
		if ( AnyEqual( state, EStatus::Destroyed, EStatus::Initial ))
			return;

		CHECK( state == EStatus::Idle );
		CHECK( _WaitAll( seconds{1} ));

		_rg.reset();

		_batchPool.Release( True{"check for assigned"} );

		if ( _resMngr )
		{
			_resMngr->Deinitialize();
			_resMngr = null;
		}

		Scheduler().UnregisterDependency< CmdBatchOnSubmit >();
		Scheduler().UnregisterDependency< RC<CommandBatch_t> >();

	  #ifdef AE_ENABLE_VULKAN
		_cmdPoolMngr = null;

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		_Deinitialize();
	  #endif
	}

/*
=================================================
	WaitNextFrame
=================================================
*/
	bool  RenderTaskScheduler::WaitNextFrame (const EThreadArray &threads, nanoseconds timeout) __NE___
	{
		ASSERT( timeout != Default );

		using TimePoint2_t		= Threading::TaskScheduler::TimePoint_t;
		const auto	end_time	= TimePoint2_t::clock::now() + timeout;

		// wait for 'EndFrameTask' and other dependencies
		const auto	frame_id = _frameId.load().Inc();
		{
			auto	begin_deps = _beginDeps[ frame_id.Remap( _FrameDepsHistory )].WriteLock();

			if_unlikely( not Scheduler().Wait( *begin_deps, threads, end_time, _ProcessTasksPerTick ))
				return false;  // time is out

			begin_deps->clear();
		}

		ASSERT( _status.load() == EStatus::Idle );

		// wait for GPU frame completion
	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		CHECK_ERR( _WaitNextFrame( frame_id, timeout ));
	  #else
		for (;;)
		{
			if ( _IsFrameCompleted( frame_id ))
				break;

			if_unlikely( TimePoint2_t::clock::now() > end_time )
				return false;  // time is out

			if_unlikely( not Scheduler().ProcessTasks( threads, Threading::EThreadSeed(usize(this) & 0xF), _ProcessTasksPerTick ))
			{
				Scheduler().DbgDetectDeadlock();
				ThreadUtils::Sleep_500us();
			}
		}
	  #endif
		return true;
	}

/*
=================================================
	BeginFrame
=================================================
*/
	bool  RenderTaskScheduler::BeginFrame (const BeginFrameConfig &cfg) __NE___
	{
		CHECK_ERR_MSG( _status.Set( EStatus::Idle, EStatus::BeginFrame ),
			"incorrect render task scheduler state, must be 'Idle'." );

		const auto	frame_id = _frameId.Inc();

		ASSERT( _beginDeps[ frame_id.Remap( _FrameDepsHistory )].ConstPtr()->empty() );

		// update frame time
		{
			_perFrameUID[ frame_id.Index() ].store( frame_id );
			Unused( _status.Set( EStatus::BeginFrame ));

			auto	cur_time	= TimePoint_t::clock::now();
			auto	last_update	= _lastUpdate.exchange( cur_time );

			_timeDelta.store( TimeCast<secondsf>( cur_time - last_update ).count() );
		}

		// recycle per-frame memory
		MemoryManager().GetGraphicsFrameAllocator().BeginFrame( frame_id );

		_resMngr->_OnBeginFrame( frame_id, cfg );

	  #ifdef AE_ENABLE_VULKAN
		CHECK( _cmdPoolMngr->NextFrame( frame_id ));
	  #endif

		// release expired resources before next frame
		const uint	frame_offset = frame_id.MaxFrames() + ResourceManager::ExpiredResFrameOffset;

		if_likely( auto prev_id = frame_id.Sub( frame_offset );  prev_id.has_value() )
		{
			AsyncTask	task = MakeRC< ResourceManager::ReleaseExpiredResourcesTask >( *prev_id );
			if_likely( Scheduler().Run( task ))
				AddNextFrameDeps( RVRef(task) );
		}

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		CHECK_ERR( _BeginFrame( cfg ));
	  #endif

		GFX_DBG_ONLY(
			if ( auto prof = GetProfiler() )
				prof->NextFrame( frame_id );

			_dbgFrameId.store( frame_id );
		)

		CHECK( _status.Set( EStatus::BeginFrame, EStatus::RecordFrame ));
		return true;
	}

/*
=================================================
	AddFrameDeps
=================================================
*/
	void  RenderTaskScheduler::AddFrameDeps (const FrameUID frameId, ArrayView<AsyncTask> deps) __NE___
	{
		ASSERT( frameId > GetFrameId() );
		ASSERT( frameId <= GetFrameId().NextCycle().Inc() );
		//ASSERT( AnyEqual( _status.Get(), EStatus::BeginFrame, EStatus::RecordFrame, EStatus::Idle ));

		return _AddFrameDeps( frameId.Remap( _FrameDepsHistory ), deps );
	}

	void  RenderTaskScheduler::_AddFrameDeps (const uint frameIdx, ArrayView<AsyncTask> deps) __NE___
	{
		if_unlikely( deps.empty() )
			return;

		auto	begin_deps	= _beginDeps[ frameIdx ].WriteNoLock();
		DeferExLock	lock	{ begin_deps };
		EXLOCK( lock );

		for (auto& dep : deps)
		{
			if_unlikely( begin_deps->size() == begin_deps->capacity() )
			{
				FrameDepsArray_t	temp {RVRef(*begin_deps)};
				begin_deps->clear();

				lock.unlock();

				AsyncTask	task = Scheduler().WaitAsync( Threading::ETaskQueue::Renderer, Tuple{ ArrayView<AsyncTask>{ temp }});

				lock.lock();
				begin_deps->push_back( RVRef(task) );
			}

			if_likely( dep )
				begin_deps->push_back( dep );
		}
	}

/*
=================================================
	_FlushQueue
=================================================
*/
	bool  RenderTaskScheduler::_FlushQueue (const EQueueType queueType, const FrameUID frameId, const bool forceFlush) __NE___
	{
		TempBatches_t	pending;

		// lock queue and submit
		{
			auto&		q		= _queueMap[ uint(queueType) ];
			auto		queue	= GetDevice().GetQueue( queueType );
			CHECK_ERR( queue );

			const auto	GetRange = [&q] () -> Pair<int, int>
			{{
				const auto	ps_bits			= BitCast<QueueData::Bitfield>( q.bits.load() );
				const int	last_submitted	= BitScanReverse( ps_bits.packed.submitted ) + 1;
				const int	last_pending	= BitScanForward( ~ps_bits.packed.pending );
				return {last_submitted, last_pending};
			}};

			// early exit before lock if no pending batches
			if ( auto r = GetRange();  r.first >= r.second or r.second <= 0 )
				return false;

			// lock queue
			DeferExLock	lock {queue->guard};
			if ( forceFlush )
			{
				lock.lock();
			}
			else
			{
				if ( not lock.try_lock() )
					return false;
			}

			// extract batches
			{
				const auto	range = GetRange();

				// batches may be submitted when waiting for the mutex
				if ( range.first >= range.second or range.second <= 0 )
					return false;

				QueueData::Bitfield	mask;
				mask.packed.submitted = ToBitMask<ulong>( range.second );
				mask.packed.submitted &= ~ToBitMask<ulong>( range.first );

				const auto	old_bits = BitCast<QueueData::Bitfield>( q.bits.fetch_or( mask.value ));	// add bits to submitted
				CHECK( not AnyBits( old_bits.packed.submitted, mask.packed.submitted ));				// already submitted

				if ( forceFlush )
				{
					CHECK_ERR_MSG( old_bits.packed.required == old_bits.packed.pending,
								   "not an all required batches are pending" );
					CHECK_ERR_MSG( old_bits.packed.required == (old_bits.packed.submitted | mask.packed.submitted),
								   "not an all required batches are submitted" );
				}

				// EMemoryOrder::Acquire is not needed because of mutex
				for (int i = Max( 0, range.first ); i < range.second; ++i)
				{
					// batch is null when bit is set by 'SkipCmdBatches()'
					if ( q.pending[i] != null )
						pending.push_back( RVRef(q.pending[i]) );
				}
			}

			if ( pending.empty() )
				return true;

			#if defined(AE_ENABLE_VULKAN)
			# if AE_VK_TIMELINE_SEMAPHORE
				CHECK_ERR( _FlushQueue_Timeline( queueType, pending ));
			# else
				CHECK_ERR( _FlushQueue_Fence( queueType, pending ));
			# endif
			#elif defined(AE_ENABLE_METAL)
				CHECK_ERR( _FlushQueue2( queueType, pending ));

			#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
				CHECK_ERR( _FlushQueue2( queueType, pending ));

			#else
			#	error not implemented
			#endif
		}

		// add to submitted batches
		{
			auto&	frame = _perFrame[ frameId.Index() ];
			EXLOCK( frame.guard );
			frame.submitted.insert( frame.submitted.end(), pending.begin(), pending.end() );	// throw
			ASSERT( frame.submitted.size() <= _MaxSubmittedBatches );
		}

		return true;
	}

/*
=================================================
	SkipCmdBatches
=================================================
*/
	void  RenderTaskScheduler::SkipCmdBatches (const EQueueType queue, uint bits) __NE___
	{
		constexpr uint	mask = ToBitMask<uint>( _MaxPendingBatches );

		ASSERT( bits == UMax or bits < mask );
		CHECK_ERRV( AllBits( _device.GetAvailableQueues(), EQueueMask(1u << uint(queue)) ));

	  #ifdef AE_ENABLE_REMOTE_GRAPHICS
		_SkipCmdBatches( queue, bits );
	  #endif

		bits &= mask;

		auto&	packed_bits = _queueMap[ uint(queue) ].bits;

		QueueData::Bitfield	exp = BitCast<QueueData::Bitfield>( packed_bits.load() );

		for (;;)
		{
			QueueData::Bitfield	desired = exp;

			desired.packed.required	= exp.packed.required | bits;
			desired.packed.pending	= exp.packed.pending  | (bits & ~exp.packed.required);

			if_likely( packed_bits.CAS( INOUT exp.value, desired.value ))
				break;

			ThreadUtils::Pause();
		}
	}

/*
=================================================
	BeginCmdBatch
----
	returns 'null' on error
=================================================
*/
	RC<RenderTaskScheduler::CommandBatch_t>  RenderTaskScheduler::BeginCmdBatch (const CmdBatchDesc &desc) __NE___
	{
		CHECK_ERR( desc.submitIdx < _MaxPendingBatches );
		CHECK_ERR( AnyEqual( _status.load(), EStatus::Idle, EStatus::BeginFrame, EStatus::RecordFrame ));
		CHECK_ERR( AllBits( _device.GetAvailableQueues(), EQueueMask(1u << uint(desc.queue)) ));

		// validate 'submitIdx'
		{
			QueueData::Bitfield	bf;
			bf.packed.required = 1ull << desc.submitIdx;

			const auto	old_bits = BitCast<QueueData::Bitfield>( _queueMap[ uint(desc.queue) ].bits.fetch_or( bf.value ));

			CHECK_ERR_MSG( not AnyBits( old_bits.packed.required,  bf.packed.required ), "batch with 'submitIdx' is already created" );
			CHECK_ERR_MSG( not AnyBits( old_bits.packed.pending,   bf.packed.required ) or
						   not AnyBits( old_bits.packed.submitted, bf.packed.required ), "batch with 'submitIdx' is marked as unused" );
		}

		uint	index;
		CHECK_ERR( _batchPool.Assign( OUT index ));

		auto&	batch = _batchPool[ index ];

		// 'GetFrameId()' may not equal to 'DbgFrameId()'

		if_likely( batch._Create( GetFrameId(), desc ))
			return RC<CommandBatch_t>{ &batch };

		_batchPool.Unassign( index );
		RETURN_ERR( "failed to allocate command batch" );
	}

/*
=================================================
	WaitAll
----
	only for debugging!
=================================================
*/
	bool  RenderTaskScheduler::WaitAll (nanoseconds timeout) __NE___
	{
		const auto	state = _status.load();
		if ( state == EStatus::Destroyed )
			return true;

		CHECK( AnyEqual( state, EStatus::Idle, EStatus::BeginFrame, EStatus::RecordFrame ));

		return _WaitAll( timeout );
	}

	bool  RenderTaskScheduler::WaitAll (const EThreadArray &threads, nanoseconds timeout) __NE___
	{
		const auto	state = _status.load();
		if ( state == EStatus::Destroyed )
			return true;

		CHECK( AnyEqual( state, EStatus::Idle, EStatus::BeginFrame, EStatus::RecordFrame ));

		const auto	end_time = TimePoint_t::clock::now() + timeout;

		return _WaitAllBatches( end_time ) and _WaitAllTasks( threads, end_time );
	}

/*
=================================================
	_WaitAllTasks
=================================================
*/
	bool  RenderTaskScheduler::_WaitAllTasks (const EThreadArray &threads, const TimePoint_t endTime) __NE___
	{
		bool	complete = true;

		for (auto& frame_deps : _beginDeps)
		{
			auto	deps = frame_deps.WriteLock();

			if ( deps->empty() )
				continue;

			if ( not Scheduler().Wait( *deps, threads, endTime, _ProcessTasksPerTick ))
			{
				complete = false;
				break;
			}
			else
				deps->clear();
		}

		return complete;
	}

/*
=================================================
	_WaitAll
=================================================
*/
	bool  RenderTaskScheduler::_WaitAll (const nanoseconds timeout) __NE___
	{
		const auto	end_time = TimePoint_t::clock::now() + timeout;

		return _WaitAllBatches( end_time );
	}

/*
=================================================
	_WaitAllBatches
=================================================
*/
#ifndef AE_ENABLE_REMOTE_GRAPHICS
	bool  RenderTaskScheduler::_WaitAllBatches (const TimePoint_t endTime) __NE___
	{
		const auto	q_mask = _device.GetAvailableQueues();

		for (;;)
		{
			FrameUID	frame_id	= GetFrameId();
			bool		complete	= true;

			for (auto q = q_mask; q != Zero;) {
				_FlushQueue( ExtractBitIndex<EQueueType>( INOUT q ), frame_id, false );
			}

			for (uint f = 0, cnt = frame_id.MaxFrames(); f < cnt; ++f)
			{
				frame_id.Inc();

				if_unlikely( not _IsFrameCompleted( frame_id ))
				{
					complete = false;
					ThreadUtils::Sleep_15ms();
				}
			}

			if ( complete )
				return true;

			if ( TimePoint_t::clock::now() > endTime )
				return false;
		}
	}
#endif
/*
=================================================
	SetProfiler
=================================================
*/
	void  RenderTaskScheduler::SetProfiler (RC<IGraphicsProfiler> profiler) __NE___
	{
		GFX_DBG_ONLY(
			_profiler.store( profiler );

			if ( not profiler )
				return;

			for (auto& q : _device.GetQueues()) {
				profiler->SetQueue( q.type, q.debugName );
			}
		)
		Unused( profiler );
	}

/*
=================================================
	DbgCheckFrameId
=================================================
*/
#if AE_DBG_GRAPHICS
	void  RenderTaskScheduler::DbgCheckFrameId (const FrameUID expected, StringView taskName) C_NE___
	{
		const FrameUID	cur = DbgFrameId();
		CHECK_MSG( cur == expected,
			"Current frame ("s << ToString(cur.Unique()) << ") doesn't match expected frame (" <<
			ToString(expected.Unique()) << ") in task '" << taskName << "'.\n"
			"This may happens when 'EndFrame' happens before current task is started/completed." );
	}
#endif

/*
=================================================
	DbgForEachBatch
=================================================
*/
#ifdef AE_DEBUG
	void  RenderTaskScheduler::DbgForEachBatch (const Threading::ITaskDependencyManager::CheckDepFn_t &fn, Bool) __NE___
	{
		using EStatus = CommandBatch_t::EStatus;

		const auto	CheckBatch = [&fn] (CommandBatch_t &batch)
		{{
			String	info;
			info << "cmdbatch '" << batch.DbgName() << "' (" << ToString<16>(usize(&batch)) << "), status: ";

			const auto	status = batch._status.load();

			switch_enum( status )
			{
				case EStatus::Destroyed :	info << "Destroyed";	break;
				case EStatus::Initial :		info << "Initial";		break;
				case EStatus::Recorded :	info << "Recorded";		break;
				case EStatus::Pending :		info << "Pending";		break;
				case EStatus::Submitted :	{ info << "Submitted";	EXLOCK( batch._onSubmitDepsGuard );		ASSERT( batch._onSubmitDeps.empty() );		break; }
				case EStatus::Completed :	{ info << "Completed";	EXLOCK( batch._onCompleteDepsGuard );	ASSERT( batch._onCompleteDeps.empty() );	break; }
			}
			switch_end

			{
				EXLOCK( batch._onSubmitDepsGuard );
				for (auto [task, idx] : batch._onSubmitDeps) {
					fn( info, task, idx );
				}
			}{
				EXLOCK( batch._onCompleteDepsGuard );
				for (auto [task, idx] : batch._onCompleteDeps) {
					fn( info, task, idx );
				}
			}
		}};

		for (usize i = 0; i < _batchPool.capacity(); ++i)
		{
			if ( _batchPool.IsAssigned( uint(i) ))
			{
				auto	batch = RC<CommandBatch_t>{ &_batchPool[ uint(i) ]};

				CheckBatch( *batch );
			}
		}
	}
#endif // AE_DEBUG

