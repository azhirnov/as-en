// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Stream/Stream.h"

namespace AE::Base
{

	//
	// Read-only sub stream
	//

	template <typename T>
	class RSubStream final : public RStream
	{
	// variables
	private:
		T				_stream;
		Bytes			_pos;
		const Bytes		_offset;
		const Bytes		_size;


	// methods
	public:
		template <typename B>
		explicit RSubStream (B && stream) :
			_stream{ FwdArg<B>(stream) }
		{
			ASSERT( AllBits( _stream->GetStreamType(), EStreamType::RandomAccess ));
		}

		template <typename B>
		RSubStream (B && stream, Bytes offset, Bytes size) :
			_stream{ FwdArg<B>(stream) },
			_offset{ Min( offset, _stream->Size() )},
			_size{ Min( size, _stream->Size() - _offset )}
		{
			ASSERT( AllBits( _stream->GetStreamType(), EStreamType::RandomAccess ));
		}
		
		bool	IsOpen ()	const override	{ return _stream and _stream->IsOpen(); }
		Bytes	Position ()	const override	{ return _pos; }
		Bytes	Size ()		const override	{ return _size; }
		
		EStreamType	GetStreamType () const override	{ return EStreamType::RandomAccess; }

		bool	SeekSet (Bytes pos) override;
		Bytes	ReadSeq (OUT void *buffer, Bytes size) override;
		Bytes	ReadRnd (Bytes offset, OUT void *buffer, Bytes size) override;
	};

	
/*
=================================================
	SeekSet
=================================================
*/
	template <typename T>
	bool  RSubStream<T>::SeekSet (Bytes pos)
	{
		ASSERT( IsOpen() );

		if_unlikely( pos > _size )
			return false;

		_pos = pos;
		return true;
	}
	
/*
=================================================
	ReadSeq
=================================================
*/
	template <typename T>
	Bytes  RSubStream<T>::ReadSeq (OUT void *buffer, Bytes size)
	{
		ASSERT( IsOpen() );

		if_likely( _stream )
		{
			Bytes	readn = _stream->ReadRnd( _offset + _pos, buffer, size );
			_pos += readn;
			return readn;
		}
		else
			return 0_b;	// error or EOF
	}
	
/*
=================================================
	ReadRnd
=================================================
*/
	template <typename T>
	Bytes  RSubStream<T>::ReadRnd (Bytes offset, OUT void *buffer, Bytes size)
	{
		ASSERT( IsOpen() );
		
		if_likely( _stream )
			return _stream->ReadRnd( _offset + offset, buffer, size );
		else
			return 0_b;	// error or EOF
	}


} // AE::Base
