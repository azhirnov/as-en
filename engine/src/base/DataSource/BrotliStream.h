// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_BROTLI

#include "base/DataSource/Stream.h"

namespace AE::Base
{

    //
    // Read-only Brotli Uncompression Stream
    //

    class BrotliRStream final : public RStream
    {
    // variables
    private:
        RC<RStream>     _stream;
        void *          _instance   = null;     // BrotliDecoderState
        Bytes           _position;              // uncompressed size
        uint            _lastResult;

        static constexpr usize  _BufferSize = 4u << 10;


    // methods
    public:
        explicit BrotliRStream (RC<RStream> stream) __NE___;
        ~BrotliRStream ()                           __NE_OV;

    // RStream //
        bool        IsOpen ()                       C_NE_OV { return _instance != null and _stream and _stream->IsOpen(); }
        PosAndSize  PositionAndSize ()              C_NE_OV { return { _position, UMax }; }

        ESourceType GetSourceType ()                C_NE_OV { return ESourceType::SequentialAccess | ESourceType::ReadAccess; }

        bool        SeekFwd (Bytes offset)          __NE_OV;
        Bytes       ReadSeq (OUT void *, Bytes)     __NE_OV;
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
            float   quality         = 0.5f;
            float   windowBits      = 0.85f;    // window size: 1u << (10..24 * windowBits)     default: 22
            float   inBlockSize     = 0.0f;     // block size:  1u << (16..24 * inBlockSize)

            Config () {}
        };


    // variables
    private:
        RC<WStream>     _stream;
        void *          _instance   = null;     // BrotliEncoderState
        Bytes           _position;              // uncompressed size

        static constexpr usize  _BufferSize = 4u << 10;


    // methods
    public:
        explicit BrotliWStream (RC<WStream> stream, const Config &cfg = Default) __NE___;
        ~BrotliWStream ()                           __NE_OV;

        void    SetTotalSize (Bytes size)           __NE___;


    // WStream //
        bool        IsOpen ()                       C_NE_OV { return _instance != null and _stream and _stream->IsOpen(); }
        Bytes       Position ()                     C_NE_OV { return _position; }
        ESourceType GetSourceType ()                C_NE_OV { return ESourceType::SequentialAccess | ESourceType::WriteAccess; }

        Bytes       Reserve (Bytes additionalSize)  __NE_OV;

        bool        SeekFwd (Bytes)                 __NE_OV { return false; }
        void        Flush ()                        __NE_OV { _Flush(); }

        Bytes       WriteSeq (const void *, Bytes)  __NE_OV;

    private:
        bool        _Flush ()                       __NE___;
    };


} // AE::Base

#endif // AE_ENABLE_BROTLI
