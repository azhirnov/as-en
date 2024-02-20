// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_UNIX_BASED
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>

# include "base/Algorithms/StringUtils.h"
# include "base/DataSource/UnixFile.h"

namespace AE::Base
{
#   include "base/DataSource/UnixFileHelper.cpp.h"

/*
=================================================
    constructor
=================================================
*/
    UnixFileRStream::UnixFileRStream (Handle_t file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file },
        _fileSize{ GetFileSize( _file )},
        _flags{ flags }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    UnixFileRStream::UnixFileRStream (const char* filename, EFlags flags)   __NE___ :
        UnixFileRStream{ Handle_t{OpenFileForRead( filename, flags )}, flags DEBUG_ONLY(, filename )}
    {
        if_unlikely( not IsOpen() )
            UNIX_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    UnixFileRStream::UnixFileRStream (NtStringView filename, EFlags flags)  __NE___ : UnixFileRStream{ filename.c_str(), flags } {}
    UnixFileRStream::UnixFileRStream (const String &filename, EFlags flags) __NE___ : UnixFileRStream{ filename.c_str(), flags } {}
    UnixFileRStream::UnixFileRStream (const Path &path, EFlags flags)       __NE___ : UnixFileRStream{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
    UnixFileRStream::~UnixFileRStream () __NE___
    {
        if ( IsOpen() )
            ::close( _file );
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  UnixFileRStream::GetSourceType () C_NE___
    {
        return  (AllBits( _flags, EFlags::SequentialScan )  ? ESourceType::SequentialAccess : ESourceType::Unknown) |
                (AllBits( _flags, EFlags::RandomAccess )    ? ESourceType::RandomAccess     : ESourceType::Unknown) |
                ESourceType::FixedSize | ESourceType::ReadAccess
            #ifndef AE_PLATFORM_APPLE
                | ESourceType::Prefetch
            #endif
                ;
    }

/*
=================================================
    _Position
=================================================
*/
    Bytes  UnixFileRStream::_Position () C_NE___
    {
        ASSERT( IsOpen() );
        return Bytes{ulong( Max( 0, GetPositionInFile( _file )) )};
    }

/*
=================================================
    SeekSet
=================================================
*/
    bool  UnixFileRStream::SeekSet (Bytes newPos) __NE___
    {
        ASSERT( IsOpen() );
        return SetPositionInFileFromBegin( _file, slong(newPos) );
    }

/*
=================================================
    SeekFwd
=================================================
*/
    bool  UnixFileRStream::SeekFwd (Bytes offset) __NE___
    {
        ASSERT( IsOpen() );
        return SetPositionInFileFromCurrent( _file, slong(offset) );
    }

/*
=================================================
    ReadSeq
=================================================
*/
    Bytes  UnixFileRStream::ReadSeq (OUT void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        ssize_t readn = ::read( _file, OUT buffer, size_t{size} );

        return Bytes{ulong( Max( 0, readn ))};
    }

/*
=================================================
    Prefetch
----
    initiates readahead on a file so that subsequent
    reads from that file will be satisfied from the cache, and not
    block on disk I/O
=================================================
*/
    bool  UnixFileRStream::Prefetch (Bytes offset, Bytes size) __NE___
    {
        ASSERT( IsOpen() );

    #ifdef AE_PLATFORM_APPLE
        Unused( offset, size );
        return false;
    #else
        //return ::posix_fadvise( _file, usize{offset}, usize{size}, POSIX_FADV_WILLNEED ) == 0;
        return ::readahead( _file, usize{offset}, usize{size} ) == 0;
    #endif
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    UnixFileWStream::UnixFileWStream (Handle_t file, EFlags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    UnixFileWStream::UnixFileWStream (const char* filename, EFlags flags)   __NE___ :
        UnixFileWStream{ Handle_t{OpenFileForWrite( filename, INOUT flags )}, flags DEBUG_ONLY(, Path{filename} )}
    {
        if_unlikely( not IsOpen() )
            UNIX_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    UnixFileWStream::UnixFileWStream (NtStringView filename, EFlags flags)  __NE___ : UnixFileWStream{ filename.c_str(), flags } {}
    UnixFileWStream::UnixFileWStream (const String &filename, EFlags flags) __NE___ : UnixFileWStream{ filename.c_str(), flags } {}
    UnixFileWStream::UnixFileWStream (const Path &path, EFlags flags)       __NE___ : UnixFileWStream{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
    UnixFileWStream::~UnixFileWStream () __NE___
    {
        if ( IsOpen() )
            ::close( _file );
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  UnixFileWStream::GetSourceType () C_NE___
    {
        return  ESourceType::SequentialAccess | ESourceType::WriteAccess;
    }

/*
=================================================
    Reserve
=================================================
*/
    Bytes  UnixFileWStream::Reserve (Bytes additionalSize) __NE___
    {
        ASSERT( IsOpen() );

        const Bytes     cur_size = GetFileSize( _file );

        const off_t     alloc    = ExtendFile( _file, off_t{cur_size}, off_t{additionalSize} );

        if_unlikely( alloc < 0 )
        {
            UNIX_CHECK_DEV( "ExtendFile failed: " );
            return cur_size;
        }
        return cur_size + alloc;
    }

/*
=================================================
    Position
=================================================
*/
    Bytes  UnixFileWStream::Position () C_NE___
    {
        ASSERT( IsOpen() );
        return Bytes{ulong( Max( 0, GetPositionInFile( _file )) )};
    }

/*
=================================================
    SeekFwd
=================================================
*/
    bool  UnixFileWStream::SeekFwd (Bytes offset) __NE___
    {
        ASSERT( IsOpen() );
        return SetPositionInFileFromCurrent( _file, slong(offset) );
    }

/*
=================================================
    WriteSeq
=================================================
*/
    Bytes  UnixFileWStream::WriteSeq (const void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        ssize_t written = ::write( _file, buffer, size_t{size} );

        return Bytes{ulong( Max( 0, written ))};
    }

/*
=================================================
    Flush
=================================================
*/
    void  UnixFileWStream::Flush () __NE___
    {
        ASSERT( IsOpen() );

        ::fsync( _file );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    UnixFileRDataSource::UnixFileRDataSource (Handle_t file, EFlags flags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file },
        _fileSize{ GetFileSize( _file )},
        _flags{ flags }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    UnixFileRDataSource::UnixFileRDataSource (NtStringView filename, EFlags flags)  __NE___ : UnixFileRDataSource{ filename.c_str(), flags } {}
    UnixFileRDataSource::UnixFileRDataSource (const String &filename, EFlags flags) __NE___ : UnixFileRDataSource{ filename.c_str(), flags } {}
    UnixFileRDataSource::UnixFileRDataSource (const char* filename, EFlags flags)   __NE___ :
        UnixFileRDataSource{ Handle_t{OpenFileForRead( filename, flags, 0 )}, flags DEBUG_ONLY(, filename )}
    {
        if_unlikely( not IsOpen() )
            UNIX_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    UnixFileRDataSource::UnixFileRDataSource (const Path &path, EFlags flags)       __NE___ : UnixFileRDataSource{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
    UnixFileRDataSource::~UnixFileRDataSource () __NE___
    {
        if ( IsOpen() )
            ::close( _file );
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  UnixFileRDataSource::GetSourceType () C_NE___
    {
        return  (AllBits( _flags, EFlags::SequentialScan )  ? ESourceType::SequentialAccess : ESourceType::Unknown) |
                (AllBits( _flags, EFlags::RandomAccess )    ? ESourceType::RandomAccess     : ESourceType::Unknown) |
                ESourceType::FixedSize | ESourceType::ReadAccess | ESourceType::ThreadSafe;
    }

/*
=================================================
    ReadBlock
=================================================
*/
    Bytes  UnixFileRDataSource::ReadBlock (const Bytes pos, OUT void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        ssize_t readn = ::pread( _file, OUT buffer, size_t{size}, off_t{pos} );
        if_likely( readn >= 0 )
            return Bytes{ulong(readn)};

        UNIX_CHECK_DEV( "ReadBlock failed: " );
        return 0_b;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    UnixFileWDataSource::UnixFileWDataSource (Handle_t file, EFlags DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    UnixFileWDataSource::UnixFileWDataSource (NtStringView filename, EFlags flags)  __NE___ : UnixFileWDataSource{ filename.c_str(), flags } {}
    UnixFileWDataSource::UnixFileWDataSource (const String &filename, EFlags flags) __NE___ : UnixFileWDataSource{ filename.c_str(), flags } {}
    UnixFileWDataSource::UnixFileWDataSource (const char* filename, EFlags flags)   __NE___ :
        UnixFileWDataSource{ Handle_t{OpenFileForWrite( filename, INOUT flags, 0 )}, flags DEBUG_ONLY(, Path{filename} )}
    {
        if_unlikely( not IsOpen() )
            UNIX_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
    }

    UnixFileWDataSource::UnixFileWDataSource (const Path &path, EFlags flags)       __NE___ : UnixFileWDataSource{ path.c_str(), flags } {}

/*
=================================================
    destructor
=================================================
*/
    UnixFileWDataSource::~UnixFileWDataSource () __NE___
    {
        if ( IsOpen() )
            ::close( _file );
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  UnixFileWDataSource::GetSourceType () C_NE___
    {
        return  ESourceType::RandomAccess | ESourceType::WriteAccess | ESourceType::ThreadSafe;
    }

/*
=================================================
    Capacity
=================================================
*/
    Bytes  UnixFileWDataSource::Capacity () C_NE___
    {
        return GetFileSize( _file );
    }

/*
=================================================
    Reserve
=================================================
*/
    Bytes  UnixFileWDataSource::Reserve (Bytes newSize) __NE___
    {
        ASSERT( IsOpen() );

        const Bytes     cur_size = GetFileSize( _file );

        if ( newSize <= cur_size )
            return cur_size;

        const off_t alloc = ExtendFile( _file, off_t{cur_size}, off_t{newSize - cur_size} );

        if_unlikely( alloc < 0 )
        {
            UNIX_CHECK_DEV( "ExtendFile failed: " );
            return cur_size;
        }
        return cur_size + alloc;
    }

/*
=================================================
    WriteBlock
=================================================
*/
    Bytes  UnixFileWDataSource::WriteBlock (const Bytes pos, const void* buffer, const Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        ssize_t written = ::pwrite( _file, buffer, ssize_t{size}, off_t{pos} );
        if_likely( written >= 0 )
            return Bytes{ulong(written)};

        UNIX_CHECK_DEV( "WriteBlock failed: " );
        return 0_b;
    }

/*
=================================================
    Flush
=================================================
*/
    void  UnixFileWDataSource::Flush () __NE___
    {
        ASSERT( IsOpen() );

        ::fsync( _file );
    }


} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
