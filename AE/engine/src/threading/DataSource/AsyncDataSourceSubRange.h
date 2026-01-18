// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/DataSource/AsyncDataSource.h"

namespace AE::Threading
{

	//
	// Async Read-only Data Source Sub Range
	//
	class AsyncRDataSourceSubRange final : public AsyncRDataSource
	{
	// variables
	private:
		RC<AsyncRDataSource>	_ds;
		Bytes					_begin;
		Bytes					_end;


	// methods
	public:
		AsyncRDataSourceSubRange (RC<AsyncRDataSource> ds, Bytes begin, Bytes end)		__NE___;

		bool			IsOpen ()														C_NE_OV	{ return _ds and _ds->IsOpen(); }
		Bytes			Size ()															C_NE_OV	{ ASSERT( IsOpen() );  return _end - _begin; }
		ReadRequestPtr  ReadBlock (Bytes pos, OUT void* data, Bytes dataSize, RC<> mem)	__NE_OV;
		ReadRequestPtr  ReadBlock (Bytes pos, Bytes size)								__NE_OV;
		bool			CancelAllRequests ()											__NE_OV;
		ReqAlign		OffsetAlign ()													C_NE_OV	{ return _ds->OffsetAlign(); }
	};



	//
	// Async Read-only Stream Sub Range
	//
	class AsyncRStreamSubRange final : public AsyncRStream
	{
	// variables
	private:
		RC<AsyncRDataSource>	_ds;
		AtomicBytes<Bytes>		_pos;
		Bytes					_end;
		Bytes					_size;


	// methods
	public:
		AsyncRStreamSubRange (RC<AsyncRDataSource> ds, Bytes begin, Bytes end)	__NE___;

		bool			IsOpen ()												C_NE_OV	{ return _ds and _ds->IsOpen(); }
		PosAndSize		PositionAndSize ()										C_NE_OV	{ ASSERT( IsOpen() );  return PosAndSize{ _pos.load(), _size }; }
		ReadRequestPtr	ReadSeq (OUT void* data, Bytes dataSize, RC<> mem)		__NE_OV;
		ReadRequestPtr	ReadSeq (Bytes size)									__NE_OV;
		bool			CancelAllRequests ()									__NE_OV;
		ReqAlign		OffsetAlign ()											C_NE_OV	{ return _ds->OffsetAlign(); }
	};


} // AE::Threading
