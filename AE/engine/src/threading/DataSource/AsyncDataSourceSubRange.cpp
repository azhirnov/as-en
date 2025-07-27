// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "threading/DataSource/AsyncDataSourceSubRange.h"

namespace AE::Threading
{

/*
=================================================
	constructor
=================================================
*/
	AsyncRDataSourceSubRange::AsyncRDataSourceSubRange (RC<AsyncRDataSource> ds, Bytes begin, Bytes end) __NE___
	{
		ASSERT_Lt( begin, end );
		ASSERT( ds and ds->IsOpen() );
		ASSERT( ds and AllBits( ds->GetSourceType(), ESourceType::FixedSize ));

		if ( ds														and
			 ds->IsOpen()											and
			 AllBits( ds->GetSourceType(), ESourceType::FixedSize )	and
			 begin < end )
		{
			const Bytes	size = ds->Size();
			ASSERT_Lt( begin, size );

			_ds		= RVRef(ds);
			_begin	= Min( size, begin );
			_end	= Min( size, end );
		}
	}

/*
=================================================
	ReadBlock
=================================================
*/
	AsyncDSRequest  AsyncRDataSourceSubRange::ReadBlock (Bytes pos, OUT void* data, Bytes dataSize, RC<> mem) __NE___
	{
		pos += _begin;
		ASSERT_Lt( pos, _end );

		dataSize = Min( _end - Min( pos, _end ), dataSize );

		if_unlikely( pos >= _end or dataSize == 0 or not _ds )
			return TaskScheduler::GetCanceledDSRequest();

		return _ds->ReadBlock( pos, OUT data, dataSize, RVRef(mem) );
	}

	AsyncDSRequest  AsyncRDataSourceSubRange::ReadBlock (Bytes pos, Bytes size) __NE___
	{
		pos += _begin;
		ASSERT_Lt( pos, _end );

		size = Min( _end - Min( pos, _end ), size );

		if_unlikely( pos >= _end or size == 0 or not _ds )
			return TaskScheduler::GetCanceledDSRequest();

		return _ds->ReadBlock( pos, size );
	}

/*
=================================================
	CancelAllRequests
=================================================
*/
	bool  AsyncRDataSourceSubRange::CancelAllRequests () __NE___
	{
		AE_LOG_DBG( "Can not cancel requests for sub-range, because other request will be cancellated too. Call 'CancelAllRequests()' for origin 'AsyncRDataSource'." );
		return false;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	AsyncRStreamSubRange::AsyncRStreamSubRange (RC<AsyncRDataSource> ds, Bytes begin, Bytes end) __NE___
	{
		ASSERT_Lt( begin, end );
		ASSERT( ds and ds->IsOpen() );
		ASSERT( ds and AllBits( ds->GetSourceType(), ESourceType::FixedSize ));

		if ( ds														and
			 ds->IsOpen()											and
			 AllBits( ds->GetSourceType(), ESourceType::FixedSize )	and
			 begin < end )
		{
			const Bytes	size = ds->Size();
			ASSERT_Lt( begin, size );

			_ds		= RVRef(ds);
			begin	= Min( size, begin );
			_end	= Min( size, end );
			_size	= _end - begin;
			_pos.store( begin );
		}
	}

/*
=================================================
	ReadSeq
=================================================
*/
	AsyncDSRequest  AsyncRStreamSubRange::ReadSeq (OUT void* data, Bytes dataSize, RC<> mem) __NE___
	{
		Bytes	pos = _pos.fetch_add( dataSize );
		dataSize = Min( _end - Min( pos, _end ), dataSize );

		if_unlikely( pos >= _end or dataSize == 0 or not _ds )
			return TaskScheduler::GetCanceledDSRequest();

		return _ds->ReadBlock( pos, OUT data, dataSize, RVRef(mem) );
	}

	AsyncDSRequest  AsyncRStreamSubRange::ReadSeq (Bytes size) __NE___
	{
		Bytes	pos = _pos.fetch_add( size );
		size = Min( _end - Min( pos, _end ), size );

		if_unlikely( pos >= _end or size == 0 or not _ds )
			return TaskScheduler::GetCanceledDSRequest();

		return _ds->ReadBlock( pos, size );
	}

/*
=================================================
	CancelAllRequests
=================================================
*/
	bool  AsyncRStreamSubRange::CancelAllRequests () __NE___
	{
		AE_LOG_DBG( "Can not cancel requests for sub-range, because other request will be cancellated too. Call 'CancelAllRequests()' for origin 'AsyncRStream'." );
		return false;
	}


} // AE::Threading
