// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/MemDataSource.h"
#include "base/DataSource/MemStream.h"

namespace AE::Base
{

/*
=================================================
	constructor
=================================================
*/
	MemRefRDataSource::MemRefRDataSource (const void* ptr, Bytes size) __NE___ :
		_dataPtr{ptr}, _size{size}
	{
		ASSERT( _dataPtr != null );
		ASSERT( _size > 0 );
	}

/*
=================================================
	_Set
=================================================
*/
	void  MemRefRDataSource::_Set (const void* ptr, Bytes size) __NE___
	{
		ASSERT( (ptr != null) == (size > 0) );

		_dataPtr	= ptr;
		_size		= size;
	}

/*
=================================================
	_Reset
=================================================
*/
	void  MemRefRDataSource::_Reset () __NE___
	{
		_dataPtr	= null;
		_size		= 0_b;
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  MemRefRDataSource::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess	| ESourceType::RandomAccess	|	// allow SeekFwd() & SeekSet()
				ESourceType::Buffered			| ESourceType::FixedSize	|
				ESourceType::ReadAccess;
	}

/*
=================================================
	ReadBlock
=================================================
*/
	Bytes  MemRefRDataSource::ReadBlock (const Bytes pos, OUT void* buffer, Bytes size) __NE___
	{
		size = Min( pos + size, _size ) - pos;

		MemCopy( OUT buffer, _dataPtr + pos, size );

		return size;
	}

/*
=================================================
	LoadFrom
=================================================
*/
	bool  MemRefRDataSource::LoadRemainingFrom (RStream &srcStream, Bytes dataSize) __NE___
	{
		CHECK_ERR( srcStream.IsOpen() );

		dataSize = Min( dataSize, srcStream.RemainingSize() );
		CHECK_ERR( _Resize( dataSize ));

		Bytes	total;
		void*	dst = ConstCast( _dataPtr );

		for (; total < dataSize;)
		{
			Bytes	readn = srcStream.ReadSeq( OUT dst + total, dataSize - total );
			total += readn;

			if_unlikely( readn == 0 )
				break;
		}

		_size = total;
		return _size > 0_b;
	}

	bool  MemRefRDataSource::LoadFrom (RStream &srcStream, Bytes offset, Bytes dataSize) __NE___
	{
		CHECK_ERR( srcStream.SeekSet( offset ));

		return LoadRemainingFrom( srcStream, dataSize );
	}

