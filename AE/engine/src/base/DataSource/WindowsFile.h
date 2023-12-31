// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Containers/UntypedStorage.h"
#include "base/DataSource/Stream.h"
#include "base/Containers/NtStringView.h"
#include "base/Utils/FileSystem.h"

#ifdef AE_PLATFORM_WINDOWS

namespace AE::Base
{

    //
    // Windows Read-only File
    //
    class WinRFileStream final : public RStream
    {
    // types
    public:
        enum class EFlags : uint
        {
            Unknown         = 0,
            RandomAccess    = 1 << 0,   // access is intended to be random
            SequentialScan  = 1 << 1,   // access is intended to be sequential from beginning to end
            NoBuffering     = 1 << 2,   // file or device is being opened with no system caching for data reads and writes
        };

    private:
        using Handle_t  = UntypedStorage< sizeof(void*), alignof(void*) >;

        static constexpr EFlags DefaultFlags    = EFlags::SequentialScan;


    // variables
    private:
        Handle_t        _file;
        const Bytes     _fileSize;
        const EFlags    _flags;

        DEBUG_ONLY( const Path  _filename;)


    // methods
    private:
        WinRFileStream (const Handle_t &file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___;

    public:
        explicit WinRFileStream (const char* filename, EFlags flags = DefaultFlags)     __NE___;
        explicit WinRFileStream (NtStringView filename, EFlags flags = DefaultFlags)    __NE___;
        explicit WinRFileStream (const String &filename, EFlags flags = DefaultFlags)   __NE___;

        explicit WinRFileStream (NtWStringView filename, EFlags flags = DefaultFlags)   __NE___;
        explicit WinRFileStream (const wchar_t* filename, EFlags flags = DefaultFlags)  __NE___;
        explicit WinRFileStream (const WString &filename, EFlags flags = DefaultFlags)  __NE___;

        explicit WinRFileStream (const Path &path, EFlags flags = DefaultFlags)         __NE___;

        ~WinRFileStream ()                                                              __NE_OV;


        // RStream //
        bool        IsOpen ()                                                           C_NE_OV;
        ESourceType GetSourceType ()                                                    C_NE_OV;
        PosAndSize  PositionAndSize ()                                                  C_NE_OV { return { _Position(), _fileSize }; }

        bool        SeekFwd (Bytes offset)                                              __NE_OV;
        bool        SeekSet (Bytes newPos)                                              __NE_OV;

        Bytes       ReadSeq (OUT void*, Bytes)                                          __NE_OV;

    private:
        ND_ Bytes  _Position ()                                                         C_NE___;
    };

    AE_BIT_OPERATORS( WinRFileStream::EFlags );



    //
    // Windows Write-only File
    //
    class WinWFileStream final : public WStream
    {
    // types
    public:
        enum class EFlags : uint
        {
            Unknown         = 0,
            NoBuffering     = 1 << 0,   // file or device is being opened with no system caching for data reads and writes
            NoCaching       = 1 << 1,   // write operations will not go through any intermediate cache, they will go directly to disk.

            //OpenRewrite   = 1 << 2,   // create new or discard previous file  // default
            OpenUpdate      = 1 << 3,   // keep previous content and update some parts in the file
        };

    private:
        using Handle_t  = UntypedStorage< sizeof(void*), alignof(void*) >;

        static constexpr EFlags DefaultFlags    = EFlags::Unknown;


    // variables
    private:
        Handle_t        _file;
        const EFlags    _flags;

        DEBUG_ONLY( const Path  _filename;)


    // methods
    private:
        WinWFileStream (const Handle_t &file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___;

    public:
        explicit WinWFileStream (const char*  filename, EFlags flags = DefaultFlags)    __NE___;
        explicit WinWFileStream (NtStringView filename, EFlags flags = DefaultFlags)    __NE___;
        explicit WinWFileStream (const String &filename, EFlags flags = DefaultFlags)   __NE___;

        explicit WinWFileStream (NtWStringView filename, EFlags flags = DefaultFlags)   __NE___;
        explicit WinWFileStream (const wchar_t* filename, EFlags flags = DefaultFlags)  __NE___;
        explicit WinWFileStream (const WString &filename, EFlags flags = DefaultFlags)  __NE___;

        explicit WinWFileStream (const Path &path, EFlags flags = DefaultFlags)         __NE___;

        ~WinWFileStream ()                                                              __NE_OV;


        // WStream //
        bool        IsOpen ()                                                           C_NE_OV;
        Bytes       Position ()                                                         C_NE_OV;
        ESourceType GetSourceType ()                                                    C_NE_OV;

        bool        SeekFwd (Bytes offset)                                              __NE_OV;
        Bytes       Reserve (Bytes additionalSize)                                      __NE_OV;

        Bytes       WriteSeq (const void*, Bytes)                                       __NE_OV;
        void        Flush ()                                                            __NE_OV;
    };

