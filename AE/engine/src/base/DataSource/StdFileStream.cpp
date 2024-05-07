// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/StdFileStream.h"
#include "base/Algorithms/StringUtils.h"
#include "base/FileSystem/FileSystem.h"

#ifdef AE_PLATFORM_WINDOWS
#	define fread	_fread_nolock
#	define fwrite	_fwrite_nolock
#	define fflush	_fflush_nolock
#	define fclose	_fclose_nolock
#	define ftell	_ftelli64_nolock
#	define fseek	_fseeki64_nolock
#endif

#if defined(AE_PLATFORM_LINUX)
#	define fseek	fseeko
#	define ftell	ftello
#endif
#ifdef AE_PLATFORM_UNIX_BASED
#   define fopen_s( _outFile_, _name_, _mode_ ) (*_outFile_ = std::fopen( _name_, _mode_ ))
#	define tmpfile_s( _outFile_ )				(*_outFile_ = std::tmpfile())
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
		FILE*	file = null;
		fopen_s( OUT &file, filename, access );
		return file;
	}

#ifdef AE_PLATFORM_WINDOWS
	ND_ static FILE*  OpenFile (const wchar_t* filename, const wchar_t* access) __NE___
	{
		FILE*	file = null;
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
		FILE*	file = null;
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
			const slong	curr = ftell( file );
			CHECK( fseek( file, 0, SEEK_END ) == 0 );

			const slong	size = ftell( file );
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
	ND_ static const char*  ModeToStr (StdFileWStream::EMode mode) __NE___
	{
		switch_enum( mode )
		{
			case StdFileWStream::EMode::OpenRewrite :	return "wb";
			case StdFileWStream::EMode::OpenUpdate :	return "rb+";	// open file for read/write
			case StdFileWStream::EMode::OpenAppend :	return "ab";
		}
		switch_end
		RETURN_ERR( "unknown file mode" );
	}

	ND_ static const wchar_t*  ModeToWStr (StdFileWStream::EMode mode) __NE___
	{
		switch_enum( mode )
		{
			case StdFileWStream::EMode::OpenRewrite :	return L"wb";
			case StdFileWStream::EMode::OpenUpdate :	return L"rb+";	// open file for read/write
			case StdFileWStream::EMode::OpenAppend :	return L"ab";
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
	StdFileRStream::StdFileRStream (FILE* file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file },
		_fileSize{ GetSize( _file )}
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{}

	StdFileRStream::StdFileRStream (NtStringView  filename)	__NE___ : StdFileRStream{ filename.c_str() } {}
	StdFileRStream::StdFileRStream (const String &filename)	__NE___ : StdFileRStream{ filename.c_str() } {}
	StdFileRStream::StdFileRStream (const char*   filename)	__NE___ : StdFileRStream{ OpenFile( filename, "rb" ) DEBUG_ONLY(, Path{filename} )}
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
	StdFileRStream::StdFileRStream (NtWStringView  filename)	__NE___ : StdFileRStream{ filename.c_str() } {}
	StdFileRStream::StdFileRStream (const WString &filename)	__NE___ : StdFileRStream{ filename.c_str() } {}
	StdFileRStream::StdFileRStream (const wchar_t* filename)	__NE___ : StdFileRStream{ OpenFile( filename, L"rb" ) DEBUG_ONLY(, Path{filename} )}
	{
		if ( _file == null )
			AE_LOG_DBG( "Can't open file: \""s << ToString(filename) << '"' );
	}
#endif

	StdFileRStream::StdFileRStream (const Path &path)			__NE___ : StdFileRStream{ path.c_str() } {}

/*
=================================================
	destructor
=================================================
*/
	StdFileRStream::~StdFileRStream () __NE___
	{
		if ( _file != null )
			fclose( _file );
	}

/*
=================================================
	SetBufferSize / SetBuffer / DisableBuffering
=================================================
*/
	bool  StdFileRStream::SetBufferSize (Bytes size) __NE___
	{
		ASSERT( IsOpen() );
		ASSERT( size > 0 );
		return std::setvbuf( _file, null, _IOFBF, usize{size} ) == 0;
	}

	bool  StdFileRStream::SetBuffer (char* buf, Bytes size) __NE___
	{
		ASSERT( IsOpen() );
		ASSERT( buf != null and size > 0 );
		return std::setvbuf( _file, buf, _IOFBF, usize{size} ) == 0;
	}

	bool  StdFileRStream::DisableBuffering () __NE___
	{
		ASSERT( IsOpen() );
		return std::setvbuf( _file, null, _IONBF, 0 ) == 0;
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  StdFileRStream::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess	| ESourceType::RandomAccess |	// allow SeekFwd() & SeekSet()
				ESourceType::FixedSize			| ESourceType::ReadAccess;
	}

/*
=================================================
	SeekSet
=================================================
*/
	bool  StdFileRStream::SeekSet (Bytes pos) __NE___
	{
		ASSERT( IsOpen() );

		if ( pos == _position )
			return true;

		_position = Min( pos, _fileSize );

		bool	res = (fseek( _file, slong(pos), SEEK_SET ) == 0);

		ASSERT_Eq( slong(_position), GetPositionInFile( _file ));
		return res;
	}

/*
=================================================
	SeekFwd
=================================================
*/
	bool  StdFileRStream::SeekFwd (Bytes offset) __NE___
	{
		ASSERT( IsOpen() );

		if ( offset == 0_b )
			return true;

		_position = Min( _position + offset, _fileSize );

		bool	res = (fseek( _file, slong(offset), SEEK_CUR ) == 0);

		ASSERT_Eq( slong(_position), GetPositionInFile( _file ));
		return res;
	}

/*
=================================================
	ReadSeq
=================================================
*/
	Bytes  StdFileRStream::ReadSeq (OUT void* buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );

		Bytes	readn{ fread( buffer, 1, usize(size), _file )};

		_position += readn;

		return readn;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	StdFileWStream::StdFileWStream (FILE* file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file }
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{}

	StdFileWStream::StdFileWStream (NtStringView  filename, EMode mode)	__NE___ : StdFileWStream{ filename.c_str(), mode } {}
	StdFileWStream::StdFileWStream (const String &filename, EMode mode)	__NE___ : StdFileWStream{ filename.c_str(), mode } {}
	StdFileWStream::StdFileWStream (const char*   filename, EMode mode)	__NE___ : StdFileWStream{ OpenFile( filename, ModeToStr(mode) ) DEBUG_ONLY(, Path{filename} )}
	{
		if ( _file == null )
			AE_LOG_DBG( "Can't open file: \""s << filename << '"' );
	}

	StdFileWStream::StdFileWStream () __NE___ :
		StdFileWStream{ OpenTempFile() DEBUG_ONLY(, Path{} )}
	{}

/*
=================================================
	constructor
=================================================
*/
#ifdef AE_PLATFORM_WINDOWS
	StdFileWStream::StdFileWStream (NtWStringView  filename, EMode mode)	__NE___ : StdFileWStream{ filename.c_str(), mode } {}
	StdFileWStream::StdFileWStream (const WString &filename, EMode mode)	__NE___ : StdFileWStream{ filename.c_str(), mode } {}
	StdFileWStream::StdFileWStream (const wchar_t* filename, EMode mode)	__NE___ : StdFileWStream{ OpenFile( filename, ModeToWStr(mode) ) DEBUG_ONLY(, Path{filename} )}
	{
		if ( _file == null )
			AE_LOG_DBG( "Can't open file: \""s << ToString(filename) << '"' );
	}
#endif

	StdFileWStream::StdFileWStream (const Path &path, EMode mode)			__NE___ : StdFileWStream{ path.c_str(), mode } {}

/*
=================================================
	destructor
=================================================
*/
	StdFileWStream::~StdFileWStream () __NE___
	{
		if ( _file != null )
			fclose( _file );
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  StdFileWStream::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess | ESourceType::WriteAccess;	// TODO: RandomAccess
	}

/*
=================================================
	Position
=================================================
*/
	Bytes  StdFileWStream::Position () C_NE___
	{
		ASSERT( IsOpen() );

		return Bytes{ulong( ftell( _file ))};
	}

/*
=================================================
	SeekFwd
=================================================
*/
	bool  StdFileWStream::SeekFwd (Bytes offset) __NE___
	{
		ASSERT( IsOpen() );

		return fseek( _file, slong(offset), SEEK_CUR ) == 0;
	}

/*
=================================================
	WriteSeq
=================================================
*/
	Bytes  StdFileWStream::WriteSeq (const void* buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );

		return Bytes{ fwrite( buffer, 1, usize(size), _file )};
	}

/*
=================================================
	Flush
=================================================
*/
	void  StdFileWStream::Flush () __NE___
	{
		ASSERT( IsOpen() );

		CHECK( fflush( _file ) == 0 );
	}

/*
=================================================
	AsWDataSource
=================================================
*
	RC<WDataSource>  StdFileWStream::AsWDataSource ()
	{
		CHECK_ERR( _file != null );

		FILE*	tmp = _file;
		_file = null;

		return RC<WDataSource>{ new StdFileWDataSource{ tmp DEBUG_ONLY(, _filename )}};
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	StdFileRDataSource::StdFileRDataSource (FILE* file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file },
		_fileSize{ GetSize( _file )}
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{}

	StdFileRDataSource::StdFileRDataSource (NtStringView  filename)	__NE___ : StdFileRDataSource{ filename.c_str() } {}
	StdFileRDataSource::StdFileRDataSource (const String &filename)	__NE___ : StdFileRDataSource{ filename.c_str() } {}
	StdFileRDataSource::StdFileRDataSource (const char*   filename)	__NE___ : StdFileRDataSource{ OpenFile( filename, "rb" ) DEBUG_ONLY(, Path{filename} )}
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
	StdFileRDataSource::StdFileRDataSource (NtWStringView  filename)	__NE___ : StdFileRDataSource{ filename.c_str() } {}
	StdFileRDataSource::StdFileRDataSource (const WString &filename)	__NE___ : StdFileRDataSource{ filename.c_str() } {}
	StdFileRDataSource::StdFileRDataSource (const wchar_t* filename)	__NE___ : StdFileRDataSource{ OpenFile( filename, L"rb" ) DEBUG_ONLY(, Path{filename} )}
	{
		if ( _file == null )
			AE_LOG_DBG( "Can't open file: \""s << ToString(filename) << '"' );
	}
#endif

	StdFileRDataSource::StdFileRDataSource (const Path &path)			__NE___ : StdFileRDataSource{ path.c_str() } {}

/*
=================================================
	destructor
=================================================
*/
	StdFileRDataSource::~StdFileRDataSource () __NE___
	{
		if ( _file != null )
			fclose( _file );
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  StdFileRDataSource::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess	| ESourceType::RandomAccess	|
				ESourceType::FixedSize			| ESourceType::ReadAccess;
	}

/*
=================================================
	ReadBlock
=================================================
*/
	Bytes  StdFileRDataSource::ReadBlock (Bytes pos, OUT void* buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );

		if ( _lastPos != pos )
		{
			if_unlikely( fseek( _file, slong(pos), SEEK_SET ) != 0 )
				return 0_b;		// error
		}

		Bytes	readn{ fread( buffer, 1, usize(size), _file )};

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
	StdFileWDataSource::StdFileWDataSource (FILE* file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file }
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{}

	StdFileWDataSource::StdFileWDataSource (NtStringView  filename, EMode mode)	__NE___ : StdFileWDataSource{ filename.c_str(), mode } {}
	StdFileWDataSource::StdFileWDataSource (const String &filename, EMode mode)	__NE___ : StdFileWDataSource{ filename.c_str(), mode } {}
	StdFileWDataSource::StdFileWDataSource (const char*   filename, EMode mode)	__NE___ : StdFileWDataSource{ OpenFile( filename, ModeToStr(mode) ) DEBUG_ONLY(, Path{filename} )}
	{
		ASSERT( mode != EMode::OpenAppend );
		if ( _file == null )
			AE_LOG_DBG( "Can't open file: \""s << filename << '"' );
	}

	StdFileWDataSource::StdFileWDataSource () __NE___ :
		StdFileWDataSource{ OpenTempFile() DEBUG_ONLY(, Path{} )}
	{}

/*
=================================================
	constructor
=================================================
*/
#ifdef AE_PLATFORM_WINDOWS
	StdFileWDataSource::StdFileWDataSource (NtWStringView  filename, EMode mode)	__NE___ : StdFileWDataSource{ filename.c_str(), mode } {}
	StdFileWDataSource::StdFileWDataSource (const WString &filename, EMode mode)	__NE___ : StdFileWDataSource{ filename.c_str(), mode } {}
	StdFileWDataSource::StdFileWDataSource (const wchar_t* filename, EMode mode)	__NE___ : StdFileWDataSource{ OpenFile( filename, ModeToWStr(mode) ) DEBUG_ONLY(, Path{filename} )}
	{
		ASSERT( mode != EMode::OpenAppend );
		if ( _file == null )
			AE_LOG_DBG( "Can't open file: \""s << ToString(filename) << '"' );
	}
#endif

	StdFileWDataSource::StdFileWDataSource (const Path &path, EMode mode)			__NE___ : StdFileWDataSource{ path.c_str(), mode } {}

/*
=================================================
	destructor
=================================================
*/
	StdFileWDataSource::~StdFileWDataSource () __NE___
	{
		if ( _file != null )
			fclose( _file );
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  StdFileWDataSource::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess | ESourceType::RandomAccess |
				ESourceType::WriteAccess;
	}

/*
=================================================
	Capacity
=================================================
*/
	Bytes  StdFileWDataSource::Capacity () C_NE___
	{
		return GetSize( _file );
	}

/*
=================================================
	WriteBlock
=================================================
*/
	Bytes  StdFileWDataSource::WriteBlock (const Bytes pos, const void* buffer, const Bytes size) __NE___
	{
		ASSERT( IsOpen() );

		if ( _lastPos != pos )
		{
			if_unlikely( fseek( _file, slong(pos), SEEK_SET ) != 0 )
				return 0_b;		// error
		}

		Bytes	written{ fwrite( buffer, 1, usize(size), _file )};

		_lastPos = pos + written;
		ASSERT_Eq( slong(_lastPos), GetPositionInFile( _file ));

		return written;
	}

/*
=================================================
	Flush
=================================================
*/
	void  StdFileWDataSource::Flush () __NE___
	{
		ASSERT( IsOpen() );

		CHECK( fflush( _file ) == 0 );
	}


} // AE::Base
