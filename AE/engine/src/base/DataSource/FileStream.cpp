// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/FileStream.h"
#include "base/Algorithms/StringUtils.h"

#ifdef AE_PLATFORM_WINDOWS
#   define fread    _fread_nolock
#   define fwrite   _fwrite_nolock
#   define fflush   _fflush_nolock
#   define fclose   _fclose_nolock
#   define ftell    _ftelli64_nolock
#   define fseek    _fseeki64_nolock
#endif

#if defined(AE_PLATFORM_LINUX)
#   define fseek    fseeko
#   define ftell    ftello
#endif
#ifdef AE_PLATFORM_UNIX_BASED
#   define fopen_s( _outFile_, _name_, _mode_ ) (*_outFile_ = std::fopen( _name_, _mode_ ))
#   define tmpfile_s( _outFile_ )               (*_outFile_ = std::tmpfile())
#endif


namespace AE::Base
{
namespace
{
/*
=================================================
    OpenFile
=================================================
*/
    ND_ static FILE*  OpenFile (const char* filename, const char* access) __NE___
    {
        FILE*   file = null;
        fopen_s( OUT &file, filename, access );
        return file;
    }

#ifdef AE_PLATFORM_WINDOWS
    ND_ static FILE*  OpenFile (const wchar_t* filename, const wchar_t* access) __NE___
    {
        FILE*   file = null;
        _wfopen_s( OUT &file, filename, access );
        return file;
    }
#endif

/*
=================================================
    OpenTempFile
=================================================
*/
    ND_ static FILE*  OpenTempFile () __NE___
    {
        FILE*   file = null;
        tmpfile_s( OUT &file );
        return file;
    }

/*
=================================================
    GetSize
=================================================
*/
    ND_ static Bytes  GetSize (FILE* file) __NE___
    {
        if_likely( file != null )
        {
            const slong curr = ftell( file );
            CHECK( fseek( file, 0, SEEK_END ) == 0 );

            const slong size = ftell( file );
            CHECK( fseek( file, curr, SEEK_SET ) == 0 );

            return Bytes(ulong(size));
        }
        return 0_b;
    }

/*
=================================================
    ModeToStr
=================================================
*/
    ND_ static const char*  ModeToStr (FileWStream::EMode mode) __NE___
    {
        switch_enum( mode )
        {
            case FileWStream::EMode::Rewrite :  return "wb";
            case FileWStream::EMode::Update :   return "rb+";   // open file for read/write
            case FileWStream::EMode::Append :   return "ab";
        }
        switch_end
        RETURN_ERR( "unknown file mode" );
    }

    ND_ static const wchar_t*  ModeToWStr (FileWStream::EMode mode) __NE___
    {
        switch_enum( mode )
        {
            case FileWStream::EMode::Rewrite :  return L"wb";
            case FileWStream::EMode::Update :   return L"rb+";  // open file for read/write
            case FileWStream::EMode::Append :   return L"ab";
        }
        switch_end
        RETURN_ERR( "unknown file mode" );
    }

/*
=================================================
    GetPositionInFile
=================================================
*/
DEBUG_ONLY(
    ND_ static slong  GetPositionInFile (FILE* file) __NE___
    {
        return ftell( file );
    }
)
} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    FileRStream::FileRStream (FILE* file DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file },
        _fileSize{ GetSize( _file )}
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    FileRStream::FileRStream (NtStringView  filename)   __NE___ : FileRStream{ filename.c_str() } {}
    FileRStream::FileRStream (const String &filename)   __NE___ : FileRStream{ filename.c_str() } {}
    FileRStream::FileRStream (const char*   filename)   __NE___ : FileRStream{ OpenFile( filename, "rb" ) DEBUG_ONLY(, Path{filename} )}
    {
        if ( _file == null )
            AE_LOG_DBG( "Can't open file: \""s << filename << '"' );
    }

/*
=================================================
    constructor
=================================================
*/
#ifdef AE_PLATFORM_WINDOWS
    FileRStream::FileRStream (NtWStringView  filename)  __NE___ : FileRStream{ filename.c_str() } {}
    FileRStream::FileRStream (const WString &filename)  __NE___ : FileRStream{ filename.c_str() } {}
    FileRStream::FileRStream (const wchar_t* filename)  __NE___ : FileRStream{ OpenFile( filename, L"rb" ) DEBUG_ONLY(, Path{filename} )}
    {
        if ( _file == null )
            AE_LOG_DBG( "Can't open file: \""s << ToString(filename) << '"' );
    }
#endif

