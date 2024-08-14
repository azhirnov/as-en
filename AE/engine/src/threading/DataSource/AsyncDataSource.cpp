// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/DataSource/AsyncDataSource.h"

namespace AE::Threading
{

/*
=================================================
	_AddOnCompleteDependency
=================================================
*/
	bool  _hidden_::IAsyncDataSourceRequest::_AddOnCompleteDependency (AsyncTask task, INOUT uint &index, const Bool isStrong) __NE___
	{
		EXLOCK( _depsGuard );

		// skip dependency if already completed
		if_unlikely( EStatus stat = Status();  stat > EStatus::_Finished )
			return stat == EStatus::Completed;

		CHECK_ERR( not _deps.IsFull() );	// check for overflow

		TaskDependency	bits;
		bits.bitIndex	= index++;
		bits.isStrong	= isStrong;

		_deps.push_back( RVRef(task), bits );
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
	void  _hidden_::IAsyncDataSourceRequest::_SetDependencyCompleteStatus (const bool complete) __NE___
	{
		EXLOCK( _depsGuard );
		for (auto dep : _deps)
		{
			//	weak/strong	& complete	-> complete
			//	strong		& cancelled	-> cancelled
			//	weak		& cancelled	-> complete
			const bool	is_canceled = dep.Get<1>().isStrong and (not complete);

			ITaskDependencyManager::_SetDependencyCompletionStatus( *dep.Get<0>(), dep.Get<1>().bitIndex, Bool{is_canceled} );
		}
		_deps.clear();
	}

/*
=================================================
	Resolve
=================================================
*/
	bool  AsyncDSRequestDependencyManager::Resolve (AnyTypeCRef dep, AsyncTask task, INOUT uint &bitIndex) __NE___
	{
		using EStatus = AsyncDSRequest::Value_t::EStatus;

		if_likely( auto* request_pp = dep.GetIf< AsyncDSRequest >() )
		{
			if_unlikely( *request_pp == null )
				return true;

			auto*	request = (*request_pp).get();	// non-null

			// 'true'	- dependency added or successfully complete.
			// 'false'	- dependency is cancelled or on an error.
			return request->_AddOnCompleteDependency( RVRef(task), INOUT bitIndex, True{"strong"} );
		}

		if_likely( auto* weak_req_pp = dep.GetIf< WeakAsyncDSRequest >() )
		{
			if_unlikely( weak_req_pp->_task == null )
				return true;

			auto*	request = weak_req_pp->_task.get();	// non-null

			Unused( request->_AddOnCompleteDependency( RVRef(task), INOUT bitIndex, False{"weak"} ));
			return true;	// always return 'true' because it is weak dependency
		}

		RETURN_ERR( "unsupported dependency type" );
	}


} // AE::Threading
