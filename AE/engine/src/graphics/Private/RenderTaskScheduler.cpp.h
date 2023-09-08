// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#   define CMDPOOLMNGR      VCommandPoolManager
#   define RTSCHEDULER      VRenderTaskScheduler

#elif defined(AE_ENABLE_METAL)
#   define RTSCHEDULER      MRenderTaskScheduler

#else
#   error not implemented
#endif
//-----------------------------------------------------------------------------


/*
=================================================
    Recycle
=================================================
*/
    void  RTSCHEDULER::CommandBatchApi::Recycle (uint indexInPool) __NE___
    {
        auto&   rts = RenderTaskScheduler();
        rts._batchPool.Unassign( indexInPool );
    }

/*
=================================================
    Submit
=================================================
*/
    void  RTSCHEDULER::CommandBatchApi::Submit (CommandBatch_t &batch, ESubmitMode mode) __NE___
    {
        auto&       rts         = RenderTaskScheduler();
        const uint  submit_idx  = batch.GetSubmitIndex();

        // should never happen
        ASSERT( submit_idx < _MaxPendingBatches );

        // add to pending batches
        {
            QueueData::Bitfield bf;
            bf.packed.pending = 1ull << submit_idx;

            auto&   q = rts._queueMap[ uint(batch.GetQueueType()) ];

            q.pending[ submit_idx ] = batch.GetRC();

            const auto  old = BitCast<QueueData::Bitfield>( q.bits.fetch_or( bf.value, EMemoryOrder::Release ));
            CHECK( not AnyBits( old.packed.pending, bf.packed.pending )); // already exists
        }

        BEGIN_ENUM_CHECKS();
        switch ( mode )
        {
            case ESubmitMode::Auto :
            case ESubmitMode::Deferred :
                break;  // TODO ?

            // try to submit
            case ESubmitMode::Immediately :
            case ESubmitMode::Force :
                rts._FlushQueue( batch.GetQueueType(), batch.GetFrameId(), (mode == ESubmitMode::Force) );
                break;
        }
        END_ENUM_CHECKS();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Recycle
=================================================
*/
    void  RTSCHEDULER::DrawCommandBatchApi::Recycle (uint indexInPool) __NE___
    {
        auto&   rts = RenderTaskScheduler();
        rts._drawBatchPool.Unassign( indexInPool );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Resolve
=================================================
*/
    bool  RTSCHEDULER::BatchSubmitDepsManager::Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) __NE___
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
    void  RTSCHEDULER::BatchSubmitDepsManager::DbgDetectDeadlock (const CheckDepFn_t &fn) __NE___
    {
        RenderTaskScheduler().DbgForEachBatch( fn, True{"pending only"} );
    }
#endif
//-----------------------------------------------------------------------------



/*
=================================================
    Resolve
=================================================
*/
    bool  RTSCHEDULER::BatchCompleteDepsManager::Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) __NE___
    {
        if_likely( auto* batch_pp = dep.GetIf< RC<CommandBatch_t> >() )
        {
            if_unlikely( *batch_pp == null )
                return true;

            CHECK_ERR( (**batch_pp)._AddOnCompleteDependency( RVRef(task), INOUT bitIndex ));
            return true;
        }
        else
        if ( auto* fence = dep.GetIf< DeviceToHostSyncPtr >() )
        {
            if_unlikely( *fence == null )
                return true;

            if_likely( auto batch2_p = DynCast<CommandBatch_t>( *fence ))
            {
                CHECK_ERR( (*batch2_p)._AddOnCompleteDependency( RVRef(task), INOUT bitIndex ));
                return true;
            }
        }
        return false;
    }

/*
=================================================
    DbgDetectDeadlock
=================================================
*/
#ifdef AE_DEBUG
    void  RTSCHEDULER::BatchCompleteDepsManager::DbgDetectDeadlock (const CheckDepFn_t &fn) __NE___
    {
        RenderTaskScheduler().DbgForEachBatch( fn, False{"submitted only"} );
    }
#endif
//-----------------------------------------------------------------------------



/*
=================================================
    BeginFrameTask::Run
=================================================
*/
    void  RTSCHEDULER::BeginFrameTask::Run () __Th___
    {
        if_unlikely( not _RunImpl() )
        {
            _ResetStates();
            CHECK_TE( false, "BeginFrame - failed" );
        }
    }

