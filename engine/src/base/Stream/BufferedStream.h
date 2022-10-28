// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Requirements:
	 * src stream must be unique
	 * only sequential access
*/

#pragma once

#include "base/Stream/Stream.h"

namespace AE::Base
{

	//
	// Buffered Read-only Stream
	//

	class BufferedRStream final : public RStream
	{
	// variables
	private:
		Array<ubyte>	_data;
		Bytes			_pos;
		RC<RStream>		_stream;


	// methods
	public:
		BufferedRStream () {}
		explicit BufferedRStream (RC<RStream> stream, Bytes bufferSize = DefaultAllocationSize);
		~BufferedRStream ();
		
		bool	IsOpen ()	const override			{ return _stream and _stream->IsOpen(); }
		Bytes	Position ()	const override;
		Bytes	Size ()		const override			{ return _stream->Size(); }
		
		EStreamType	GetStreamType () const override;

		bool	SeekSet (Bytes pos) override		{ Unused( pos ); return false; } // only linear access
		Bytes	ReadSeq (OUT void *buffer, Bytes size) override;
		
		void	UpdateFastStream (OUT const void* &begin, OUT const void* &end) override;
		void	EndFastStream (const void* ptr) override;
		

	private:
		void  _ReadNext ();
	};



	//
	// Buffered Write-only Stream
	//
	
	class BufferedWStream final : public WStream
	{
	// variables
	private:
		Array<ubyte>	_data;
		Bytes			_pos;
		RC<WStream>		_stream;


	// methods
	public:
		explicit BufferedWStream (RC<WStream> stream, Bytes bufferSize = DefaultAllocationSize);
		~BufferedWStream ();
		
		bool	IsOpen ()	const override			{ return _stream and _stream->IsOpen(); }
		Bytes	Position ()	const override;
		Bytes	Size ()		const override			{ return Position(); }
		
		EStreamType	GetStreamType () const override	{ return EStreamType::Buffered | EStreamType::SequentialAccess; }

		bool	SeekSet (Bytes pos) override		{ Unused( pos ); return false; } // only linear access
		Bytes	Write2 (const void *buffer, Bytes size) override;

		void	Flush () override;
		void	UpdateFastStream (OUT void* &begin, OUT const void* &end, Bytes reserve = DefaultAllocationSize) override;
		void	EndFastStream (const void* ptr) override;

	private:
		void  _Flush ();
	};


} // AE::Base