	bool  MemRefRDataSource::LoadFrom (RDataSource &srcDS, Bytes offset, Bytes dataSize) __NE___
	{
		CHECK_ERR( srcDS.IsOpen() );
		CHECK_ERR( offset < srcDS.Size() );

		dataSize = Min( dataSize, srcDS.Size() - offset );
		CHECK_ERR( _Resize( dataSize ));

		Bytes	total;
		void*	dst = ConstCast( _dataPtr );

		for (; total < dataSize;)
		{
			Bytes	readn = srcDS.ReadBlock( offset + total, OUT dst + total, dataSize - total );
			total += readn;

			if_unlikely( readn == 0 )
				break;
		}

		_size = total;
		return _size > 0_b;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ArrayRDataSource::ArrayRDataSource (Array<ubyte> data) __NE___ :
		_data{ RVRef(data) }
	{
		_Set( _data.data(), ArraySizeOf(_data) );
	}

	ArrayRDataSource::ArrayRDataSource (const void* ptr, Bytes size) __NE___
	{
		NOTHROW( _data.assign( Cast<ubyte>(ptr), Cast<ubyte>(ptr + size) ));

		_Set( _data.data(), ArraySizeOf(_data) );
	}

/*
=================================================
	ReleaseData
=================================================
*/
	Array<ubyte>  ArrayRDataSource::ReleaseData () __NE___
	{
		Array<ubyte>	tmp;
		std::swap( tmp, _data );
		_Reset();
		return tmp;
	}

/*
=================================================
	_Resize
=================================================
*/
	bool  ArrayRDataSource::_Resize (Bytes newSize) __NE___
	{
		NOTHROW_ERR( _data.resize( usize{newSize} ));
		_Set( _data.data(), ArraySizeOf(_data) );
		return true;
	}

/*
=================================================
	DecompressFrom
=================================================
*/
	bool  ArrayRDataSource::DecompressFrom (RStream &srcStream) __NE___
	{
		ArrayWStream	dst	{0_b};
		Bytes			size = DataSourceUtils::BufferedCopy( dst, srcStream );

		_data = dst.ReleaseData();
		CHECK_ERR( size <= ArraySizeOf(_data) );

		_Set( _data.data(), size );
		return size > 0_b;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	SharedMemRDataSource::SharedMemRDataSource (RC<SharedMem> data) __NE___ :
		_data{ RVRef(data) }
	{
		if ( _data )
			_Set( _data->Data(), _data->Size() );
	}

	SharedMemRDataSource::SharedMemRDataSource (const void* ptr, Bytes size) __NE___ :
		_data{ SharedMem::Create( AE::GetDefaultAllocator(), size )}
	{
		if ( _data )
		{
			MemCopy( OUT _data->Data(), ptr, size );
			_Set( _data->Data(), _data->Size() );
		}
	}

/*
=================================================
	ReleaseData
=================================================
*/
	RC<SharedMem>  SharedMemRDataSource::ReleaseData () __NE___
	{
		RC<SharedMem>	tmp = RVRef(_data);
		_Reset();
		return tmp;
	}

/*
=================================================
	_Resize
=================================================
*/
	bool  SharedMemRDataSource::_Resize (Bytes newSize) __NE___
	{
		CHECK_ERR( not _data );

		_data = SharedMem::Create( AE::GetDefaultAllocator(), newSize );
		CHECK_ERR( _data );

		_Set( _data->Data(), _data->Size() );
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Set
=================================================
*/
	void  MemRefWDataSource::_Set (void* ptr, Bytes size) __NE___
	{
		ASSERT( (ptr != null) == (size > 0) );
		ASSERT( size <= _maxSize );

		_dataPtr	= ptr;
		_size		= size;
	}

/*
=================================================
	_Reset
=================================================
*/
	void  MemRefWDataSource::_Reset () __NE___
	{
		_dataPtr	= null;
		_size		= 0_b;
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  MemRefWDataSource::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess	| ESourceType::RandomAccess	|
				ESourceType::Buffered			| ESourceType::WriteAccess;
	}

/*
=================================================
	WriteBlock
=================================================
*/
	Bytes  MemRefWDataSource::WriteBlock (const Bytes pos, const void* buffer, Bytes size) __NE___
	{
		CHECK_ERR( pos <= _maxSize );

		if_unlikely( pos + size > _size )
			CHECK_ERR( _Resize( pos + size ));

		size = Min( size, _size - pos );

		MemCopy( OUT _dataPtr + pos, buffer, size );
		return size;
	}

/*
=================================================
	StoreTo
=================================================
*/
	bool  MemRefWDataSource::StoreTo (WStream &dstFile) C_NE___
	{
		CHECK_ERR( dstFile.IsOpen() );
		ASSERT( _size > 0 );
		return dstFile.Write( _dataPtr, _size );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ArrayWDataSource::ArrayWDataSource () __NE___ :
		MemRefWDataSource{ Bytes{UMax} }
	{
		NOTHROW( _data.reserve( usize(DefaultAllocationSize) ));
	}

	ArrayWDataSource::ArrayWDataSource (Bytes bufferSize, Bytes maxSize) __NE___ :
		MemRefWDataSource{ maxSize }
	{
		NOTHROW( _data.reserve( usize(bufferSize) ));
		_Set( _data.data(), ArraySizeOf(_data) );
	}

	ArrayWDataSource::ArrayWDataSource (Array<ubyte> data, Bytes maxSize) __NE___ :
		MemRefWDataSource{ Max( maxSize, data.size() )},
		_data{ RVRef(data) }
	{
		_Set( _data.data(), ArraySizeOf(_data) );
	}

/*
=================================================
	ReleaseData
=================================================
*/
	Array<ubyte>  ArrayWDataSource::ReleaseData () __NE___
	{
		Array<ubyte>	tmp;
		_data.resize( usize{_size} );
		std::swap( tmp, _data );
		_Reset();
		return tmp;
	}

/*
=================================================
	_Resize
=================================================
*/
	bool  ArrayWDataSource::_Resize (Bytes newSize) __NE___
	{
		newSize = Min( newSize, _maxSize );
		NOTHROW_ERR( _data.resize( usize{newSize} ));
		_Set( _data.data(), ArraySizeOf(_data) );
		return true;
	}
//-----------------------------------------------------------------------------



} // AE::Base