/*
=================================================
    BeginFrameTask::_RunImpl
=================================================
*/
    bool  RTSCHEDULER::BeginFrameTask::_RunImpl () __Th___
    {
        auto&   rts = RenderTaskScheduler();

        // check frame UID
        CHECK_ERR_MSG( rts._perFrameUID[ _frameId.Index() ].load() == _frameId,
                        "Invalid frame UID, task will be canceled" );

        if_unlikely( not rts._IsFrameCompleted( _frameId ))
        {
            Continue();
            return true;
        }

        CHECK_ERR_MSG( rts._SetState( EState::BeginFrame, EState::RecordFrame ),
                        "incorrect render task scheduler state, must be 'EState::BeginFrame'" );

        // update frame time
        {
            auto    cur_time    = TimePoint_t::clock::now();
            auto    last_update = rts._lastUpdate.exchange( cur_time );

            rts._timeDelta.store( TimeCast<secondsf>( cur_time - last_update ).count() );
        }

        rts._resMngr->OnBeginFrame( _frameId, _config );

      #ifdef AE_ENABLE_VULKAN
        CHECK( rts._cmdPoolMngr->NextFrame( _frameId ));

        // release expired resources before next frame
        const uint  frame_offset = _frameId.MaxFrames() + ResourceManager_t::ExpiredResFrameOffset;

        if_likely( auto prev_id = _frameId.Sub( frame_offset );  prev_id.has_value() )
        {
            AsyncTask   task = MakeRC< ResourceManager_t::VReleaseExpiredResourcesTask >( *prev_id );
            if_likely( Scheduler().Run( task ))
                rts.AddNextFrameDeps( RVRef(task) );
        }
      #endif

        DBG_GRAPHICS_ONLY(
            if ( auto prof = rts.GetProfiler() )
                prof->NextFrame( _frameId );

            rts._dbgFrameId.store( _frameId );
        )

        return true;
    }

/*
=================================================
    BeginFrameTask::OnCancel
=================================================
*/
    void  RTSCHEDULER::BeginFrameTask::OnCancel () __NE___
    {
        _ResetStates();
    }

