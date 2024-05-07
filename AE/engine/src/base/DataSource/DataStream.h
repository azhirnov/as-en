// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Stream is used for sequential data access.
*/

#pragma once

#include "base/Containers/ArrayView.h"
#include "base/Memory/MemUtils.h"
#include "base/DataSource/DataSource.h"

namespace AE::Base
{

	//
	// Read-only Data Stream
	//

	class RStream : public IDataSource
	{
	// types
	public:
		struct PosAndSize
		{
			Bytes	pos;
			Bytes	size;	// UMax if 'FixedSize' flag is not returned by 'GetSourceType()'

			PosAndSize ()						__NE___	{}
			PosAndSize (Bytes pos, Bytes size)	__NE___	: pos{pos}, size{size} {}

			ND_ Bytes  Remaining ()				C_NE___	{ return size - pos; }
		};


	// interface
	public:
		ND_ virtual PosAndSize	PositionAndSize ()										C_NE___ = 0;

		// requires 'SequentialAccess' in 'GetSourceType()'
			virtual bool		SeekFwd (Bytes offset)									__NE___ = 0;

		// returns size of readn data
		ND_ virtual Bytes		ReadSeq (OUT void* buffer, Bytes size)					__NE___ = 0;

			virtual bool		Prefetch (Bytes offset, Bytes size)						__NE___ { Unused( offset, size );  return false; }

		// requires 'RandomAccess' in 'GetSourceType()'
			virtual bool		SeekSet (Bytes newPos)									__NE___;

		// api for FastStream
			virtual void		UpdateFastStream (OUT const void* &begin,
												  OUT const void* &end)					__NE___;
			virtual void		EndFastStream (const void* ptr)							__NE___;
		ND_ virtual Bytes		GetFastStreamPosition (const void* ptr)					__NE___;


	// methods
	public:
		RStream ()																		__NE___ {}


		// IDataSource //
			ESourceType		GetSourceType ()											C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::ReadAccess; }

		ND_ Bytes			Position ()													C_NE___	{ return PositionAndSize().pos; }
		ND_ Bytes			Size ()														C_NE___	{ return PositionAndSize().size; }
		ND_ Bytes			RemainingSize ()											C_NE___	{ auto tmp = PositionAndSize();  return tmp.Remaining(); }


		ND_ bool  Read (OUT void* buffer, Bytes size)									__NE___;

		template <typename T, typename A, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Read (usize length, OUT BasicString<T,A> &str)						__NE___;

		template <typename T, typename A, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Read (Bytes size, OUT BasicString<T,A> &str)							__NE___;

		template <typename T, typename A, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Read (usize count, OUT Array<T,A> &arr)								__NE___;

		template <typename T, typename A, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Read (Bytes size, OUT Array<T,A> &arr)								__NE___;

		template <typename T, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Read (OUT T &data)													__NE___;

		ND_ bool  Read (Bytes size, OUT MemChunkList &mem)							__NE___;
	};



	//
	// Write-only Data Stream
	//

	class WStream : public IDataSource
	{
	// interface
	public:
		// returns remaining size
		ND_ virtual Bytes	Reserve (Bytes additionalSize)								__NE___	{ DBG_WARNING( "Reserve() is not supported" );  Unused( additionalSize );  return 0_b; }

		ND_ virtual Bytes	Position ()													C_NE___ = 0;	// same as 'Size()'

			virtual bool	SeekFwd (Bytes offset)										__NE___ = 0;

		// returns size of written data
		ND_ virtual Bytes	WriteSeq (const void* buffer, Bytes size)					__NE___ = 0;

			virtual void	Flush ()													__NE___ = 0;

		// api for FastStream
			virtual void	UpdateFastStream (OUT void* &begin, OUT const void* &end,
											  Bytes reserve = DefaultAllocationSize)	__NE___;
			virtual void	EndFastStream (const void* ptr)								__NE___;
		ND_ virtual Bytes	GetFastStreamPosition (const void* ptr)						__NE___;


	// methods
	public:
		WStream ()																		__NE___ {}


		// IDataSource //
		ESourceType		GetSourceType ()												C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::WriteAccess; }


		ND_ bool  Write (const void* buffer, Bytes size)								__NE___;

		template <typename T, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Write (ArrayView<T> buf)												__NE___;

		template <typename T, typename A, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Write (const BasicString<T,A> &str)									__NE___;

		template <typename T, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Write (BasicStringView<T> str)										__NE___;

		template <typename T, ENABLEIF( IsTriviallySerializable<T> )>
		ND_ bool  Write (const T &data)													__NE___;

		ND_ bool  Write (const MemChunkList &mem)										__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	SeekSet
=================================================
*/
	inline bool  RStream::SeekSet (Bytes newPos) __NE___
	{
		const Bytes	pos = Position();
		if ( newPos >= pos )
			return SeekFwd( newPos - pos );

		DBG_WARNING( "SeekSet() is not supported" );
		return false;
	}

/*
=================================================
	UpdateFastStream / EndFastStream / GetFastStreamPosition
=================================================
*/
	inline void  RStream::UpdateFastStream (OUT const void* &begin, OUT const void* &end) __NE___
	{
		begin	= null;
		end		= null;
	}

	inline void  RStream::EndFastStream (const void* ptr) __NE___
	{
		Unused( ptr );
	}

	inline Bytes  RStream::GetFastStreamPosition (const void* ptr) __NE___
	{
		Unused( ptr );
		return 0_b;
	}

