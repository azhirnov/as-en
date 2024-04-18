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
		AtomicByte< Bytes >		_pos;
		RC<AsyncRDataSource>	_ds;


	// methods
	public:
		AsyncRDataSourceAsStream (RC<AsyncRDataSource> ds, Bytes offset = 0_b)		__NE___ : _pos{offset}, _ds{RVRef(ds)} { ASSERT(_ds); }

		bool			IsOpen ()													C_NE_OV	{ return _ds and _ds->IsOpen(); }

		PosAndSize		PositionAndSize ()											C_NE_OV	{ return PosAndSize{ _pos.load(), _ds->Size() }; }

		ReadRequestPtr	ReadSeq (void* data, Bytes dataSize, RC<> mem)				__NE_OV;
		ReadRequestPtr	ReadSeq (Bytes size)										__NE_OV;
		bool			CancelAllRequests ()										__NE_OV	{ return _ds->CancelAllRequests(); }
	};
//-----------------------------------------------------------------------------



	//
	// Write-only Async Data Source as Stream
	//
	class AsyncWDataSourceAsStream final : public AsyncWStream
	{
	// variables
	private:
		AtomicByte< Bytes >		_pos;
		RC<AsyncWDataSource>	_ds;


	// methods
	public:
		AsyncWDataSourceAsStream (RC<AsyncWDataSource> ds, Bytes offset = 0_b)		__NE___ : _pos{offset}, _ds{RVRef(ds)} { ASSERT(_ds); }

		bool			IsOpen ()													C_NE_OV	{ return _ds and _ds->IsOpen(); }

		Bytes			Position ()													C_NE_OV	{ return _pos.load(); }

		RC<SharedMem>	Alloc (const SizeAndAlign value)							__NE_OV	{ return _ds->Alloc( value ); }
		WriteRequestPtr	WriteSeq (const void* data, Bytes dataSize, RC<> mem)		__NE_OV;
		bool			CancelAllRequests ()										__NE_OV	{ return _ds->CancelAllRequests(); }
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
