// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_UNIX_BASED

namespace
{
	using RFileFlags	= UnixFileRStream::EMode;
	using WFileFlags	= UnixFileWStream::EMode;

/*
=================================================
	OpenFileForRead
=================================================
*/
#ifdef AE_PLATFORM_APPLE
	ND_ static int  OpenFileForRead (const char* filename, RFileFlags inFlags, int addFlags = 0) __NE___
	{
		int	flags	= O_RDONLY | addFlags;
		int	file	= ::open( filename, flags );

		if ( file >= 0 and AllBits( inFlags, RFileFlags::Direct ))
			CHECK( ::fcntl( file, F_GLOBAL_NOCACHE , 1 ) == 0 );

		return file;
	}

#else
	ND_ static int  OpenFileForRead (const char* filename, RFileFlags inFlags, int addFlags = 0) __NE___
	{
		int	flags	= O_RDONLY | addFlags;
		int	advise	= POSIX_FADV_NORMAL;

		for (auto t : BitfieldIterate( inFlags ))
		{
			switch_enum( t )
			{
				case RFileFlags::RandomAccess :		advise = POSIX_FADV_RANDOM;			break;
				case RFileFlags::SequentialScan :	advise = POSIX_FADV_SEQUENTIAL;		break;
				case RFileFlags::Direct :			flags |= O_DIRECT;					break;
				case RFileFlags::Unix_LargeFile :	flags |= O_LARGEFILE;				break;
				case RFileFlags::Unknown :
				default :							RETURN_ERR( "unknown rfile open flag!", -1 );
			}
			switch_end
		}

		int	file = ::open( filename, flags );

		if ( file >= 0 )
			CHECK( ::posix_fadvise( file, 0, 0, advise ) == 0 );

		return file;
	}
#endif

/*
=================================================
	OpenFileForWrite
=================================================
*/
#ifdef AE_PLATFORM_APPLE
	ND_ static int  OpenFileForWrite2 (const char* filename, WFileFlags inFlags, int addFlags) __NE___
	{
		int mode	= S_IRUSR | S_IWUSR;
		int	flags	= O_WRONLY | addFlags;

		if ( not AnyBits( inFlags, WFileFlags::OpenUpdate | WFileFlags::OpenAppend ))
			flags |= O_CREAT | O_TRUNC;

		if ( AllBits( inFlags, WFileFlags::OpenAppend ))
			flags |= O_APPEND;

		int	file	= ::open( filename, flags, mode );

		if ( file >= 0 and AllBits( inFlags, WFileFlags::Direct ))
			CHECK( ::fcntl( file, F_GLOBAL_NOCACHE , 1 ) == 0 );

		return file;
	}

#else
	ND_ static int  OpenFileForWrite2 (const char* filename, WFileFlags inFlags, int addFlags) __NE___
	{
		int	flags	= O_CREAT | O_TRUNC | O_WRONLY | addFlags;
		int mode	= S_IRUSR | S_IWUSR;

		for (auto t : BitfieldIterate( inFlags ))
		{
			switch_enum( t )
			{
				case WFileFlags::Direct :			flags |= O_DIRECT ;				break;
				case WFileFlags::Unix_LargeFile :	flags |= O_LARGEFILE;			break;
				case WFileFlags::OpenUpdate :		flags &= ~(O_TRUNC | O_CREAT);	break;
				case WFileFlags::OpenAppend :		flags &= ~(O_TRUNC | O_CREAT);	flags |= O_APPEND;	break;
				case WFileFlags::SharedRead :		break;
				case WFileFlags::Unknown :
				default :							RETURN_ERR( "unknown wfile open flag!", -1 );
			}
			switch_end
		}

		int	file = ::open( filename, flags, mode );
		return file;
	}
#endif

