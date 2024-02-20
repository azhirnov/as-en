// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_ZSTD

# include "base/Defines/StdInclude.h"
# include "zstd.h"
# include "base/DataSource/ZStdStream.h"
# include "base/Algorithms/StringUtils.h"

namespace AE::Base
{

/*
=================================================
    constructor
=================================================
*/
    ZStdRStream::ZStdRStream (RC<RStream> stream) __NE___ :
        _stream{ RVRef(stream) }
    {
        _context = ZSTD_createDStream();
        ASSERT( _context != null );

        if ( _context != null )
        {
            //ZSTD_DCtx_setParameter( static_cast< ZSTD_DStream *>(_context), ZSTD_dParameter::ZSTD_d_windowLogMax,  );
        }
    }

/*
=================================================
    destructor
=================================================
*/
    ZStdRStream::~ZStdRStream () __NE___
    {
        ZSTD_freeDStream( static_cast< ZSTD_DStream *>(_context) );
    }

/*
=================================================
    SeekFwd
=================================================
*/
    bool  ZStdRStream::SeekFwd (Bytes offset) __NE___
    {
        ASSERT( IsOpen() );

        if_likely( offset == 0_b )
        {
            ASSERT_Eq( _position, _stream->Position() );
            return true;
        }

        char    buf [_BufferSize];

        for (; offset > 0_b;)
        {
            Bytes   part_size   = Min( offset, Sizeof( buf ));
            Bytes   read        = ReadSeq( OUT buf, part_size );

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
    Bytes  ZStdRStream::ReadSeq (OUT void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        ubyte           temp    [_BufferSize];
        ZSTD_inBuffer   in_buf  { temp, 0, 0 };
        ZSTD_outBuffer  out_buf { buffer, usize{size}, 0 };
        usize           code    = 1;

        _stream->SeekSet( _position );

        for (; (code >= 0) and (out_buf.pos < size);)
        {
            ASSERT_Eq( _position, _stream->Position() );

            in_buf.size = usize{_stream->ReadSeq( OUT temp, Sizeof(temp) )};
            in_buf.pos  = 0;

            if_unlikely( in_buf.size == 0 )
                return Bytes{out_buf.pos};

            code = ZSTD_decompressStream( static_cast< ZSTD_DStream *>(_context), INOUT &out_buf, INOUT &in_buf );

            _position += in_buf.pos;

            ASSERT_MSG( not ZSTD_isError( code ), ZSTD_getErrorName( code ));
        }

        return Bytes{out_buf.pos};
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ExtractConfig
=================================================
*/
namespace {
    static void  ExtractConfig (const ZStdWStream::Config &cfg,
                                OUT int &level)
    {
        level   = int( ZSTD_maxCLevel() * Saturate( cfg.level ) + 0.5f);
    }
}
/*
=================================================
    constructor
=================================================
*/
    ZStdWStream::ZStdWStream (RC<WStream> stream, const Config &cfg) __NE___ :
        _stream{ RVRef(stream) }
    {
        _context = ZSTD_createCStream();
        ASSERT( _context != null );

        if ( _context != null )
        {
            int     comp_lvl = 0;
            ExtractConfig( cfg, OUT comp_lvl );

            ZSTD_CCtx_setParameter( static_cast< ZSTD_CStream *>(_context), ZSTD_cParameter::ZSTD_c_compressionLevel,   comp_lvl );
            // TODO: more params
        }
    }

/*
=================================================
    destructor
=================================================
*/
    ZStdWStream::~ZStdWStream () __NE___
    {
        _End();
        ZSTD_freeCStream( static_cast< ZSTD_CStream *>(_context) );
    }

/*
=================================================
    SetTotalSize
=================================================
*
    void  ZStdWStream::SetTotalSize (Bytes size) __NE___
    {
    }

/*
=================================================
    Reserve
=================================================
*/
    Bytes  ZStdWStream::Reserve (Bytes) __NE___
    {
        return 0_b;
    }

/*
=================================================
    WriteSeq
=================================================
*/
    Bytes  ZStdWStream::WriteSeq (const void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        // ZSTD_compressStream2 or ZSTD_compressStream

        ubyte           temp    [_BufferSize];
        ZSTD_inBuffer   in_buf  { buffer, usize{size}, 0 };
        ZSTD_outBuffer  out_buf { temp, _BufferSize, 0 };
        usize           code    = 1;

        for (; code > 0;)
        {
            out_buf.pos = 0;

            code = ZSTD_compressStream2( static_cast< ZSTD_CStream *>(_context), INOUT &out_buf, INOUT &in_buf, ZSTD_e_continue );

            ASSERT_MSG( not ZSTD_isError( code ), ZSTD_getErrorName( code ));

            if ( out_buf.pos > 0 )
                CHECK_ERR( _stream->Write( &temp[0], Bytes{out_buf.pos} ));
        }

        _position += size;
        return size;
    }

/*
=================================================
    _Flush
=================================================
*/
    bool  ZStdWStream::_Flush () __NE___
    {
        ASSERT( IsOpen() );

        ubyte           temp    [_BufferSize];
        ZSTD_inBuffer   in_buf  { null, 0, 0 };
        ZSTD_outBuffer  out_buf { temp, _BufferSize, 0 };
        usize           code    = 1;

        for (; code > 0;)
        {
            out_buf.pos = 0;

            code = ZSTD_compressStream2( static_cast< ZSTD_CStream *>(_context), INOUT &out_buf, INOUT &in_buf, ZSTD_e_flush );

            if_unlikely( ZSTD_isError( code ))
            {
                ASSERT_MSG( false, ZSTD_getErrorName( code ));
                return false;
            }

            if ( out_buf.pos > 0 )
                CHECK_ERR( _stream->Write( &temp[0], Bytes{out_buf.pos} ));
        }
        return true;
    }

/*
=================================================
    _End
=================================================
*/
    bool  ZStdWStream::_End () __NE___
    {
        ASSERT( IsOpen() );

        ubyte           temp    [_BufferSize];
        ZSTD_inBuffer   in_buf  { null, 0, 0 };
        ZSTD_outBuffer  out_buf { temp, _BufferSize, 0 };
        usize           code    = 1;

        for (; code > 0;)
        {
            out_buf.pos = 0;

            code = ZSTD_compressStream2( static_cast< ZSTD_CStream *>(_context), INOUT &out_buf, INOUT &in_buf, ZSTD_e_end );

            if_unlikely( ZSTD_isError( code ))
            {
                ASSERT_MSG( false, ZSTD_getErrorName( code ));
                return false;
            }

            if ( out_buf.pos > 0 )
                CHECK_ERR( _stream->Write( &temp[0], Bytes{out_buf.pos} ));
        }
        return true;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Compress
=================================================
*/
    bool  ZStdUtils::Compress (OUT void* dstData, INOUT Bytes &dstSize,
                               const void* srcData, Bytes srcSize,
                               const ZStdWStream::Config &cfg) __NE___
    {
        int     comp_lvl = 0;
        ExtractConfig( cfg, OUT comp_lvl );

        usize   comp_size = ZSTD_compress( OUT dstData, usize{dstSize}, srcData, usize{srcSize}, comp_lvl );

        if_likely( ZSTD_isError( comp_size ) == 0 )
        {
            dstSize = Bytes{comp_size};
            return true;
        }

        ASSERT_MSG( false, ZSTD_getErrorName( comp_size ));
        dstSize = 0_b;
        return false;
    }

/*
=================================================
    Decompress
=================================================
*/
    bool  ZStdUtils::Decompress (OUT void* dstData, INOUT Bytes &dstSize,
                                 const void* srcData, Bytes srcSize) __NE___
    {
        usize   dec_size = ZSTD_decompress( OUT dstData, usize{dstSize}, srcData, usize{srcSize} );

        if_likely( ZSTD_isError( dec_size ) == 0 )
        {
            dstSize = Bytes{dec_size};
            return true;
        }

        ASSERT_MSG( false, ZSTD_getErrorName( dec_size ));
        dstSize = 0_b;
        return false;
    }


} // AE::Base

#endif // AE_ENABLE_ZSTD