    FileRStream::FileRStream (const Path &path)         __NE___ : FileRStream{ path.c_str() } {}

/*
=================================================
    destructor
=================================================
*/
    FileRStream::~FileRStream () __NE___
    {
        if ( _file != null )
            fclose( _file );
    }

/*
=================================================
    SetBufferSize / SetBuffer / DisableBuffering
=================================================
*/
    bool  FileRStream::SetBufferSize (Bytes size) __NE___
    {
        ASSERT( IsOpen() );
        ASSERT( size > 0 );
        return std::setvbuf( _file, null, _IOFBF, usize{size} ) == 0;
    }

    bool  FileRStream::SetBuffer (char* buf, Bytes size) __NE___
    {
        ASSERT( IsOpen() );
        ASSERT( buf != null and size > 0 );
        return std::setvbuf( _file, buf, _IOFBF, usize{size} ) == 0;
    }

    bool  FileRStream::DisableBuffering () __NE___
    {
        ASSERT( IsOpen() );
        return std::setvbuf( _file, null, _IONBF, 0 ) == 0;
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  FileRStream::GetSourceType () C_NE___
    {
        return  ESourceType::SequentialAccess   | ESourceType::RandomAccess |
                ESourceType::FixedSize          | ESourceType::ReadAccess;
    }

/*
=================================================
    SeekSet
=================================================
*/
    bool  FileRStream::SeekSet (Bytes pos) __NE___
    {
        ASSERT( IsOpen() );

        if ( pos == _position )
            return true;

        _position = Min( pos, _fileSize );

        bool    res = (fseek( _file, slong(pos), SEEK_SET ) == 0);

        ASSERT_Eq( slong(_position), GetPositionInFile( _file ));
        return res;
    }

/*
=================================================
    SeekFwd
=================================================
*/
    bool  FileRStream::SeekFwd (Bytes offset) __NE___
    {
        ASSERT( IsOpen() );

        if ( offset == 0_b )
            return true;

        _position = Min( _position + offset, _fileSize );

        bool    res = (fseek( _file, slong(offset), SEEK_CUR ) == 0);

        ASSERT_Eq( slong(_position), GetPositionInFile( _file ));
        return res;
    }

/*
=================================================
    ReadSeq
=================================================
*/
    Bytes  FileRStream::ReadSeq (OUT void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        Bytes   readn{ fread( buffer, 1, usize(size), _file )};

        _position += readn;

        return readn;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    FileWStream::FileWStream (FILE* file DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    FileWStream::FileWStream (NtStringView  filename, EMode mode)   __NE___ : FileWStream{ filename.c_str(), mode } {}
    FileWStream::FileWStream (const String &filename, EMode mode)   __NE___ : FileWStream{ filename.c_str(), mode } {}
    FileWStream::FileWStream (const char*   filename, EMode mode)   __NE___ : FileWStream{ OpenFile( filename, ModeToStr(mode) ) DEBUG_ONLY(, Path{filename} )}
    {
        if ( _file == null )
            AE_LOG_DBG( "Can't open file: \""s << filename << '"' );
    }

    FileWStream::FileWStream () __NE___ :
        FileWStream{ OpenTempFile() DEBUG_ONLY(, Path{} )}
    {}

/*
=================================================
    constructor
=================================================
*/
#ifdef AE_PLATFORM_WINDOWS
    FileWStream::FileWStream (NtWStringView  filename, EMode mode)  __NE___ : FileWStream{ filename.c_str(), mode } {}
    FileWStream::FileWStream (const WString &filename, EMode mode)  __NE___ : FileWStream{ filename.c_str(), mode } {}
    FileWStream::FileWStream (const wchar_t* filename, EMode mode)  __NE___ : FileWStream{ OpenFile( filename, ModeToWStr(mode) ) DEBUG_ONLY(, Path{filename} )}
    {
        if ( _file == null )
            AE_LOG_DBG( "Can't open file: \""s << ToString(filename) << '"' );
    }
#endif

    FileWStream::FileWStream (const Path &path, EMode mode)         __NE___ : FileWStream{ path.c_str(), mode } {}

/*
=================================================
    destructor
=================================================
*/
    FileWStream::~FileWStream () __NE___
    {
        if ( _file != null )
            fclose( _file );
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  FileWStream::GetSourceType () C_NE___
    {
        return  ESourceType::SequentialAccess | ESourceType::WriteAccess;   // TODO: RandomAccess
    }

/*
=================================================
    Position
=================================================
*/
    Bytes  FileWStream::Position () C_NE___
    {
        ASSERT( IsOpen() );

        return Bytes{ulong( ftell( _file ))};
    }

/*
=================================================
    SeekFwd
=================================================
*/
    bool  FileWStream::SeekFwd (Bytes offset) __NE___
    {
        ASSERT( IsOpen() );

        return fseek( _file, slong(offset), SEEK_CUR ) == 0;
    }

/*
=================================================
    WriteSeq
=================================================
*/
    Bytes  FileWStream::WriteSeq (const void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        return Bytes{ fwrite( buffer, 1, usize(size), _file )};
    }

/*
=================================================
    Flush
=================================================
*/
    void  FileWStream::Flush () __NE___
    {
        ASSERT( IsOpen() );

        CHECK( fflush( _file ) == 0 );
    }

/*
=================================================
    AsWDataSource
=================================================
*
    RC<WDataSource>  FileWStream::AsWDataSource ()
    {
        CHECK_ERR( _file != null );

        FILE*   tmp = _file;
        _file = null;

        return RC<WDataSource>{ new FileWDataSource{ tmp DEBUG_ONLY(, _filename )}};
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    FileRDataSource::FileRDataSource (FILE* file DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file },
        _fileSize{ GetSize( _file )}
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    FileRDataSource::FileRDataSource (NtStringView  filename)   __NE___ : FileRDataSource{ filename.c_str() } {}
    FileRDataSource::FileRDataSource (const String &filename)   __NE___ : FileRDataSource{ filename.c_str() } {}
    FileRDataSource::FileRDataSource (const char*   filename)   __NE___ : FileRDataSource{ OpenFile( filename, "rb" ) DEBUG_ONLY(, Path{filename} )}
    {
        if ( _file == null )
            AE_LOG_DBG( "Can't open file: \""s << filename << '"' );
    }

/*
=================================================
    constructor
=================================================
*/
#ifdef AE_PLATFORM_WINDOWS
    FileRDataSource::FileRDataSource (NtWStringView  filename)  __NE___ : FileRDataSource{ filename.c_str() } {}
    FileRDataSource::FileRDataSource (const WString &filename)  __NE___ : FileRDataSource{ filename.c_str() } {}
    FileRDataSource::FileRDataSource (const wchar_t* filename)  __NE___ : FileRDataSource{ OpenFile( filename, L"rb" ) DEBUG_ONLY(, Path{filename} )}
    {
        if ( _file == null )
            AE_LOG_DBG( "Can't open file: \""s << ToString(filename) << '"' );
    }
#endif

    FileRDataSource::FileRDataSource (const Path &path)         __NE___ : FileRDataSource{ path.c_str() } {}

/*
=================================================
    destructor
=================================================
*/
    FileRDataSource::~FileRDataSource () __NE___
    {
        if ( _file != null )
            fclose( _file );
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  FileRDataSource::GetSourceType () C_NE___
    {
        return  ESourceType::SequentialAccess   | ESourceType::RandomAccess |
                ESourceType::FixedSize          | ESourceType::ReadAccess;
    }

/*
=================================================
    ReadBlock
=================================================
*/
    Bytes  FileRDataSource::ReadBlock (Bytes pos, OUT void* buffer, Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        if ( _lastPos != pos )
        {
            if_unlikely( fseek( _file, slong(pos), SEEK_SET ) != 0 )
                return 0_b;     // error
        }

        Bytes   readn{ fread( buffer, 1, usize(size), _file )};

        _lastPos = pos + readn;
        ASSERT_Eq( slong(_lastPos), GetPositionInFile( _file ));

        return readn;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    FileWDataSource::FileWDataSource (FILE* file DEBUG_ONLY(, Path filename)) __NE___ :
        _file{ file }
        DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
    {}

    FileWDataSource::FileWDataSource (NtStringView  filename, EMode mode)   __NE___ : FileWDataSource{ filename.c_str(), mode } {}
    FileWDataSource::FileWDataSource (const String &filename, EMode mode)   __NE___ : FileWDataSource{ filename.c_str(), mode } {}
    FileWDataSource::FileWDataSource (const char*   filename, EMode mode)   __NE___ : FileWDataSource{ OpenFile( filename, ModeToStr(mode) ) DEBUG_ONLY(, Path{filename} )}
    {
        ASSERT( mode != EMode::Append );
        if ( _file == null )
            AE_LOG_DBG( "Can't open file: \""s << filename << '"' );
    }

    FileWDataSource::FileWDataSource () __NE___ :
        FileWDataSource{ OpenTempFile() DEBUG_ONLY(, Path{} )}
    {}

/*
=================================================
    constructor
=================================================
*/
#ifdef AE_PLATFORM_WINDOWS
    FileWDataSource::FileWDataSource (NtWStringView  filename, EMode mode)  __NE___ : FileWDataSource{ filename.c_str(), mode } {}
    FileWDataSource::FileWDataSource (const WString &filename, EMode mode)  __NE___ : FileWDataSource{ filename.c_str(), mode } {}
    FileWDataSource::FileWDataSource (const wchar_t* filename, EMode mode)  __NE___ : FileWDataSource{ OpenFile( filename, ModeToWStr(mode) ) DEBUG_ONLY(, Path{filename} )}
    {
        ASSERT( mode != EMode::Append );
        if ( _file == null )
            AE_LOG_DBG( "Can't open file: \""s << ToString(filename) << '"' );
    }
#endif

    FileWDataSource::FileWDataSource (const Path &path, EMode mode)         __NE___ : FileWDataSource{ path.c_str(), mode } {}

/*
=================================================
    destructor
=================================================
*/
    FileWDataSource::~FileWDataSource () __NE___
    {
        if ( _file != null )
            fclose( _file );
    }

/*
=================================================
    GetSourceType
=================================================
*/
    IDataSource::ESourceType  FileWDataSource::GetSourceType () C_NE___
    {
        return  ESourceType::SequentialAccess | ESourceType::RandomAccess |
                ESourceType::WriteAccess;
    }

/*
=================================================
    Capacity
=================================================
*/
    Bytes  FileWDataSource::Capacity () C_NE___
    {
        return GetSize( _file );
    }

/*
=================================================
    WriteBlock
=================================================
*/
    Bytes  FileWDataSource::WriteBlock (const Bytes pos, const void* buffer, const Bytes size) __NE___
    {
        ASSERT( IsOpen() );

        if ( _lastPos != pos )
        {
            if_unlikely( fseek( _file, slong(pos), SEEK_SET ) != 0 )
                return 0_b;     // error
        }

        Bytes   written{ fwrite( buffer, 1, usize(size), _file )};

        _lastPos = pos + written;
        ASSERT_Eq( slong(_lastPos), GetPositionInFile( _file ));

        return written;
    }

/*
=================================================
    Flush
=================================================
*/
    void  FileWDataSource::Flush () __NE___
    {
        ASSERT( IsOpen() );

        CHECK( fflush( _file ) == 0 );
    }


} // AE::Base
