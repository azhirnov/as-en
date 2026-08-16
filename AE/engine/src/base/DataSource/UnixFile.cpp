// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "base/Defines/StdInclude.h"

#ifdef AE_PLATFORM_UNIX_BASED
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <linux/stat.h>

# include "base/Algorithms/ToString.h"
# include "base/DataSource/UnixFile.h"
# include "base/FileSystem/FileSystem.h"

namespace AE::Base
{
#	include "base/DataSource/UnixFileHelper.cpp.h"

/*
=================================================
	constructor
=================================================
*/
	UnixFileRStream::UnixFileRStream (Handle_t file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file },
		_fileSize{ GetFileSize( _file )},
		_align{ GetDirectAccessAlign( _file DEBUG_ONLY(, filename ))}
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{
		if_unlikely( not IsOpen() )
			UNIX_CHECK_DEV( "Can't open file: \""s << ToString(_filename) << "\": " );
	}

	UnixFileRStream::UnixFileRStream (const char* filename, EMode mode)		__NE___ :
		UnixFileRStream{ Handle_t{OpenFileForRead( filename, mode )} DEBUG_ONLY(, filename )}
	{}

	UnixFileRStream::UnixFileRStream (NtStringView filename, EMode mode)	__NE___ : UnixFileRStream{ filename.c_str(), mode } {}
	UnixFileRStream::UnixFileRStream (const String &filename, EMode mode)	__NE___ : UnixFileRStream{ filename.c_str(), mode } {}
	UnixFileRStream::UnixFileRStream (const Path &path, EMode mode)			__NE___ : UnixFileRStream{ path.c_str(), mode } {}

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
		return	ESourceType::SequentialAccess	| ESourceType::RandomAccess |	// allow SeekFwd() & SeekSet()
				ESourceType::FixedSize			| ESourceType::ReadOnly	|
				ESourceType::ThreadSafe;
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

		ssize_t	readn = ::read( _file, OUT buffer, size_t{size} );

		return Bytes{ulong( Max( 0, readn ))};
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	UnixFileWStream::UnixFileWStream (Handle_t file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file },
		_align{ GetDirectAccessAlign( _file DEBUG_ONLY(, filename ))}
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{
		if_unlikely( not IsOpen() )
			UNIX_CHECK_DEV( "Can't open file: \""s << ToString(_filename) << "\": " );
	}

	UnixFileWStream::UnixFileWStream (const char* filename, EMode mode)		__NE___ :
		UnixFileWStream{ Handle_t{OpenFileForWrite( filename, INOUT mode )} DEBUG_ONLY(, Path{filename} )}
	{}

	UnixFileWStream::UnixFileWStream (NtStringView filename, EMode mode)	__NE___	: UnixFileWStream{ filename.c_str(), mode } {}
	UnixFileWStream::UnixFileWStream (const String &filename, EMode mode)	__NE___	: UnixFileWStream{ filename.c_str(), mode } {}
	UnixFileWStream::UnixFileWStream (const Path &path, EMode mode)			__NE___	: UnixFileWStream{ path.c_str(), mode } {}

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
		return	ESourceType::SequentialAccess | ESourceType::WriteOnly |
				ESourceType::ThreadSafe | ESourceType::RandomAccess;
	}

/*
=================================================
	Reserve
=================================================
*/
	Bytes  UnixFileWStream::Reserve (Bytes additionalSize) __NE___
	{
		ASSERT( IsOpen() );

		const Bytes		cur_size = GetFileSize( _file );

		const off_t		alloc 	 = ExtendFile( _file, off_t{cur_size}, off_t{additionalSize} );

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
	UpdateAt
=================================================
*/
	bool  UnixFileWStream::UpdateAt (Bytes pos) __NE___
	{
		ASSERT( IsOpen() );
		return SetPositionInFileFromBegin( _file, slong(pos) );
	}

/*
=================================================
	WriteSeq
=================================================
*/
	Bytes  UnixFileWStream::WriteSeq (const void* buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );

		ssize_t	written = ::write( _file, buffer, size_t{size} );

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
	UnixFileRDataSource::UnixFileRDataSource (Handle_t file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file },
		_fileSize{ GetFileSize( _file )},
		_align{ GetDirectAccessAlign( _file DEBUG_ONLY(, filename ))}
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{
		if_unlikely( not IsOpen() )
			UNIX_CHECK_DEV( "Can't open file: \""s << ToString(_filename) << "\": " );
	}

	UnixFileRDataSource::UnixFileRDataSource (NtStringView filename, EMode mode)	__NE___	: UnixFileRDataSource{ filename.c_str(), mode } {}
	UnixFileRDataSource::UnixFileRDataSource (const String &filename, EMode mode)	__NE___	: UnixFileRDataSource{ filename.c_str(), mode } {}
	UnixFileRDataSource::UnixFileRDataSource (const char* filename, EMode mode)		__NE___	:
		UnixFileRDataSource{ Handle_t{OpenFileForRead( filename, mode, 0 )} DEBUG_ONLY(, filename )}
	{}

	UnixFileRDataSource::UnixFileRDataSource (const Path &path, EMode mode)			__NE___	: UnixFileRDataSource{ path.c_str(), mode } {}

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
		return	ESourceType::SequentialAccess	| ESourceType::RandomAccess	|
				ESourceType::FixedSize			| ESourceType::ReadOnly	|
				ESourceType::ThreadSafe;
	}

/*
=================================================
	ReadBlock
=================================================
*/
	Bytes  UnixFileRDataSource::ReadBlock (const Bytes pos, OUT void* buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );

		ssize_t	readn = ::pread( _file, OUT buffer, size_t{size}, off_t{pos} );
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
	UnixFileWDataSource::UnixFileWDataSource (Handle_t file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file },
		_align{ GetDirectAccessAlign( _file DEBUG_ONLY(, filename ))}
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{
		if_unlikely( not IsOpen() )
			UNIX_CHECK_DEV( "Can't open file: \""s << ToString(_filename) << "\": " );
	}

	UnixFileWDataSource::UnixFileWDataSource (NtStringView filename, EMode mode)	__NE___	: UnixFileWDataSource{ filename.c_str(), mode } {}
	UnixFileWDataSource::UnixFileWDataSource (const String &filename, EMode mode)	__NE___	: UnixFileWDataSource{ filename.c_str(), mode } {}
	UnixFileWDataSource::UnixFileWDataSource (const char* filename, EMode mode)		__NE___	:
		UnixFileWDataSource{ Handle_t{OpenFileForWrite( filename, INOUT mode, 0 )} DEBUG_ONLY(, Path{filename} )}
	{}

	UnixFileWDataSource::UnixFileWDataSource (const Path &path, EMode mode)			__NE___ : UnixFileWDataSource{ path.c_str(), mode } {}

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
		return	ESourceType::RandomAccess | ESourceType::WriteOnly |
				ESourceType::ThreadSafe;
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

		const Bytes		cur_size = GetFileSize( _file );

		if ( newSize <= cur_size )
			return cur_size;

		const off_t	alloc = ExtendFile( _file, off_t{cur_size}, off_t{newSize - cur_size} );

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

		ssize_t	written = ::pwrite( _file, buffer, ssize_t{size}, off_t{pos} );
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
