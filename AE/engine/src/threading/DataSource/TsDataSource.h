// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/Common.h"

namespace AE::Threading
{

	//
	// Thread-safe Read-only Data Source
	//

	template <typename T>
	class TsRDataSource final : public RDataSource
	{
	// variables
	private:
		mutable Mutex	_guard;
		T				_dataSource;


	// methods
	public:
		template <typename ...Args, ENABLEIF( IsConstructible< T, Args... >)>
		explicit TsRDataSource (Args&& ...args)		__NE___	: _dataSource{ FwdArg<Args>(args)...} {}
		~TsRDataSource ()							__NE_OV	{}


		// RDataSource //
		bool		IsOpen ()						C_NE_OV	{ EXLOCK( _guard );  return _dataSource and _dataSource->IsOpen(); }
		ESourceType	GetSourceType ()				C_NE_OV	{ EXLOCK( _guard );  return _dataSource->GetSourceType() | ESourceType::ThreadSafe; }

		Bytes		Size ()							C_NE_OV	{ EXLOCK( _guard );  return _dataSource->Size(); }

		Bytes  ReadBlock (Bytes pos, OUT void* buffer, Bytes size) __NE_OV
		{
			EXLOCK( _guard );
			return _dataSource->ReadBlock( pos, OUT buffer, size );
		}
	};



	//
	// Thread-safe Write-only Data Source
	//

	template <typename T>
	class TsWDataSource final : public WDataSource
	{
	// variables
	private:
		mutable Mutex	_guard;
		T				_dataSource;


	// methods
	public:
		template <typename ...Args, ENABLEIF( IsConstructible< T, Args... >)>
		explicit TsWDataSource (Args&& ...args)		__NE___	: _dataSource{ FwdArg<Args>(args)...} {}
		~TsWDataSource ()							__NE_OV {}


		// WDataSource //
		bool		IsOpen ()						C_NE_OV	{ EXLOCK( _guard );  return _dataSource->IsOpen(); }
		ESourceType	GetSourceType ()				C_NE_OV	{ EXLOCK( _guard );  return _dataSource->GetSourceType() | ESourceType::ThreadSafe; }

		Bytes  WriteBlock (Bytes pos, const void* buffer, Bytes size) __NE_OV
		{
			EXLOCK( _guard );
			return _dataSource->WriteBlock( pos, buffer, size );
		}

		void  Flush ()								__NE_OV
		{
			EXLOCK( _guard );
			return _dataSource->Flush();
		}
	};


} // AE::Threading
