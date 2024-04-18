// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/DataSource/UnixAsyncDataSource.h"

namespace AE::Threading
{

/*
=================================================
	constructor
=================================================
*/
	UnixIOService::_RequestBase::_RequestBase () __NE___
	{
	}

/*
=================================================
	_Cancel
=================================================
*/
	bool  UnixIOService::_RequestBase::_Cancel () __NE___
	{
		return false;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Create
=================================================
*/
	bool  UnixIOService::ReadRequest::_Create (RC<UnixAsyncRDataSource> file, Bytes pos, void* data, Bytes dataSize, RC<> mem) __NE___
	{
		// initialize
		_Init( RVRef(mem) );
		_dataSource	= RVRef(file);
		_offset		= pos;
		_data		= data;

		// read data
		ssize_t	readn = ::pread( _dataSource->Handle(), OUT _data, size_t{dataSize}, off_t{pos} );
		if_likely( readn >= 0 )
		{
			_Complete( Bytes{usize(readn)}, true );
			return true;
		}

		_Complete( 0_b, false );

		UNIX_CHECK_DEV( "Read request failed: " );
		return false;
	}

/*
=================================================
	GetResult
=================================================
*/
	UnixIOService::ReadRequest::Result  UnixIOService::ReadRequest::GetResult () C_NE___
	{
		ASSERT( IsFinished() );

		Result	res;
		res.dataSize	= _actualSize.load();
		res.pos			= _offset;
		res.data		= IsCompleted() ? _data : null;

		return res;
	}

/*
=================================================
	_GetResult
=================================================
*/
	UnixIOService::ReadRequest::ResultWithRC  UnixIOService::ReadRequest::_GetResult () __NE___
	{
		ASSERT( IsFinished() );

		ResultWithRC	res;
		res.dataSize	= _actualSize.load();
		res.rc			= _memRC;
		res.pos			= _offset;
		res.data		= IsCompleted() ? _data : null;

		return res;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Create
=================================================
*/
	bool  UnixIOService::WriteRequest::_Create (RC<UnixAsyncWDataSource> file, Bytes pos, const void* data, Bytes dataSize, RC<> mem) __NE___
	{
		// initialize
		_Init( RVRef(mem) );
		_dataSource	= RVRef(file);
		_offset		= pos;

		// write data
		ssize_t	written = ::pwrite( _dataSource->Handle(), data, ssize_t{dataSize}, off_t{pos} );
		if_likely( written >= 0 )
		{
			_Complete( Bytes{usize(written)}, true );
			return true;
		}

		_Complete( 0_b, false );

		UNIX_CHECK_DEV( "Write request failed: " );
		return false;
	}

/*
=================================================
	GetResult
=================================================
*/
	UnixIOService::WriteRequest::Result  UnixIOService::WriteRequest::GetResult () C_NE___
	{
		ASSERT( IsFinished() );

		Result	res;
		res.dataSize	= _actualSize.load();
		res.data		= null;
		res.pos			= _offset;

		return res;
	}

/*
=================================================
	_GetResult
=================================================
*/
	UnixIOService::WriteRequest::ResultWithRC  UnixIOService::WriteRequest::_GetResult () __NE___
	{
		ASSERT( IsFinished() );

		ResultWithRC	res;
		res.dataSize	= _actualSize.load();
		res.pos			= _offset;

		return res;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	CancelAllRequests
=================================================
*/
	bool  UnixAsyncRDataSource::CancelAllRequests () __NE___
	{
		return false;
	}

	bool  UnixAsyncWDataSource::CancelAllRequests () __NE___
	{
		return false;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
----
	warning: Scheduler().GetFileIOService() is not valid here
=================================================
*/
	UnixIOService::UnixIOService (uint) __NE___
	{}

/*
=================================================
	destructor
=================================================
*/
	UnixIOService::~UnixIOService () __NE___
	{}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  UnixIOService::IsInitialized () C_NE___
	{
		return true;
	}

/*
=================================================
	ProcessEvents
=================================================
*/
	usize  UnixIOService::ProcessEvents () __NE___
	{
		ASSERT( not IsInitialized() or Scheduler().GetFileIOService() == this );

		return 0;
	}


} // AE::Threading
