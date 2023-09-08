// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/Stream.h"

namespace AE::Base
{

/*
=================================================
    BufferedCopy
=================================================
*/
    Bytes  DataSourceUtils::BufferedCopy (WStream &dstStream, RStream &srcStream, const Bytes srcSize, const TempBuffer &buffer) __NE___
    {
        CHECK_ERR( dstStream.IsOpen() and srcStream.IsOpen() );
        CHECK_ERR( buffer.size > 0 and buffer.ptr != null );

        Bytes   copied;

        for (; copied < srcSize;)
        {
            Bytes   part_size   = Min( srcSize - copied, buffer.size );
            Bytes   readn       = srcStream.ReadSeq( OUT buffer.ptr, part_size );

            if_unlikely( readn == 0 )
                break;

            Bytes   written = dstStream.WriteSeq( buffer.ptr, readn );
            CHECK_ERR( readn == written );

            copied += written;
        }

        dstStream.Flush();
        return copied;
    }

    Bytes  DataSourceUtils::BufferedCopy (WStream &dstStream, RStream &srcStream, const Bytes srcSize) __NE___
    {
        ubyte   buffer [_BufferSize];
        return BufferedCopy( dstStream, srcStream, srcSize, TempBuffer{ buffer, Bytes{_BufferSize} });
    }

    Bytes  DataSourceUtils::BufferedCopy (WStream &dstStream, RStream &srcStream, const TempBuffer &buffer) __NE___
    {
        return BufferedCopy( dstStream, srcStream, UMax, buffer );
    }

    Bytes  DataSourceUtils::BufferedCopy (WStream &dstStream, RStream &srcStream) __NE___
    {
        return BufferedCopy( dstStream, srcStream, Bytes{UMax} );
    }

/*
=================================================
    BufferedCopy
=================================================
*/
    Bytes  DataSourceUtils::BufferedCopy (WStream &dstStream, RDataSource &srcDS, const Bytes srcOffset, const Bytes srcSize, const TempBuffer &buffer) __NE___
    {
        CHECK_ERR( dstStream.IsOpen() and srcDS.IsOpen() );
        CHECK_ERR( buffer.size > 0 and buffer.ptr != null );

        const Bytes align   = srcDS.OffsetAlign();
        Bytes       copied;
        Bytes       offset;

        for (; copied < srcSize;)
        {
            CHECK_ERR( IsAligned( srcOffset + offset, align ));

            Bytes   part_size   = Min( srcSize - copied, buffer.size );
            Bytes   readn       = srcDS.ReadBlock( srcOffset + offset, OUT buffer.ptr, part_size );

            if_unlikely( readn == 0 )
                break;

            offset += readn;

            Bytes   written = dstStream.WriteSeq( buffer.ptr, readn );
            CHECK_ERR( readn == written );

            copied += written;
        }

        dstStream.Flush();
        return copied;
    }

    Bytes  DataSourceUtils::BufferedCopy (WStream &dstStream, RDataSource &srcDS, const Bytes srcOffset, const Bytes srcSize) __NE___
    {
        ubyte   buffer [_BufferSize];
        return BufferedCopy( dstStream, srcDS, srcOffset, srcSize, TempBuffer{ buffer, Bytes{_BufferSize} });
    }

    Bytes  DataSourceUtils::BufferedCopy (WStream &dstStream, RDataSource &srcDS, const TempBuffer &buffer) __NE___
    {
        return BufferedCopy( dstStream, srcDS, 0_b, UMax, buffer );
    }

    Bytes  DataSourceUtils::BufferedCopy (WStream &dstStream, RDataSource &srcDS) __NE___
    {
        return BufferedCopy( dstStream, srcDS, 0_b, Bytes{UMax} );
    }

/*
=================================================
    Compare
=================================================
*/
namespace {
    static constexpr DataSourceUtils::CmpResult InvalidCmpResult {0_b, 1};
}

    template <bool Strict>
    DataSourceUtils::CmpResult  DataSourceUtils::_Compare (RStream &lhs, RStream &rhs, const Bytes size, const TempBuffer &lBuffer, const TempBuffer &rBuffer) __NE___
    {
        CHECK_ERR( lhs.IsOpen() and rhs.IsOpen(), InvalidCmpResult );
        CHECK_ERR( lBuffer.size > 0 and lBuffer.ptr != null, InvalidCmpResult );
        CHECK_ERR( rBuffer.size > 0 and rBuffer.ptr != null, InvalidCmpResult );

        const Bytes buf_size = Min( lBuffer.size, rBuffer.size );

        CmpResult   res;
        for (; res.processed < size; )
        {
            Bytes   part_size   = Min( size - res.processed, buf_size );
            Bytes   l_readn     = lhs.ReadSeq( OUT lBuffer.ptr, part_size );
            Bytes   r_readn     = rhs.ReadSeq( OUT rBuffer.ptr, part_size );
            Bytes   readn       = Min( l_readn, r_readn );

            res.diff        = std::memcmp( lBuffer.ptr, rBuffer.ptr, usize{readn} );
            res.processed   += readn;

            if_unlikely( res.diff != 0 )
            {
                if constexpr( Strict )
                {
                    usize   i = 0;
                    for (;; ++i)
                    {
                        if_unlikely( i >= usize{readn} or
                                     static_cast< const char *>(lBuffer.ptr)[i] != static_cast< const char *>(rBuffer.ptr)[i] )
                            break;
                    }
                    res.processed -= (readn - i);
                }
                break;
            }

            if_unlikely( l_readn == 0 or l_readn != r_readn )
            {
                res.diff = slong{l_readn} - slong{r_readn};
                break;
            }
        }
        return res;
    }

    DataSourceUtils::CmpResult  DataSourceUtils::Compare (RStream &lhs, RStream &rhs, const Bytes size, const TempBuffer &lBuffer, const TempBuffer &rBuffer) __NE___
    {
        return _Compare<false>( lhs, rhs, size, lBuffer, rBuffer );
    }

    DataSourceUtils::CmpResult  DataSourceUtils::Compare (RStream &lhs, RStream &rhs, const Bytes size) __NE___
    {
        ubyte   lbuffer [_BufferSize];
        ubyte   rbuffer [_BufferSize];

        return Compare( lhs, rhs, size, TempBuffer{ lbuffer, Bytes{_BufferSize} }, TempBuffer{ rbuffer, Bytes{_BufferSize} });
    }

    DataSourceUtils::CmpResult  DataSourceUtils::Compare (RStream &lhs, RStream &rhs) __NE___
    {
        return Compare( lhs, rhs, UMax );
    }

/*
=================================================
    CompareStrict
=================================================
*/
    DataSourceUtils::CmpResult  DataSourceUtils::CompareStrict (RStream &lhs, RStream &rhs, Bytes size, const TempBuffer &lBuffer, const TempBuffer &rBuffer) __NE___
    {
        return _Compare<true>( lhs, rhs, size, lBuffer, rBuffer );
    }

    DataSourceUtils::CmpResult  DataSourceUtils::CompareStrict (RStream &lhs, RStream &rhs, Bytes size) __NE___
    {
        ubyte   lbuffer [_BufferSize];
        ubyte   rbuffer [_BufferSize];

        return CompareStrict( lhs, rhs, size, TempBuffer{ lbuffer, Bytes{_BufferSize} }, TempBuffer{ rbuffer, Bytes{_BufferSize} });
    }

    DataSourceUtils::CmpResult  DataSourceUtils::CompareStrict (RStream &lhs, RStream &rhs) __NE___
    {
        return CompareStrict( lhs, rhs, UMax );
    }

} // AE::Base
