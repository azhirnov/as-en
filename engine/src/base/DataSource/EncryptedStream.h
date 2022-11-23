// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/Stream.h"

namespace AE::Base
{

	//
	// Encrypted Read-only Stream
	//

	template <typename Encoder>
	class EncryptedRStream final : public RStream
	{
	// variables
	private:
		Encoder			_enc;
		RC<RStream>		_stream;


	// methods
	public:
		explicit EncryptedRStream (RC<RStream> stream, const Encoder &enc = Default) __Th___ :
			_enc{ enc },	// throw
			_stream{ RVRef(stream) }
		{}

		bool		IsOpen ()			C_NE_OV		{ return _stream and _stream->IsOpen(); }
		PosAndSize	PositionAndSize ()	C_NE_OV		{ return { _position, UMax }; }
		
		bool	SeekFwd (Bytes offset)	__NE_OV
		{
			return false;	// TODO
		}

		Bytes	ReadSeq (OUT void *buffer, Bytes size) __NE_OV
		{
			if_likely( _stream )
			{
				Bytes	readn = _stream->ReadSeq( OUT buffer, size );

				_enc.Encode( _position, INOUT buffer, readn );
				_position += readn;

				return readn;
			}
			return 0_b;
		}
	};

	

	//
	// Encrypted Write-only Stream
	//
	
	template <typename Decoder>
	class EncryptedWStream final : public WStream
	{
	// variables
	private:
		Decoder			_dec;
		RC<WStream>		_stream;
		

	// methods
	public:
		explicit EncryptedWStream (RC<WStream> stream, const Decoder &dec = Default) __Th___ :
			_dec{ dec },	// throw
			_stream{ RVRef(stream) }
		{}

		bool	IsOpen ()						C_NE_OV	{ return _stream and _stream->IsOpen(); }
		Bytes	Position ()						C_NE_OV	{ return _position; }
		
		Bytes	Reserve (Bytes additionalSize)	__NE_OV	{ return 0_b; }
		
		bool	SeekFwd (Bytes offset)			__NE_OV	{ return false; }
		
		void	Flush ()						__NE_OV	{}

		Bytes	WriteSeq (const void *buffer, Bytes size) __NE_OV
		{
			if_likely( _stream )
			{
				Bytes	written;
				for (; written < size;)
				{
					ubyte	temp [1024];
					Bytes	part_size	= Min( Sizeof(temp), size );

					_dec.Decode( OUT temp, buffer + written, part_size );

					Bytes	wr = _stream->WriteSeq( temp, part_size );
					written += wr;

					if_unlikely( wr == 0 )
						break;
				}
				return written;
			}
			return 0_b;
		}
	};


} // AE::Base
