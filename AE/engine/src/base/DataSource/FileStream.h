// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    TODO:
        setbuf, setvbuf 
*/

#pragma once

#include "base/DataSource/Stream.h"
#include "base/Containers/NtStringView.h"
#include "base/Utils/FileSystem.h"

namespace AE::Base
{

    //
    // Read-only File Stream
    //

    class FileRStream final : public RStream
    {
        friend class FileRDataSource;

    // variables
    private:
        FILE*           _file       = null;
        Bytes const     _fileSize;
        Bytes           _position;

        DEBUG_ONLY( const Path  _filename;)


    // methods
    private:
        explicit FileRStream (FILE* file DEBUG_ONLY(, Path filename)) __NE___;

    public:
        explicit FileRStream (const char* filename)     __NE___;
        explicit FileRStream (NtStringView filename)    __NE___;
        explicit FileRStream (const String &filename)   __NE___;
        explicit FileRStream (const Path &path)         __NE___;

    #ifdef AE_PLATFORM_WINDOWS
        explicit FileRStream (NtWStringView filename)   __NE___;
        explicit FileRStream (const wchar_t* filename)  __NE___;
        explicit FileRStream (const WString &filename)  __NE___;
    #endif

        ~FileRStream ()                         __NE_OV;


        // RStream //
        bool        IsOpen ()                   C_NE_OV { return _file != null; }
        PosAndSize  PositionAndSize ()          C_NE_OV { return { _position, _fileSize }; }
        ESourceType GetSourceType ()            C_NE_OV;

        bool        SeekSet (Bytes pos)         __NE_OV;    // require ESourceType::RandomAccess
        bool        SeekFwd (Bytes offset)      __NE_OV;

        Bytes       ReadSeq (OUT void *, Bytes) __NE_OV;

        RC<RDataSource>  AsRDataSource ();
    };



    //
    // Write-only File Stream
    //

    class FileWStream final : public WStream
    {
        friend class FileWDataSource;

    // types
    public:
        enum class EMode
        {
            Rewrite,    // create new or discard previous file
            Update,     // keep previous content and update some parts in the file
            Append,     // write data to the end of the file
        };


    // variables
    private:
        FILE*       _file   = null;

        DEBUG_ONLY( const Path  _filename;)


    // methods
    private:
        explicit FileWStream (FILE* file DEBUG_ONLY(, Path filename))               __NE___;

    public:
        explicit FileWStream (const char* filename, EMode mode = EMode::Rewrite)    __NE___;
        explicit FileWStream (NtStringView filename, EMode mode = EMode::Rewrite)   __NE___;
        explicit FileWStream (const String &filename, EMode mode = EMode::Rewrite)  __NE___;

        explicit FileWStream (const Path &path, EMode mode = EMode::Rewrite)        __NE___;

    #ifdef AE_PLATFORM_WINDOWS
        explicit FileWStream (NtWStringView filename, EMode mode = EMode::Rewrite)  __NE___;
        explicit FileWStream (const wchar_t* filename, EMode mode = EMode::Rewrite) __NE___;
        explicit FileWStream (const WString &filename, EMode mode = EMode::Rewrite) __NE___;
    #endif

        ~FileWStream ()                                                             __NE_OV;


        // WStream //
        bool        IsOpen ()                                                       C_NE_OV { return _file != null; }
        Bytes       Position ()                                                     C_NE_OV;
        ESourceType GetSourceType ()                                                C_NE_OV;

        bool        SeekFwd (Bytes offset)                                          __NE_OV;
        Bytes       Reserve (Bytes)                                                 __NE_OV { return 0_b; }

        Bytes       WriteSeq (const void *buffer, Bytes size)                       __NE_OV;
        void        Flush ()                                                        __NE_OV;
    };
//-----------------------------------------------------------------------------



    //
    // Read-only File Data Source
    //

    class FileRDataSource final : public RDataSource
    {
        friend class FileRStream;

    // variables
    private:
        FILE*           _file       = null;
        Bytes const     _fileSize;
        Bytes           _lastPos;

        DEBUG_ONLY( const Path  _filename;)


    // methods
    private:
        explicit FileRDataSource (FILE* file DEBUG_ONLY(, Path filename))           __NE___;

    public:
        explicit FileRDataSource (const char* filename)                             __NE___;
        explicit FileRDataSource (NtStringView filename)                            __NE___;
        explicit FileRDataSource (const String &filename)                           __NE___;

        explicit FileRDataSource (const Path &path)                                 __NE___;

    #ifdef AE_PLATFORM_WINDOWS
        explicit FileRDataSource (NtWStringView filename)                           __NE___;
        explicit FileRDataSource (const wchar_t* filename)                          __NE___;
        explicit FileRDataSource (const WString &filename)                          __NE___;
    #endif

        ~FileRDataSource ()                                                         __NE_OV;


        // RDataSource //
        bool        IsOpen ()                                                       C_NE_OV { return _file != null; }
        ESourceType GetSourceType ()                                                C_NE_OV;
        Bytes       Size ()                                                         C_NE_OV { return _fileSize; }

        Bytes       ReadBlock (Bytes offset, OUT void *buffer, Bytes size)          __NE_OV;
    };



    //
    // Write-only File Data Source
    //

    class FileWDataSource final : public WDataSource
    {
        friend class FileWStream;

    // types
    public:
        using EMode = FileWStream::EMode;


    // variables
    private:
        FILE*       _file       = null;
        Bytes       _lastPos;

        DEBUG_ONLY( const Path  _filename;)


    // methods
    private:
        explicit FileWDataSource (FILE* file DEBUG_ONLY(, Path filename))               __NE___;

    public:
        explicit FileWDataSource (const char* filename, EMode mode = EMode::Rewrite)    __NE___;
        explicit FileWDataSource (NtStringView filename, EMode mode = EMode::Rewrite)   __NE___;
        explicit FileWDataSource (const String &filename, EMode mode = EMode::Rewrite)  __NE___;

        explicit FileWDataSource (const Path &path, EMode mode = EMode::Rewrite)        __NE___;

    #ifdef AE_PLATFORM_WINDOWS
        explicit FileWDataSource (NtWStringView filename, EMode mode = EMode::Rewrite)  __NE___;
        explicit FileWDataSource (const wchar_t* filename, EMode mode = EMode::Rewrite) __NE___;
        explicit FileWDataSource (const WString &filename, EMode mode = EMode::Rewrite) __NE___;
    #endif

        ~FileWDataSource ()                                                             __NE_OV;


        // WStream //
        bool        IsOpen ()                                                           C_NE_OV { return _file != null; }
        ESourceType GetSourceType ()                                                    C_NE_OV;
        Bytes       Capacity ()                                                         C_NE_OV;

        Bytes       Reserve (Bytes)                                                     __NE_OV { return UMax; }

        Bytes       WriteBlock (Bytes offset, const void *buffer, Bytes size)           __NE_OV;
        void        Flush ()                                                            __NE_OV;
    };


} // AE::Base


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

# ifdef _FILE_OFFSET_BITS
#  if _FILE_OFFSET_BITS == 64
#   pragma detect_mismatch( "_FILE_OFFSET_BITS", "64" )
#  else
#   pragma detect_mismatch( "_FILE_OFFSET_BITS", "32" )
#  endif
# endif

#endif // AE_CPP_DETECT_MISMATCH
