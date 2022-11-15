// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Fast stream is used to provide read/write access without virtual function call.
*/

#pragma once

#include "base/DataSource/Stream.h"

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
		FastRStream (FastRStream &&)				__NE___ = default;

		explicit FastRStream (RC<RStream> stream)	__NE___;

		~FastRStream ()								__NE___;

		ND_ bool	Empty ()						C_NE___	{ return _ptr == _end; }
		ND_ Bytes	RemainingSize ()				C_NE___	{ return Bytes{_end} - Bytes{_ptr}; }
		
		template <typename T>
		ND_ forceinline bool  Read (OUT T& value)	__NE___
		{
			return Read( OUT &value, SizeOf<T> );
		}

		ND_ forceinline bool  Read (OUT void* buffer, Bytes size) __NE___;
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
		FastWStream (FastWStream &&)				__NE___ = default;

		explicit FastWStream (RC<WStream> stream)	__NE___;

		~FastWStream ()								__NE___;
		
		ND_ bool	Empty ()						C_NE___	{ return _ptr == _end; }
		ND_ Bytes	RemainingSize ()				C_NE___	{ return Bytes{_end} - Bytes{_ptr}; }
		
		template <typename T>
		ND_ forceinline bool  Write (const T& value)__NE___
		{
			return Write( &value, SizeOf<T> );
		}

		ND_ forceinline bool  Write (const void* buffer, Bytes size) __NE___;
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
			ASSERT( _ptr != null );
		}
	}
	
/*
=================================================
	destructor
=================================================
*/
	inline FastRStream::~FastRStream () __NE___
	{
		if ( (_stream != null) & (_ptr <= _end) )
			_stream->EndFastStream( _ptr );
	}

/*
=================================================
	Read
=================================================
*/
	forceinline bool  FastRStream::Read (OUT void* buffer, Bytes size) __NE___
	{
		ASSERT( size > 0 );
		ASSERT( _ptr != null );

		for_likely(; size > 0; )
		{
			Bytes	part_size = Min( Bytes{_end} - Bytes{_ptr}, size );
			MemCopy( OUT buffer, _ptr, part_size );
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
	inline FastWStream::FastWStream (RC<WStream> stream) __NE___ :
		_stream{ RVRef(stream) }
	{
		ASSERT( _stream and _stream->IsOpen() );
		
		if_likely( _stream and _stream->IsOpen() )
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
	inline FastWStream::~FastWStream () __NE___
	{
		if ( (_stream != null) & (_ptr <= _end) )
			_stream->EndFastStream( _ptr );
	}

/*
=================================================
	Write
=================================================
*/
	forceinline bool  FastWStream::Write (const void* buffer, Bytes size) __NE___
	{
		ASSERT( size > 0 );
		ASSERT( _ptr != null );
		
		for_likely(; size > 0; )
		{
			Bytes	part_size = Min( Bytes{_end} - Bytes{_ptr}, size );
			MemCopy( OUT _ptr, buffer, part_size );
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
