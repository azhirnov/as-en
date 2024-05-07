// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Fast stream is used to provide read/write access without virtual function call.
*/

#pragma once

#include "base/DataSource/DataStream.h"

namespace AE::Base
{

	//
	// Read-only Fast stream
	//
	struct FastRStream
	{
	// variables
	private:
		const void*		_ptr	= null;
		const void*		_end	= null;
		RC<RStream>		_stream;


	// methods
	public:
		FastRStream (FastRStream &&)									__NE___ = default;

		explicit FastRStream (const void* ptr, const void* end)			__NE___	: _ptr{ptr}, _end{end} { ASSERT( _ptr <= _end ); }
		explicit FastRStream (RC<RStream> stream)						__NE___;

		~FastRStream ()													__NE___;

		ND_ bool	Empty ()											C_NE___	{ return _ptr >= _end; }
		ND_ Bytes	RemainingSize ()									C_NE___	{ ASSERT( _ptr <= _end );  return Bytes{_end} - Bytes{_ptr}; }
		ND_ Bytes	Position ()											C_NE___;

		template <typename T>
		ND_ forceinline bool  Read (OUT T& value)						__NE___	{ return Read( OUT &value, SizeOf<T> ); }
		ND_ forceinline bool  Read (OUT void* buffer, Bytes size)		__NE___;
		ND_ forceinline Bytes ReadRemaining (OUT void* buffer)			__NE___;
	};


	//
	// Write-only Fast stream
	//
	struct FastWStream
	{
	// variables
	private:
		void*			_ptr	= null;
		const void*		_end	= null;
		RC<WStream>		_stream;


	// methods
	public:
		FastWStream (FastWStream &&)									__NE___ = default;

		explicit FastWStream (void* ptr, const void* end)				__NE___	: _ptr{ptr}, _end{end} { ASSERT( _ptr <= _end ); }
		explicit FastWStream (RC<WStream> stream)						__NE___;

		~FastWStream ()													__NE___;

		ND_ bool	Empty ()											C_NE___	{ return _ptr >= _end; }
		ND_ Bytes	RemainingSize ()									C_NE___	{ ASSERT( _ptr <= _end );  return Bytes{_end} - Bytes{_ptr}; }
		ND_ Bytes	Position ()											C_NE___;

		template <typename T>
		ND_ forceinline bool  Write (const T& value)					__NE___	{ return Write( &value, SizeOf<T> ); }
		ND_ forceinline bool  Write (const void* buffer, Bytes size)	__NE___;
	};
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline FastRStream::FastRStream (RC<RStream> stream) __NE___ : _stream{ RVRef(stream) }
	{
		ASSERT( _stream and _stream->IsOpen() );

		if_likely( _stream and _stream->IsOpen() )
		{
			ASSERT( AllBits( _stream->GetSourceType(), RStream::ESourceType::Buffered ));

			_stream->UpdateFastStream( OUT _ptr, OUT _end );
			NonNull( _ptr );
		}
	}

/*
=================================================
	destructor
=================================================
*/
	inline FastRStream::~FastRStream () __NE___
	{
		if ( (_stream != null) and (not Empty()) )
			_stream->EndFastStream( _ptr );
	}

/*
=================================================
	Position
----
	returns current position in stream
=================================================
*/
	inline Bytes  FastRStream::Position () C_NE___
	{
		if_likely( _stream )
			return _stream->GetFastStreamPosition( _ptr );
		else
			return UMax;  // error
	}

/*
=================================================
	Read
=================================================
*/
	forceinline bool  FastRStream::Read (OUT void* buffer, Bytes size) __NE___
	{
		//ASSERT( size > 0 );
		NonNull( _ptr );

		for_likely(; size > 0; )
		{
			Bytes	part_size = Min( Bytes{_end} - Bytes{_ptr}, size );
			MemCopy( OUT buffer, _ptr, part_size );
			_ptr += part_size;

			if_likely( size == part_size )
				return true; // early exit

			buffer += part_size;
			size   -= part_size;

			if_likely( _stream )
			{
				_stream->UpdateFastStream( OUT _ptr, OUT _end );
				NonNull( _ptr );
			}

			if_unlikely( Empty() )
				return false; // stream is completely read
		}

		//DBG_WARNING( "should never happen" );
		return true;
	}

/*
=================================================
	ReadRemaining
=================================================
*/
	forceinline Bytes  FastRStream::ReadRemaining (OUT void* buffer) __NE___
	{
		NonNull( _ptr );

		const Bytes		size = RemainingSize();

		MemCopy( OUT buffer, _ptr, size );
		_ptr += size;

		return size;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	inline FastWStream::FastWStream (RC<WStream> stream) __NE___ :
		_stream{ RVRef(stream) }
	{
		ASSERT( _stream and _stream->IsOpen() );

		if_likely( _stream and _stream->IsOpen() )
		{
			_stream->UpdateFastStream( OUT _ptr, OUT _end );
			NonNull( _ptr );
		}
	}

/*
=================================================
	destructor
=================================================
*/
	inline FastWStream::~FastWStream () __NE___
	{
		if ( (_stream != null) and (not Empty()) )
			_stream->EndFastStream( _ptr );
	}

/*
=================================================
	Position
----
	returns current position in stream
=================================================
*/
	inline Bytes  FastWStream::Position () C_NE___
	{
		if_likely( _stream )
			return _stream->GetFastStreamPosition( _ptr );
		else
			return UMax;  // error
	}

/*
=================================================
	Write
=================================================
*/
	forceinline bool  FastWStream::Write (const void* buffer, Bytes size) __NE___
	{
		//ASSERT( size > 0 );
		NonNull( _ptr );

		for_likely(; size > 0; )
		{
			Bytes	part_size = Min( Bytes{_end} - Bytes{_ptr}, size );
			MemCopy( OUT _ptr, buffer, part_size );
			_ptr += part_size;

			if_likely( size == part_size )
				return true; // early exit

			buffer += part_size;
			size   -= part_size;

			if_likely( _stream )
			{
				_stream->UpdateFastStream( OUT _ptr, OUT _end );
				NonNull( _ptr );
			}

			if_unlikely( Empty() )
				return false; // stream is completely read
		}

		//DBG_WARNING( "should never happen" );
		return true;
	}


} // AE::Serializing