/*
=================================================
	Read
=================================================
*/
	inline bool  RStream::Read (OUT void* buffer, Bytes size) __NE___
	{
		return ReadSeq( buffer, size ) == size;
	}

	template <typename T, typename A, ENABLEIF_IMPL( IsTriviallySerializable<T> )>
	bool  RStream::Read (usize length, OUT BasicString<T,A> &str) __NE___
	{
		NOTHROW_ERR( str.resize( length ));

		Bytes	expected_size	{ sizeof(str[0]) * str.length() };
		Bytes	current_size	= ReadSeq( str.data(), expected_size );

		str.resize( usize(current_size / sizeof(str[0])) );		// nothrow

		return str.length() == length;
	}

	template <typename T, typename A, ENABLEIF_IMPL( IsTriviallySerializable<T> )>
	bool  RStream::Read (Bytes size, OUT BasicString<T,A> &str) __NE___
	{
		ASSERT( IsMultipleOf( size, sizeof(T) ));
		return Read( usize(size) / sizeof(T), OUT str );
	}

	template <typename T, typename A, ENABLEIF_IMPL( IsTriviallySerializable<T> )>
	bool  RStream::Read (usize count, OUT Array<T,A> &arr) __NE___
	{
		NOTHROW_ERR( arr.resize( count ));

		Bytes	expected_size	{ sizeof(arr[0]) * arr.size() };
		Bytes	current_size	= ReadSeq( arr.data(), expected_size );

		arr.resize( usize(current_size / sizeof(arr[0])) );		// nothrow

		return arr.size() == count;
	}

	template <typename T, typename A, ENABLEIF_IMPL( IsTriviallySerializable<T> )>
	bool  RStream::Read (Bytes size, OUT Array<T,A> &arr) __NE___
	{
		ASSERT( IsMultipleOf( size, sizeof(T) ));
		return Read( usize(size) / sizeof(T), OUT arr );
	}

	template <typename T, ENABLEIF_IMPL( IsTriviallySerializable<T> )>
	bool  RStream::Read (OUT T &data) __NE___
	{
		return ReadSeq( AddressOf(data), Sizeof(data) ) == Sizeof(data);
	}

	inline bool  RStream::Read (Bytes dataSize, OUT MemChunkList &mem) __NE___
	{
		const Bytes		chunk_size	= mem.ChunkDataSize();
		auto*			chunk		= mem.First();

		if_unlikely( (chunk == null) or (dataSize == 0) )
			return true;

		ASSERT( dataSize <= mem.Capacity() );

		bool	ok = true;
		for (; (chunk != null) and ok;)
		{
			Bytes	size = Min( dataSize, chunk_size );
			ok = (ReadSeq( OUT chunk->Data(), size ) == size);

			chunk		= chunk->next;
			dataSize	-= size;
		}
		return ok;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	UpdateFastStream / EndFastStream / GetFastStreamPosition
=================================================
*/
	inline void  WStream::UpdateFastStream (OUT void* &begin, OUT const void* &end, Bytes reserve) __NE___
	{
		Unused( reserve );
		begin	= null;
		end		= null;
	}

	inline void  WStream::EndFastStream (const void* ptr) __NE___
	{
		Unused( ptr );
	}

	inline Bytes  WStream::GetFastStreamPosition (const void* ptr) __NE___
	{
		Unused( ptr );
		return 0_b;
	}

/*
=================================================
	Write
=================================================
*/
	inline bool  WStream::Write (const void* buffer, Bytes size) __NE___
	{
		return WriteSeq( buffer, size ) == size;
	}

	template <typename T, ENABLEIF_IMPL( IsTriviallySerializable<T> )>
	bool  WStream::Write (ArrayView<T> buf) __NE___
	{
		Bytes	size { sizeof(buf[0]) * buf.size() };

		return WriteSeq( buf.data(), size ) == size;
	}

	template <typename T, typename A, ENABLEIF_IMPL( IsTriviallySerializable<T> )>
	bool  WStream::Write (const BasicString<T,A> &str) __NE___
	{
		return Write( BasicStringView<T>{ str });
	}

	template <typename T, ENABLEIF_IMPL( IsTriviallySerializable<T> )>
	bool  WStream::Write (BasicStringView<T> str) __NE___
	{
		if ( str.empty() )
			return true;

		Bytes	size { sizeof(str[0]) * str.length() };

		return WriteSeq( str.data(), size ) == size;
	}

	template <typename T, ENABLEIF_IMPL( IsTriviallySerializable<T> )>
	bool  WStream::Write (const T &data) __NE___
	{
		return WriteSeq( AddressOf(data), Sizeof(data) ) == Sizeof(data);
	}

	inline bool  WStream::Write (const MemChunkList &mem) __NE___
	{
		const Bytes		chunk_size	= mem.ChunkDataSize();
		Bytes			data_size	= mem.Size();
		auto*			chunk		= mem.First();

		if_unlikely( (chunk == null) or (data_size == 0) )
			return true;

		bool	ok = true;
		for (; (chunk != null) and ok;)
		{
			Bytes	size = Min( data_size, chunk_size );
			ok = (WriteSeq( chunk->Data(), size ) == size);

			chunk		= chunk->next;
			data_size	-= size;
		}

		return ok;
	}


} // AE::Base

