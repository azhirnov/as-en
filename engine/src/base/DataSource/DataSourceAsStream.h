// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/Stream.h"

namespace AE::Base
{

	//
	// Read-only Data Source as Stream
	//

	template <typename T>
	class RDataSourceAsStream final : public RStream
	{
	// variables
	private:
		T				_dataSource;
		Bytes			_pos;
		const Bytes		_offset;
		const Bytes		_size;

		static constexpr ESourceType	_TypeMask = ESourceType::RandomAccess | ESourceType::FixedSize | ESourceType::ReadAccess;


	// methods
	public:
		template <typename B>
		explicit RDataSourceAsStream (B && dataSource) :
			_dataSource{ FwdArg<B>(dataSource) },
			_offset{ 0_b },
			_size{ _dataSource->Size() }
		{
			CHECK( AllBits( _dataSource->GetSourceType(), _TypeMask ));
		}

		template <typename B>
		RDataSourceAsStream (B && dataSource, Bytes offset, Bytes size) :
			_dataSource{ FwdArg<B>(dataSource) },
			_offset{ Min( offset, _dataSource->Size() )},
			_size{ Min( size, _dataSource->Size() - _offset )}
		{
			CHECK( AllBits( _dataSource->GetSourceType(), _TypeMask ));
		}
		

		// RStream //
		bool		IsOpen ()			C_NE_OV		{ return _dataSource and _dataSource->IsOpen(); }
		PosAndSize	PositionAndSize ()	C_NE_OV		{ return { _pos, _size }; }
		ESourceType	GetSourceType ()	C_NE_OV;

		bool	SeekSet (Bytes pos)		__NE_OV;
		bool	SeekFwd (Bytes offset)	__NE_OV;

		Bytes	ReadSeq (OUT void *buffer, Bytes size) __NE_OV;
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	GetSourceType
=================================================
*/
	template <typename T>
	IDataSource::ESourceType  RDataSourceAsStream<T>::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess	| ESourceType::RandomAccess |
				ESourceType::FixedSize			| ESourceType::ReadAccess;
		//		(_dataSource->GetSourceType() & ESourceType::ThreadSafe);	// TODO ?
	}

/*
=================================================
	SeekSet
=================================================
*/
	template <typename T>
	bool  RDataSourceAsStream<T>::SeekSet (Bytes pos) __NE___
	{
		ASSERT( IsOpen() );

		if_unlikely( pos > _size )
			return false;

		_pos = pos;
		return true;
	}
	
/*
=================================================
	SeekFwd
=================================================
*/
	template <typename T>
	bool  RDataSourceAsStream<T>::SeekFwd (Bytes offset) __NE___
	{
		ASSERT( IsOpen() );

		if_unlikely( _pos + offset > _size )
			return false;

		_pos += offset;
		return true;
	}
	
/*
=================================================
	ReadSeq
=================================================
*/
	template <typename T>
	Bytes  RDataSourceAsStream<T>::ReadSeq (OUT void *buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );

		if_likely( _dataSource )
		{
			size = Min( _pos + size, _size ) - _pos;

			Bytes	readn = _dataSource->ReadBlock( _offset + _pos, OUT buffer, size );
			_pos += readn;
			return readn;
		}
		else
			return 0_b;	// error or EOF
	}


} // AE::Base