	ND_ static int  OpenFileForWrite (const char* filename, INOUT WFileFlags &flags, int addFlags = 0) __NE___
	{
		int	file = OpenFileForWrite2( filename, flags, addFlags );

		if ( file < 0 and AllBits( flags, WFileFlags::OpenUpdate ))
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
	ND_ static Bytes  GetFileSize (int file) __NE___
	{
		struct stat st;

		if_likely( ::fstat( file, OUT &st ) == 0 )
			return Bytes{ulong( st.st_size )};

		return 0_b;
	}

/*
=================================================
	SetPositionInFile*
=================================================
*/
	ND_ static bool  SetPositionInFileFromBegin (int file, slong pos) __NE___
	{
	#ifdef AE_PLATFORM_APPLE
		return ::lseek( file, pos, SEEK_SET ) >- 0;
	#else
		return ::lseek64( file, pos, SEEK_SET ) >= 0;
	#endif
	}

	ND_ static bool  SetPositionInFileFromCurrent (int file, slong pos) __NE___
	{
	#ifdef AE_PLATFORM_APPLE
		return ::lseek( file, pos, SEEK_CUR ) >= 0;
	#else
		return ::lseek64( file, pos, SEEK_CUR ) >= 0;
	#endif
	}

/*
=================================================
	GetPositionInFile
=================================================
*/
	Nd__In slong  GetPositionInFile (int file) __NE___
	{
	#ifdef AE_PLATFORM_APPLE
		return ::lseek( file, 0, SEEK_CUR );
	#else
		return ::lseek64( file, 0, SEEK_CUR );
	#endif
	}

/*
=================================================
	ExtendFile
=================================================
*/
	Nd__In off_t  ExtendFile (int file, off_t offset, off_t length) __NE___
	{
	#ifdef AE_PLATFORM_APPLE
		// https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/fcntl.2.html

		fstore_t 	store = {};
		store.fst_flags		= F_ALLOCATECONTIG;	// Allocate contiguous space.
		store.fst_posmode	= F_PEOFPOSMODE;	// Allocate from the physical end of file.
		store.fst_offset	= 0;
		store.fst_length	= length;

		int	ret = ::fcntl( file, F_PREALLOCATE, &store );
		if_unlikely( ret == -1 )
		{
			//store.fst_flags = F_ALLOCATEALL;	// Allocate all requested space or no space at all.
			ret = ::fcntl( file, F_PREALLOCATE, &store );
			if_unlikely( ret == -1 )
				return -1;
		}
		if_unlikely( ::ftruncate( file, offset+length ) == -1)
			return -1;

		return size_t(store.fst_bytesalloc);

	#else
		// https://man7.org/linux/man-pages/man3/posix_fallocate.3.html
		return 	::posix_fallocate( file, offset, length ) == 0 ?
					length : -1;
	#endif
	}

/*
=================================================
	ExtendFile
=================================================
*/
#ifdef AE_PLATFORM_ANDROID
	#ifndef AT_EMPTY_PATH
	# define AT_EMPTY_PATH 0x1000
	#endif

	bool  android_statx (int fd, unsigned mask, OUT struct statx* sx)
	{
		#ifdef SYS_statx
			return syscall( SYS_statx, fd, "", AT_EMPTY_PATH, mask, sx ) == 0;
		#elif defined(__NR_statx)
			return syscall( __NR_statx, fd, "", AT_EMPTY_PATH, mask, sx ) == 0;
		#else
			errno = ENOSYS;
			return false;
		#endif
	}
	#define STATX( _fd_, _mask_, _outStx_ )		android_statx( _fd_, _mask_, _outStx_ )
#else
	#define STATX( _fd_, _mask_, _outStx_ )		(::statx( _fd_, "", AT_EMPTY_PATH | AT_STATX_SYNC_AS_STAT, _mask_, _outStx_ ) != -1)
#endif

	IDataSource::ReqAlign  GetDirectAccessAlign (int fd) __NE___
	{
		static constexpr POTBytes	logical_sector_size		{PowerOfTwo(9)};	// 512_b
		static constexpr POTBytes	physical_sector_size	{PowerOfTwo(12)};	// 4_KiB

		struct statx	stx{};

		if ( not STATX( fd, STATX_DIOALIGN, OUT &stx ) or
			 NoBits( stx.stx_mask, STATX_DIOALIGN ))
		{
			// default
			return IDataSource::ReqAlign{ logical_sector_size, physical_sector_size };
		}

		return IDataSource::ReqAlign{ POTBytes{stx.stx_dio_offset_align}, POTBytes{stx.stx_dio_mem_align} };
	}

} // namespace

#endif // AE_PLATFORM_UNIX_BASED
