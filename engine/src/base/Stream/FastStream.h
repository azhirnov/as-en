// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Fast stream is used to provide read/write access without virtual function call.
*/

#pragma once

#include "base/Stream/Stream.h"

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
		FastRStream (FastRStream &&) = default;

		explicit FastRStream (RC<RStream> stream);

		ND_ bool	Empty ()			const	{ return _ptr == _end; }
		ND_ Bytes	RemainingSize ()	const	{ return Bytes{_end} - Bytes{_ptr}; }
		
		template <typename T>
		ND_ forceinline bool  Read (OUT T& value)
		{
			return Read( OUT &value, SizeOf<T> );
		}

		ND_ forceinline bool  Read (OUT void* buffer, Bytes size);
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
		FastWStream (FastWStream &&) = default;

		explicit FastWStream (RC<WStream> stream);

		~FastWStream ();
		
		ND_ bool	Empty ()			const	{ return _ptr == _end; }
		ND_ Bytes	RemainingSize ()	const	{ return Bytes{_end} - Bytes{_ptr}; }
		
		template <typename T>
		ND_ forceinline bool  Write (const T& value)
		{
			return Write( &value, SizeOf<T> );
		}

		ND_ forceinline bool  Write (const void* buffer, Bytes size);
	};
//-----------------------------------------------------------------------------
	

	
/*
=================================================
	constructor
=================================================
*/
	inline FastRStream::FastRStream (RC<RStream> stream) : _stream{ RVRef(stream) }
	{
		ASSERT( _stream != null );
		ASSERT( _stream->IsOpen() );

		if_likely( _stream != null and _stream->IsOpen() )
		{
			_stream->UpdateFastStream( OUT _ptr, OUT _end );
			ASSERT( _ptr != null );
		}
	}

/*
=================================================
	Read
=================================================
*/
	forceinline bool  FastRStream::Read (OUT void* buffer, Bytes size)
	{
		ASSERT( size > 0 );
		ASSERT( _ptr != null );

		for_likely(; size > 0; )
		{
			Bytes	part_size = Min( Bytes{_end} - Bytes{_ptr}, size );
			std::memcpy( OUT buffer, _ptr, usize(part_size) );
			_ptr += part_size;
			
			if_likely( size == part_size )
				return true; // early exit

			buffer += part_size;
			size   -= part_size;

			_stream->UpdateFastStream( OUT _ptr, OUT _end );
			CHECK( _ptr != null );
			
			if_unlikely( _ptr == _end )
				return false; // stream is completely read
		}

		DBG_WARNING( "should never happen" );
		return true;
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	constructor
=================================================
*/
	inline FastWStream::FastWStream (RC<WStream> stream) : _stream{ RVRef(stream) }
	{
		ASSERT( _stream );
		ASSERT( _stream->IsOpen() );
		
		if_likely( _stream != null and _stream->IsOpen() )
		{
			_stream->UpdateFastStream( OUT _ptr, OUT _end );
			ASSERT( _ptr != null );
		}
	}
	
/*
=================================================
	destructor
=================================================
*/
	inline FastWStream::~FastWStream ()
	{
		if ( (_stream != null) & (_ptr <= _end) )
			_stream->EndFastStream( _ptr );
	}

/*
=================================================
	Write
=================================================
*/
	forceinline bool  FastWStream::Write (const void* buffer, Bytes size)
	{
		ASSERT( size > 0 );
		ASSERT( _ptr != null );
		
		for_likely(; size > 0; )
		{
			Bytes	part_size = Min( Bytes{_end} - Bytes{_ptr}, size );
			std::memcpy( OUT _ptr, buffer, usize(part_size) );
			_ptr += part_size;
			
			if_likely( size == part_size )
				return true; // early exit

			buffer += part_size;
			size   -= part_size;

			_stream->UpdateFastStream( OUT _ptr, OUT _end );
			CHECK( _ptr != null );
			
			if_unlikely( _ptr == _end )
				return false; // stream is completely read
		}
		
		DBG_WARNING( "should never happen" );
		return true;
	}

} // AE::Serializing
