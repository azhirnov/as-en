// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_PLATFORM_WINDOWS
# include "base/Platforms/WindowsHeader.cpp.h"
# include "base/Platforms/WindowsUtils.h"
# include "base/Algorithms/StringUtils.h"
# include "base/DataSource/WindowsFile.h"
# include "base/FileSystem/FileSystem.h"

namespace AE::Base
{
#	include "base/DataSource/WindowsFileHelper.cpp.h"

/*
=================================================
	constructor
=================================================
*/
	WinFileRStream::WinFileRStream (const Handle_t &file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file.Ref<HANDLE>() },
		_fileSize{ GetFileSize( _file.Ref<HANDLE>() )}
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{}

	WinFileRStream::WinFileRStream (const char* filename, EMode mode)		__NE___ :
		WinFileRStream{ Handle_t{OpenFileForRead( filename, mode )} DEBUG_ONLY(, filename )}
	{
		if_unlikely( not IsOpen() )
			WIN_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
	}

	WinFileRStream::WinFileRStream (const wchar_t* filename, EMode mode)	__NE___ :
		WinFileRStream{ Handle_t{OpenFileForRead( filename, mode )} DEBUG_ONLY(, filename )}
	{
		if_unlikely( not IsOpen() )
			WIN_CHECK_DEV( "Can't open file: \""s << ToString(filename) << "\": " );
	}

	WinFileRStream::WinFileRStream (NtStringView filename, EMode mode)		__NE___ : WinFileRStream{ filename.c_str(), mode } {}
	WinFileRStream::WinFileRStream (const String &filename, EMode mode)		__NE___ : WinFileRStream{ filename.c_str(), mode } {}

	WinFileRStream::WinFileRStream (NtWStringView filename, EMode mode)		__NE___ : WinFileRStream{ filename.c_str(), mode } {}
	WinFileRStream::WinFileRStream (const WString &filename, EMode mode)	__NE___ : WinFileRStream{ filename.c_str(), mode } {}

	WinFileRStream::WinFileRStream (const Path &path, EMode mode)			__NE___ : WinFileRStream{ path.c_str(), mode } {}

/*
=================================================
	destructor
=================================================
*/
	WinFileRStream::~WinFileRStream () __NE___
	{
		if ( IsOpen() )
			::CloseHandle( _file.Ref<HANDLE>() );
	}

/*
=================================================
	IsOpen
=================================================
*/
	bool  WinFileRStream::IsOpen () C_NE___
	{
		return _file.Ref<HANDLE>() != INVALID_HANDLE_VALUE;
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  WinFileRStream::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess	| ESourceType::RandomAccess |	// allow SeekFwd() & SeekSet()
				ESourceType::FixedSize			| ESourceType::ReadAccess	|
				ESourceType::ThreadSafe;
	}

/*
=================================================
	_Position
=================================================
*/
	Bytes  WinFileRStream::_Position () C_NE___
	{
		ASSERT( IsOpen() );
		return Bytes{ulong( Max( 0, GetPositionInFile( _file.Ref<HANDLE>() )) )};
	}

/*
=================================================
	SeekSet
=================================================
*/
	bool  WinFileRStream::SeekSet (Bytes newPos) __NE___
	{
		ASSERT( IsOpen() );

		LARGE_INTEGER	info;
		info.QuadPart = slong(newPos);

		return ::SetFilePointerEx( _file.Ref<HANDLE>(), info, null, FILE_BEGIN ) != FALSE;	// winxp
	}

/*
=================================================
	SeekFwd
=================================================
*/
	bool  WinFileRStream::SeekFwd (Bytes offset) __NE___
	{
		ASSERT( IsOpen() );

		LARGE_INTEGER	info;
		info.QuadPart = slong(offset);

		return ::SetFilePointerEx( _file.Ref<HANDLE>(), info, null, FILE_CURRENT ) != FALSE;	// winxp
	}

/*
=================================================
	ReadSeq
=================================================
*/
	Bytes  WinFileRStream::ReadSeq (OUT void* buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );
		ASSERT_Lt( size, MaxValue<DWORD>() );

		DWORD	readn = 0;
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
	WinFileWStream::WinFileWStream (const Handle_t &file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file.Ref<HANDLE>() }
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{}

