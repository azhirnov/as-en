// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_WINDOWS

namespace
{
	using RFileFlags	= WinFileRStream::EMode;
	using WFileFlags	= WinFileWStream::EMode;

/*
=================================================
	FileFlagCast (RFileFlags)
----
	https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea
=================================================
*/
	ND_ static DWORD  FileFlagCast (RFileFlags values) __NE___
	{
		DWORD	flags = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_POSIX_SEMANTICS;

		for (auto t : BitfieldIterate( values ))
		{
			switch_enum( t )
			{
				case RFileFlags::RandomAccess :		flags |= FILE_FLAG_RANDOM_ACCESS;	break;
				case RFileFlags::SequentialScan :	flags |= FILE_FLAG_SEQUENTIAL_SCAN;	break;
				case RFileFlags::Win_NoBuffering :	flags |= FILE_FLAG_NO_BUFFERING;	break;
				case RFileFlags::Unknown :
				default :							RETURN_ERR( "unknown rfile open flag!", FILE_ATTRIBUTE_NORMAL );
			}
			switch_end
		}

		// validation
		{
			DBG_CHECK_MSG( not AllBits( flags, FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_SEQUENTIAL_SCAN ),
						   "combination of 'RandomAccess' and 'SequentialScan' is not supported" );
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
		DWORD	flags = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_POSIX_SEMANTICS;

		for (auto t : BitfieldIterate( values ))
		{
			switch_enum( t )
			{
				case WFileFlags::Win_NoBuffering :	flags |= FILE_FLAG_NO_BUFFERING;	break;
				case WFileFlags::Win_NoCaching :	flags |= FILE_FLAG_WRITE_THROUGH;	break;

				case WFileFlags::Direct :
			//	case WFileFlags::OpenRewrite :
				case WFileFlags::OpenUpdate :
				case WFileFlags::OpenAppend :
				case WFileFlags::SharedRead :		break;
				case WFileFlags::Unknown :
				default :							RETURN_ERR( "unknown wfile open flag!", FILE_ATTRIBUTE_NORMAL );
			}
			switch_end
		}
		return flags;
	}

/*
=================================================
	OpenFileForRead
=================================================
*/
	template <typename T>
	ND_ HANDLE  OpenFileForRead (const T* filename, RFileFlags flags, DWORD addFlags = 0) __NE___
	{
		DWORD	dwDesiredAccess			= GENERIC_READ;
		DWORD	dwShareMode				= FILE_SHARE_READ | FILE_SHARE_WRITE;	// file may be opened for read and write by another process
		DWORD	dwCreationDisposition	= OPEN_EXISTING;
		DWORD	dwFlagsAndAttributes	= FileFlagCast( flags ) | addFlags;
		
		if constexpr( IsSameTypes< T, char >)
		{
			return ::CreateFileA( filename,			// winxp
								  dwDesiredAccess, dwShareMode,
								  null,		// default security
								  dwCreationDisposition,
								  dwFlagsAndAttributes,
								  null );
		}
		if constexpr( IsSameTypes< T, wchar_t >)
		{
			return ::CreateFileW( filename,			// winxp
								  dwDesiredAccess, dwShareMode,
								  null,		// default security
								  dwCreationDisposition,
								  dwFlagsAndAttributes,
								  null );
		}
	}

/*
=================================================
	OpenFileForWrite
=================================================
*/
	template <typename T>
	ND_ HANDLE  OpenFileForWrite2 (const T* filename, WFileFlags flags, DWORD addFlags) __NE___
	{
		DWORD	dwDesiredAccess			= AllBits( flags, WFileFlags::OpenAppend ) ? FILE_APPEND_DATA : GENERIC_WRITE;
		DWORD	dwShareMode				= AllBits( flags, WFileFlags::SharedRead ) ? FILE_SHARE_READ : 0;	// file may be opened for read by another process
		DWORD	dwCreationDisposition	= AnyBits( flags, WFileFlags::OpenUpdate | WFileFlags::OpenAppend ) ? OPEN_EXISTING : CREATE_ALWAYS;
		DWORD	dwFlagsAndAttributes	= FileFlagCast( flags ) | addFlags;

		if constexpr( IsSameTypes< T, char >)
		{
			return ::CreateFileA( filename,			// winxp
								  dwDesiredAccess, dwShareMode,
								  null,				// default security
								  dwCreationDisposition,
								  dwFlagsAndAttributes,
								  null );
		}
		if constexpr( IsSameTypes< T, wchar_t >)
		{
			return ::CreateFileW( filename,			// winxp
								  dwDesiredAccess, dwShareMode,
								  null,				// default security
								  dwCreationDisposition,
								  dwFlagsAndAttributes,
								  null );
		}
	}

	template <typename T>
	ND_ static HANDLE  OpenFileForWrite (const T* filename, INOUT WFileFlags &flags, DWORD addFlags = 0) __NE___
	{
		HANDLE	file = OpenFileForWrite2( filename, flags, addFlags );

		if ( file == INVALID_HANDLE_VALUE and AllBits( flags, WFileFlags::OpenUpdate ))
		{
			flags	&= ~WFileFlags::OpenUpdate;
			//flags	|= WFileFlags::OpenRewrite;

			file	= OpenFileForWrite2( filename, flags, addFlags );
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
		LARGE_INTEGER	size = {};

		if_likely(::GetFileSizeEx( file, OUT &size ) != FALSE )	// winxp
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
		LARGE_INTEGER	offset	= {};
		LARGE_INTEGER	current	= {};

		if_likely( ::SetFilePointerEx( file, offset, OUT &current, FILE_CURRENT ) != FALSE )	// winxp
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
		LARGE_INTEGER	li;
		li.QuadPart		= ulong(offset);

		ov.Offset		= li.LowPart;
		ov.OffsetHigh	= li.HighPart;
	}

/*
=================================================
	GetOverlappedOffset
=================================================
*/
	ND_ inline Bytes  GetOverlappedOffset (const OVERLAPPED &ov)
	{
		LARGE_INTEGER	li;
		li.LowPart		= ov.Offset;
		li.HighPart		= ov.OffsetHigh;

		return Bytes{ulong(li.QuadPart)};
	}


} // namespace

#endif // AE_PLATFORM_WINDOWS
