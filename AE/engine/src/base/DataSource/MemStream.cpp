// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/MemStream.h"
#include "base/Math/Vec.h"
#include "base/Algorithms/StringUtils.h"

namespace AE::Base
{

/*
=================================================
	constructor
=================================================
*/
	MemRefRStream::MemRefRStream (const void* ptr, Bytes size) __NE___ :
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
	void  MemRefRStream::_Set (const void* ptr, Bytes size) __NE___
	{
		ASSERT( ptr != null );
		ASSERT( size > 0 );

		_dataPtr	= ptr;
		_size		= size;
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  MemRefRStream::GetSourceType () C_NE___
	{
		return	ESourceType::Buffered		| ESourceType::SequentialAccess |
				ESourceType::RandomAccess	| ESourceType::FixedSize		|
				ESourceType::ReadAccess;
	}

/*
=================================================
	SeekSet / SeekFwd
=================================================
*/
	bool  MemRefRStream::SeekSet (Bytes pos) __NE___
	{
		_pos = Min( pos, _size );
		return _pos == pos;
	}

	bool  MemRefRStream::SeekFwd (Bytes offset) __NE___
	{
		Bytes	pos = Min( offset + _pos, _size );
		return _pos == pos;
	}

/*
=================================================
	ReadSeq
=================================================
*/
	Bytes  MemRefRStream::ReadSeq (OUT void* buffer, Bytes size) __NE___
	{
		size = Min( size, _size - _pos );

		MemCopy( OUT buffer, _dataPtr + _pos, size );
		_pos += size;

		return size;
	}

/*
=================================================
	UpdateFastStream
=================================================
*/
	void  MemRefRStream::UpdateFastStream (OUT const void* &begin, OUT const void* &end) __NE___
	{
		ASSERT( _pos <= _size );
		ASSERT( begin == null or ( (begin >= _dataPtr) and (end <= _dataPtr + _size) ));

		// commit readn size
		_pos = ( (begin >= _dataPtr) and (begin <= _dataPtr + _size) ) ?
				Bytes{begin} - Bytes{_dataPtr} :
				_pos;
		ASSERT( _pos <= _size );

		begin	= _dataPtr + _pos;
		end		= _dataPtr + _size;
	}

/*
=================================================
	EndFastStream
=================================================
*/
	void  MemRefRStream::EndFastStream (const void* ptr) __NE___
	{
		if ( (ptr >= _dataPtr) and (ptr <= _dataPtr + _size) )
		{
			_pos = Bytes{ptr} - Bytes{_dataPtr};
		}
		else
		{
			ASSERT( ptr == null );
		}
	}

/*
=================================================
	GetFastStreamPosition
=================================================
*/
	Bytes  MemRefRStream::GetFastStreamPosition (const void* ptr) __NE___
	{
		return Bytes{ptr} - Bytes{_dataPtr};
	}

/*
=================================================
	ToSubStream
=================================================
*/
	RC<MemRefRStream>  MemRefRStream::ToSubStream (Bytes offset, Bytes size) C_Th___
	{
		CHECK_ERR( size + offset <= _size );
		return MakeRC<MemRefRStream>( _dataPtr + offset, size );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	MemRStream::MemRStream (Array<ubyte> data) __NE___ : _data{RVRef(data)}
	{
		_Set( _data.data(), ArraySizeOf(_data) );
	}

	MemRStream::MemRStream (const void* ptr, Bytes size) __NE___
	{
		NOTHROW( _data.assign( Cast<ubyte>(ptr), Cast<ubyte>(ptr + size) ));

		_Set( _data.data(), ArraySizeOf(_data) );
	}

/*
=================================================
	Decompress
=================================================
*/
	bool  MemRStream::Decompress (RStream &srcStream) __NE___
	{
		MemWStream	dst;
		Bytes		size = DataSourceUtils::BufferedCopy( dst, srcStream );

		_data = dst.ReleaseData();
		_Set( _data.data(), size );

		return size > 0_b;
	}

/*
=================================================
	Load
=================================================
*/
	bool  MemRStream::LoadRemaining (RStream &srcStream, const Bytes dataSize) __NE___
	{
		MemWStream	dst;
		Bytes		size = DataSourceUtils::BufferedCopy( dst, srcStream, dataSize );

		_data = dst.ReleaseData();
		_Set( _data.data(), size );

		return size > 0_b;
	}

	bool  MemRStream::Load (RStream &srcStream, const Bytes offset, const Bytes dataSize) __NE___
	{
		CHECK_ERR( srcStream.SeekSet( offset ));

		return LoadRemaining( srcStream, dataSize );
	}

	bool  MemRStream::Load (RDataSource &srcDS, Bytes offset, Bytes dataSize) __NE___
	{
		MemWStream	dst;
		Bytes		size = DataSourceUtils::BufferedCopy( dst, srcDS, offset, dataSize );

		_data = dst.ReleaseData();
		_Set( _data.data(), size );

		return size > 0_b;
	}

/*
=================================================
	ReleaseData
=================================================
*/
	Array<ubyte>  MemRStream::ReleaseData () __NE___
	{
		Array<ubyte>	tmp;
		std::swap( tmp, _data );

		_dataPtr	= null;
		_size		= 0_b;
		_pos		= 0_b;

		return tmp;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	MemWStream::MemWStream () __NE___
	{
		NOTHROW( _data.reserve( usize(DefaultAllocationSize) ));
	}

	MemWStream::MemWStream (Bytes bufferSize) __NE___
	{
		NOTHROW( _data.reserve( usize(bufferSize) ));
	}

/*
=================================================
	SeekFwd
=================================================
*/
	bool  MemWStream::SeekFwd (Bytes offset) __NE___
	{
		if_unlikely( (_pos + offset) > Bytes{_data.size()} )
			return false;

		_pos += offset;
		return true;
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  MemWStream::GetSourceType () C_NE___
	{
		return	ESourceType::Buffered		| ESourceType::SequentialAccess |
				ESourceType::RandomAccess	| ESourceType::WriteAccess;
	}

/*
=================================================
	Reserve
=================================================
*/
	Bytes  MemWStream::Reserve (Bytes additionalSize) __NE___
	{
		NOTHROW( _data.reserve( Max( _data.capacity(), usize(_pos + additionalSize) )));

		return ArraySizeOf(_data) - _pos;
	}

/*
=================================================
	WriteSeq
=================================================
*/
	Bytes  MemWStream::WriteSeq (const void* buffer, Bytes size) __NE___
	{
		NOTHROW_ERR( _data.resize( Max( _data.size(), usize(_pos + size) )));

		MemCopy( OUT _data.data() + _pos, buffer, size );

		_pos += size;
		return size;
	}

/*
=================================================
	Clear
=================================================
*/
	void  MemWStream::Clear () __NE___
	{
		_pos = 0_b;
		Reconstruct( _data );	// release memory
	}

/*
=================================================
	UpdateFastStream
=================================================
*/
	void  MemWStream::UpdateFastStream (OUT void* &begin, OUT const void* &end, Bytes reserve) __NE___
	{
		ASSERT( begin == null or ( (begin >= _data.data()) and (end <= _data.data() + _data.size()) ));

		// commit written size
		_pos = ( (begin >= _data.data()) and (begin <= _data.data() + _data.size()) ) ?
				Bytes{begin} - Bytes{_data.data()} :
				_pos;

		ASSERT( _pos <= _data.size() );
		_data.resize( usize( _pos + reserve ));

		begin	= _data.data() + _pos;
		end		= _data.data() + _data.size();
	}

/*
=================================================
	EndFastStream
=================================================
*/
	void  MemWStream::EndFastStream (const void* ptr) __NE___
	{
		if ( (ptr >= _data.data() + _pos) and (ptr <= _data.data() + _data.size()) )
		{
			_pos = Bytes{ptr} - Bytes{_data.data()};
		}
		else
		{
			ASSERT( ptr == null );
		}
	}

/*
=================================================
	GetFastStreamPosition
=================================================
*/
	Bytes  MemWStream::GetFastStreamPosition (const void* ptr) __NE___
	{
		return Bytes{ptr} - Bytes{_data.data()};
	}

/*
=================================================
	Store
=================================================
*/
	bool  MemWStream::Store (WStream &dstFile) C_NE___
	{
		CHECK_ERR( dstFile.IsOpen() );
		ASSERT_Gt( Position(), 0 );
		return dstFile.Write( _data.data(), Position() );
	}

/*
=================================================
	ReleaseData
=================================================
*/
	Array<ubyte>  MemWStream::ReleaseData () __NE___
	{
		if ( _pos < _data.size() )
			_data.resize( usize(_pos) );

		Array<ubyte>	temp;
		std::swap( temp, _data );

		_pos = 0_b;
		return temp;
	}


} // AE::Base
