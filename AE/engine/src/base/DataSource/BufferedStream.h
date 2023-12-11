// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Requirements:
     * src stream must be unique
     * only sequential access
*/

#pragma once

#include "base/DataSource/Stream.h"

namespace AE::Base
{

    //
    // Buffered Read-only Stream
    //

    class BufferedRStream final : public RStream
    {
    // variables
    private:
        Array<ubyte>    _data;
        Bytes           _pos;       // position inside '_data'
        RC<RStream>     _stream;


    // methods
    public:
        explicit BufferedRStream (RC<RStream> stream,
                                  Bytes bufferSize = DefaultAllocationSize)     __NE___;
        ~BufferedRStream ()                                                     __NE___;


        // RStream //
        bool        IsOpen ()                                                   C_NE_OV { return _stream and _stream->IsOpen() and _data.capacity() > 0; }
        PosAndSize  PositionAndSize ()                                          C_NE_OV;

        ESourceType GetSourceType ()                                            C_NE_OV;

        bool    SeekFwd (Bytes offset)                                          __NE_OV;
        Bytes   ReadSeq (OUT void* buffer, Bytes size)                          __NE_OV;

        void    UpdateFastStream (OUT const void* &begin, OUT const void* &end) __NE_OV;
        void    EndFastStream (const void* ptr)                                 __NE_OV;


    private:
        void    _ReadNext ()                                                    __NE___;
    };



    //
    // Buffered Write-only Stream
    //

    class BufferedWStream final : public WStream
    {
    // variables
    private:
        Array<ubyte>    _data;
        Bytes           _pos;       // position inside '_data'
        RC<WStream>     _stream;


    // methods
    public:
        explicit BufferedWStream (RC<WStream> stream,
                                  Bytes bufferSize = DefaultAllocationSize)     __NE___;
        ~BufferedWStream ()                                                     __NE___;

        bool        IsOpen ()                                                   C_NE_OV { return _stream and _stream->IsOpen() and _data.capacity() > 0; }
        Bytes       Position ()                                                 C_NE_OV;
        ESourceType GetSourceType ()                                            C_NE_OV { return ESourceType::Buffered | ESourceType::SequentialAccess | ESourceType::WriteAccess; }

        bool    SeekFwd (Bytes offset)                                          __NE_OV;
        Bytes   WriteSeq (const void* buffer, Bytes size)                       __NE_OV;

        void    Flush ()                                                        __NE_OV;

        void    UpdateFastStream (OUT void* &begin, OUT const void* &end,
                                  Bytes reserve = DefaultAllocationSize)        __NE_OV;
        void    EndFastStream (const void* ptr)                                 __NE_OV;

    private:
        void    _Flush ()                                                       __NE___;
    };


} // AE::Base