	WinFileWStream::WinFileWStream (const char* filename, EMode mode)		__NE___ :
		WinFileWStream{ Handle_t{OpenFileForWrite( filename, INOUT mode )} DEBUG_ONLY(, Path{filename} )}
	{
		if_unlikely( not IsOpen() )
			WIN_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
	}

	WinFileWStream::WinFileWStream (const wchar_t* filename, EMode mode)	__NE___ :
		WinFileWStream{ Handle_t{OpenFileForWrite( filename, INOUT mode )} DEBUG_ONLY(, Path{filename} )}
	{
		if_unlikely( not IsOpen() )
			WIN_CHECK_DEV( "Can't open file: \""s << ToString(filename) << "\": " );
	}

	WinFileWStream::WinFileWStream (NtStringView filename, EMode mode)		__NE___	: WinFileWStream{ filename.c_str(), mode } {}
	WinFileWStream::WinFileWStream (const String &filename, EMode mode)		__NE___	: WinFileWStream{ filename.c_str(), mode } {}

	WinFileWStream::WinFileWStream (NtWStringView filename, EMode mode)		__NE___	: WinFileWStream{ filename.c_str(), mode } {}
	WinFileWStream::WinFileWStream (const WString &filename, EMode mode)	__NE___	: WinFileWStream{ filename.c_str(), mode } {}

	WinFileWStream::WinFileWStream (const Path &path, EMode mode)			__NE___	: WinFileWStream{ path.c_str(), mode } {}

/*
=================================================
	destructor
=================================================
*/
	WinFileWStream::~WinFileWStream () __NE___
	{
		if ( IsOpen() )
			::CloseHandle( _file.Ref<HANDLE>() );
	}

/*
=================================================
	IsOpen
=================================================
*/
	bool  WinFileWStream::IsOpen () C_NE___
	{
		return _file.Ref<HANDLE>() != INVALID_HANDLE_VALUE;
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  WinFileWStream::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess | ESourceType::WriteAccess |
				ESourceType::ThreadSafe;
	}

/*
=================================================
	Reserve
=================================================
*/
	Bytes  WinFileWStream::Reserve (Bytes additionalSize) __NE___
	{
		ASSERT( IsOpen() );

		const Bytes		cur_size = GetFileSize( _file.Ref<HANDLE>() );

		LARGE_INTEGER	req_pos;
		LARGE_INTEGER	cur_pos = {};

		req_pos.QuadPart = ulong(cur_size + additionalSize);

		if_unlikely( ::SetFilePointerEx( _file.Ref<HANDLE>(), req_pos, OUT &cur_pos, FILE_BEGIN ) == FALSE )	// winxp
			return cur_size;  // error

		if_unlikely( ::SetEndOfFile( _file.Ref<HANDLE>() ) == FALSE )	// winxp
			return cur_size;  // error

		return Bytes{ulong(cur_pos.QuadPart)};  // ok
	}

/*
=================================================
	Position
=================================================
*/
	Bytes  WinFileWStream::Position () C_NE___
	{
		ASSERT( IsOpen() );
		return Bytes{ulong( Max( 0, GetPositionInFile( _file.Ref<HANDLE>() )) )};
	}

/*
=================================================
	SeekFwd
=================================================
*/
	bool  WinFileWStream::SeekFwd (Bytes offset) __NE___
	{
		ASSERT( IsOpen() );

		LARGE_INTEGER	info;
		info.QuadPart = slong(offset);

		return ::SetFilePointerEx( _file.Ref<HANDLE>(), info, null, FILE_CURRENT ) != FALSE;	// winxp
	}

/*
=================================================
	WriteSeq
=================================================
*/
	Bytes  WinFileWStream::WriteSeq (const void* buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );
		ASSERT_Lt( size, MaxValue<DWORD>() );

		DWORD	written = 0;
		if_likely( ::WriteFile( _file.Ref<HANDLE>(), buffer, DWORD(size), OUT &written, null ) != FALSE )
			return Bytes{written};

		return 0_b;
	}

