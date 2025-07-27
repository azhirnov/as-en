// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/DataSource/AsyncDataSource.h"

namespace AE::_Coro_
{
	constinit StaticRC<IAsyncDataSourceRequest>  IAsyncDataSourceRequest::CanceledRequest::s_canceled { _ConstInitStaticRC(0), EStatus::Canceled };

/*
=================================================
	_AddOnCompleteDependency
=================================================
*/
	bool  IAsyncDataSourceRequest::_AddOnCompleteDependency (AsyncTaskImpl &task, const Bool isStrong) __NE___
	{
		EXLOCK( _depsGuard );

		// skip dependency if already completed
		if_unlikely( EStatus stat = Status();  stat > EStatus::_Finished )
			return stat == EStatus::Completed;

		CHECK_ERR( not _deps.IsFull() );	// check for overflow

		AsyncTaskImpl::TaskDependencyManagerApi::IncWaitCounter( task );

		_deps.emplace_back( task.GetRC() ).SetExtra( uint{isStrong} );
		return true;
	}

/*
=================================================
	_SetDependencyCompleteStatus
----
	Allow to run tasks which waits when request is completed.
	'complete' - indicates that request is successfully complete or canceled/failed.
=================================================
*/
	void  IAsyncDataSourceRequest::_SetDependencyCompleteStatus (const bool complete) __NE___
	{
		EXLOCK( _depsGuard );
		for (auto dep : _deps)
		{
			//	weak/strong	& complete	-> complete
			//	strong		& cancelled	-> cancelled
			//	weak		& cancelled	-> complete
			const bool	is_strong	= dep.Extra() != 0;
			const bool	is_canceled = is_strong and (not complete);

			AsyncTaskImpl::TaskDependencyManagerApi::SetDependencyCompletionStatus( *dep, Bool{is_canceled} );
		}
		_deps.clear();
	}

} // AE::_Coro_


namespace AE::Threading
{
/*
=================================================
	Resolve
=================================================
*/
	bool  AsyncDSRequestDependencyManager::Resolve (AnyTypeCRef dep, Task &task, Bool defaultIsStrongDep) __NE___
	{
		// only 'Background' task should depends on AsyncDSRequest
		ASSERT( AnyEqual( task.QueueType(), ETaskQueue::Background, ETaskQueue::Unknown ));

		if_likely( auto* request_pp = dep.GetIf< AsyncDSRequest >() )
		{
			if_unlikely( *request_pp == null )
				return true;

			auto*	request = (*request_pp).get();	// non-null

			// 'true'	- dependency added or successfully complete.
			// 'false'	- dependency is cancelled or on an error.
			return request->_AddOnCompleteDependency( task, defaultIsStrongDep );
		}

		if_likely( auto* weak_req_pp = dep.GetIf< WeakAsyncDSRequest >() )
		{
			if_unlikely( weak_req_pp->_task == null )
				return true;

			auto*	request = weak_req_pp->_task.get();	// non-null

			Unused( request->_AddOnCompleteDependency( task, False{"weak"} ));
			return true;	// always return 'true' because it is weak dependency
		}

		RETURN_ERR( "unsupported dependency type" );
	}


} // AE::Threading
