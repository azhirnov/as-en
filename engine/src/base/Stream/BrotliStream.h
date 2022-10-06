// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_BROTLI

#include "base/Stream/Stream.h"

namespace AE::Base
{

	//
	// Read-only Brotli Uncompression Stream
	//

	class BrotliRStream final : public RStream
	{
	// variables
	private:
		RC<RStream>		_stream;
		void *			_instance	= null;		// BrotliDecoderState
		Array<ubyte>	_buffer;
		Bytes			_position;				// uncompressed size
		uint			_lastResult;

		static const usize	_BufferSize	= 4u << 10;


	// methods
	public:
		explicit BrotliRStream (const RC<RStream> &stream);
		~BrotliRStream () override;

		bool	IsOpen ()	const override		{ return _instance and _stream and _stream->IsOpen(); }
		Bytes	Position ()	const override		{ return _position; }
		Bytes	Size ()		const override;
		
		bool	SeekSet (Bytes pos) override;
		Bytes	Read2 (OUT void *buffer, Bytes size) override;
	};


	
	//
	// Write-only Brotli Compression Stream
	//

	class BrotliWStream final : public WStream
	{
	// types
	public:
		struct Config
		{
			float	quality = 0.5f;

			Config () {}
		};


	// variables
	private:
		RC<WStream>		_stream;
		void *			_instance	= null;		// BrotliEncoderState
		Array<ubyte>	_buffer;
		Bytes			_position;				// uncompressed size

		static const usize	_BufferSize	= 4u << 10;


	// methods
	public:
		explicit BrotliWStream (const RC<WStream> &stream, const Config &cfg = Default);
		~BrotliWStream () override;
		
		bool	IsOpen ()	const override		{ return _instance and _stream and _stream->IsOpen(); }
		Bytes	Position ()	const override		{ return _position; }
		Bytes	Size ()		const override		{ return Position(); }
		
		bool	SeekSet (Bytes) override		{ return false; }
		Bytes	Write2 (const void *buffer, Bytes size) override;
		void	Flush () override				{ _Flush(); }

	private:
		bool _Flush ();
	};


}	// AE::Base

#endif	// AE_ENABLE_BROTLI
