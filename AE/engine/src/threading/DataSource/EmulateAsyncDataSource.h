// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "threading/DataSource/AsyncDataSource.h"

namespace AE::Threading
{

	//
	// Emulated Async Read-only Data Source
	//
	class EmulatedAsyncRDataSource final : public AsyncRDataSource
	{
	// types
	private:
		using EStatus	= _Coro_::IAsyncDataSourceRequest::EStatus;

		class _Request final : public _Coro_::IAsyncDataSourceRequest
		{
			friend class EmulatedAsyncRDataSource;

		// variables
		public:
			Bytes const			pos;
			void* const			data		= null;
			Bytes const			dataSize;
		protected:
			RC<>				_memRC;		// keep memory alive
			mutable AtomicTask	_task;

		// methods
		public:
			_Request (Bytes pos, void* data, Bytes dataSize, RC<> mem)	__NE___ : pos{pos}, data{data}, dataSize{dataSize}, _memRC{RVRef(mem)} {}

			Atomic<EStatus>&  EditStatus ()								__NE___	{ return _status; }

			void		Complete (Bytes readn)							__NE___;

			// IAsyncDataSourceRequest //
			Result		GetResult ()									C_NE_OV;
			bool		Cancel ()										__NE_OV;
		};


	// variables
	private:
		RC<RDataSource>		_ds;


	// methods
	public:
		EmulatedAsyncRDataSource (RC<RDataSource> ds)									__NE___ : _ds{RVRef(ds)} {}

		bool			IsOpen ()														C_NE_OV	{ return _ds and _ds->IsOpen(); }
		Bytes			Size ()															C_NE_OV	{ ASSERT( IsOpen() );  return _ds->Size(); }
		ReadRequestPtr  ReadBlock (Bytes pos, OUT void* data, Bytes dataSize, RC<> mem)	__NE_OV;
		ReadRequestPtr  ReadBlock (Bytes pos, Bytes size)								__NE_OV;
		bool			CancelAllRequests ()											__NE_OV;
		ESourceType		GetSourceType ()												C_NE_OV;
		ReqAlign		DirectAccessAlign ()											C_NE_OV	{ return _ds->DirectAccessAlign(); }

	private:
		static AsyncCoro  _AsyncReadBlock (RC<_Request> req, RC<RDataSource> ds)		__NE___;
	};


} // AE::Threading
