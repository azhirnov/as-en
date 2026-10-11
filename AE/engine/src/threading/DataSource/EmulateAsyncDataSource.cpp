// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "threading/DataSource/EmulateAsyncDataSource.h"

namespace AE::Threading
{

/*
=================================================
	GetResult
=================================================
*/
	_Coro_::IAsyncDataSourceRequest::Result  EmulatedAsyncRDataSource::_Request::GetResult () C_NE___
	{
		Result	res;
		res.status = _status.load();

		if ( auto task = _task.load();  task and task->IsCompleted() )
		{
			ASSERT( res.status == EStatus::Completed );
			_task.reset();
		}

		res.pos			= pos;
		res.dataSize	= _actualSize.load();
		res.data		= data;
		res.rc			= _memRC;
		return	res;
	}

/*
=================================================
	Cancel
=================================================
*/
	bool  EmulatedAsyncRDataSource::_Request::Cancel () __NE___
	{
		EStatus	expected = _status.load();
		if ( expected > EStatus::_Finished )
			return false;

		if ( auto task = _task.load();  not Scheduler().Cancel( task ))
			return false;

		_status.store( EStatus::Canceled );
		return true;
	}

/*
=================================================
	Complete
=================================================
*/
	void  EmulatedAsyncRDataSource::_Request::Complete (Bytes readn) __NE___
	{
		MemoryBarrier( std::memory_order_release );

		_status.store( EStatus::Completed );
		_actualSize.store( readn );
	}

/*
=================================================
	ReadBlock
=================================================
*/
	AsyncDSRequest  EmulatedAsyncRDataSource::ReadBlock (Bytes pos, OUT void* data, Bytes dataSize, RC<> mem) __NE___
	{
		auto	req = MakeRC<_Request>( pos, data, dataSize, RVRef(mem) );

		req->EditStatus().store( EStatus::Pending );
		req->_task.store( Scheduler().Run( ETaskQueue::Background, _AsyncReadBlock( req, _ds )));

		return req;
	}

	AsyncDSRequest  EmulatedAsyncRDataSource::ReadBlock (Bytes pos, Bytes size) __NE___
	{
		RC<SharedMem>	mem		= SharedMem::Create( AE::GetDefaultAllocator(), size );	// TODO: optimize
		void*			data	= mem ? mem->Data() : null;
		return ReadBlock( pos, OUT data, size, RVRef(mem) );
	}

/*
=================================================
	CancelAllRequests
=================================================
*/
	bool  EmulatedAsyncRDataSource::CancelAllRequests () __NE___
	{
		// TODO
		return false;
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  EmulatedAsyncRDataSource::GetSourceType () C_NE___
	{
		return _ds->GetSourceType() | ESourceType::Async | ESourceType::ThreadSafe;
	}

/*
=================================================
	_AsyncReadBlock
=================================================
*/
	AsyncCoro  EmulatedAsyncRDataSource::_AsyncReadBlock (RC<_Request> req, RC<RDataSource> ds) __NE___
	{
		req->EditStatus().store( EStatus::InProgress );

		// synchronously read
		Bytes	readn = ds->ReadBlock( req->pos, req->data, req->dataSize );

		req->Complete( readn );
		co_return;
	}

} // AE::Threading
