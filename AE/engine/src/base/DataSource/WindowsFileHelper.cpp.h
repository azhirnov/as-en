// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS

namespace
{
    using RFileFlags    = WinRFileStream::EFlags;
    using WFileFlags    = WinWFileStream::EFlags;

/*
=================================================
    FileFlagCast (RFileFlags)
----
    https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
=================================================
*/
    ND_ static DWORD  FileFlagCast (RFileFlags values) __NE___
    {
        DWORD   flags = FILE_ATTRIBUTE_NORMAL;
        while ( values != Zero )
        {
            RFileFlags  t = ExtractBit( INOUT values );

            BEGIN_ENUM_CHECKS();
            switch ( t )
            {
                case RFileFlags::RandomAccess :     flags |= FILE_FLAG_RANDOM_ACCESS;   break;
                case RFileFlags::SequentialScan :   flags |= FILE_FLAG_SEQUENTIAL_SCAN; break;
                case RFileFlags::NoBuffering :      flags |= FILE_FLAG_NO_BUFFERING;    break;
                case RFileFlags::Unknown :
                default :                           RETURN_ERR( "unknown rfile open flag!", FILE_ATTRIBUTE_NORMAL );
            }
            END_ENUM_CHECKS();
        }

        // validation
        {
            DBG_CHECK_MSG( not AllBits( flags, FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_SEQUENTIAL_SCAN ),
                           "'RandomAccess | SequentialScan' is not supported" );
        }
        return flags;
    }

/*
=================================================
    FileFlagCast (WFileFlags)
=================================================
*/
    ND_ static DWORD  FileFlagCast (WFileFlags values) __NE___
    {
        DWORD   flags = FILE_ATTRIBUTE_NORMAL;
        while ( values != Zero )
        {
            WFileFlags  t = ExtractBit( INOUT values );

            BEGIN_ENUM_CHECKS();
            switch ( t )
            {
                case WFileFlags::NoBuffering :      flags |= FILE_FLAG_NO_BUFFERING;    break;
                case WFileFlags::NoCaching :        flags |= FILE_FLAG_WRITE_THROUGH;   break;

            //  case WFileFlags::OpenRewrite :
                case WFileFlags::OpenUpdate :
                case WFileFlags::Unknown :
                default :                           RETURN_ERR( "unknown wfile open flag!", FILE_ATTRIBUTE_NORMAL );
            }
            END_ENUM_CHECKS();
        }
        return flags;
    }

/*
=================================================
    OpenFileForRead
=================================================
*/
    ND_ static HANDLE  OpenFileForRead (const char* filename, RFileFlags flags, DWORD addFlags = 0) __NE___
    {
        return ::CreateFileA( filename,         // winxp
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              null,     // default security
                              OPEN_EXISTING,
                              FileFlagCast( flags ) | addFlags,
                              null );
    }

    ND_ static HANDLE  OpenFileForRead (const wchar_t* filename, RFileFlags flags, DWORD addFlags = 0) __NE___
    {
        return ::CreateFileW( filename,         // winxp
                              GENERIC_READ,
                              FILE_SHARE_READ,
                              null,     // default security
                              OPEN_EXISTING,
                              FileFlagCast( flags ) | addFlags,
                              null );
    }

/*
=================================================
    OpenFileForWrite
=================================================
*/
    ND_ static HANDLE  OpenFileForWrite2 (const char* filename, WFileFlags flags, DWORD addFlags) __NE___
    {
        return ::CreateFileA( filename,         // winxp
                              GENERIC_WRITE,
                              0,        // share mode
                              null,     // default security
                              AllBits( flags, WFileFlags::OpenUpdate ) ? OPEN_EXISTING : CREATE_ALWAYS,
                              FileFlagCast( flags ) | addFlags,
                              null );
    }

    ND_ static HANDLE  OpenFileForWrite2 (const wchar_t* filename, WFileFlags flags, DWORD addFlags) __NE___
    {
        return ::CreateFileW( filename,         // winxp
                              GENERIC_WRITE,
                              0,        // share mode
                              null,     // default security
                              AllBits( flags, WFileFlags::OpenUpdate ) ? OPEN_EXISTING : CREATE_ALWAYS,
                              FileFlagCast( flags ) | addFlags,
                              null );
    }

    template <typename T>
    ND_ static HANDLE  OpenFileForWrite (const T* filename, INOUT WFileFlags &flags, DWORD addFlags = 0) __NE___
    {
        HANDLE  file = OpenFileForWrite2( filename, flags, addFlags );

        if ( file == INVALID_HANDLE_VALUE and AllBits( flags, WFileFlags::OpenUpdate ))
        {
            flags   &= ~WFileFlags::OpenUpdate;
            //flags |= WFileFlags::OpenRewrite;

            file    = OpenFileForWrite2( filename, flags, addFlags );
        }
        return file;
    }

/*
=================================================
    GetFileSize
=================================================
*/
    ND_ static Bytes  GetFileSize (HANDLE file) __NE___
    {
        LARGE_INTEGER   size = {};

        if_likely(::GetFileSizeEx( file, OUT &size ) != FALSE ) // winxp
            return Bytes{ulong( size.QuadPart )};

        return 0_b;
    }

/*
=================================================
    GetPositionInFile
=================================================
*/
    ND_ inline slong  GetPositionInFile (HANDLE file) __NE___
    {
        LARGE_INTEGER   offset  = {};
        LARGE_INTEGER   current = {};

        if_likely( ::SetFilePointerEx( file, offset, OUT &current, FILE_CURRENT ) != FALSE )    // winxp
            return current.QuadPart;

        return 0;
    }

/*
=================================================
    SetOverlappedOffset
=================================================
*/
    inline void  SetOverlappedOffset (INOUT OVERLAPPED &ov, Bytes offset)
    {
        LARGE_INTEGER   li;
        li.QuadPart     = ulong(offset);

        ov.Offset       = li.LowPart;
        ov.OffsetHigh   = li.HighPart;
    }

/*
=================================================
    GetOverlappedOffset
=================================================
*/
    ND_ inline Bytes  GetOverlappedOffset (const OVERLAPPED &ov)
    {
        LARGE_INTEGER   li;
        li.LowPart      = ov.Offset;
        li.HighPart     = ov.OffsetHigh;

        return Bytes{ulong(li.QuadPart)};
    }


} // namespace

#endif // AE_PLATFORM_WINDOWS
