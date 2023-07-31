// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/DataSource/AsyncDataSource.h"

namespace AE::Threading
{

	//
	// Read-only Async Data Source as Stream
	//
	class AsyncRDataSourceAsStream final : public AsyncRStream
	{
	// variables
	private:
		BytesAtomic< Bytes >	_pos;
		RC<AsyncRDataSource>	_ds;


	// methods
	public:
		AsyncRDataSourceAsStream (RC<AsyncRDataSource> ds, Bytes offset = 0_b)		__NE___ : _pos{ offset }, _ds{ RVRef(ds) } {}

		ReadRequestPtr	ReadSeq (void* data, Bytes dataSize, RC<> mem)				__NE_OV;
		ReadRequestPtr	ReadSeq (Bytes size)										__NE_OV;
	};
//-----------------------------------------------------------------------------



	//
	// Write-only Async Data Source as Stream
	//
	class AsyncWDataSourceAsStream final : public AsyncWStream
	{
	// variables
	private:
		BytesAtomic< Bytes >	_pos;
		RC<AsyncWDataSource>	_ds;


	// methods
	public:
		AsyncWDataSourceAsStream (RC<AsyncWDataSource> ds, Bytes offset = 0_b)		__NE___ : _pos{ offset }, _ds{ RVRef(ds) } {}

		RC<SharedMem>	Alloc (Bytes size)											__NE_OV	{ return _ds->Alloc( size ); }
		WriteRequestPtr	WriteSeq (const void* data, Bytes dataSize, RC<> mem)		__NE_OV;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	ReadSeq
=================================================
*/
	inline AsyncRStream::ReadRequestPtr  AsyncRDataSourceAsStream::ReadSeq (void* data, Bytes dataSize, RC<> mem) __NE___
	{
		Bytes	pos = _pos.fetch_add( dataSize );
		return _ds->ReadBlock( pos, data, dataSize, RVRef(mem) );
	}

	inline AsyncRStream::ReadRequestPtr  AsyncRDataSourceAsStream::ReadSeq (Bytes size) __NE___
	{
		Bytes	pos = _pos.fetch_add( size );
		return _ds->ReadBlock( pos, size );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	WriteSeq
=================================================
*/
	inline AsyncWStream::WriteRequestPtr  AsyncWDataSourceAsStream::WriteSeq (const void* data, Bytes dataSize, RC<> mem) __NE___
	{
		Bytes	pos = _pos.fetch_add( dataSize );
		return _ds->WriteBlock( pos, data, dataSize, RVRef(mem) );
	}


} // AE::Threading
