// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.h"
# include "base/Platforms/WindowsUtils.h"
# include "base/Algorithms/StringUtils.h"
# include "base/DataSource/WindowsFile.h"

namespace AE::Base
{
#   include "base/DataSource/WindowsFileHelper.cpp.h"

/*
=================================================
    constructor
=================================================
*/
    WinRFileStream::WinRFileStream (const Handle_t &file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file.Ref<HANDLE>() },
        _fileSize{ GetFileSize( _file.Ref<HANDLE>() )},
        _flags{ flags }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    WinRFileStream::WinRFileStream (const char* filename, EFlags flags)     __NE___ :
        WinRFileStream{ Handle_t{OpenFileForRead( filename, flags )}, flags DEBUG_ONLY(, filename )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    WinRFileStream::WinRFileStream (const wchar_t* filename, EFlags flags)  __NE___ :
        WinRFileStream{ Handle_t{OpenFileForRead( filename, flags )}, flags DEBUG_ONLY(, filename )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << ToString(filename) << "\": " );
    }

    WinRFileStream::WinRFileStream (NtStringView filename, EFlags flags)    __NE___ : WinRFileStream{ filename.c_str(), flags } {}
    WinRFileStream::WinRFileStream (const String &filename, EFlags flags)   __NE___ : WinRFileStream{ filename.c_str(), flags } {}

    WinRFileStream::WinRFileStream (NtWStringView filename, EFlags flags)   __NE___ : WinRFileStream{ filename.c_str(), flags } {}
    WinRFileStream::WinRFileStream (const WString &filename, EFlags flags)  __NE___ : WinRFileStream{ filename.c_str(), flags } {}

