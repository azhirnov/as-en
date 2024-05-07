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
		ASSERT( (ptr != null) == (size > 0) );
		ASSERT( _pos <= size );

		_dataPtr	= ptr;
		_size		= size;
		_pos		= Min( _pos, size );
	}

/*
=================================================
	_Reset
=================================================
*/
	void  MemRefRStream::_Reset () __NE___
	{
		_dataPtr	= null;
		_size		= 0_b;
		_pos		= 0_b;
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  MemRefRStream::GetSourceType () C_NE___
	{
		return	ESourceType::RandomAccess	| ESourceType::SequentialAccess |	// allow SeekFwd() & SeekSet()
				ESourceType::Buffered		| ESourceType::FixedSize		|
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
		CHECK_ERR( (size == UMax and offset < _size) or size + offset <= _size );

		return MakeRC<MemRefRStream>( _dataPtr + offset, Min( size, _size - offset ));
	}

/*
=================================================
	LoadFrom
=================================================
*/
	bool  MemRefRStream::LoadRemainingFrom (RStream &srcStream, Bytes dataSize) __NE___
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

	bool  MemRefRStream::LoadFrom (RStream &srcStream, const Bytes offset, const Bytes dataSize) __NE___
	{
		CHECK_ERR( srcStream.SeekSet( offset ));

		return LoadRemainingFrom( srcStream, dataSize );
	}

	bool  MemRefRStream::LoadFrom (RDataSource &srcDS, const Bytes offset, Bytes dataSize) __NE___
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
	ArrayRStream::ArrayRStream (Array<ubyte> data) __NE___ : _data{RVRef(data)}
	{
		_Set( _data.data(), ArraySizeOf(_data) );
	}

	ArrayRStream::ArrayRStream (const void* ptr, Bytes size) __NE___
	{
		NOTHROW( _data.assign( Cast<ubyte>(ptr), Cast<ubyte>(ptr + size) ));

		_Set( _data.data(), ArraySizeOf(_data) );
	}

/*
=================================================
	ReleaseData
=================================================
*/
	Array<ubyte>  ArrayRStream::ReleaseData () __NE___
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
	bool  ArrayRStream::_Resize (Bytes newSize) __NE___
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
	bool  ArrayRStream::DecompressFrom (RStream &srcStream) __NE___
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
	SharedMemRStream::SharedMemRStream (RC<SharedMem> data) __NE___ :
		_data{ RVRef(data) }
	{
		if ( _data )
			_Set( _data->Data(), _data->Size() );
	}

	SharedMemRStream::SharedMemRStream (const void* ptr, Bytes size) __NE___ :
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
	RC<SharedMem>  SharedMemRStream::ReleaseData () __NE___
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
	bool  SharedMemRStream::_Resize (Bytes newSize) __NE___
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
	void  MemRefWStream::_Set (void* ptr, Bytes size) __NE___
	{
		ASSERT( (ptr != null) == (size > 0) );
		ASSERT( _pos <= size );

		_dataPtr	= ptr;
		_capacity	= size;
		_pos		= Min( _pos, size );
	}

/*
=================================================
	_Reset
=================================================
*/
	void  MemRefWStream::_Reset () __NE___
	{
		_dataPtr	= null;
		_capacity	= 0_b;
		_pos		= 0_b;
	}

/*
=================================================
	SeekFwd
=================================================
*/
	bool  MemRefWStream::SeekFwd (Bytes offset) __NE___
	{
		if_unlikely( (_pos + offset) > _capacity )
			return false;

		_pos += offset;
		return true;
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  MemRefWStream::GetSourceType () C_NE___
	{
		return	ESourceType::RandomAccess	| ESourceType::SequentialAccess |	// allow SeekFwd() & SeekSet()
				ESourceType::Buffered		| ESourceType::WriteAccess;
	}

/*
=================================================
	WriteSeq
=================================================
*/
	Bytes  MemRefWStream::WriteSeq (const void* buffer, Bytes size) __NE___
	{
		if_unlikely( _pos + size > _capacity )
			CHECK_ERR( _Resize( _pos + size ));

		ASSERT( _pos + size <= _capacity );
		MemCopy( OUT _dataPtr + _pos, buffer, size );

		_pos += size;
		return size;
	}

/*
=================================================
	UpdateFastStream
=================================================
*/
	void  MemRefWStream::UpdateFastStream (OUT void* &begin, OUT const void* &end, Bytes reserve) __NE___
	{
		ASSERT( begin == null or ( (begin >= _dataPtr) and (end <= _dataPtr + _capacity) ));

		// commit written size
		_pos = ( (begin >= _dataPtr) and (begin <= _dataPtr + _capacity) ) ?
				Bytes{begin} - Bytes{_dataPtr} :
				_pos;

		if_unlikely( _pos + reserve > _capacity )
			CHECK_ERRV( _Resize( _pos + reserve ));

		ASSERT( _pos + reserve <= _capacity );

		begin	= _dataPtr + _pos;
		end		= _dataPtr + _pos + reserve;	// TODO: use _capacity to make available all allocated space
	}

/*
=================================================
	EndFastStream
=================================================
*/
	void  MemRefWStream::EndFastStream (const void* ptr) __NE___
	{
		if ( (ptr >= _dataPtr + _pos) and (ptr <= _dataPtr + _capacity) )
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
	Bytes  MemRefWStream::GetFastStreamPosition (const void* ptr) __NE___
	{
		return Bytes{ptr} - Bytes{_dataPtr};
	}

/*
=================================================
	StoreTo
=================================================
*/
	bool  MemRefWStream::StoreTo (WStream &dstFile) C_NE___
	{
		CHECK_ERR( dstFile.IsOpen() );
		ASSERT( Position() > 0 );
		return dstFile.Write( _dataPtr, Position() );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ArrayWStream::ArrayWStream () __NE___
	{
		NOTHROW( _data.resize( usize(DefaultAllocationSize) ));
		_Set( _data.data(), ArraySizeOf(_data) );
	}

	ArrayWStream::ArrayWStream (Bytes bufferSize) __NE___
	{
		NOTHROW( _data.resize( usize(bufferSize) ));
		_Set( _data.data(), ArraySizeOf(_data) );
	}

	ArrayWStream::ArrayWStream (Array<ubyte> data) __NE___ :
		_data{ RVRef(data) }
	{
		_Set( _data.data(), ArraySizeOf(_data) );
	}

/*
=================================================
	ReleaseData
=================================================
*/
	Array<ubyte>  ArrayWStream::ReleaseData () __NE___
	{
		Array<ubyte>	tmp;
		_data.resize( usize{_pos} );
		std::swap( tmp, _data );
		_Reset();
		return tmp;
	}

/*
=================================================
	_Resize
=================================================
*/
	bool  ArrayWStream::_Resize (Bytes newSize) __NE___
	{
		NOTHROW_ERR( _data.resize( usize{newSize} ));
		_Set( _data.data(), ArraySizeOf(_data) );
		return true;
	}

/*
=================================================
	Reserve
=================================================
*/
	Bytes  ArrayWStream::Reserve (Bytes additionalSize) __NE___
	{
		NOTHROW( _data.reserve( Max( _data.capacity(), usize(_pos + additionalSize) )));
		_Set( _data.data(), ArraySizeOf(_data) );
		return ArraySizeOf(_data) - _pos;
	}

/*
=================================================
	Clear
=================================================
*/
	void  ArrayWStream::Clear () __NE___
	{
		Reconstruct( INOUT _data );
		_Reset();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	SharedMemWStream::SharedMemWStream (Bytes bufferSize) __NE___ :
		_data{ SharedMem::Create( AE::GetDefaultAllocator(), bufferSize )}
	{
		if ( _data )
			_Set( _data->Data(), _data->Size() );
	}

	SharedMemWStream::SharedMemWStream (RC<SharedMem> data) __NE___ :
		_data{ RVRef(data) }
	{
		if ( _data )
			_Set( _data->Data(), _data->Size() );
	}

/*
=================================================
	ReleaseData
----
	warning: actual size will be less than size of SharedMem
=================================================
*/
	RC<SharedMem>  SharedMemWStream::ReleaseData () __NE___
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
	bool  SharedMemWStream::_Resize (Bytes newSize) __NE___
	{
		CHECK_ERR( not _data );

		_data = SharedMem::Create( AE::GetDefaultAllocator(), newSize );
		CHECK_ERR( _data );

		_Set( _data->Data(), _data->Size() );
		return true;
	}

/*
=================================================
	Clear
=================================================
*/
	void  SharedMemWStream::Clear () __NE___
	{
		_data = null;
		_Reset();
	}


} // AE::Base
