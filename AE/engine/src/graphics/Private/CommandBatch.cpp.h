// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#if defined(AE_ENABLE_VULKAN)
#	define SUFFIX					V
#	define CMDBATCH					VCommandBatch
#	if AE_VK_TIMELINE_SEMAPHORE
#	  define AE_TIMELINE_SEMAPHORE	1
#	else
#	  define AE_TIMELINE_SEMAPHORE	0
#	  define ENABLE_VK_VIRTUAL_FENCE
#	endif

#elif defined(AE_ENABLE_METAL)
#	define SUFFIX					M
#	define CMDBATCH					MCommandBatch
#	define AE_TIMELINE_SEMAPHORE	1

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#	define SUFFIX					R
#	define CMDBATCH					RCommandBatch
#	define AE_TIMELINE_SEMAPHORE	1

#else
#	error not implemented
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
		String	str;
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
#ifndef AE_ENABLE_REMOTE_GRAPHICS
	void  CMDBATCH::_ReleaseObject () __NE___
	{
		MemoryBarrier( EMemoryOrder::Acquire );

		const EStatus	status = _status.load();

		if_unlikely( status != EStatus::Completed )
		{
			_cmdPool.Reset();
		}
		ASSERT( status == EStatus::Completed );

		// delete anyway
		_status.store( EStatus::Destroyed );

		GFX_DBG_ONLY({
			EXLOCK( _onCompleteDepsGuard );
			CHECK( _onCompleteDeps.empty() );
		})
		GFX_DBG_ONLY({
			EXLOCK( _onSubmitDepsGuard );
			CHECK( _onSubmitDeps.empty() );
		})

		MemoryBarrier( EMemoryOrder::Release );
		RenderTaskScheduler::CommandBatchApi::Recycle( this );
	}
#endif
/*
=================================================
	EndRecordingAndSubmit
=================================================
*/
	bool  CMDBATCH::EndRecordingAndSubmit () __NE___
	{
		CHECK_ERR( _EndRecording() );
		return _Submit();
	}

