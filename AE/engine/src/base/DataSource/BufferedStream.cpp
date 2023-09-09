// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/BufferedStream.h"
#include "base/Algorithms/ArrayUtils.h"
#include "base/Math/Vec.h"

namespace AE::Base
{

/*
=================================================
    constructor
=================================================
*/
    BufferedRStream::BufferedRStream (RC<RStream> stream, Bytes bufferSize) __NE___ :
        _stream{ RVRef(stream) }
    {
        if ( not _stream ) return;

        CATCH( _data.reserve( usize(bufferSize) ));

        if ( IsOpen() )
            _ReadNext();
    }

/*
=================================================
    destructor
=================================================
*/
    BufferedRStream::~BufferedRStream () __NE___
    {
        // some data may not be readn, so rollback to last consumed data

        const bool  not_completelly_consumed    = _stream and (ArraySizeOf(_data) != _pos);
        const bool  allow_seek_set              = AllBits( _stream->GetSourceType(), ESourceType::RandomAccess );

        if ( not_completelly_consumed and allow_seek_set )
            CHECK( _stream->SeekSet( Position() ))
        else
            CHECK( not not_completelly_consumed );
    }

/*
=================================================
    GetSourceType
=================================================
*/
    RStream::ESourceType  BufferedRStream::GetSourceType () C_NE___
    {
        constexpr auto  mask = ESourceType::FixedSize;

        return  ESourceType::Buffered   | ESourceType::SequentialAccess |
                ESourceType::ReadAccess |
                (_stream->GetSourceType() & mask);
    }

/*
=================================================
    _ReadNext
=================================================
*/
    void  BufferedRStream::_ReadNext () __NE___
    {
        ASSERT( _pos == _data.size() );
        _pos = 0_b;

        _data.resize( _data.capacity() );   // should not throw

        Bytes   readn = _stream->ReadSeq( _data.data(), ArraySizeOf(_data) );

        ASSERT( usize(readn) <= _data.capacity() );
        _data.resize( usize(readn) );       // should not throw
    }

/*
=================================================
    PositionAndSize
=================================================
*/
    RStream::PosAndSize  BufferedRStream::PositionAndSize () C_NE___
    {
        ASSERT( IsOpen() );

        PosAndSize  pos_size = _stream->PositionAndSize();

        pos_size.pos = pos_size.pos - ArraySizeOf(_data) + _pos;

        return pos_size;
    }

/*
=================================================
    SeekFwd
=================================================
*/
    bool  BufferedRStream::SeekFwd (Bytes offset) __NE___
    {
        ASSERT( IsOpen() );

        Bytes   readn;
        for (;;)
        {
            if_unlikely( _pos == _data.size() )
                _ReadNext();

            Bytes   part_size = Min( offset - readn, ArraySizeOf(_data) - _pos );
            if ( part_size == 0 )
                break;

            readn   += part_size;
            _pos    += part_size;
        }
        return true;
    }

/*
=================================================
    ReadSeq
=================================================
*/
    Bytes  BufferedRStream::ReadSeq (OUT void *buffer, const Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        Bytes   readn;
        for (;;)
        {
            if_unlikely( _pos == _data.size() )
                _ReadNext();

            Bytes   part_size = Min( size - readn, ArraySizeOf(_data) - _pos );
            if ( part_size == 0 )
                break;

            MemCopy( OUT buffer, _data.data() + _pos, part_size );

            readn   += part_size;
            _pos    += part_size;
        }
        return readn;
    }

/*
=================================================
    UpdateFastStream
=================================================
*/
    void  BufferedRStream::UpdateFastStream (OUT const void* &begin, OUT const void* &end) __NE___
    {
        ASSERT( IsOpen() );
        ASSERT( begin == null or ( (begin == end) and (end == _data.data() + _data.size()) ));

        if_unlikely( begin != null )
            _ReadNext();

        _pos    = ArraySizeOf( _data );
        begin   = _data.data();
        end     = _data.data() + _pos;
    }

/*
=================================================
    EndFastStream
=================================================
*/
    void  BufferedRStream::EndFastStream (const void* ptr) __NE___
    {
        ASSERT( IsOpen() );

        if ( (ptr >= _data.data()) and (ptr <= _data.data() + _data.size()) )
        {
            _pos = Bytes{ptr} - Bytes{_data.data()};
        }
        else
        {
            ASSERT( ptr == null );
        }
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    BufferedWStream::BufferedWStream (RC<WStream> stream, Bytes bufferSize) __NE___ :
        _stream{ RVRef(stream) }
    {
        CATCH( _data.resize( usize(bufferSize) ));
    }

/*
=================================================
    destructor
=================================================
*/
    BufferedWStream::~BufferedWStream () __NE___
    {
        _Flush();
    }

/*
=================================================
    Position
=================================================
*/
    Bytes  BufferedWStream::Position () C_NE___
    {
        ASSERT( IsOpen() );
        return _stream->Position() + _pos;
    }

/*
=================================================
    Reserve
=================================================
*/
    Bytes  BufferedWStream::Reserve (Bytes) __NE___
    {
        // TODO
        return 0_b;
    }

/*
=================================================
    SeekFwd
=================================================
*/
    bool  BufferedWStream::SeekFwd (Bytes offset) __NE___
    {
        ASSERT( IsOpen() );

        _Flush();

        bool    result = true;

        if ( offset > 0_b )
        {
            _data.resize( Min( _data.capacity(), usize(offset) ));  // should not throw
            ZeroMem( OUT _data.data(), ArraySizeOf(_data) );

            for (; offset > 0_b; )
            {
                Bytes   part_size = Min( offset, ArraySizeOf(_data) );

                result &= _stream->Write( _data.data(), part_size );
                offset -= part_size;
            }
        }
        return result;
    }

/*
=================================================
    WriteSeq
=================================================
*/
    Bytes  BufferedWStream::WriteSeq (const void *buffer, const Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        Bytes   written;
        for (; written < size;)
        {
            if ( _pos == _data.size() )
                _Flush();

            Bytes   part_size = Min( size - written, ArraySizeOf(_data) - _pos );
            MemCopy( OUT _data.data() + _pos, buffer, part_size );

            written += part_size;
            _pos    += part_size;
        }
        return written;
    }

/*
=================================================
    Flush
=================================================
*/
    void  BufferedWStream::Flush () __NE___
    {
        ASSERT( IsOpen() );

        _Flush();
        _stream->Flush();
    }

    inline void  BufferedWStream::_Flush () __NE___
    {
        if ( _pos > 0 )
        {
            CHECK( _stream->Write( _data.data(), _pos ));
            _pos = 0_b;
        }
    }

/*
=================================================
    UpdateFastStream
=================================================
*/
    void  BufferedWStream::UpdateFastStream (OUT void* &begin, OUT const void* &end, Bytes reserve) __NE___
    {
        ASSERT( IsOpen() );
        ASSERT( begin == null or ( (begin >= _data.data()) and (end <= _data.data() + _data.size()) ));

        _pos = ( (begin >= _data.data()) & (begin <= _data.data() + _data.size()) ) ?
                Bytes{begin} - Bytes{_data.data()} :
                _pos;
        ASSERT( _pos <= _data.size() );

        _Flush();
        _pos = Min( ArraySizeOf(_data), reserve );

        begin   = _data.data();
        end     = _data.data() + _pos;
    }

/*
=================================================
    EndFastStream
=================================================
*/
    void  BufferedWStream::EndFastStream (const void* ptr) __NE___
    {
        ASSERT( IsOpen() );

        if ( (ptr >= _data.data() + _pos) and (ptr <= _data.data() + _data.size()) )
        {
            _pos = Bytes{ptr} - Bytes{_data.data()};
            _Flush();
        }
        else
        {
            ASSERT( ptr == null );
        }
    }


} // AE::Base
