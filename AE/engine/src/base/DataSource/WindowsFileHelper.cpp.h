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

		// TODO: use CreateFile2 (win8)

		if constexpr( IsSame< T, char >)
		{
			return ::CreateFileA( filename,			// winxp
								  dwDesiredAccess, dwShareMode,
								  null,		// default security
								  dwCreationDisposition,
								  dwFlagsAndAttributes,
								  null );
		}
		if constexpr( IsSame< T, wchar_t >)
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

		if constexpr( IsSame< T, char >)
		{
			return ::CreateFileA( filename,			// winxp
								  dwDesiredAccess, dwShareMode,
								  null,				// default security
								  dwCreationDisposition,
								  dwFlagsAndAttributes,
								  null );
		}
		if constexpr( IsSame< T, wchar_t >)
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
	Nd__In slong  GetPositionInFile (HANDLE file) __NE___
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
	inline void  SetOverlappedOffset (INOUT OVERLAPPED &ov, Bytes offset) __NE___
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
	Nd__In Bytes  GetOverlappedOffset (const OVERLAPPED &ov) __NE___
	{
		LARGE_INTEGER	li;
		li.LowPart		= ov.Offset;
		li.HighPart		= ov.OffsetHigh;

		return Bytes{ulong(li.QuadPart)};
	}

/*
=================================================
	GetLogicalBytesPerSector
----
	docs:
	https://learn.microsoft.com/en-us/windows/win32/fileio/file-buffering
	https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-deviceiocontrol
	https://learn.microsoft.com/en-us/windows/win32/w8cookbook/advanced-format--4k--disk-compatibility-update?redirectedfrom=MSDN
=================================================
*/
	ND_ static IDataSource::ReqAlign  GetLogicalBytesPerSector (HANDLE file) __NE___
	{
		static constexpr POTBytes	logical_sector_size		{PowerOfTwo(9)};	// 512_b
		static constexpr POTBytes	physical_sector_size	{PowerOfTwo(12)};	// 4_KiB

		WCHAR	path [MAX_PATH+1];
		bool	ok = ::GetFinalPathNameByHandleW( file, OUT path, DWORD(CountOf( path )), FILE_NAME_NORMALIZED | VOLUME_NAME_DOS ) != 0;  // winvista

		if ( not ok or path[0] == 0 )
		{
			DBG_WARNING( "failed to get file path" );
			return IDataSource::ReqAlign{ logical_sector_size, physical_sector_size };
		}

		WStringView  str {path};

		usize	pos = str.find( L":\\" );
		ASSERT( pos < str.size() );

		IDataSource::ReqAlign	res;


		const auto	Variant1 = [pos, str, OUT &res] ()
		{{
			WStringView  part = SubString( str, pos-1, 3 );

			DWORD	spc, bps = 0, freec, totalc;
			bool	ok = ::GetDiskFreeSpaceW( NtWStringView{part}.c_str(), OUT &spc, OUT &bps, OUT &freec, OUT &totalc ) != 0;  // winxp
			ASSERT( ok );

			if ( not ok or bps < 512 )
				return false;

			res = IDataSource::ReqAlign{ POTBytes{Bytes{bps}}, physical_sector_size };
			return true;
		}};


		const auto	Variant2 = [pos, str, OUT &res] ()
		{{
			// "Microsoft strongly recommends that developers align unbuffered I/O to the physical sector size as reported by
			//  the IOCTL_STORAGE_QUERY_PROPERTY control code to help ensure their applications are prepared for this sector size transition."

			WCHAR	vol_name [10] = L"\\\\.\\C:";
			vol_name[4] = str[pos-1];

			HANDLE	volume = ::CreateFileW( vol_name, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, null, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, null );
			if ( volume == INVALID_HANDLE_VALUE )
			{
				WIN_CHECK( "CreateFile failed to open volume: " );
				return false;
			}

			ON_DESTROY( [&volume](){ ::CloseHandle(volume); });

			STORAGE_PROPERTY_QUERY	query = {};
			query.PropertyId	= StorageAccessAlignmentProperty;
			query.QueryType		= PropertyStandardQuery;

			STORAGE_ACCESS_ALIGNMENT_DESCRIPTOR	prop = {};
			DWORD	written = 0;

			bool	ok = ::DeviceIoControl( volume, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query), OUT &prop, sizeof(prop), OUT &written, null ) != 0;  // winxp
			if ( not ok )
			{
				WIN_CHECK( "DeviceIoControl failed: " );
				return false;
			}

			res = IDataSource::ReqAlign{ Bytes{prop.BytesPerLogicalSector}, Bytes{prop.BytesPerPhysicalSector} };
			return true;
		}};

		if ( Variant2() )
			return res;

		if ( Variant1() )
			return res;

		return IDataSource::ReqAlign{ logical_sector_size, physical_sector_size };
	}


} // namespace

#endif // AE_PLATFORM_WINDOWS