/*
=================================================
    BeginFrameTask::_ResetStates
=================================================
*/
    inline void  RTSCHEDULER::BeginFrameTask::_ResetStates() __NE___
    {
        auto&   rts = RenderTaskScheduler();

        // check frame UID
        CHECK_MSG( rts._perFrameUID[ _frameId.Index() ].load() == _frameId, "Invalid frame UID" );

        rts._SetState( EState::Idle );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    EndFrameTask::Run
=================================================
*/
    void  RTSCHEDULER::EndFrameTask::Run () __Th___
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
    bool  RTSCHEDULER::EndFrameTask::_RunImpl () __Th___
    {
        auto&   rts = RenderTaskScheduler();

        // check frame UID
        CHECK_ERR_MSG( rts._perFrameUID[ _frameId.Index() ].load() == _frameId,
                        "Invalid frame UID, task will be canceled" );

        const auto  q_mask = rts.GetDevice().GetAvailableQueues();

        // Force flush all pending batches.
        // Timeline semaphore allow to submit commands without strict ordering.
        for (auto q = q_mask; q != Zero;)
        {
            rts._FlushQueue( ExtractBitLog2<EQueueType>( INOUT q ), _frameId, true );
        }

        for (auto& q : rts._queueMap)
        {
            auto    bf = BitCast<QueueData::Bitfield>( q.bits.exchange( 0 ));

            CHECK_ERR_MSG( bf.packed.pending == bf.packed.required and
                           bf.packed.pending == bf.packed.submitted,
                           "Some batches are not submitted, EndFrame task must depends on all tasks which submit batches" );

            q.pending.fill( Default );
        }

        rts._resMngr->OnEndFrame( _frameId );

        CHECK_ERR_MSG( rts._SetState( EState::RecordFrame, EState::Idle ),
                        "incorrect render task scheduler state, must be 'EState::RecordFrame'" );

        DBG_GRAPHICS_ONLY(
            CHECK( _frameId == rts.DbgFrameId() );
            rts._dbgFrameId.store( Default );
        )
        return true;
    }

/*
=================================================
    EndFrameTask::OnCancel
=================================================
*/
    void  RTSCHEDULER::EndFrameTask::OnCancel () __NE___
    {
        _ResetStates();
    }

/*
=================================================
    EndFrameTask::_ResetStates
=================================================
*/
    inline void  RTSCHEDULER::EndFrameTask::_ResetStates () __NE___
    {
        auto&   rts = RenderTaskScheduler();

        // check frame UID
        CHECK_MSG( rts._perFrameUID[ _frameId.Index() ].load() == _frameId, "Invalid frame UID" );

        for (auto& q : rts._queueMap)
        {
            q.bits.store( 0 );
            q.pending.fill( Default );
        }

        rts._SetState( EState::Idle );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    RTSCHEDULER::RTSCHEDULER (const Device_t &dev) __NE___ :
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
    RTSCHEDULER::~RTSCHEDULER () __NE___
    {
        Deinitialize();
    }

/*
=================================================
    _Instance
=================================================
*/
    INTERNAL_LINKAGE( InPlace<RTSCHEDULER>  s_RenderTaskScheduler );

    RTSCHEDULER&  RTSCHEDULER::_Instance () __NE___
    {
        return s_RenderTaskScheduler.AsRef();
    }

/*
=================================================
    CreateInstance
=================================================
*/
    void  RTSCHEDULER::CreateInstance (const Device_t &dev) __NE___
    {
        s_RenderTaskScheduler.Create( dev );

        MemoryBarrier( EMemoryOrder::Release );
    }

/*
=================================================
    DestroyInstance
=================================================
*/
    void  RTSCHEDULER::DestroyInstance () __NE___
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
    bool  RTSCHEDULER::Initialize (const GraphicsCreateInfo &info) __NE___
    {
        CHECK_ERR( _SetState( EState::Initial, EState::Initialization ));

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
        #else
        #   error not implemented
        #endif

        for (uint i = 0; i < info.maxFrames; ++i) {
            _perFrame[i].submitted.reserve( _MaxSubmittedBatches );     // throw
        }

        _resMngr.reset( new ResourceManager_t{ _device });
        CHECK_ERR( _resMngr->Initialize( info ));

      #ifdef AE_ENABLE_VULKAN
        _cmdPoolMngr.reset( new VCommandPoolManager{ _device });
      #endif

        Scheduler().RegisterDependency< CmdBatchOnSubmit >( _submitDepMngr );

        Scheduler().RegisterDependency< RC<CommandBatch_t> >( _completeDepMngr );
        Scheduler().RegisterDependency< DeviceToHostSyncPtr >( _completeDepMngr );

        if ( info.useRenderGraph )
            _rg.reset( new RenderGraph_t{} );

        CHECK_ERR( _SetState( EState::Initialization, EState::Idle ));
        return true;
    }

/*
=================================================
    Deinitialize
=================================================
*/
    void  RTSCHEDULER::Deinitialize () __NE___
    {
        CHECK_ERRV( _SetState( EState::Idle, EState::Destroyed ));

        CHECK( _WaitAll( seconds{10} ));

        _rg.reset();

        _batchPool.Release( True{"check for assigned"} );

      #ifdef AE_ENABLE_VULKAN
        _cmdPoolMngr = null;
      #endif

        if ( _resMngr )
        {
            _resMngr->Deinitialize();
            _resMngr = null;
        }

        Scheduler().UnregisterDependency< CmdBatchOnSubmit >();
        Scheduler().UnregisterDependency< RC<CommandBatch_t> >();
        Scheduler().UnregisterDependency< DeviceToHostSyncPtr >();
    }

/*
=================================================
    AddFrameDeps
=================================================
*/
    void  RTSCHEDULER::AddFrameDeps (FrameUID frameId, ArrayView<AsyncTask> deps) __NE___
    {
        ASSERT( frameId > GetFrameId() );
        ASSERT( frameId <= GetFrameId().NextCycle() );
        //ASSERT( AnyEqual( _GetState(), EState::BeginFrame, EState::RecordFrame, EState::Idle ));

        if_unlikely( deps.empty() )
            return;

        auto        begin_deps2 = _beginDeps.WriteNoLock();
        DeferExLock lock        { begin_deps2 };
        EXLOCK( lock );

        auto&       begin_deps = (*begin_deps2)[ frameId.Index() ];

        for (auto& dep : deps)
        {
            if_unlikely( begin_deps.size() == begin_deps.capacity() )
            {
                BeginDepsArray_t    temp {RVRef(begin_deps)};
                begin_deps.clear();

                lock.unlock();

                AsyncTask   task = Scheduler().WaitAsync( Threading::ETaskQueue::Renderer, Tuple{ ArrayView<AsyncTask>{ temp }});

                lock.lock();
                begin_deps.push_back( RVRef(task) );
            }

            if_likely( dep )
                begin_deps.push_back( dep );    // nothrow
        }
    }

    void  RTSCHEDULER::AddFrameDeps (FrameUID frameId, AsyncTask dep) __NE___
    {
        if ( not dep )
            return;

        return AddFrameDeps( frameId, ArrayView<AsyncTask>{ dep });
    }

/*
=================================================
    WaitFrame
=================================================
*/
    AsyncTask  RTSCHEDULER::WaitFrame (FrameUID frameId) __NE___
    {
        ASSERT( frameId > GetFrameId() );
        ASSERT( frameId <= GetFrameId().NextCycle() );

        auto    begin_deps2 = _beginDeps.WriteNoLock();
        EXLOCK( begin_deps2 );

        auto&   begin_deps = (*begin_deps2)[ frameId.Index() ];

        if ( begin_deps.empty() )
            return null;

        BeginDepsArray_t    temp {RVRef(begin_deps)};
        begin_deps.clear();

        return Scheduler().WaitAsync( ETaskQueue::Renderer, Tuple{ ArrayView<AsyncTask>{ temp }} );
    }

/*
=================================================
    _FlushQueue
=================================================
*/
    bool  RTSCHEDULER::_FlushQueue (EQueueType queueType, FrameUID frameId, bool forceFlush)
    {
        TempBatches_t   pending;

        // lock queue and submit
        {
            auto&       q       = _queueMap[ uint(queueType) ];
            auto        queue   = GetDevice().GetQueue( queueType );
            CHECK_ERR( queue );

            const auto  GetRange = [&q] () -> Pair<int, int>
            {{
                const auto  ps_bits         = BitCast<QueueData::Bitfield>( q.bits.load() );
                const int   last_submitted  = BitScanReverse( ps_bits.packed.submitted ) + 1;
                const int   last_pending    = BitScanForward( ~ps_bits.packed.pending );
                return {last_submitted, last_pending};
            }};

            // early exit before lock if no pending batches
            if ( auto r = GetRange();  r.first >= r.second or r.second <= 0 )
                return false;

            // lock queue
            DeferExLock lock {queue->guard};
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
                const auto  range = GetRange();

                // batches may be submitted when waiting for the mutex
                if ( range.first >= range.second or range.second <= 0 )
                    return false;

                QueueData::Bitfield mask;
                mask.packed.submitted = ToBitMask<ulong>( range.second );
                mask.packed.submitted &= ~ToBitMask<ulong>( range.first );

                const auto  old_bits = BitCast<QueueData::Bitfield>( q.bits.fetch_or( mask.value ));    // add bits to submitted
                CHECK( not AnyBits( old_bits.packed.submitted, mask.packed.submitted ));                // already submitted

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
            #else
            #   error not implemented
            #endif
        }

        // add to submitted batches
        {
            auto&   frame = _perFrame[ frameId.Index() ];
            EXLOCK( frame.guard );
            frame.submitted.insert( frame.submitted.end(), pending.begin(), pending.end() );    // throw
            ASSERT( frame.submitted.size() <= _MaxSubmittedBatches );
        }

        return true;
    }

/*
=================================================
    SkipCmdBatches
=================================================
*/
    void  RTSCHEDULER::SkipCmdBatches (EQueueType queue, uint bits) __NE___
    {
        constexpr uint  mask = ToBitMask<uint>( _MaxPendingBatches );

        ASSERT( bits == UMax or bits < mask );
        CHECK_ERRV( AllBits( _device.GetAvailableQueues(), EQueueMask(1u << uint(queue)) ));

        bits &= mask;

        auto&   packed_bits = _queueMap[ uint(queue) ].bits;

        QueueData::Bitfield exp = BitCast<QueueData::Bitfield>( packed_bits.load() );

        for (;;)
        {
            QueueData::Bitfield desired = exp;

            desired.packed.required = exp.packed.required | bits;
            desired.packed.pending  = exp.packed.pending  | (bits & ~exp.packed.required);

            if ( packed_bits.CAS( INOUT exp.value, desired.value ))
                return;

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
    RC<RTSCHEDULER::CommandBatch_t>  RTSCHEDULER::BeginCmdBatch (EQueueType queue, uint submitIdx, DebugLabel dbg, void* userData) __NE___
    {
        CHECK_ERR( submitIdx < _MaxPendingBatches );
        CHECK_ERR( AnyEqual( _GetState(), EState::Idle, EState::BeginFrame, EState::RecordFrame ));
        CHECK_ERR( AllBits( _device.GetAvailableQueues(), EQueueMask(1u << uint(queue)) ));

        // validate 'submitIdx'
        {
            QueueData::Bitfield bf;
            bf.packed.required = 1ull << submitIdx;

            const auto  old_bits = BitCast<QueueData::Bitfield>( _queueMap[ uint(queue) ].bits.fetch_or( bf.value ));

            CHECK_ERR_MSG( not AnyBits( old_bits.packed.required,  bf.packed.required ), "batch with 'submitIdx' is already created" );
            CHECK_ERR_MSG( not AnyBits( old_bits.packed.pending,   bf.packed.required ) or
                           not AnyBits( old_bits.packed.submitted, bf.packed.required ), "batch with 'submitIdx' is marked as unused" );
        }

        uint    index;
        CHECK_ERR( _batchPool.Assign( OUT index, [](auto* ptr, uint idx) { new (ptr) CommandBatch_t{ idx }; }));

        auto&   batch = _batchPool[ index ];

        // 'GetFrameId()' may not equal to 'DbgFrameId()'

        if_likely( batch._Create( queue, GetFrameId(), submitIdx, dbg, userData ))
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
    bool  RTSCHEDULER::WaitAll (milliseconds timeout) __NE___
    {
        CHECK_ERR( AnyEqual( _GetState(), EState::Idle, EState::BeginFrame, EState::RecordFrame ));

        return _WaitAll( timeout );
    }

/*
=================================================
    _WaitAll
=================================================
*/
    bool  RTSCHEDULER::_WaitAll (milliseconds timeout)
    {
        using Clock_t = std::chrono::high_resolution_clock;

        const auto  sleep_time  = milliseconds{10};
        const auto  start       = Clock_t::now();
        const auto  q_mask      = _device.GetAvailableQueues();

        for (;;)
        {
            FrameUID    frame_id    = GetFrameId();
            bool        complete    = true;

            for (auto q = q_mask; q != Zero;) {
                _FlushQueue( ExtractBitLog2<EQueueType>( INOUT q ), frame_id, false );
            }

            for (uint f = 0, cnt = frame_id.MaxFrames(); f < cnt; ++f)
            {
                frame_id.Inc();

                if_unlikely( not _IsFrameCompleted( frame_id ))
                {
                    complete = false;
                    ThreadUtils::Sleep( sleep_time );
                }
            }

            if ( complete )
                return true;

            auto    dt = Clock_t::now() - start;
            if ( dt > timeout )
                return false;
        }
    }

/*
=================================================
    SetProfiler
=================================================
*/
    void  RTSCHEDULER::SetProfiler (RC<IGraphicsProfiler> profiler) __NE___
    {
    DBG_GRAPHICS_ONLY(
        _profiler = RVRef(profiler);

        if ( not profiler )
            return;

        for (auto& q : _device.GetQueues()) {
            profiler->SetQueue( q.type, q.debugName );
        }
    )}

/*
=================================================
    DbgCheckFrameId
=================================================
*/
#if AE_DBG_GRAPHICS
    void  RTSCHEDULER::DbgCheckFrameId (const FrameUID expected, StringView taskName) C_NE___
    {
        const FrameUID  cur = DbgFrameId();
        CHECK_MSG( cur == expected,
            "Current frame ("s << ToString(ulong(cur.Unique())) << ") dosn't match expected frame (" <<
            ToString(ulong(expected.Unique())) << ") in task '" << taskName << "'.\n"
            "This may happens when RenderTask doesn't depends on 'BeginFrame' task or 'EndFrame' happens before current task is started/completed." );
    }
#endif

/*
=================================================
    DbgForEachBatch
=================================================
*/
#ifdef AE_DEBUG
    void  RTSCHEDULER::DbgForEachBatch (const Threading::ITaskDependencyManager::CheckDepFn_t &fn, Bool) __NE___
    {
        using EStatus = CommandBatch_t::EStatus;

        const auto  CheckBatch = [&fn] (CommandBatch_t &batch)
        {{
            String  info;
            info << "cmdbatch '" << batch.DbgName() << "' (" << ToString<16>(usize(&batch)) << "), status: ";

            const auto  status = batch._status.load();

            BEGIN_ENUM_CHECKS();
            switch ( status )
            {
                case EStatus::Destroyed :   info << "Destroyed";    break;
                case EStatus::Initial :     info << "Initial";      break;
                case EStatus::Recorded :    info << "Recorded";     break;
                case EStatus::Pending :     info << "Pending";      break;
                case EStatus::Submitted :   { info << "Submitted";  EXLOCK( batch._onSubmitDepsGuard );     ASSERT( batch._onSubmitDeps.empty() );      break; }
                case EStatus::Completed :   { info << "Completed";  EXLOCK( batch._onCompleteDepsGuard );   ASSERT( batch._onCompleteDeps.empty() );    break; }
            }
            END_ENUM_CHECKS();

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
                auto    batch = RC<CommandBatch_t>{ &_batchPool[ uint(i) ]};

                CheckBatch( *batch );
            }
        }
    }
#endif // AE_DEBUG

