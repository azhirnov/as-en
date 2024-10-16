// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_BROTLI

# include "base/Defines/StdInclude.h"
# include "brotli/decode.h"
# include "brotli/encode.h"

# include "base/DataSource/BrotliStream.h"
# include "base/Math/Vec.h"
# include "base/Algorithms/StringUtils.h"
# include "base/Memory/IAllocator.h"

namespace AE::Base
{
namespace
{
	static void*  BrotliAlloc (void* opaque, size_t size) __NE___
	{
		return Cast<IAllocator>(opaque)->Allocate( Bytes{size} );
	}

	static void   BrotliFree (void* opaque, void* address) __NE___
	{
		Cast<IAllocator>(opaque)->Deallocate( address );
	}
}
//-----------------------------------------------------------------------------


/*
=================================================
	constructor
=================================================
*/
	BrotliRStream::BrotliRStream (RC<RStream> stream, RC<IAllocator> alloc) __NE___ :
		_stream{ RVRef(stream) },
		_position{ _stream->Position() },
		_lastResult{ BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT },
		_allocator{ RVRef(alloc) }
	{
		if ( _allocator ) {
			_instance = BrotliDecoderCreateInstance( &BrotliAlloc, &BrotliFree, _allocator.get() );
		}else{
			_instance = BrotliDecoderCreateInstance( null, null, null );
		}
		ASSERT( _instance != null );
	}

/*
=================================================
	destructor
=================================================
*/
	BrotliRStream::~BrotliRStream () __NE___
	{
		if ( _instance != null )
			BrotliDecoderDestroyInstance( static_cast<BrotliDecoderState *>(_instance) );
	}

/*
=================================================
	SeekFwd
=================================================
*/
	bool  BrotliRStream::SeekFwd (Bytes offset) __NE___
	{
		ASSERT( IsOpen() );

		if_likely( offset == 0_b )
		{
			ASSERT_Eq( _position, _stream->Position() );
			return true;
		}

		ubyte	buf [_BufferSize];

		for (; offset > 0_b;)
		{
			Bytes	part_size	= Min( offset, Sizeof( buf ));
			Bytes	read		= ReadSeq( OUT buf, part_size );

			if_unlikely( part_size != read )
				return false;

			offset -= part_size;
		}
		return true;
	}

/*
=================================================
	ReadSeq
=================================================
*/
	Bytes  BrotliRStream::ReadSeq (OUT void* buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );
		ASSERT_Eq( _position, _stream->Position() );

		BrotliDecoderResult	result	= BrotliDecoderResult(_lastResult);
		Bytes				written;

		// decompress next part
		if ( result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT )
		{
			usize			available_out	= usize{size};
			ubyte*			next_out		= Cast<ubyte>( buffer );
			usize			available_in	= 0;
			ubyte const*	next_in			= null;

			result = BrotliDecoderDecompressStream( static_cast<BrotliDecoderState *>(_instance), INOUT &available_in, INOUT &next_in,
													INOUT &available_out, INOUT &next_out, null );

			written += (size - available_out);
		}

		// read next part and decompress
		ubyte	temp [_BufferSize];

		for (; (result == BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT) and (written < size);)
		{
			usize			available_out	= usize{size - written};
			ubyte*			next_out		= Cast<ubyte>( buffer + written );
			usize			available_in	= usize{_stream->ReadSeq( OUT temp, Sizeof(temp) )};
			ubyte const*	next_in			= &temp[0];

			if_unlikely( available_in == 0 )
				return written;

			_position += available_in;

			result = BrotliDecoderDecompressStream( static_cast<BrotliDecoderState *>(_instance), INOUT &available_in, INOUT &next_in,
													INOUT &available_out, INOUT &next_out, null );

			written += ((size - written) - available_out);
		}

