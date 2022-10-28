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
		Bytes			_position;				// uncompressed size
		uint			_lastResult;

		static const usize	_BufferSize	= 4u << 10;


	// methods
	public:
		explicit BrotliRStream (const RC<RStream> &stream);
		~BrotliRStream () override;

		bool	IsOpen ()		const override			{ return _instance and _stream and _stream->IsOpen(); }
		Bytes	Position ()		const override			{ return _position; }
		Bytes	Size ()			const override			{ DBG_WARNING( "not supported for compressed file" );  return UMax; }
		
		EStreamType	GetStreamType () const override		{ return EStreamType::SequentialAccess; }

		bool	SeekSet (Bytes pos) override;
		Bytes	ReadSeq (OUT void *buffer, Bytes size) override;
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
			float	quality			= 0.5f;
			float	windowBits		= 0.85f;	// window size: 1u << (10..24 * windowBits)		default: 22
			float	inBlockSize		= 0.0f;		// block size:  1u << (16..24 * inBlockSize)

			Config () {}
		};


	// variables
	private:
		RC<WStream>		_stream;
		void *			_instance	= null;		// BrotliEncoderState
		Bytes			_position;				// uncompressed size

		static const usize	_BufferSize	= 4u << 10;


	// methods
	public:
		explicit BrotliWStream (const RC<WStream> &stream, const Config &cfg = Default);
		~BrotliWStream () override;

		void	Reserve (Bytes size);
		
		bool	IsOpen ()	const override				{ return _instance and _stream and _stream->IsOpen(); }
		Bytes	Position ()	const override				{ return _position; }
		Bytes	Size ()		const override				{ return _position; }
		
		EStreamType	GetStreamType () const override		{ return EStreamType::SequentialAccess; }

		bool	SeekSet (Bytes) override				{ return false; }
		Bytes	Write2 (const void *buffer, Bytes size) override;
		void	Flush () override						{ _Flush(); }

	private:
		bool _Flush ();
	};


} // AE::Base

#endif // AE_ENABLE_BROTLI
