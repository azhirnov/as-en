// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#   define SUFFIX           V
#   define CMDBATCH         VCommandBatch
#   if not AE_VK_TIMELINE_SEMAPHORE
#     define ENABLE_VK_TIMELINE_SEMAPHORE
#   endif

#elif defined(AE_ENABLE_METAL)
#   define SUFFIX           M
#   define CMDBATCH         MCommandBatch

#else
#   error not implemented
#endif
//-----------------------------------------------------------------------------



/*
=================================================
    DbgFullName
=================================================
*/
#if AE_DBG_GRAPHICS
    String  RenderTask::DbgFullName () C_NE___
    {
        String  str;
        str << _batch->DbgName() << " |" << ToString(GetExecutionIndex()) << "| " << _dbgName;
        return str;
    }
#endif
//-----------------------------------------------------------------------------



/*
=================================================
    SetSubmissionMode
=================================================
*/
    void  CMDBATCH::SetSubmissionMode (ESubmitMode mode) __NE___
    {
        _submitMode = mode;

        ASSERT( IsRecording() );
    }

/*
=================================================
    _ReleaseObject
=================================================
*/
    void  CMDBATCH::_ReleaseObject () __NE___
    {
        MemoryBarrier( EMemoryOrder::Acquire );

        const EStatus   status = _status.load();

        if_unlikely( status != EStatus::Completed )
        {
            _cmdPool.Reset();
        }
        ASSERT( status == EStatus::Completed );

        // delete anyway
        _status.store( EStatus::Destroyed );

        DEBUG_ONLY({
            EXLOCK( _onCompleteDepsGuard );
            ASSERT( _onCompleteDeps.empty() );
        })
        DEBUG_ONLY({
            EXLOCK( _onSubmitDepsGuard );
            ASSERT( _onSubmitDeps.empty() );
        })

        MemoryBarrier( EMemoryOrder::Release );
        RenderTaskScheduler_t::CommandBatchApi::Recycle( _indexInPool );
    }

/*
=================================================
    _Submit
=================================================
*/
    bool  CMDBATCH::_Submit () __NE___
    {
        CHECK_ERR( _status.exchange( EStatus::Pending ) == EStatus::Recorded );

        _cmdPool.Lock();
        CHECK_ERR( _cmdPool.IsReady() );

      #if defined(AE_ENABLE_VULKAN)
        CHECK_ERR( _cmdPool.CommitIndirectBuffers( RenderTaskScheduler().GetCommandPoolManager(), GetQueueType(), GetCmdBufType() ));
      #elif defined(AE_ENABLE_METAL)
        CHECK_ERR( _cmdPool.CommitIndirectBuffers( GetQueueType(), GetCmdBufType() ));
      #else
      # error not implemented
      #endif

        RenderTaskScheduler_t::CommandBatchApi::Submit( *this, _submitMode );
        return true;
    }

/*
=================================================
    _AddOnSubmitDependency
=================================================
*/
    bool  CMDBATCH::_AddOnSubmitDependency (AsyncTask task, INOUT uint &index) __NE___
    {
        EXLOCK( _onSubmitDepsGuard );

        // skip dependency if already submitted
        if_unlikely( IsSubmitted() )
            return true;

        CHECK_ERR( _onSubmitDeps.size() < _onSubmitDeps.capacity() );   // check for overflow

        _onSubmitDeps.push_back( RVRef(task), TaskDependency{ index, true });
        ++index;

        return true;
    }

/*
=================================================
    _AddOnCompleteDependency
=================================================
*/
    bool  CMDBATCH::_AddOnCompleteDependency (AsyncTask task, INOUT uint &index) __NE___
    {
        EXLOCK( _onCompleteDepsGuard );

        // skip dependency if already completed
        if_unlikely( IsCompleted() )
            return true;

        CHECK_ERR( _onCompleteDeps.size() < _onCompleteDeps.capacity() );   // check for overflow

        _onCompleteDeps.push_back( RVRef(task), TaskDependency{ index, true });
        ++index;

        return true;
    }

/*
=================================================
    _OnSubmit2
=================================================
*/
    void  CMDBATCH::_OnSubmit2 () __NE___
    {
        CHECK( _status.exchange( EStatus::Submitted ) == EStatus::Pending );
        _cmdPool.Reset();

        // allow to run tasks which waits when batch is submitted
        {
            EXLOCK( _onSubmitDepsGuard );
            for (auto dep : _onSubmitDeps) {
                Threading::ITaskDependencyManager::_SetDependencyCompletionStatus( dep.Get<0>(), dep.Get<1>().bitIndex, false );
            }
            _onSubmitDeps.clear();
        }
    }

/*
=================================================
    _OnComplete
=================================================
*/
    void  CMDBATCH::_OnComplete () __NE___
    {
      #ifdef AE_ENABLE_VULKAN
        #if AE_VK_TIMELINE_SEMAPHORE
            ++_tlSemaphoreVal;
        #else
            _fence = null;
        #endif
      #endif

        DEBUG_ONLY({
            EXLOCK( _gpuInDepsGuard );
            ASSERT( _gpuInDeps.empty() );   // 'AddInputSemaphore()' used after submission
        })
        DEBUG_ONLY({
            EXLOCK( _gpuOutDepsGuard );
            ASSERT( _gpuOutDeps.empty() );  // 'AddOutputSemaphore()' used after submission
        })

        // allow to run tasks which waits when batch is complete
        {
            EXLOCK( _onCompleteDepsGuard );
            for (auto dep : _onCompleteDeps) {
                Threading::ITaskDependencyManager::_SetDependencyCompletionStatus( dep.Get<0>(), dep.Get<1>().bitIndex, false );
            }
            _onCompleteDeps.clear();
        }

        DBG_GRAPHICS_ONLY(
            if ( _profiler )
                _profiler->BatchComplete( this );
            _profiler = null;
        )

        CHECK( _status.exchange( EStatus::Completed ) == EStatus::Submitted );
    }

/*
=================================================
    _SetTaskBarriers
=================================================
*/
    void  CMDBATCH::_SetTaskBarriers (const TaskBarriers_t* pBarriers, uint index) __NE___
    {
        if ( pBarriers == null )
            return;

        ASSERT( _perTaskBarriers[ index ] == null );
        ASSERT( IsRecording() );
        ASSERT( index < _perTaskBarriers.size() );
        ASSERT_Lt( index, CurrentCmdBufIndex()*2+2 );

        _perTaskBarriers[ index ] = pBarriers;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _DbgCheckFrameId
=================================================
*/
#if AE_DBG_GRAPHICS
    void  RenderTask::_DbgCheckFrameId () C_NE___
    {
        RenderTaskScheduler().DbgCheckFrameId( GetFrameId(), DbgFullName() );
    }
#endif
