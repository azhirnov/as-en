// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Stream/Stream.h"

namespace AE::Base
{

	//
	// Buffered read-only Stream
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
		~BufferedRStream () {}
		
		ND_ bool	IsOpen ()	const override		{ return _stream and _stream->IsOpen(); }
		ND_ Bytes	Position ()	const override;
		ND_ Bytes	Size ()		const override		{ return _stream->Size(); }

			bool	SeekSet (Bytes pos) override	{ Unused( pos ); return false; }	// only linear access
		ND_ Bytes	Read2 (OUT void *buffer, Bytes size) override;
		
			void	UpdateFastStream (OUT const void* &begin, OUT const void* &end) override;

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
		~BufferedWStream () {}
		
		ND_ bool	IsOpen ()	const override		{ return _stream and _stream->IsOpen(); }
		ND_ Bytes	Position ()	const override;
		ND_ Bytes	Size ()		const override		{ return Position(); }
		
			bool	SeekSet (Bytes pos) override	{ Unused( pos ); return false; }	// only linear access
		ND_ Bytes	Write2 (const void *buffer, Bytes size) override;

			void	Flush () override;
			void	UpdateFastStream (OUT void* &begin, OUT const void* &end, Bytes reserve = DefaultAllocationSize) override;
			void	EndFastStream (const void* ptr) override;

	private:
		void  _Flush ();
	};


}	// AE::Base
