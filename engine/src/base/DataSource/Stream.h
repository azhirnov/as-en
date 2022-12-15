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
	// Read-only Stream
	//
	
	class RStream : public IDataSource
	{
	// types
	public:
		struct PosAndSize
		{
			Bytes	pos;
			Bytes	size;	// UMax if 'FixedSize' flag is not returned by 'GetSourceType()'

			PosAndSize () {}
			PosAndSize (Bytes pos, Bytes size) : pos{pos}, size{size} {}

			ND_ Bytes  Remaining () const	{ return size - pos; }
		};


	// interface
	public:
		ND_ virtual PosAndSize	PositionAndSize ()		C_NE___ = 0;

			virtual bool		SeekFwd (Bytes offset)	__NE___ = 0;
			
		// returns size of readn data
		ND_ virtual Bytes		ReadSeq (OUT void *buffer, Bytes size) __NE___ = 0;
		

		virtual bool	SeekSet (Bytes newPos)			__NE___
		{
			const Bytes	pos = Position();
			if ( newPos >= pos )
				return SeekFwd( newPos - pos );
			return false;
		}
		

		virtual void  UpdateFastStream (OUT const void* &begin, OUT const void* &end) __NE___
		{
			begin	= null;
			end		= null;
		}

		virtual void  EndFastStream (const void* ptr)				__NE___
		{
			Unused( ptr );
		}

		ND_ virtual Bytes  GetFastStreamPosition (const void* ptr)	__NE___
		{
			Unused( ptr );
			return 0_b;
		}
		

	// methods
	public:
		RStream ()										__NE___ {}
		

		// IDataSource //
			ESourceType		GetSourceType ()			C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::ReadAccess; }
		
		ND_ Bytes			Position ()					C_NE___	{ return PositionAndSize().pos; }
		ND_ Bytes			Size ()						C_NE___	{ return PositionAndSize().size; }
		ND_ Bytes			RemainingSize ()			C_NE___	{ auto tmp = PositionAndSize();  return tmp.Remaining(); }
		

		bool  Read (OUT void *buffer, Bytes size)		__NE___
		{
			return ReadSeq( buffer, size ) == size;
		}
		

		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (usize length, OUT BasicString<T,A> &str) __NE___
		{
			CATCH_ERR( str.resize( length ));

			Bytes	expected_size	{ sizeof(str[0]) * str.length() };
			Bytes	current_size	= ReadSeq( str.data(), expected_size );
		
			str.resize( usize(current_size / sizeof(str[0])) );		// nothrow

			return str.length() == length;
		}
		

		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (Bytes size, OUT BasicString<T,A> &str) __NE___
		{
			ASSERT( IsAligned( size, sizeof(T) ));
			return Read( usize(size) / sizeof(T), OUT str );
		}


		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (usize count, OUT Array<T,A> &arr) __NE___
		{
			CATCH_ERR( arr.resize( count ));

			Bytes	expected_size	{ sizeof(arr[0]) * arr.size() };
			Bytes	current_size	= ReadSeq( arr.data(), expected_size );
		
			arr.resize( usize(current_size / sizeof(arr[0])) );		// nothrow

			return arr.size() == count;
		}
		

		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Read (Bytes size, OUT Array<T,A> &arr) __NE___
		{
			ASSERT( IsAligned( size, sizeof(T) ));
			return Read( usize(size) / sizeof(T), OUT arr );
		}


		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Read (OUT T &data) __NE___
		{
			return ReadSeq( AddressOf(data), Sizeof(data) ) == Sizeof(data);
		}
	};



	//
	// Write-only Stream
	//
	
	class WStream : public IDataSource
	{
	// interface
	public:
		// returns remaining size
		ND_ virtual Bytes	Reserve (Bytes additionalSize)	__NE___ = 0;

		ND_ virtual Bytes	Position ()						C_NE___ = 0;		// same as 'Size()'
		
			virtual bool	SeekFwd (Bytes offset)			__NE___ = 0;
		
		// returns size of written data
		ND_ virtual Bytes	WriteSeq (const void *buffer, Bytes size) __NE___ = 0;

			virtual void	Flush ()						__NE___ = 0;
			

		virtual void  UpdateFastStream (OUT void* &begin, OUT const void* &end, Bytes reserve = DefaultAllocationSize) __NE___
		{
			Unused( reserve );
			begin	= null;
			end		= null;
		}

		virtual void  EndFastStream (const void* ptr)				__NE___
		{
			Unused( ptr );
		}

		ND_ virtual Bytes  GetFastStreamPosition (const void* ptr)	__NE___
		{
			Unused( ptr );
			return 0_b;
		}
		

		
	// methods
	public:
		WStream ()											__NE___ {}
		

		// IDataSource //
		ESourceType		GetSourceType ()					C_NE_OV	{ return ESourceType::SequentialAccess | ESourceType::WriteAccess; }


		bool  Write (const void *buffer, Bytes size)		__NE___
		{
			return WriteSeq( buffer, size ) == size;
		}
		

		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Write (ArrayView<T> buf) __NE___
		{
			Bytes	size { sizeof(buf[0]) * buf.size() };

			return WriteSeq( buf.data(), size ) == size;
		}
		
	
		template <typename T, typename A>
		EnableIf<IsTrivial<T>, bool>  Write (const BasicString<T,A> str) __NE___
		{
			return Write( BasicStringView<T>{ str });
		}


		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Write (BasicStringView<T> str) __NE___
		{
			if ( str.empty() )
				return true;

			Bytes	size { sizeof(str[0]) * str.length() };

			return WriteSeq( str.data(), size ) == size;
		}


		template <typename T>
		EnableIf<IsTrivial<T>, bool>  Write (const T &data) __NE___
		{
			return WriteSeq( AddressOf(data), Sizeof(data) ) == Sizeof(data);
		}
	};


} // AE::Base