    WinRFileStream::WinRFileStream (const Path &path, EFlags flags)         __NE___ : WinRFileStream{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
    WinRFileStream::~WinRFileStream () __NE___
    {
        if ( IsOpen() )
            ::CloseHandle( _file.Ref<HANDLE>() );
    }

/*
=================================================
    IsOpen
=================================================
*/
    bool  WinRFileStream::IsOpen () C_NE___
    {
        return _file.Ref<HANDLE>() != INVALID_HANDLE_VALUE;
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  WinRFileStream::GetSourceType () C_NE___
    {
        return  (AllBits( _flags, EFlags::SequentialScan )  ? ESourceType::SequentialAccess : ESourceType::Unknown) |
                (AllBits( _flags, EFlags::RandomAccess )    ? ESourceType::RandomAccess     : ESourceType::Unknown) |
                ESourceType::FixedSize | ESourceType::ReadAccess;
    }

/*
=================================================
    _Position
=================================================
*/
    Bytes  WinRFileStream::_Position () C_NE___
    {
        ASSERT( IsOpen() );
        return Bytes{ulong( GetPositionInFile( _file.Ref<HANDLE>() ))};
    }

/*
=================================================
    SeekSet
=================================================
*/
    bool  WinRFileStream::SeekSet (Bytes newPos) __NE___
    {
        ASSERT( IsOpen() );

        LARGE_INTEGER   info;
        info.QuadPart = slong(newPos);

        return ::SetFilePointerEx( _file.Ref<HANDLE>(), info, null, FILE_BEGIN ) != FALSE;  // winxp
    }

/*
=================================================
    SeekFwd
=================================================
*/
    bool  WinRFileStream::SeekFwd (Bytes offset) __NE___
    {
        ASSERT( IsOpen() );

        LARGE_INTEGER   info;
        info.QuadPart = slong(offset);

        return ::SetFilePointerEx( _file.Ref<HANDLE>(), info, null, FILE_CURRENT ) != FALSE;    // winxp
    }

/*
=================================================
    ReadSeq
=================================================
*/
    Bytes  WinRFileStream::ReadSeq (OUT void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );
        ASSERT_Lt( size, MaxValue<DWORD>() );

        DWORD   readn = 0;
        if_likely( ::ReadFile( _file.Ref<HANDLE>(), OUT buffer, DWORD(size), OUT &readn, null ) != FALSE )
            return Bytes{readn};

        return 0_b;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    WinWFileStream::WinWFileStream (const Handle_t &file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file.Ref<HANDLE>() },
        _flags{ flags }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    WinWFileStream::WinWFileStream (const char* filename, EFlags flags)     __NE___ :
        WinWFileStream{ Handle_t{OpenFileForWrite( filename, INOUT flags )}, flags DEBUG_ONLY(, Path{filename} )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    WinWFileStream::WinWFileStream (const wchar_t *filename, EFlags flags)  __NE___ :
        WinWFileStream{ Handle_t{OpenFileForWrite( filename, INOUT flags )}, flags DEBUG_ONLY(, Path{filename} )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << ToAnsiString<char>(filename) << "\": " );
    }

    WinWFileStream::WinWFileStream (NtStringView filename, EFlags flags)    __NE___ : WinWFileStream{ filename.c_str(), flags } {}
    WinWFileStream::WinWFileStream (const String &filename, EFlags flags)   __NE___ : WinWFileStream{ filename.c_str(), flags } {}

    WinWFileStream::WinWFileStream (NtWStringView filename, EFlags flags)   __NE___ : WinWFileStream{ filename.c_str(), flags } {}
    WinWFileStream::WinWFileStream (const WString &filename, EFlags flags)  __NE___ : WinWFileStream{ filename.c_str(), flags } {}

    WinWFileStream::WinWFileStream (const Path &path, EFlags flags)         __NE___ : WinWFileStream{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
    WinWFileStream::~WinWFileStream ()
    {
        if ( IsOpen() )
            ::CloseHandle( _file.Ref<HANDLE>() );
    }

/*
=================================================
    IsOpen
=================================================
*/
    bool  WinWFileStream::IsOpen () C_NE___
    {
        return _file.Ref<HANDLE>() != INVALID_HANDLE_VALUE;
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  WinWFileStream::GetSourceType () C_NE___
    {
        return  ESourceType::SequentialAccess | ESourceType::WriteAccess;
    }

/*
=================================================
    Reserve
=================================================
*/
    Bytes  WinWFileStream::Reserve (Bytes additionalSize) __NE___
    {
        ASSERT( IsOpen() );

        const Bytes     cur_size = GetFileSize( _file.Ref<HANDLE>() );

        LARGE_INTEGER   req_pos;
        LARGE_INTEGER   cur_pos = {};

        req_pos.QuadPart = ulong(cur_size + additionalSize);

        if_unlikely( ::SetFilePointerEx( _file.Ref<HANDLE>(), req_pos, OUT &cur_pos, FILE_BEGIN ) == FALSE )    // winxp
            return cur_size;  // error

        if_unlikely( ::SetEndOfFile( _file.Ref<HANDLE>() ) == FALSE )   // winxp
            return cur_size;  // error

        return Bytes{ulong(cur_pos.QuadPart)};  // ok
    }

/*
=================================================
    Position
=================================================
*/
    Bytes  WinWFileStream::Position () C_NE___
    {
        ASSERT( IsOpen() );
        return Bytes{ulong( GetPositionInFile( _file.Ref<HANDLE>() ))};
    }

/*
=================================================
    Size
=================================================
*
    Bytes  WinWFileStream::Size () const
    {
        LARGE_INTEGER   size = {};

        if ( ::GetFileSizeEx( _file.Ref<HANDLE>(), OUT &size ) != FALSE )   // winxp
            return Bytes{ulong( size.QuadPart )};

        return 0_b;
    }

/*
=================================================
    SeekFwd
=================================================
*/
    bool  WinWFileStream::SeekFwd (Bytes offset) __NE___
    {
        ASSERT( IsOpen() );

        LARGE_INTEGER   info;
        info.QuadPart = slong(offset);

        return ::SetFilePointerEx( _file.Ref<HANDLE>(), info, null, FILE_CURRENT ) != FALSE;    // winxp
    }

/*
=================================================
    WriteSeq
=================================================
*/
    Bytes  WinWFileStream::WriteSeq (const void *buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );
        ASSERT_Lt( size, MaxValue<DWORD>() );

        DWORD   written = 0;
        if_likely( ::WriteFile( _file.Ref<HANDLE>(), buffer, DWORD(size), OUT &written, null ) != FALSE )
            return Bytes{written};

        return 0_b;
    }

/*
=================================================
    Flush
=================================================
*/
    void  WinWFileStream::Flush () __NE___
    {
        ASSERT( IsOpen() );

        ::FlushFileBuffers( _file.Ref<HANDLE>() );  // winxp
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    WinRFileDataSource::WinRFileDataSource (const Handle_t &file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file.Ref<HANDLE>() },
        _fileSize{ GetFileSize( _file.Ref<HANDLE>() )},
        _flags{ flags }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    WinRFileDataSource::WinRFileDataSource (NtStringView filename, EFlags flags)    __NE___ : WinRFileDataSource{ filename.c_str(), flags } {}
    WinRFileDataSource::WinRFileDataSource (const String &filename, EFlags flags)   __NE___ : WinRFileDataSource{ filename.c_str(), flags } {}
    WinRFileDataSource::WinRFileDataSource (const char* filename, EFlags flags)     __NE___ :
        WinRFileDataSource{ Handle_t{OpenFileForRead( filename, flags, FILE_FLAG_OVERLAPPED )}, flags DEBUG_ONLY(, filename )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    WinRFileDataSource::WinRFileDataSource (NtWStringView filename, EFlags flags)   __NE___ : WinRFileDataSource{ filename.c_str(), flags } {}
    WinRFileDataSource::WinRFileDataSource (const WString &filename, EFlags flags)  __NE___ : WinRFileDataSource{ filename.c_str(), flags } {}
    WinRFileDataSource::WinRFileDataSource (const wchar_t* filename, EFlags flags)  __NE___ :
        WinRFileDataSource{ Handle_t{OpenFileForRead( filename, flags, FILE_FLAG_OVERLAPPED )}, flags DEBUG_ONLY(, filename )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << ToString(filename) << "\": " );
    }

    WinRFileDataSource::WinRFileDataSource (const Path &path, EFlags flags)         __NE___ : WinRFileDataSource{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
    WinRFileDataSource::~WinRFileDataSource () __NE___
    {
        if ( IsOpen() )
            ::CloseHandle( _file.Ref<HANDLE>() );
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  WinRFileDataSource::GetSourceType () C_NE___
    {
        return  (AllBits( _flags, EFlags::SequentialScan )  ? ESourceType::SequentialAccess : ESourceType::Unknown) |
                (AllBits( _flags, EFlags::RandomAccess )    ? ESourceType::RandomAccess     : ESourceType::Unknown) |
                ESourceType::FixedSize | ESourceType::ReadAccess | ESourceType::ThreadSafe;
    }

/*
=================================================
    IsOpen
=================================================
*/
    bool  WinRFileDataSource::IsOpen () C_NE___
    {
        return _file.Ref<HANDLE>() != INVALID_HANDLE_VALUE;
    }

/*
=================================================
    ReadBlock
----
    based on:
    https://learn.microsoft.com/en-us/troubleshoot/windows/win32/asynchronous-disk-io-synchronous
=================================================
*/
    Bytes  WinRFileDataSource::ReadBlock (Bytes offset, OUT void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );
        ASSERT_Lt( size, MaxValue<DWORD>() );

        OVERLAPPED  ov = {};
        SetOverlappedOffset( INOUT ov, offset );

        DWORD   readn = 0;
        if_likely( ::ReadFile( _file.Ref<HANDLE>(), OUT buffer, DWORD(size), OUT &readn, INOUT &ov ) != FALSE )
            return Bytes{readn};  // ok

        auto    err = ::GetLastError();
        if_unlikely( err != ERROR_IO_PENDING )
        {
            WIN_CHECK_DEV2( err, "ReadFile failed: " );
            return 0_b;  // error
        }

        // wait for completion
        if_likely( ::GetOverlappedResult( _file.Ref<HANDLE>(), &ov, OUT &readn, TRUE ) != FALSE )   // winxp
        {
            return Bytes{readn};  // ok
        }
        else
        {
            WIN_CHECK_DEV( "GetOverlappedResult failed: " );
            return 0_b;  // error
        }
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    WinWFileDataSource::WinWFileDataSource (const Handle_t &file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file.Ref<HANDLE>() },
        _flags{ flags }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    WinWFileDataSource::WinWFileDataSource (NtStringView filename, EFlags flags)    __NE___ : WinWFileDataSource{ filename.c_str(), flags } {}
    WinWFileDataSource::WinWFileDataSource (const String &filename, EFlags flags)   __NE___ : WinWFileDataSource{ filename.c_str(), flags } {}
    WinWFileDataSource::WinWFileDataSource (const char* filename, EFlags flags)     __NE___ :
        WinWFileDataSource{ Handle_t{OpenFileForWrite( filename, INOUT flags, FILE_FLAG_OVERLAPPED )}, flags DEBUG_ONLY(, Path{filename} )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    WinWFileDataSource::WinWFileDataSource (NtWStringView filename, EFlags flags)   __NE___ : WinWFileDataSource{ filename.c_str(), flags } {}
    WinWFileDataSource::WinWFileDataSource (const WString &filename, EFlags flags)  __NE___ : WinWFileDataSource{ filename.c_str(), flags } {}
    WinWFileDataSource::WinWFileDataSource (const wchar_t* filename, EFlags flags)  __NE___ :
        WinWFileDataSource{ Handle_t{OpenFileForWrite( filename, INOUT flags, FILE_FLAG_OVERLAPPED )}, flags DEBUG_ONLY(, Path{filename} )}
    {
        if_unlikely( not IsOpen() )
            WIN_CHECK_DEV( "Can't open file: \""s << ToString(filename) << "\": " );
    }

    WinWFileDataSource::WinWFileDataSource (const Path &path, EFlags flags)         __NE___ : WinWFileDataSource{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
    WinWFileDataSource::~WinWFileDataSource ()
    {
        if ( IsOpen() )
            ::CloseHandle( _file.Ref<HANDLE>() );
    }

/*
=================================================
    IsOpen
=================================================
*/
    bool  WinWFileDataSource::IsOpen () C_NE___
    {
        return _file.Ref<HANDLE>() != INVALID_HANDLE_VALUE;
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  WinWFileDataSource::GetSourceType () C_NE___
    {
        return  ESourceType::RandomAccess | ESourceType::WriteAccess | ESourceType::ThreadSafe;
    }

/*
=================================================
    Capacity
=================================================
*/
    Bytes  WinWFileDataSource::Capacity () C_NE___
    {
        return GetFileSize( _file.Ref<HANDLE>() );
    }

/*
=================================================
    Reserve
=================================================
*/
    Bytes  WinWFileDataSource::Reserve (Bytes newSize) __NE___
    {
        ASSERT( IsOpen() );

        const Bytes     cur_size = GetFileSize( _file.Ref<HANDLE>() );

        if ( newSize <= cur_size )
            return cur_size;

        LARGE_INTEGER   req_pos;
        LARGE_INTEGER   cur_pos = {};

        req_pos.QuadPart = ulong(newSize);

        if_unlikely( ::SetFilePointerEx( _file.Ref<HANDLE>(), req_pos, OUT &cur_pos, FILE_BEGIN ) == FALSE )    // winxp
            return cur_size;  // error

        if_unlikely( ::SetEndOfFile( _file.Ref<HANDLE>() ) == FALSE )   // winxp
            return cur_size;  // error

        return Bytes{ulong(cur_pos.QuadPart)};  // ok
    }

/*
=================================================
    WriteBlock
=================================================
*/
    Bytes  WinWFileDataSource::WriteBlock (Bytes offset, const void *buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );
        ASSERT_Lt( size, MaxValue<DWORD>() );

        OVERLAPPED  ov = {};
        SetOverlappedOffset( INOUT ov, offset );

        DWORD   written = 0;
        if_likely( ::WriteFile( _file.Ref<HANDLE>(), buffer, DWORD(size), OUT &written, INOUT &ov ) != FALSE )
            return Bytes{written};  // ok

        auto    err = ::GetLastError();
        if_unlikely( err != ERROR_IO_PENDING )
        {
            WIN_CHECK_DEV2( err, "WriteFile failed: " );
            return 0_b;  // error
        }

        // wait for completion
        if_likely( ::GetOverlappedResult( _file.Ref<HANDLE>(), &ov, OUT &written, TRUE ) != FALSE ) // winxp
        {
            return Bytes{written};  // ok
        }
        else
        {
            WIN_CHECK_DEV( "GetOverlappedResult failed: " );
            return 0_b;  // error
        }
    }

/*
=================================================
    Flush
=================================================
*/
    void  WinWFileDataSource::Flush () __NE___
    {
        ASSERT( IsOpen() );

        ::FlushFileBuffers( _file.Ref<HANDLE>() );  // winxp
    }


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