/*
=================================================
	_Submit
=================================================
*/
	bool  CMDBATCH::_Submit () __NE___
	{
		CHECK_ERR( _status.Set( EStatus::Recorded, EStatus::Pending ));

		_cmdPool.Lock();
		CHECK_ERR( _cmdPool.IsReady() );

	  #if defined(AE_ENABLE_VULKAN)
		CHECK_ERR( _cmdPool.CommitIndirectBuffers( GraphicsScheduler().GetCommandPoolManager(), GetQueueType(), GetCmdBufType() ));
	  #elif defined(AE_ENABLE_METAL)
		CHECK_ERR( _cmdPool.CommitIndirectBuffers( GetQueueType(), GetCmdBufType() ));
	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		// do nothing
	  #else
	  #	error not implemented
	  #endif

		RenderTaskScheduler::CommandBatchApi::Submit( *this, _submitMode );
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

		CHECK_ERR( not _onSubmitDeps.IsFull() );

		_onSubmitDeps.push_back( RVRef(task), TaskDependency{ index, True{"strong ref"} });
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

		CHECK_ERR( not _onCompleteDeps.IsFull() );

		_onCompleteDeps.push_back( RVRef(task), TaskDependency{ index, True{"strong ref"} });
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
		CHECK( _status.Set( EStatus::Pending, EStatus::Submitted ));
		_cmdPool.Reset();

		// allow to run tasks which waits when batch is submitted
		{
			EXLOCK( _onSubmitDepsGuard );
			for (auto dep : _onSubmitDeps) {
				Threading::ITaskDependencyManager::_SetDependencyCompletionStatus( *dep.Get<0>(), dep.Get<1>().bitIndex, False{"not canceled"} );
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
		#if AE_TIMELINE_SEMAPHORE
			++_tlSemaphoreVal;
		#endif
		#ifdef ENABLE_VK_VIRTUAL_FENCE
			_fence = null;
		#endif

		GFX_DBG_ONLY({
			EXLOCK( _gpuInDepsGuard );
			CHECK( _gpuInDeps.empty() );	// 'AddInputSemaphore()' used after submission
		})
		GFX_DBG_ONLY({
			EXLOCK( _gpuOutDepsGuard );
			CHECK( _gpuOutDeps.empty() );	// 'AddOutputSemaphore()' used after submission
		})

		// allow to run tasks which waits when batch is complete
		{
			EXLOCK( _onCompleteDepsGuard );
			for (auto dep : _onCompleteDeps) {
				Threading::ITaskDependencyManager::_SetDependencyCompletionStatus( *dep.Get<0>(), dep.Get<1>().bitIndex, False{"not canceled"} );
			}
			_onCompleteDeps.clear();
		}

		GFX_DBG_ONLY(
			if ( _profiler )
				_profiler->BatchComplete( this );
			_profiler = null;
		)

		CHECK( _status.Set( EStatus::Submitted, EStatus::Completed ));
	}

/*
=================================================
	_SetTaskBarriers
=================================================
*/
	void  CMDBATCH::_SetTaskBarriers (const TaskBarriersPtr_t pBarriers, uint index) __NE___
	{
		if ( pBarriers == null )
			return;

		ASSERT( _perTaskBarriers[ index ] == null );
		ASSERT( IsRecording() );
		ASSERT( index < _perTaskBarriers.size() );
	//	ASSERT_Lt( index, CurrentCmdBufIndex()*2+2 );	// TODO ?

		_perTaskBarriers[ index ] = pBarriers.get();
	}

/*
=================================================
	AddInputSemaphore
=================================================
*/
	bool  CMDBATCH::AddInputSemaphore (GpuSyncObj_t sem, ulong value) __NE___
	{
	  #if not AE_TIMELINE_SEMAPHORE
		ASSERT( value == 0 );
	  #endif

	  #if defined(AE_ENABLE_VULKAN)
		CHECK_ERR( sem != Default );
	  #elif defined(AE_ENABLE_METAL) and defined(AE_ENABLE_REMOTE_GRAPHICS)
		CHECK_ERR( sem );
	  #endif

		CHECK_ERR( not IsSubmitted() );

		EXLOCK( _gpuInDepsGuard );
		CHECK_ERR( not _gpuInDeps.IsFull() );

		auto&	val = _gpuInDeps.emplace( sem, value ).first->second;
		val = Min( val, value );

		return true;
	}

	bool  CMDBATCH::AddInputSemaphore (const CmdBatchDependency_t &dep) __NE___
	{
		return AddInputSemaphore( dep.semaphore, dep.value );
	}

/*
=================================================
	AddInputDependency
----
	GPU to GPU dependency.
	returns 'false' on overflow or if batch is already completed.
=================================================
*/
	bool  CMDBATCH::AddInputDependency (RC<CMDBATCH> batch) __NE___
	{
		if_unlikely( not batch )
			return true;

		return AddInputDependency( *batch );
	}

	bool  CMDBATCH::AddInputDependency (const CMDBATCH &batch) __NE___
	{
		CHECK_ERR( not IsSubmitted() );

		// different queues
	  #if AE_TIMELINE_SEMAPHORE
		if_likely( batch.GetQueueType() != GetQueueType() )
			return AddInputSemaphore( batch.GetSemaphore() );
	  #endif

		// same queue
		CHECK_ERR( batch.GetQueueType() == GetQueueType() );
		CHECK_ERR( batch.GetSubmitIndex() < GetSubmitIndex() );

		return true;
	}

/*
=================================================
	AddOutputSemaphore
=================================================
*/
	bool  CMDBATCH::AddOutputSemaphore (GpuSyncObj_t sem, ulong value) __NE___
	{
	  #if not AE_TIMELINE_SEMAPHORE
		ASSERT( value == 0 );
	  #endif

	  #if defined(AE_ENABLE_VULKAN)
		CHECK_ERR( sem != Default );
	  #elif defined(AE_ENABLE_METAL) and defined(AE_ENABLE_REMOTE_GRAPHICS)
		CHECK_ERR( sem );
	  #endif

		CHECK_ERR( not IsSubmitted() );

		EXLOCK( _gpuOutDepsGuard );
		CHECK_ERR( not _gpuOutDeps.IsFull() );

		auto&	val = _gpuOutDeps.emplace( sem, value ).first->second;
		val = Min( val, value );

		return true;
	}

	bool  CMDBATCH::AddOutputSemaphore (const CmdBatchDependency_t &dep) __NE___
	{
		return AddOutputSemaphore( dep.semaphore, dep.value );
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
		GraphicsScheduler().DbgCheckFrameId( GetFrameId(), DbgFullName() );
	}
#endif