    AE_BIT_OPERATORS( WinWFileStream::EFlags );
//-----------------------------------------------------------------------------



    //
    // Read-only File Data Source
    //

    class WinRFileDataSource final : public RDataSource
    {
    // types
    public:
        using EFlags = WinRFileStream::EFlags;

    private:
        using Handle_t  = UntypedStorage< sizeof(void*), alignof(void*) >;

        static constexpr EFlags DefaultFlags    = EFlags::RandomAccess;


    // variables
    private:
        Handle_t        _file;
        Bytes const     _fileSize;
        const EFlags    _flags;

        DEBUG_ONLY( const Path  _filename;)


    // methods
    private:
        WinRFileDataSource (const Handle_t &file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___;

    public:
        explicit WinRFileDataSource (const char* filename, EFlags flags = DefaultFlags)     __NE___;
        explicit WinRFileDataSource (NtStringView filename, EFlags flags = DefaultFlags)    __NE___;
        explicit WinRFileDataSource (const String &filename, EFlags flags = DefaultFlags)   __NE___;

        explicit WinRFileDataSource (NtWStringView filename, EFlags flags = DefaultFlags)   __NE___;
        explicit WinRFileDataSource (const wchar_t* filename, EFlags flags = DefaultFlags)  __NE___;
        explicit WinRFileDataSource (const WString &filename, EFlags flags = DefaultFlags)  __NE___;

        explicit WinRFileDataSource (const Path &path, EFlags flags = DefaultFlags)         __NE___;

        ~WinRFileDataSource ()                                                              __NE_OV;


        // RDataSource //
        bool        IsOpen ()                                                               C_NE_OV;
        ESourceType GetSourceType ()                                                        C_NE_OV;
        Bytes       Size ()                                                                 C_NE_OV { return _fileSize; }

        Bytes       ReadBlock (Bytes, OUT void*, Bytes)                                     __NE_OV;
    };



    //
    // Write-only File Data Source
    //

    class WinWFileDataSource final : public WDataSource
    {
    // types
    public:
        using EFlags = WinWFileStream::EFlags;

    private:
        using Handle_t  = UntypedStorage< sizeof(void*), alignof(void*) >;

        static constexpr EFlags DefaultFlags    = EFlags::Unknown;


    // variables
    private:
        Handle_t        _file;
        const EFlags    _flags;

        DEBUG_ONLY( const Path  _filename;)


    // methods
    private:
        WinWFileDataSource (const Handle_t &file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___;

    public:
        explicit WinWFileDataSource (const char* filename, EFlags flags = DefaultFlags)     __NE___;
        explicit WinWFileDataSource (NtStringView filename, EFlags flags = DefaultFlags)    __NE___;
        explicit WinWFileDataSource (const String &filename, EFlags flags = DefaultFlags)   __NE___;

        explicit WinWFileDataSource (NtWStringView filename, EFlags flags = DefaultFlags)   __NE___;
        explicit WinWFileDataSource (const wchar_t* filename, EFlags flags = DefaultFlags)  __NE___;
        explicit WinWFileDataSource (const WString &filename, EFlags flags = DefaultFlags)  __NE___;

        explicit WinWFileDataSource (const Path &path, EFlags flags = DefaultFlags)         __NE___;

        ~WinWFileDataSource ()                                                              __NE_OV;


        // WStream //
        bool        IsOpen ()                                                               C_NE_OV;
        ESourceType GetSourceType ()                                                        C_NE_OV;
        Bytes       Capacity ()                                                             C_NE_OV;

        Bytes       Reserve (Bytes)                                                         __NE_OV;

        Bytes       WriteBlock (Bytes, const void*, Bytes)                                  __NE_OV;
        void        Flush ()                                                                __NE_OV;
    };


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
