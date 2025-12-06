// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_LZ4
# include "base/Defines/StdInclude.h"
# include "lz4.h"
# include "lz4hc.h"
# include "lz4frame.h"
# include "base/DataSource/Lz4Stream.h"

# if LZ4_VERSION_MAJOR != 1 or LZ4_VERSION_MINOR != 10
#	pragma message( "required lz4 version 1.10.0" )
# endif

namespace AE::Base
{

/*
=================================================
	constructor
=================================================
*/
	Lz4RStream::Lz4RStream (RC<RStream> stream) __NE___ :
		_stream{ RVRef(stream) }
	{
		ASSERT_Eq( LZ4F_getVersion(), LZ4F_VERSION );

		LZ4F_dctx*			ctx = null;
		LZ4F_errorCode_t	err = LZ4F_createDecompressionContext( OUT &ctx, LZ4F_VERSION );

		_context = ctx;

		Unused( err );
		ASSERT_MSG( not LZ4F_isError( err ),
			"LZ4 error: "s << LZ4F_getErrorName(err) );
		ASSERT( _context != null );
	}

/*
=================================================
	destructor
=================================================
*/
	Lz4RStream::~Lz4RStream () __NE___
	{
		LZ4F_freeDecompressionContext( static_cast< LZ4F_dctx *>(_context) );
	}

/*
=================================================
	IsOpen
=================================================
*/
	bool  Lz4RStream::IsOpen () C_NE___
	{
		return	_context != null	and
				_stream				and
				_stream->IsOpen();
	}

/*
=================================================
	SeekFwd
=================================================
*/
	bool  Lz4RStream::SeekFwd (Bytes offset) __NE___
	{
		ASSERT( IsOpen() );

		if_likely( offset == 0_b )
		{
			ASSERT_Eq( _position, _stream->Position() );
			return true;
		}

		char	buf [c_ReadBuffer];

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
	Bytes  Lz4RStream::ReadSeq (OUT void* buffer, const Bytes bufferSize) __NE___
	{
		ASSERT( IsOpen() );
		ASSERT( bufferSize < MaxValue<int>() );

		Bytes	written;
		char	temp		[c_ReadBuffer];
		usize	exp_size	= 1;

		const LZ4F_decompressOptions_t	options = {};

		for (; (exp_size != 0) and (written < bufferSize);)
		{
			_stream->SeekSet( _position );
			ASSERT_Eq( _position, _stream->Position() );

			Bytes	readn = _stream->ReadSeq( OUT temp, Sizeof(temp) );
			if_unlikely( readn == 0 )
				break;

			usize	src_size = usize{readn};
			usize	dst_size = usize{bufferSize - written};

			exp_size = LZ4F_decompress(  static_cast< LZ4F_dctx *>(_context),
										 OUT buffer + written, INOUT &dst_size,
										 temp, INOUT &src_size,
										 &options );

			CHECK_ERR_MSG( not LZ4F_isError( exp_size ),
				"LZ4 error: "s << LZ4F_getErrorName( exp_size ));

			ASSERT( src_size <= readn );

			_position	+= src_size;
			written		+= dst_size;
		}

		return written;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ExtractConfig
=================================================
*/
namespace {
	static void  ExtractConfig (const Lz4WStream::Config &cfg,
								OUT int &level)
	{
		level = int(Lerp( 0.0f, float(LZ4HC_CLEVEL_MAX), cfg.level ) + 0.5f);
	}
}
/*
=================================================
	constructor
=================================================
*/
	Lz4WStream::Lz4WStream (RC<WStream> stream, const Config &cfg) __NE___ :
		_stream{ RVRef(stream) }
	{
		ASSERT_Eq( LZ4F_getVersion(), LZ4F_VERSION );

		LZ4F_cctx*			ctx = null;
		LZ4F_errorCode_t	err = LZ4F_createCompressionContext( OUT &ctx, LZ4F_VERSION );

		_context = ctx;

		Unused( err );
		ASSERT_MSG( not LZ4F_isError( err ),
			"LZ4 error: "s << LZ4F_getErrorName( err ));
		ASSERT( _context != null );

		ExtractConfig( cfg, OUT _compressionLevel );
	}

/*
=================================================
	destructor
=================================================
*/
	Lz4WStream::~Lz4WStream () __NE___
	{
		LZ4F_freeCompressionContext( static_cast< LZ4F_cctx *>(_context) );
	}

/*
=================================================
	WriteSeq
=================================================
*/
	Bytes  Lz4WStream::WriteSeq (const void* buffer, const Bytes bufferSize) __NE___
	{
		ASSERT( IsOpen() );
		ASSERT( bufferSize < MaxValue<int>() );

		LZ4F_preferences_t	pref LZ4F_INIT_PREFERENCES;
		pref.compressionLevel = _compressionLevel;

		Array<char>		temp;
		temp.resize( LZ4F_compressBound( usize{bufferSize}, &pref ));  // throw

		if_unlikely( _position == 0 )
		{
			size_t  size = LZ4F_compressBegin( static_cast< LZ4F_cctx *>(_context), OUT temp.data(), temp.size(), &pref );
			CHECK_ERR_MSG( not LZ4F_isError( size ),
				"LZ4 error: "s << LZ4F_getErrorName( size ));

			CHECK_ERR( _stream->Write( &temp[0], Bytes{size} ));
		}

		const LZ4F_compressOptions_t	options = {};

		usize	exp_size = LZ4F_compressUpdate( static_cast< LZ4F_cctx *>(_context),
												OUT temp.data(), temp.size(),
												buffer, usize{bufferSize},
												&options );
		CHECK_ERR_MSG( not LZ4F_isError( exp_size ),
			"LZ4 error: "s << LZ4F_getErrorName( exp_size ));

		CHECK_ERR( _stream->Write( &temp[0], Bytes{exp_size} ));

		_position += exp_size;
		return bufferSize;
	}

/*
=================================================
	Reserve
=================================================
*/
	Bytes  Lz4WStream::Reserve (Bytes) __NE___
	{
		return 0_b;
	}

/*
=================================================
	_Flush
=================================================
*/
	bool  Lz4WStream::_Flush () __NE___
	{
		LZ4F_preferences_t	pref LZ4F_INIT_PREFERENCES;
		pref.compressionLevel = _compressionLevel;

		Array<char>		temp;
		temp.resize( LZ4F_compressBound( 0, &pref ) + c_BufferSize );  // throw

		const LZ4F_compressOptions_t	options = {};

		usize	written = LZ4F_flush( static_cast< LZ4F_cctx *>(_context), OUT temp.data(), temp.size(), &options );

		CHECK_ERR_MSG( not LZ4F_isError( written ),
			"LZ4 error: "s << LZ4F_getErrorName( written ));

		CHECK_ERR( _stream->Write( &temp[0], Bytes{written} ));
		return true;
	}

/*
=================================================
	_End
=================================================
*/
	bool  Lz4WStream::_End () __NE___
	{
		LZ4F_preferences_t	pref LZ4F_INIT_PREFERENCES;
		pref.compressionLevel = _compressionLevel;

		Array<char>		temp;
		temp.resize( LZ4F_compressBound( 0, &pref ) + c_BufferSize );  // throw

		const LZ4F_compressOptions_t	options = {};

		usize	written = LZ4F_compressEnd( static_cast< LZ4F_cctx *>(_context), OUT temp.data(), temp.size(), &options );

		CHECK_ERR_MSG( not LZ4F_isError( written ),
			"LZ4 error: "s << LZ4F_getErrorName( written ));

		CHECK_ERR( _stream->Write( &temp[0], Bytes{written} ));
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	MaxCompressedSize
=================================================
*/
	Bytes  Lz4Utils::MaxCompressedSize (Bytes nonCompressedSize) __NE___
	{
		CHECK_ERR( nonCompressedSize <= MaxValue<int>() );
		return Bytes{ulong( LZ4_compressBound( int(nonCompressedSize) ))};
	}

/*
=================================================
	Compress
=================================================
*/
	bool  Lz4Utils::Compress (OUT void* dstData, INOUT Bytes &dstSize,
							  const void* srcData, Bytes srcSize,
							  const Lz4WStream::Config &cfg) __NE___
	{
		CHECK_ERR( srcSize <= MaxValue<int>() );
		CHECK_ERR( dstSize <= MaxValue<int>() );
		ASSERT_GE( int(dstSize), LZ4_compressBound( int(srcSize) ));

		if ( cfg.hc )
		{
			const int lvl		= int(Lerp( float(LZ4HC_CLEVEL_MIN), float(LZ4HC_CLEVEL_MAX), cfg.level ) + 0.5f);
			const int comp_size	= LZ4_compress_HC( Cast<char>(srcData), OUT Cast<char>(dstData), int(srcSize), int(dstSize), lvl );
			CHECK_ERR( comp_size > 0 );

			dstSize = Bytes{uint(comp_size)};
			return true;
		}
		else
		{
			const int comp_size = LZ4_compress_default( Cast<char>(srcData), OUT Cast<char>(dstData), int(srcSize), int(dstSize) );
			CHECK_ERR( comp_size > 0 );

			dstSize = Bytes{uint(comp_size)};
			return true;
		}
	}

/*
=================================================
	Decompress
=================================================
*/
	bool  Lz4Utils::Decompress (OUT void* dstData, INOUT Bytes &dstSize,
								const void* srcData, Bytes srcSize) __NE___
	{
		CHECK_ERR( srcSize <= MaxValue<int>() );
		CHECK_ERR( dstSize <= MaxValue<int>() );

		int decomp_size = LZ4_decompress_safe( Cast<char>(srcData), OUT Cast<char>(dstData), int(srcSize), int(dstSize) );
		CHECK_ERR( decomp_size > 0 );

		dstSize = Bytes{uint(decomp_size)};
		return true;
	}

/*
=================================================
	CompressFrame
=================================================
*/
	bool  Lz4Utils::CompressFrame (OUT void* dstData, INOUT Bytes &dstSize,
									const void* srcData, Bytes srcSize,
									const Lz4WStream::Config &cfg) __NE___
	{
		LZ4F_preferences_t	pref LZ4F_INIT_PREFERENCES;

		ExtractConfig( cfg, OUT pref.compressionLevel );

		const size_t comp_size = LZ4F_compressFrame( OUT dstData, size_t{dstSize}, srcData, size_t{srcSize}, &pref );
		CHECK_ERR_MSG( not LZ4F_isError( comp_size ),
			"LZ4 error: "s << LZ4F_getErrorName( comp_size ));

		dstSize = Bytes{comp_size};
		return true;
	}

/*
=================================================
	DecompressFrame
=================================================
*/
	bool  Lz4Utils::DecompressFrame (OUT void* dstData, INOUT Bytes &dstSize,
									 const void* srcData, Bytes srcSize) __NE___
	{
		LZ4F_dctx*			ctx = null;
		LZ4F_errorCode_t	err;

		err = LZ4F_createDecompressionContext( OUT &ctx, LZ4F_VERSION );
		CHECK_ERR_MSG( not LZ4F_isError( err ),
			"LZ4 error: "s << LZ4F_getErrorName( err ));

		LZ4F_decompressOptions_t	options = {};

		Bytes	written;
		Bytes	readn;
		usize	exp_size = 1;

		for (; exp_size != 0 and written < dstSize;)
		{
			usize	dst_size	= usize{dstSize - written};
			usize	src_size	= usize{srcSize - readn};

			exp_size = LZ4F_decompress( ctx, OUT dstData + written, INOUT &dst_size, srcData + readn, INOUT &src_size, &options );
			CHECK_ERR_MSG( not LZ4F_isError( exp_size ),
				"LZ4 error: "s << LZ4F_getErrorName( exp_size ));

			written += dst_size;
			readn   += src_size;
		}

		LZ4F_freeDecompressionContext( ctx );

		dstSize = written;
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Allocate
=================================================
*
	bool  Lz4BufferedWStream::Allocate (Bytes blockSize) __NE___
	{
		if ( _decompressedBlock != null or _compressed != null )
			Deallocate();

		_decompressedBlock	= Allocator_t::Allocate( c_BlockSize );
		_capacity			= Bytes32u{ LZ4_compressBound( int{c_BlockSize} )};
		_compressed			= Allocator_t::Allocate( _capacity );

		_stream = LZ4_createStream();

		CHECK_ERR( _decompressedBlock != null );
		CHECK_ERR( _compressed != null );
		CHECK_ERR( _stream != null );

		return true;
	}

/*
=================================================
	Deallocate
=================================================
*
	void  Lz4BufferedWStream::Deallocate () __NE___
	{
		LZ4_freeStream( static_cast< LZ4_stream_t *>(_stream) );

		Allocator_t::Deallocate( _decompressedBlock );
		Allocator_t::Deallocate( _compressed );

		_stream				= null;
		_decompressedBlock	= null;
		_compressed			= null;
		_capacity			= 0;
		_compressedSize		= 0;
	}

/*
=================================================
	Write
=================================================
*
	bool  Lz4BufferedWStream::Write (const void* data, Bytes dataSize) __NE___
	{
		ASSERT( _decompressedBlock != null );
		ASSERT( _compressed != null );

		LZ4_compress_fast_continue( static_cast< LZ4_stream_t *>(_stream),
									_decompressedBlock, _compressed,
									_decompressedSize, _capacity, 0 );
	}
	*/

} // AE::Base

#endif // AE_ENABLE_LZ4