/*
=================================================
	Flush
=================================================
*/
	void  WinFileWStream::Flush () __NE___
	{
		ASSERT( IsOpen() );

		CHECK( ::FlushFileBuffers( _file.Ref<HANDLE>() ) != 0 );	// winxp
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	WinFileRDataSource::WinFileRDataSource (const Handle_t &file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file.Ref<HANDLE>() },
		_fileSize{ GetFileSize( _file.Ref<HANDLE>() )}
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{}

	WinFileRDataSource::WinFileRDataSource (NtStringView filename, EMode mode)		__NE___	: WinFileRDataSource{ filename.c_str(), mode } {}
	WinFileRDataSource::WinFileRDataSource (const String &filename, EMode mode)		__NE___	: WinFileRDataSource{ filename.c_str(), mode } {}
	WinFileRDataSource::WinFileRDataSource (const char* filename, EMode mode)		__NE___	:
		WinFileRDataSource{ Handle_t{OpenFileForRead( filename, mode, FILE_FLAG_OVERLAPPED )} DEBUG_ONLY(, filename )}
	{
		if_unlikely( not IsOpen() )
			WIN_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
	}

	WinFileRDataSource::WinFileRDataSource (NtWStringView filename, EMode mode)		__NE___	: WinFileRDataSource{ filename.c_str(), mode } {}
	WinFileRDataSource::WinFileRDataSource (const WString &filename, EMode mode)	__NE___	: WinFileRDataSource{ filename.c_str(), mode } {}
	WinFileRDataSource::WinFileRDataSource (const wchar_t* filename, EMode mode)	__NE___	:
		WinFileRDataSource{ Handle_t{OpenFileForRead( filename, mode, FILE_FLAG_OVERLAPPED )} DEBUG_ONLY(, filename )}
	{
		if_unlikely( not IsOpen() )
			WIN_CHECK_DEV( "Can't open file: \""s << ToString(filename) << "\": " );
	}

	WinFileRDataSource::WinFileRDataSource (const Path &path, EMode mode)			__NE___	: WinFileRDataSource{ path.c_str(), mode } {}

/*
=================================================
	destructor
=================================================
*/
	WinFileRDataSource::~WinFileRDataSource () __NE___
	{
		if ( IsOpen() )
			::CloseHandle( _file.Ref<HANDLE>() );
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  WinFileRDataSource::GetSourceType () C_NE___
	{
		return	ESourceType::SequentialAccess	| ESourceType::RandomAccess |	// allow SeekFwd() & SeekSet()
				ESourceType::FixedSize			| ESourceType::ReadAccess	|
				ESourceType::ThreadSafe;
	}

/*
=================================================
	IsOpen
=================================================
*/
	bool  WinFileRDataSource::IsOpen () C_NE___
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
	Bytes  WinFileRDataSource::ReadBlock (const Bytes pos, OUT void* buffer, Bytes size) __NE___
	{
		ASSERT( IsOpen() );
		ASSERT_Lt( size, MaxValue<DWORD>() );

		OVERLAPPED	ov = {};
		SetOverlappedOffset( INOUT ov, pos );

		DWORD	readn = 0;
		if_likely( ::ReadFile( _file.Ref<HANDLE>(), OUT buffer, DWORD(size), OUT &readn, INOUT &ov ) != FALSE )
			return Bytes{readn};  // ok

		auto	err = ::GetLastError();
		if_unlikely( err != ERROR_IO_PENDING )
		{
			WIN_CHECK_DEV2( err, "ReadFile failed: " );
			return 0_b;  // error
		}

		// wait for completion
		if_likely( ::GetOverlappedResult( _file.Ref<HANDLE>(), &ov, OUT &readn, TRUE ) != FALSE )	// winxp
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
	WinFileWDataSource::WinFileWDataSource (const Handle_t &file DEBUG_ONLY(, Path filename)) __NE___ :
		_file{ file.Ref<HANDLE>() }
		DEBUG_ONLY(, _filename{ FileSystem::ToAbsolute( filename )})
	{}

	WinFileWDataSource::WinFileWDataSource (NtStringView filename, EMode mode)		__NE___	: WinFileWDataSource{ filename.c_str(), mode } {}
	WinFileWDataSource::WinFileWDataSource (const String &filename, EMode mode)		__NE___	: WinFileWDataSource{ filename.c_str(), mode } {}
	WinFileWDataSource::WinFileWDataSource (const char* filename, EMode mode)		__NE___	:
		WinFileWDataSource{ Handle_t{OpenFileForWrite( filename, INOUT mode, FILE_FLAG_OVERLAPPED )} DEBUG_ONLY(, Path{filename} )}
	{
		if_unlikely( not IsOpen() )
			WIN_CHECK_DEV( "Can't open file: \""s << filename << "\": " );
	}

	WinFileWDataSource::WinFileWDataSource (NtWStringView filename, EMode mode)		__NE___	: WinFileWDataSource{ filename.c_str(), mode } {}
	WinFileWDataSource::WinFileWDataSource (const WString &filename, EMode mode)	__NE___	: WinFileWDataSource{ filename.c_str(), mode } {}
	WinFileWDataSource::WinFileWDataSource (const wchar_t* filename, EMode mode)	__NE___	:
		WinFileWDataSource{ Handle_t{OpenFileForWrite( filename, INOUT mode, FILE_FLAG_OVERLAPPED )} DEBUG_ONLY(, Path{filename} )}
	{
		if_unlikely( not IsOpen() )
			WIN_CHECK_DEV( "Can't open file: \""s << ToString(filename) << "\": " );
	}

	WinFileWDataSource::WinFileWDataSource (const Path &path, EMode mode)			__NE___	: WinFileWDataSource{ path.c_str(), mode } {}

/*
=================================================
	destructor
=================================================
*/
	WinFileWDataSource::~WinFileWDataSource () __NE___
	{
		if ( IsOpen() )
			::CloseHandle( _file.Ref<HANDLE>() );
	}

/*
=================================================
	IsOpen
=================================================
*/
	bool  WinFileWDataSource::IsOpen () C_NE___
	{
		return _file.Ref<HANDLE>() != INVALID_HANDLE_VALUE;
	}

/*
=================================================
	GetSourceType
=================================================
*/
	IDataSource::ESourceType  WinFileWDataSource::GetSourceType () C_NE___
	{
		return	ESourceType::RandomAccess | ESourceType::WriteAccess |
				ESourceType::ThreadSafe;
	}

/*
=================================================
	Capacity
=================================================
*/
	Bytes  WinFileWDataSource::Capacity () C_NE___
	{
		return GetFileSize( _file.Ref<HANDLE>() );
	}

/*
=================================================
	Reserve
=================================================
*/
	Bytes  WinFileWDataSource::Reserve (Bytes newSize) __NE___
	{
		ASSERT( IsOpen() );

		const Bytes		cur_size = GetFileSize( _file.Ref<HANDLE>() );

		if ( newSize <= cur_size )
			return cur_size;

		LARGE_INTEGER	req_pos;
		LARGE_INTEGER	cur_pos = {};

		req_pos.QuadPart = ulong(newSize);

		if_unlikely( ::SetFilePointerEx( _file.Ref<HANDLE>(), req_pos, OUT &cur_pos, FILE_BEGIN ) == FALSE )	// winxp
			return cur_size;  // error

		if_unlikely( ::SetEndOfFile( _file.Ref<HANDLE>() ) == FALSE )	// winxp
			return cur_size;  // error

		return Bytes{ulong(cur_pos.QuadPart)};  // ok
	}

/*
=================================================
	WriteBlock
=================================================
*/
	Bytes  WinFileWDataSource::WriteBlock (const Bytes pos, const void* buffer, const Bytes size) __NE___
	{
		ASSERT( IsOpen() );
		ASSERT_Lt( size, MaxValue<DWORD>() );

		OVERLAPPED	ov = {};
		SetOverlappedOffset( INOUT ov, pos );

		DWORD	written = 0;
		if_likely( ::WriteFile( _file.Ref<HANDLE>(), buffer, DWORD(size), OUT &written, INOUT &ov ) != FALSE )
			return Bytes{written};  // ok

		auto	err = ::GetLastError();
		if_unlikely( err != ERROR_IO_PENDING )
		{
			WIN_CHECK_DEV2( err, "WriteFile failed: " );
			return 0_b;  // error
		}

		// wait for completion
		if_likely( ::GetOverlappedResult( _file.Ref<HANDLE>(), &ov, OUT &written, TRUE ) != FALSE )	// winxp
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
	void  WinFileWDataSource::Flush () __NE___
	{
		ASSERT( IsOpen() );

		CHECK( ::FlushFileBuffers( _file.Ref<HANDLE>() ) != 0 );	// winxp
	}


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