		switch ( result )
		{
			case BROTLI_DECODER_RESULT_SUCCESS :
			case BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT :
				_lastResult = uint(BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT);
				return written;

			case BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT :
				_lastResult = uint(result);
				return written;

			case BROTLI_DECODER_RESULT_ERROR :		// not supported
			default :
				DBG_WARNING( "" );
				return written;
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ExtractConfig
=================================================
*/
namespace {
	static void  ExtractConfig (const BrotliWStream::Config &cfg,
								OUT int &quality,
								OUT int &lgwin,
								OUT int &lgblock)
	{
		quality	= int( Lerp( float(BROTLI_MIN_QUALITY), float(BROTLI_MAX_QUALITY), Saturate( cfg.quality )) + 0.5f );
		lgwin	= int( Lerp( float(BROTLI_MIN_WINDOW_BITS), float(BROTLI_MAX_WINDOW_BITS), Saturate( cfg.windowBits )) + 0.5f );
		lgblock	= int( Lerp( float(BROTLI_MIN_INPUT_BLOCK_BITS), float(BROTLI_MAX_INPUT_BLOCK_BITS), Saturate( cfg.inBlockSize )) + 0.5f );
	}
}
/*
=================================================
	constructor
=================================================
*/
	BrotliWStream::BrotliWStream (RC<WStream> stream, const Config &cfg, RC<IAllocator> alloc) __NE___ :
		_stream{ RVRef(stream) },
		_allocator{ RVRef(alloc) }
	{
		if ( _allocator ){
			_instance = BrotliEncoderCreateInstance( &BrotliAlloc, &BrotliFree, _allocator.get() );
		}else{
			_instance = BrotliEncoderCreateInstance( null, null, null );
		}
		ASSERT( _instance != null );

		if ( _instance != null )
		{
			int	quality, lgwin, lgblock;
			ExtractConfig( cfg, OUT quality, OUT lgwin, OUT lgblock );

			BrotliEncoderSetParameter( static_cast<BrotliEncoderState *>(_instance),	BROTLI_PARAM_MODE,		BROTLI_MODE_GENERIC );
			BrotliEncoderSetParameter( static_cast<BrotliEncoderState *>(_instance),	BROTLI_PARAM_QUALITY,	quality );
			BrotliEncoderSetParameter( static_cast<BrotliEncoderState *>(_instance),	BROTLI_PARAM_LGWIN,		lgwin );
			BrotliEncoderSetParameter( static_cast<BrotliEncoderState *>(_instance),	BROTLI_PARAM_LGBLOCK,	lgblock );
		}
	}

/*
=================================================
	destructor
=================================================
*/
	BrotliWStream::~BrotliWStream () __NE___
	{
		if ( _instance != null )
		{
			CHECK( _Flush() );
			BrotliEncoderDestroyInstance( static_cast<BrotliEncoderState *>(_instance) );
		}
	}

/*
=================================================
	SetTotalSize
=================================================
*/
	void  BrotliWStream::SetTotalSize (Bytes size) __NE___
	{
		ASSERT( IsOpen() );
		CHECK_ERRV( size < ~0u );

		BrotliEncoderSetParameter( static_cast<BrotliEncoderState *>(_instance), BROTLI_PARAM_SIZE_HINT, uint(size) );
	}

/*
=================================================
	Reserve
=================================================
*/
	Bytes	BrotliWStream::Reserve (Bytes additionalSize) __NE___
	{
		SetTotalSize( Position() + additionalSize );
		return additionalSize;
	}

/*
=================================================
	WriteSeq
=================================================
*/
	Bytes	BrotliWStream::WriteSeq (const void* buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );
		ASSERT( not BrotliEncoderIsFinished( static_cast<BrotliEncoderState *>(_instance) ));

		ubyte			temp [_BufferSize];
		usize			available_in	= usize{size};
		ubyte const*	next_in			= static_cast<ubyte const *>(buffer);

		for (; available_in > 0;)
		{
			usize	available_out	= _BufferSize;
			ubyte*	next_out		= &temp[0];

			CHECK_ERR( BrotliEncoderCompressStream( static_cast<BrotliEncoderState *>(_instance), BROTLI_OPERATION_PROCESS,
													INOUT &available_in, INOUT &next_in,
													INOUT &available_out, INOUT &next_out, null ) == BROTLI_TRUE );

			const Bytes	out_size = Bytes(next_out - &temp[0]);

			if_unlikely( out_size > 0 )
				CHECK_ERR( _stream->Write( &temp[0], out_size ));
		}
		_position += size;
		return size;
	}

/*
=================================================
	_Flush
=================================================
*/
	bool  BrotliWStream::_Flush () __NE___
	{
		ASSERT( IsOpen() );

		if ( BrotliEncoderIsFinished( static_cast<BrotliEncoderState *>(_instance) ))
			return true;

		ubyte	temp [_BufferSize];

		for (;;)
		{
			usize	available_in	= 0;
			usize	available_out	= _BufferSize;
			ubyte*	next_out		= &temp[0];

			CHECK_ERR( BrotliEncoderCompressStream( static_cast<BrotliEncoderState *>(_instance), BROTLI_OPERATION_FINISH,
													INOUT &available_in, null,
													INOUT &available_out, INOUT &next_out, null ) == BROTLI_TRUE );

			const Bytes	out_size = Bytes(next_out - &temp[0]);

			if_unlikely( out_size == 0 )
				break;

			CHECK_ERR( _stream->Write( &temp[0], out_size ));
		}
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Compress
=================================================
*/
	bool  BrotliUtils::Compress (OUT void* dstData, INOUT Bytes &dstSize,
								 const void* srcData, Bytes srcSize,
								 const BrotliWStream::Config &cfg) __NE___
	{
		int	quality, lgwin, lgblock;
		ExtractConfig( cfg, OUT quality, OUT lgwin, OUT lgblock );

		usize	encoded_size = usize{dstSize};

		bool	res = BrotliEncoderCompress( quality, lgwin, BROTLI_MODE_GENERIC,
											 usize{srcSize}, Cast<uint8_t>(srcData),
											 INOUT &encoded_size, OUT Cast<uint8_t>(dstData) ) == BROTLI_TRUE;
		dstSize = Bytes{res ? encoded_size : 0};
		return res;
	}

/*
=================================================
	Decompress
=================================================
*/
	bool  BrotliUtils::Decompress (OUT void* dstData, INOUT Bytes &dstSize,
									const void* srcData, Bytes srcSize) __NE___
	{
		usize	decoded_size = usize{dstSize};
		bool	res = BrotliDecoderDecompress(	usize{srcSize}, Cast<uint8_t>(srcData),
												INOUT &decoded_size, OUT Cast<uint8_t>(dstData) ) == BROTLI_DECODER_RESULT_SUCCESS;
		dstSize = Bytes{res ? decoded_size : 0};
		return res;
	}


} // AE::Base

#endif // AE_ENABLE_BROTLI
